#ifndef REGULARCAMERARESOURCE_H
#define REGULARCAMERARESOURCE_H
#include <opencv/highgui.h>

class RegularCamera {
public:
	RegularCamera(int width, int height);
	virtual ~RegularCamera();

	IplImage* getFrame();

protected:
private:
	CvCapture* capture;
};

#endif // REGULARCAMERARESOURCE_H
