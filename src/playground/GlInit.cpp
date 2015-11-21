/*
 * GlInit.cpp
 *
 *  Created on: Nov 21, 2015
 *      Author: johan
 */

#include "glincludes.h"

#include <iostream>

#include <stdio.h>
#include <stdlib.h>

GlWindow::GlWindow(int w , int h) {

	window_ = NULL;

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		exit(-1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	std::cout << "Creating GL 3.3 window..." << std::endl;
	window_ = glfwCreateWindow( w, h, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window_ == NULL ){

		std::cout << "failed! Trying to fake 2.1 instead" << std::endl;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

		window_ = glfwCreateWindow( w, h, "Tutorial 02 - Red triangle", NULL, NULL);
		if( window_ == NULL ){
			fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
			getchar();
			glfwTerminate();
			exit(-1);
		}
	}


	glfwMakeContextCurrent(window_);
	glewExperimental = true; // Needed for core profile

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}

}

GlWindow::~GlWindow() {
	glfwTerminate();
}

GLFWwindow* GlWindow::window() {
	return window_;
}

void GlWindow::placeOnLastMonitor() {
	GlUtil::placeWindowOnLastMonitor(window());
}

void GlWindow::setInputMode(int mode, int value) {
	glfwSetInputMode(window(), mode, value);
}

void GlWindow::swapBuffers() {
	glfwSwapBuffers(window());
}

void GlWindow::pollEvents() {
	glfwPollEvents();
}

bool GlWindow::isKeyPressed(int key) {
	return glfwGetKey(window(), key) == GLFW_PRESS;
}

bool GlWindow::wantsToClose() {
	return glfwWindowShouldClose(window());
}
