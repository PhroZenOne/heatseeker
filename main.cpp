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


///
// Initialize the shader and program object
//
int init(ESContext & esContext) {

	regularCamera = new RegularCamera(esContext.width, esContext.height);

	irCamera = new SeekThermal();
	std::cout << "init" << std::endl;

	GlData * glData = esContext.glData;


	std::ifstream inFrag1("mixedfragmentshader.glsl");
	std::string fShaderStr1((std::istreambuf_iterator<char>(inFrag1)), std::istreambuf_iterator<char>());

	std::ifstream inVert1("mixedvertexshader.glsl");
	std::string vShaderStr1((std::istreambuf_iterator<char>(inVert1)), std::istreambuf_iterator<char>());

	// Load the shaders and get a linked program object
	glData->mixedProgramObject = esLoadProgram(vShaderStr1.c_str(), fShaderStr1.c_str());



	std::ifstream inFrag2("ironlyfragmentshader.glsl");
	std::string fShaderStr2((std::istreambuf_iterator<char>(inFrag2)), std::istreambuf_iterator<char>());

	std::ifstream inVert2("ironlyvertexshader.glsl");
	std::string vShaderStr2((std::istreambuf_iterator<char>(inVert2)), std::istreambuf_iterator<char>());

	// Load the shaders and get a linked program object
	glData->irOnlyProgramObject = esLoadProgram(vShaderStr2.c_str(), fShaderStr2.c_str());



	std::ifstream inFrag3("webcamonlyfragmentshader.glsl");
	std::string fShaderStr3((std::istreambuf_iterator<char>(inFrag3)), std::istreambuf_iterator<char>());

	std::ifstream inVert3("webcamonlyvertexshader.glsl");
	std::string vShaderStr3((std::istreambuf_iterator<char>(inVert3)), std::istreambuf_iterator<char>());

	// Load the shaders and get a linked program object
	glData->webcamOnlyProgramObject = esLoadProgram(vShaderStr3.c_str(), fShaderStr3.c_str());



	glData->positionLoc = glGetAttribLocation(glData->mixedProgramObject, "a_position");
	glData->texCoordLoc = glGetAttribLocation(glData->mixedProgramObject, "a_tex_coord");

	// Get a handle for our "textureSampler" uniform
	glData->cameraMapLoc = glGetUniformLocation(glData->mixedProgramObject, "s_baseMap");
	glData->irMapLoc = glGetUniformLocation(glData->mixedProgramObject, "s_irMap");

	glData->irTransformLoc = glGetUniformLocation(glData->mixedProgramObject, "u_transform_ir");

	glData->cameraMapTexId = getCameraTexture();
	glData->irMapTexId = getIrCameraTexture();

	if (glData->cameraMapTexId == 0 || glData->irMapTexId == 0)
		return FALSE;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return TRUE;
}

bool updateCameraTexture(GlData * glData) {
	if (regularCamera->hasFrame()) {
		// Bind the base map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glData->cameraMapTexId);

		cv::Mat frame = regularCamera->getFrame();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.size().width, frame.size().height, GL_RGBA, GL_UNSIGNED_BYTE, frame.data);

		// Set the base map sampler to texture unit to 0
		glUniform1i(glData->cameraMapLoc, 0);
	}
}

bool updateIrCameraTexture(GlData * glData) {
	if (irCamera->hasFrame()) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, glData->irMapTexId);

		ThermalFrame irFrame = irCamera->getFrame();

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, irFrame.width, irFrame.height, GL_LUMINANCE, GL_UNSIGNED_BYTE, irFrame.imageData);

		glUniform1i(glData->irMapLoc, 1);
	}
}

void draw(ESContext & esContext) {
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

	// Set the viewport
	glViewport(0, 0, esContext.width, esContext.height);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(glData->mixedProgramObject);

	// Load the vertex position
	glVertexAttribPointer(glData->positionLoc, 3, GL_FLOAT,
	                      GL_FALSE, 5 * sizeof(GLfloat), vVertices);
	// Load the texture coordinate
	glVertexAttribPointer(glData->texCoordLoc, 2, GL_FLOAT,
	                      GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

	glEnableVertexAttribArray(glData->positionLoc);
	glEnableVertexAttribArray(glData->texCoordLoc);

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

	// Update color frame
	if (updateCameraTexture(glData)) {
		data.webCamFrameCount++;
	};

	// Update IR frame
	if (updateIrCameraTexture(glData)) {
		data.irFrameCount++;
	};
}


void shutdown(ESContext & esContext) {
	std::cout << "shutting down" << std::endl;

	GlData * glData = esContext.glData;

	// Delete texture object
	glDeleteTextures(1, &glData->cameraMapTexId);
	glDeleteTextures(1, &glData->irMapTexId);

	// Delete program object
	glDeleteProgram(glData->mixedProgramObject);

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
		draw(esContext);

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
