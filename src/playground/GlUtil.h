/*
 * GlUtil.h
 *
 *  Created on: Nov 21, 2015
 *      Author: johan
 */

#ifndef PLAYGROUND_GLUTIL_H_
#define PLAYGROUND_GLUTIL_H_

class GlUtil {
public:
	GlUtil();
	virtual ~GlUtil();

	static void placeWindowOnLastMonitor(GLFWwindow * window);

};

#endif /* PLAYGROUND_GLUTIL_H_ */
