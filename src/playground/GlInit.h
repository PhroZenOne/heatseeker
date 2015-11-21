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
	GlInit();
	virtual ~GlInit();
	GLFWwindow* window();

private:
	GLFWwindow* window_;
};

#endif /* PLAYGROUND_GLINIT_H_ */
