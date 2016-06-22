#ifndef REGULARCAMERARESOURCE_H
#define REGULARCAMERARESOURCE_H
#include "cv.h"
#include "highgui.h"
#include "concurrentbuffer.h"
#include "camera.h"

class RegularCamera {
public:
	RegularCamera(int width, int height);
	virtual ~RegularCamera();
	cv::Mat getFrame();
	bool hasFrame();
	void startCapture();
	void stopCapture();

protected:
private:
	void captureFrame();
	ConcurrentBuffer<cv::Mat> frameBuffer;
	std::thread cameraThread;
	cv::VideoCapture webCam;
	CCamera * piCam;
	bool alive;
	int width;
	int height;
};

#endif // REGULARCAMERARESOURCE_H
