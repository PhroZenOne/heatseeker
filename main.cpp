#include <iostream>
#include <fstream>
#include "regular_camera.h"
#include "thermal.h"
#include <opencv/highgui.h>
#include "esUtil.h"
#include <thread>
#include <chrono>

RegularCamera * regularCamera;
SeekThermal * irCamera;

enum CameraMode { irOnly, mixed, cameraOnly };
enum ScreenBrightness { low, medium, high };

static volatile bool alive = true;

//Updates with current pin data every frame
typedef struct _userInputStates {
	CameraMode cameraMode;
	ScreenBrightness brightness;
	bool shutdownSwitchPos = false;
} UserInputStates;


GLuint getCameraTexture() {
	while (!regularCamera->hasFrame()) {
		std::cerr << "Waiting for regular camera to warm up." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	cv::Mat frame = regularCamera->getFrame();
	GLuint texId;

	if (frame.data == NULL) {
		std::cerr << "Could not get hold of a frame buffer from the camera" << std::endl;
		return 0;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.size().width, frame.size().height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texId;
}


GLuint getIrCameraTexture() {

	while (!irCamera->hasFrame()) {
		std::cerr << "Waiting for ir camera to warm up" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

	std::ifstream inFrag("fragmentshader.glsl");
	std::string fShaderStr((std::istreambuf_iterator<char>(inFrag)), std::istreambuf_iterator<char>());

	std::ifstream inVert("vertexshader.glsl");
	std::string vShaderStr((std::istreambuf_iterator<char>(inVert)), std::istreambuf_iterator<char>());


	// Load the shaders and get a linked program object
	glData->programObject = esLoadProgram(vShaderStr.c_str(), fShaderStr.c_str());

	glData->positionLoc = glGetAttribLocation(glData->programObject, "a_position");
	glData->texCoordLoc = glGetAttribLocation(glData->programObject, "a_tex_coord");

	// Get a handle for our "textureSampler" uniform
	glData->cameraMapLoc = glGetUniformLocation(glData->programObject, "s_baseMap");
	glData->irMapLoc = glGetUniformLocation(glData->programObject, "s_irMap");

	glData->irTransformLoc = glGetUniformLocation(glData->programObject, "u_transform_ir");

	glData->cameraMapTexId = getCameraTexture();
	glData->irMapTexId = getIrCameraTexture();

	if (glData->cameraMapTexId == 0 || glData->irMapTexId == 0)
		return FALSE;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return TRUE;
}

void updateCameraTexture(GlData * glData) {
	if (regularCamera->hasFrame()) {
		// Bind the base map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, glData->cameraMapTexId);

		cv::Mat frame = regularCamera->getFrame();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.size().width, frame.size().height, GL_RGB, GL_UNSIGNED_BYTE, frame.data);

		// Set the base map sampler to texture unit to 0
		glUniform1i(glData->cameraMapLoc, 0);
	}
}

void updateIrCameraTexture(GlData * glData) {
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
	glUseProgram(glData->programObject);

	// Load the vertex position
	glVertexAttribPointer(glData->positionLoc, 3, GL_FLOAT,
	                      GL_FALSE, 5 * sizeof(GLfloat), vVertices);
	// Load the texture coordinate
	glVertexAttribPointer(glData->texCoordLoc, 2, GL_FLOAT,
	                      GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

	glEnableVertexAttribArray(glData->positionLoc);
	glEnableVertexAttribArray(glData->texCoordLoc);

	updateCameraTexture(glData);
	updateIrCameraTexture(glData);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void update(ESContext & esContext, float deltatime) {
	GlData * glData = esContext.glData;
	updateCameraTexture(glData);
	updateIrCameraTexture(glData);
}


void shutdown(ESContext & esContext) {
	std::cout << "shutting down" << std::endl;

	GlData * glData = esContext.glData;

	// Delete texture object
	glDeleteTextures(1, &glData->cameraMapTexId);
	glDeleteTextures(1, &glData->irMapTexId);

	// Delete program object
	glDeleteProgram(glData->programObject);

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
	UserInputStates input;

	std::cout << "Initializing context" << std::endl;
	esInitContext(&esContext);
	esContext.glData = &glData;

	std::cout << "Grabbing a display" << std::endl;
	esCreateWindow(&esContext, " This string is not in use ", esContext.width, esContext.height, ES_WINDOW_RGB);

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

	while (alive) {
		gettimeofday(&t2, &tz);
		deltatime = (float)(t2.tv_sec - t1.tv_sec + (t2.tv_usec - t1.tv_usec) * 1e-6);
		t1 = t2;

		if (esContext.updateFunc != NULL)
			update(esContext, deltatime);
		if (esContext.drawFunc != NULL)
			draw(esContext);

		eglSwapBuffers(esContext.eglDisplay, esContext.eglSurface);

		totaltime += deltatime;
		frames++;
		if (totaltime >  2.0f) {
			printf("%4d frames rendered in %1.4f seconds -> FPS=%3.4f\n", frames, totaltime, frames / totaltime);
			totaltime -= 2.0f;
			frames = 0;
		}
	}

	std::cout << "Shutting down." << std::endl;
	shutdown(esContext);

	std::cout << "Bye bye" << std::endl;
	return 0;
}
