#include "regular_camera.h"
#include <opencv/highgui.h>
#include <iostream>
#include <raspicam/raspicam_cv.h>


RegularCamera::RegularCamera(int width, int height) {

	if (piCame.open()) {
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

RegularCamera::startCapture() {
	alive = true;
	frame_queue  = new concurrent_queue<Mat>;
	cameraThread = new thread(&RegularCamera::captureFrame);

}

RegularCamera::captureFrame() {
	while (alive) {
		Mat frame;
		if (piCam.isOpened()) {
			piCam >> frame;
		} else if (webCam) {
			webCam >> frame;
		}

		frameBuffer.write(frame);
		//relase frame resource
		frame.release();
	}
}

RegularCamera::stopCapture() {
	alive = false;
	cameraThread.join();
	delete frame_queue;
	delete cameraThread;
	if (webCam.isOpened()) {
		webCam.release();
	}
	if (piCam.isOpened()) {
		piCam.release();
	}
}

RegularCamera::~RegularCamera() {
	std::cout << "Camera going of stack." << std::endl;
	stopCapture();
}


cv::Mat RegularCamera::getFrame() {
	return frameBuffer.read();
}
