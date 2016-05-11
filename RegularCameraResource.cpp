#include "RegularCameraResource.h"
#include <opencv/highgui.h>
#include <iostream>
RegularCameraResource::RegularCameraResource(int width, int height) {
	capture = cvCaptureFromCAM(-1);
	if (!capture) {
		std::cout << "Can not open the camera!" << std::endl;
		return;
	}

	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, width);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, height);

}

RegularCameraResource::~RegularCameraResource() {
	std::cout << "Releaseing the camera." << std::endl;
	cvReleaseCapture(&capture);
}


IplImage* RegularCameraResource::getFrame() {
	return cvQueryFrame(capture);
}
