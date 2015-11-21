/*
 * GlInit.h
 *
 *  Created on: Nov 21, 2015
 *      Author: johan
 */

#ifndef PLAYGROUND_GLINIT_H_
#define PLAYGROUND_GLINIT_H_

class GlInit {
public:
	GlInit(int w = 1024, int h = 768);
	virtual ~GlInit();
	GLFWwindow* window();

	operator GLFWwindow*();

private:
	GLFWwindow* window_;
};

typedef GlInit GlWindow;

#endif /* PLAYGROUND_GLINIT_H_ */
