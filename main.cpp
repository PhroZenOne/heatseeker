#include <iostream>
#include <fstream>
#include "RegularCameraResource.h"
#include "thermal.h"
#include <opencv/highgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include "esUtil.h"


RegularCameraResource * camera;
SeekThermal * irCamera;

GLuint LoadTexture(const char *fileName) {
	int width,
	    height;
	char *buffer = esLoadTGA(fileName, &width, &height);
	GLuint texId;

	if (buffer == NULL) {
		esLogMessage("Error loading (%s) image.\n", fileName);
		return 0;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	free(buffer);

	return texId;
}


GLuint GetRegularCameraTexture() {
	IplImage* frame = irCamera->getFrame();
	GLuint texId;

	if (frame->imageData == NULL) {
		esLogMessage("Error loading (%s) buffer from camera.\n");
		return 0;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame->imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texId;
}


GLuint GetIrCameraTexture() {
	ThermalFrame* frame = irCamera->getFrame();
	GLuint texId;

	if (frame->imageData == NULL) {
		esLogMessage("Error loading (%s) buffer from ir-camera.\n");
		return 0;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame->imageData);
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

	camera = new RegularCameraResource(esContext->width, esContext->height);
	irCamera = new SeekThermal();

	irCamera.

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
	glData->irMapLoc = glGetUniformLocation(glData->programObject, "s_lightMap");

	glData->irTransformLoc = glGetUniformLocation(glData->programObject, "u_transform_ir");

	glData->cameraMapTexId = GetRegularCameraTexture();
	glData->irMapTexId = LoadTexture("lightmap.tga");

	if (glData->cameraMapTexId == 0 || glData->irMapTexId == 0)
		return FALSE;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	return TRUE;
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

	glm::mat3 trans = glm::mat3(1.0f);
	trans = glm::rotate(trans, 0.0f);
	trans = glm::scale(trans, glm::vec2(0.5f, 0.5f));
	trans = glm::translate(trans, glm::vec2(0.5f, 0.5f));

	//TODO: figure out why the hell i need to inverse it to make any sense.
	trans = glm::inverse(trans);

	glUniformMatrix3fv(
	    glData->irTransformLoc,
	    1, //how many matrices we want to send
	    GL_FALSE, //transpose the matrix
	    glm::value_ptr(trans) //a pointer to an array containing the entries for
	    //the matrix
	);

	// Bind the base map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, glData->cameraMapTexId);

	IplImage* frame = camera->getFrame();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->width, frame->height, GL_RGB, GL_UNSIGNED_BYTE, frame->imageData);

	// Set the base map sampler to texture unit to 0
	glUniform1i(glData->cameraMapLoc, 0);

	// Bind the light map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, glData->irMapTexId);

	// Set the light map sampler to texture unit 1
	glUniform1i(glData->irMapLoc, 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}


void ShutDown(ESContext *esContext) {
	GlData * glData = esContext->glData;

	// Delete texture object
	glDeleteTextures(1, &glData->cameraMapTexId);
	glDeleteTextures(1, &glData->irMapTexId);

	// Delete program object
	glDeleteProgram(glData->programObject);

	delete camera;
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
