#include "regular_camera.h"
#include <opencv/highgui.h>
#include <iostream>


RegularCamera::RegularCamera(int width, int height) : width(width), height(height), webCam(0) {


	piCam = StartCamera(width, height, 30 , 1, true);

	if (piCam == NULL) {
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
	if (piCam != NULL) {
		StopCamera();
	}
	std::cout << "Camera going of stack." << std::endl;
}

void RegularCamera::startCapture() {
	alive = true;
	cameraThread = std::thread(&RegularCamera::captureFrame, this);
}

void RegularCamera::captureFrame() {
	while (alive) {
		if (piCam != NULL) {
			const void* frame_data; int frame_sz;
			if (piCam->BeginReadFrame(0, frame_data, frame_sz)) {
				char imageData[frame_sz];
				memcpy(&imageData[0], frame_data, frame_sz);

				cv::Mat frame(cv::Size(width, height), CV_8UC1, &imageData, cv::Mat::AUTO_STEP);

				if (frame.data == NULL) {
					std::cout << "Missing frame data." << std::endl;
				} else {
					frameBuffer.write(frame);
					//relase frame resource
				}
				frame.release();
				piCam->EndReadFrame(0);
			}
		} else if (webCam.isOpened()) {
			cv::Mat frame;
			webCam >> frame;
			if (frame.data == NULL) {
				std::cout << "Missing frame data." << std::endl;
			} else {
				frameBuffer.write(frame);
				//relase frame resource
			}
			frame.release();
		}

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
