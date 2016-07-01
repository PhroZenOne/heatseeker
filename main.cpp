#include <iostream>
#include <fstream>
#include "regular_camera.h"
#include "thermal.h"
#include <opencv/highgui.h>
#include "esUtil.h"
#include <thread>
#include <chrono>
#include "input.h"

RegularCamera * regularCamera;
SeekThermal * irCamera;

static volatile bool alive = true;

//Updates with current pin data every frame
typedef struct _RandomStuffDataHolder {
	CameraMode cameraMode;
	ScreenBrightness brightness;
	bool going_to_shutdown;
	int irFrameCount;
	int webCamFrameCount;

	// Initialize cameraMode and brightness to invalid values to make sure we update them in the first loop.
	_RandomStuffDataHolder() : cameraMode(CameraMode(-1)), brightness(ScreenBrightness(-1)), going_to_shutdown(false), irFrameCount(0), webCamFrameCount(0)
	{}
} RandomStuffDataHolder;


GLuint getCameraTexture() {
	while (!regularCamera->hasFrame()) {
		std::cerr << "Waiting for regular camera to warm up." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	cv::Mat frame = regularCamera->getFrame();
	GLuint texId;

	if (frame.data == NULL) {
		std::cerr << "Could not get hold of a frame buffer from the camera" << std::endl;
		return 0;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frame.size().width, frame.size().height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texId;
}


GLuint getIrCameraTexture() {

	while (!irCamera->hasFrame()) {
		std::cerr << "Waiting for ir camera to warm up" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	ThermalFrame irFrame = irCamera->getFrame();
	std::cout << "Got frame in getIrCameraTexture" << std::endl;
	GLuint texId;

	if (irFrame.imageData == NULL) {
		std::cout << "Error loading buffer from ir-camera.\n" << std::endl;
		return 0;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, irFrame.width, irFrame.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, irFrame.imageData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	std::cout << "Got 2 in getIrCameraTexture" << std::endl;

	return texId;
}

void setupShader(const std::string &vertexShaderSource, const std::string &fragmentShaderSource,
	GLuint *programObject, GLint *positionLoc, GLint *texCoordLoc, GLint *calibrationLoc, GLint *irMapLoc, GLint *colorMapLoc)
{
	//std::cout << "Building shader...";
	//std::cout << "Vertex shader:" << std::endl << vertexShaderSource.c_str() << std::endl << std::endl;
	//std::cout << "Fragment shader:" << std::endl << fragmentShaderSource.c_str() << std::endl << std::endl;
	*programObject  = esLoadProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	if (positionLoc)
		*positionLoc    = glGetAttribLocation( *programObject, "a_position");
	if (texCoordLoc)
		*texCoordLoc    = glGetAttribLocation( *programObject, "a_tex_coord");
	if (calibrationLoc)
		*calibrationLoc = glGetUniformLocation(*programObject, "u_calibration");
	if (irMapLoc)
		*irMapLoc       = glGetUniformLocation(*programObject, "s_ir");
	if (colorMapLoc)
		*colorMapLoc    = glGetUniformLocation(*programObject, "s_color");
}

///
// Initialize the shader and program object
//
int init(ESContext & esContext) {

	regularCamera = new RegularCamera(esContext.width, esContext.height);

	irCamera = new SeekThermal();
	std::cout << "init" << std::endl;

	GlData * glData = esContext.glData;

	// Load the common vertex shader
	std::ifstream commonVSStream("heastvertexshader.glsl");
	std::string commonVSSource((std::istreambuf_iterator<char>(commonVSStream)), std::istreambuf_iterator<char>());

	std::cout << "Building IR Only shader" << std::endl;

	// Setup the irOnly shader
	std::ifstream irOnlyFSStream("ironlyfragmentshader.glsl");
	std::string irOnlyFSSource((std::istreambuf_iterator<char>(irOnlyFSStream)), std::istreambuf_iterator<char>());

	setupShader(commonVSSource, irOnlyFSSource,
		&glData->irOnlyProgramObject, &glData->irOnlyPositionLoc, &glData->irOnlyTexCoordLoc,
		NULL, &glData->irOnlyIrMapLoc, NULL);

	std::cout << "Building Mixed Grayscale shader" << std::endl;

	// Setup the mixedGrayscale shader
	std::ifstream mixedGrayscaleFSStream("mixedgrayscalefragmentshader.glsl");
	std::string mixedGrayscaleFSSource((std::istreambuf_iterator<char>(mixedGrayscaleFSStream)), std::istreambuf_iterator<char>());
	setupShader(commonVSSource, mixedGrayscaleFSSource,
		&glData->mixedGrayscaleProgramObject, &glData->mixedGrayscalePositionLoc, &glData->mixedGrayscaleTexCoordLoc,
		&glData->mixedGrayscaleCalibrationLoc, &glData->mixedGrayscaleIrMapLoc, &glData->mixedGrayscaleColorMapLoc);

	std::cout << "Building Mixed Blend shader" << std::endl;

	// Setup the mixedGrayscale shader
	std::ifstream mixedBlendFSStream("mixedblendfragmentshader.glsl");
	std::string mixedBlendFSSource((std::istreambuf_iterator<char>(mixedBlendFSStream)), std::istreambuf_iterator<char>());
	setupShader(commonVSSource, mixedBlendFSSource,
		&glData->mixedBlendProgramObject, &glData->mixedBlendPositionLoc, &glData->mixedBlendTexCoordLoc,
		&glData->mixedBlendCalibrationLoc, &glData->mixedBlendIrMapLoc, &glData->mixedBlendColorMapLoc);

	glData->irMapTexId = getIrCameraTexture();
	glData->colorMapTexId = getCameraTexture();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return TRUE;
}

bool updateCameraTexture(GlData * glData, GLuint texId, GLint loc) {
	if (regularCamera->hasFrame()) {
		// Bind the base map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texId);

		cv::Mat frame = regularCamera->getFrame(); 

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.size().width, frame.size().height, GL_RGBA, GL_UNSIGNED_BYTE, frame.data);

		// Set the base map sampler to texture unit to 0
		glUniform1i(loc, 0);
	}
}

bool updateIrCameraTexture(GlData * glData, GLuint texId, GLint loc) {
	if (irCamera->hasFrame()) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texId);

		ThermalFrame irFrame = irCamera->getFrame();

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, irFrame.width, irFrame.height, GL_LUMINANCE, GL_UNSIGNED_BYTE, irFrame.imageData);

		glUniform1i(loc, 1);
	}
}

void draw(ESContext & esContext, RandomStuffDataHolder & data) {
	GlData * glData = esContext.glData;
	GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,  // Position 0
	                        0.0f,  0.0f,        // TexCoord 0
	                        -1.0f, -1.0f, 0.0f,  // Position 1
	                        0.0f,  1.0f,        // TexCoord 1
	                        1.0f, -1.0f, 0.0f,  // Position 2
	                        1.0f,  1.0f,        // TexCoord 2
	                        1.0f,  1.0f, 0.0f,  // Position 3
	                        1.0f,  0.0f         // TexCoord 3
	                      };
	GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

	GLfloat calibrate[] = {
		-0.5f, -0.5f, 0.f, 0.f,
		 1.5f, -0.5f, 0.f, 0.f,
		-0.5f,  1.5f, 0.f, 0.f,
		 1.5f,  1.5f, 0.f, 0.f
	};

	// Set the viewport
	glViewport(0, 0, esContext.width, esContext.height);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	switch (data.cameraMode) {
		case mixedGrayscale:
			// Use the program object
			glUseProgram(glData->mixedGrayscaleProgramObject);

			// Load the vertex position
			glVertexAttribPointer(glData->mixedGrayscalePositionLoc, 3, GL_FLOAT,
			                      GL_FALSE, 5 * sizeof(GLfloat), vVertices);
			// Load the texture coordinate
			glVertexAttribPointer(glData->mixedGrayscaleTexCoordLoc, 2, GL_FLOAT,
			                      GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

			glEnableVertexAttribArray(glData->mixedGrayscalePositionLoc);
			glEnableVertexAttribArray(glData->mixedGrayscaleTexCoordLoc);

			glUniformMatrix4fv(glData->mixedBlendCalibrationLoc, 1, false, calibrate);
			break;
		case mixedBlend:
			// Use the program object
			glUseProgram(glData->mixedBlendProgramObject);

			// Load the vertex position
			glVertexAttribPointer(glData->mixedBlendPositionLoc, 3, GL_FLOAT,
			                      GL_FALSE, 5 * sizeof(GLfloat), vVertices);
			// Load the texture coordinate
			glVertexAttribPointer(glData->mixedBlendTexCoordLoc, 2, GL_FLOAT,
			                      GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

			glEnableVertexAttribArray(glData->mixedBlendPositionLoc);
			glEnableVertexAttribArray(glData->mixedBlendTexCoordLoc);

			glUniformMatrix4fv(glData->mixedBlendCalibrationLoc, 1, false, calibrate);
			break;
		case irOnly:
			// Use the program object
			glUseProgram(glData->irOnlyProgramObject);

			// Load the vertex position
			glVertexAttribPointer(glData->irOnlyPositionLoc, 3, GL_FLOAT,
			                      GL_FALSE, 5 * sizeof(GLfloat), vVertices);
			// Load the texture coordinate
			glVertexAttribPointer(glData->irOnlyTexCoordLoc, 2, GL_FLOAT,
			                      GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

			glEnableVertexAttribArray(glData->irOnlyPositionLoc);
			glEnableVertexAttribArray(glData->irOnlyTexCoordLoc);
			break;
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

bool set_backlight_brightness(const string value)
{
	string setdirStr = "/sys/class/backlight/rpi_backlight/brightness";

	int handle = open(setdirStr.c_str(), O_WRONLY | O_SYNC); // open direction file for gpio
	if (handle < 0) {
		perror("could not open SYSFS backlight brightness device");
		return false;
	}

	//if (dir.compare("in") != 0 && dir.compare("out") != 0) {
	//	fprintf(stderr, "Invalid direction value. Should be \"in\" or \"out\". \n");
	//	exit(1);
	//}

	int res = write(handle, value.c_str(), value.length());
	if (res < 0) {
		perror("could not write to SYSFS backlight brightness device");
		return false;
	}

	res = close(handle);
	if (res < 0) {
		perror("could not close SYSFS backlight brightness device");
		return false;
	}

	return true;
}

void update(ESContext & esContext, float deltatime, GPIOInput &input, RandomStuffDataHolder & data) {
	GlData * glData = esContext.glData;

	// Poll GPIO
	input.update();
	if (input.shutdown()) {
		data.going_to_shutdown = true;
		alive = false;
		return;
	}
	if (input.brightness() != data.brightness) {
		data.brightness = input.brightness();
		switch (data.brightness)
		{
			case low:
				set_backlight_brightness("32");
				break;
			case medium:
				set_backlight_brightness("96");
				break;
			case high:
				set_backlight_brightness("255");
				break;
		}
	}
	if (input.mode() != data.cameraMode) {
		data.cameraMode = input.mode();
	}

	switch (data.cameraMode)
	{
		case irOnly:
			if (updateIrCameraTexture(glData, glData->irMapTexId, glData->irOnlyIrMapLoc)) {
				data.irFrameCount++;
			};
			break;
		case mixedGrayscale:
			if (updateIrCameraTexture(glData, glData->irMapTexId, glData->mixedGrayscaleIrMapLoc)) {
				data.irFrameCount++;
			};
			if (updateCameraTexture(glData, glData->colorMapTexId, glData->mixedGrayscaleColorMapLoc)) {
				data.webCamFrameCount++;
			};
			break;
		case mixedBlend:
			if (updateIrCameraTexture(glData, glData->irMapTexId, glData->mixedBlendIrMapLoc)) {
				data.irFrameCount++;
			};
			if (updateCameraTexture(glData, glData->colorMapTexId, glData->mixedBlendColorMapLoc)) {
				data.webCamFrameCount++;
			};
			break;
	}
}


void shutdown(ESContext & esContext) {
	std::cout << "shutting down" << std::endl;

	GlData * glData = esContext.glData;

	// Delete texture object
	glDeleteTextures(1, &glData->colorMapTexId);
	glDeleteTextures(1, &glData->irMapTexId);

	// Delete program object
	glDeleteProgram(glData->mixedGrayscaleProgramObject);
	glDeleteProgram(glData->mixedBlendProgramObject);
	glDeleteProgram(glData->irOnlyProgramObject);

	delete regularCamera;
	delete irCamera;
}

void exitHandler(int dummy) {
	alive = false;
}


int main(void) {
	std::cout << "Starting" << std::endl;

	ESContext esContext;
	GlData glData;
	RandomStuffDataHolder data;
	GPIOInput input;

	std::cout << "Initializing context" << std::endl;
	esInitContext(&esContext);
	esContext.glData = &glData;

	std::cout << "Grabbing a display" << std::endl;
	esCreateWindow(&esContext, esContext.width, esContext.height, ES_WINDOW_RGB);

	if (!init(esContext)) {
		std::cerr << "Could not init context" << std::endl;
		return 1;
	}

	struct timeval t1, t2;
	struct timezone tz;
	float deltatime;
	float totaltime = 0.0f;
	unsigned int frames = 0;

	gettimeofday(&t1 , &tz);

	signal(SIGINT, exitHandler);
	signal(SIGTERM, exitHandler);

	while (alive) {
		gettimeofday(&t2, &tz);
		deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
		t1 = t2;

		update(esContext, deltatime, input, data);
		draw(esContext, data);

		eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);

		totaltime += deltatime;
		frames++;
		if (totaltime >  2.0f) {
			printf("%4d webcam frames rendered in %1.4f seconds -> FPS=%3.4f\n", data.webCamFrameCount, totaltime, data.webCamFrameCount / totaltime);
			printf("%4d ir frames rendered in %1.4f seconds -> FPS=%3.4f\n", data.irFrameCount, totaltime, data.irFrameCount / totaltime);
			printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames / totaltime);

			printf("Mode is: %d\n", input.mode());
			printf("Brightness is: %d\n", input.brightness());
			printf("Standby button: %s\n", input.standby() ? "True" : "False");
			printf("Shutdown button: %s\n", input.shutdown() ? "True" : "False");

			totaltime -= 2.0f;
			frames = 0;
			data.irFrameCount = 0;
			data.webCamFrameCount = 0;
		}
	}

	std::cout << "Shutting down HeAST software." << std::endl;
	shutdown(esContext);

	if (data.going_to_shutdown) {
		std::cout << "Shutting down system." << std::endl;
		system("shutdown now");
	}

	std::cout << "Bye bye" << std::endl;
	return 0;
}
