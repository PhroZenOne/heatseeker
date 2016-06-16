#include <iostream>
#include <fstream>
#include "regular_camera.h"
#include "thermal.h"
#include <opencv/highgui.h>
#include "esUtil.h"

RegularCamera * regularCamera;
SeekThermal * irCamera;

GLuint GetCameraTexture() {
	cv::Mat frame = regularCamera->getFrame();
	GLuint texId;

	if (frame.data == NULL) {
		esLogMessage("Error loading (%s) buffer from camera.\n");
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


GLuint GetIrCameraTexture() {
	ThermalFrame* irFrame = irCamera->getFrame();
	GLuint texId;

	if (irFrame->imageData == NULL) {
		std::cout << "Error loading (%s) buffer from ir-camera.\n" << std::endl;
		return 0;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	char imageData[65536];
	memset(&imageData[0], 255, sizeof(imageData));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, irFrame->width, irFrame->height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, imageData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texId;
}


///
// Initialize the shader and program object
//
int Init(ESContext *esContext) {

	regularCamera = new RegularCamera(esContext->width, esContext->height);

	irCamera = new SeekThermal();
	std::cout << "init" << std::endl;

	GlData * glData = esContext->glData;

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

	glData->cameraMapTexId = GetCameraTexture();
	glData->irMapTexId = GetIrCameraTexture();

	if (glData->cameraMapTexId == 0 || glData->irMapTexId == 0)
		return FALSE;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return TRUE;
}

void updateCameraTexture(GlData * glData) {
	// Bind the base map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glData->cameraMapTexId);

	cv::Mat frame = regularCamera->getFrame();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame.size().width, frame.size().height, GL_RGB, GL_UNSIGNED_BYTE, frame.data);

	// Set the base map sampler to texture unit to 0
	glUniform1i(glData->cameraMapLoc, 0);
}

void updateIrCameraTexture(GlData * glData) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glData->irMapTexId);

	ThermalFrame* irFrame = irCamera->getFrame();

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, irFrame->width, irFrame->height, GL_LUMINANCE, GL_UNSIGNED_BYTE, irFrame->imageData);

	glUniform1i(glData->irMapLoc, 1);
}

void Draw(ESContext *esContext) {
	GlData * glData = esContext->glData;
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
	glViewport(0, 0, esContext->width, esContext->height);

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


void ShutDown(ESContext *esContext) {
	std::cout << "shutting down" << std::endl;

	GlData * glData = esContext->glData;

	// Delete texture object
	glDeleteTextures(1, &glData->cameraMapTexId);
	glDeleteTextures(1, &glData->irMapTexId);

	// Delete program object
	glDeleteProgram(glData->programObject);

	delete regularCamera;
	delete irCamera;
}



int main(void) {
	std::cout << "Running main" << std::endl;

	ESContext esContext;
	GlData glData;

	esInitContext(&esContext);
	esContext.glData = &glData;

	esCreateWindow(&esContext, "MultiTexture", esContext.width, esContext.height, ES_WINDOW_RGB);

	if (!Init(&esContext))
		return 0;

	esRegisterDrawFunc(&esContext, Draw);

	esMainLoop(&esContext);

	ShutDown(&esContext);

}
