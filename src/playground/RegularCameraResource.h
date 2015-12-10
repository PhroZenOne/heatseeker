#ifndef REGULARCAMERARESOURCE_H
#define REGULARCAMERARESOURCE_H
#include <opencv/highgui.h>

class RegularCameraResource
{
    public:
        RegularCameraResource(int width, int height);
        virtual ~RegularCameraResource();

        IplImage* getFrame();

    protected:
    private:
    CvCapture* capture;
};

#endif // REGULARCAMERARESOURCE_H
