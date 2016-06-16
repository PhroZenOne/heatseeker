#ifndef REGULARCAMERARESOURCE_H
#define REGULARCAMERARESOURCE_H
#include "opencv2/opencv.hpp"
#include <raspicam/raspicam_cv.h>

class RegularCamera {
public:
	RegularCamera(int width, int height);
	virtual ~RegularCamera();
	ConcurrentBuffer<cv::Mat> frameBuffer;

protected:
private:
	thread * cameraThread;
	VideoCapture webCam(0);
	raspicam::RaspiCam piCam;
	boolean alive;
};

#endif // REGULARCAMERARESOURCE_H
