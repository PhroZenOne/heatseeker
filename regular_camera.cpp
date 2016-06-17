#include "regular_camera.h"
#include <opencv/highgui.h>
#include <iostream>
#include <raspicam/raspicam_cv.h>


RegularCamera::RegularCamera(int width, int height) : webCam(0) {

	if (piCam.open()) {
		webCam.set(CV_CAP_PROP_FRAME_WIDTH, width);
		webCam.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	} else {
		std::cout << "Can not open the raspberry camera, looking for webcam." << std::endl;
		if (!webCam.isOpened()) {
			std::cout << "No webcam either!" << std::endl;
			return;
		}
		webCam.set(CV_CAP_PROP_FRAME_WIDTH, width);
		webCam.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	}

	startCapture();
}


RegularCamera::~RegularCamera() {
	stopCapture();
	if (webCam.isOpened()) {
		webCam.release();
	}
	if (piCam.isOpened()) {
		piCam.release();
	}
	std::cout << "Camera going of stack." << std::endl;
}

void RegularCamera::startCapture() {
	alive = true;
	cameraThread = std::thread(&RegularCamera::captureFrame, this);
}

void RegularCamera::captureFrame() {
	while (alive) {
		cv::Mat frame;
		if (piCam.isOpened()) {
			piCam.grab();
			piCam.retrieve(frame);
		} else if (webCam.isOpened()) {
			webCam >> frame;
		}

		frameBuffer.write(frame);
		//relase frame resource
		frame.release();
	}
}

void RegularCamera::stopCapture() {
	alive = false;
	cameraThread.join();
}

bool RegularCamera::hasFrame() {
	return frameBuffer.hasNext();
}

cv::Mat RegularCamera::getFrame() {
	return frameBuffer.read();
}
