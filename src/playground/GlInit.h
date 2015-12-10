/*
 * GlInit.h
 *
 *  Created on: Nov 21, 2015
 *      Author: johan
 */

#ifndef PLAYGROUND_GLINIT_H_
#define PLAYGROUND_GLINIT_H_

class GlWindow {
public:
	GlWindow(int w = 1024, int h = 768);
	virtual ~GlWindow();
	GLFWwindow* window();

	void placeOnLastMonitor();
	void setInputMode(int mode, int value);
	void swapBuffers();
	void pollEvents();

	bool isKeyPressed(int key);
	bool wantsToClose();
	void getFramebufferSize(int &width, int &height);

private:
	GLFWwindow* window_;
};

#endif /* PLAYGROUND_GLINIT_H_ */
