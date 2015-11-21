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

GlInit::GlInit() {

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
	window_ = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window_ == NULL ){

		std::cout << "failed! Trying to fake 2.1 instead" << std::endl;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

		window_ = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
		if( window_ == NULL ){
			fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
			getchar();
			glfwTerminate();
			exit(-1);
		}
	}


	glfwMakeContextCurrent(window_);

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}

}

GlInit::~GlInit() {
	glfwTerminate();
}

GLFWwindow* GlInit::window() {
	return window_;
}
