#ifndef REGULARCAMERARESOURCE_H
#define REGULARCAMERARESOURCE_H
#include "cv.h"
#include "highgui.h"
#include <raspicam/raspicam_cv.h>
#include "concurrent_buffer.h"

class RegularCamera {
public:
	RegularCamera(int width, int height);
	virtual ~RegularCamera();
	cv::Mat getFrame();

protected:
private:
	void startCapture();
	void stopCapture();
	void captureFrame();
	ConcurrentBuffer<cv::Mat> frameBuffer;
	std::thread cameraThread;
	cv::VideoCapture webCam;
	raspicam::RaspiCam_Cv piCam;
	bool alive;
};

#endif // REGULARCAMERARESOURCE_H
