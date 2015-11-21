/*
 * GlUtil.cpp
 *
 *  Created on: Nov 21, 2015
 *      Author: johan
 */

#include "glincludes.h"
#include <algorithm>

GlUtil::GlUtil() {
	// TODO Auto-generated constructor stub

}

GlUtil::~GlUtil() {
	// TODO Auto-generated destructor stub
}

void GlUtil::placeWindowOnLastMonitor(GLFWwindow* window) {
	//I currently want it to run on the last screen so a quick hack to put it on the correct screen.

	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	int max_x = 0;
	int max_y = 0;

	for (int i = 0; i < count; i++) {
		int monitor_x, monitor_y;
		glfwGetMonitorPos(monitors[i], &monitor_x, &monitor_y);
		const struct GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

		max_x = std::max(monitor_x, max_x);
		if (max_x == monitor_x) {
			max_y = std::max(mode->height / 2, max_y);
		}
	}

	glfwSetWindowPos(window, max_x, max_y);
}
