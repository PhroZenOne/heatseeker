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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	std::cout << "Creating GL 2.1 window..." << std::endl;
	window_ = glfwCreateWindow( w, h, "HeatSeeker", NULL, NULL);

	glfwMakeContextCurrent(window_);

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
	return glfwWindowShouldClose(window()) != 0;
}

void GlWindow::getFramebufferSize(int & width, int & height){
    glfwGetFramebufferSize(window(), & width, & height);
}
