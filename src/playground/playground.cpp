#include "glincludes.h"
#include "RegularCameraResource.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <opencv/highgui.h>

#include <common/shader.hpp>


int main(void) {

    int naitiveWidth = 1920;
    int naitiveHeight = 1080;

    RegularCameraResource camera = RegularCameraResource(naitiveWidth, naitiveHeight);

	GlWindow window(naitiveWidth, naitiveHeight);

	window.placeOnLastMonitor();

	window.setInputMode(GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint programID = LoadShaders("vertexshader", "fragmentshader");
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 19:10 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(45.0f, (float) naitiveWidth / (float) naitiveHeight, 0.1f, naitiveWidth + 100.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, naitiveWidth),
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	const GLfloat quadVertices[] = {
		0.f-naitiveWidth, 0.f-naitiveWidth, 0.0f,
		0.f-naitiveWidth, 0.f+naitiveHeight, 0.0f,
		0.f+naitiveWidth, 0.f+naitiveHeight, 0.0f,
		0.f+naitiveWidth, 0.f-naitiveWidth, 0.0f
	};

	const GLfloat g_uv_buffer_data[] = {
		1, 1,
		1, 0,
		0, 0,
		0, 1,
	};

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices),
			quadVertices, GL_STATIC_DRAW);


	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	GLuint texture;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
	GL_LINEAR);

	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(TextureID, 0);

	IplImage* frame = camera.getFrame();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_BGR, GL_UNSIGNED_BYTE, frame->imageData);


	while (!(window.wantsToClose() || window.isKeyPressed(GLFW_KEY_ESCAPE))) {

        int width, height;
        window.getFramebufferSize(width, height);
        glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programID);

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        frame = camera.getFrame();

		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, frame->width, frame->height, GL_BGR, GL_UNSIGNED_BYTE, frame->imageData);

		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(vertexPosition_modelspaceID, // The attribute we want to configure
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*) 0            // array buffer offset
		);


		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);

		glDrawArrays(GL_QUADS, 0, 4);

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);

		window.swapBuffers();
		window.pollEvents();

	}

	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);

	return 0;
}

