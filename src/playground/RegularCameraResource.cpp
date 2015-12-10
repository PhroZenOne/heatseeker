#include "RegularCameraResource.h"
#include <opencv/highgui.h>
#include <iostream>
RegularCameraResource::RegularCameraResource(int width, int height)
{
    capture = cvCaptureFromCAM(CV_CAP_ANY);
	if (!capture){
		std::cout << "Can not open the camera!" << std::endl;
		return;
	}

	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 1920 );
    cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 1080 );

}

RegularCameraResource::~RegularCameraResource()
{
    cvReleaseCapture(&capture);
}


IplImage* RegularCameraResource::getFrame(){
    return cvQueryFrame( capture );
}
