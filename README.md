***This is a non working repo at the moment. (WIP)***

**Credits**
Lots of code taken from: https://github.com/rzva/ThermalView


**Getting it up and running**

My build is running with Lite version of rasbian: 2016-05-10-raspbian-jessie-lite.iso found at  https://www.raspberrypi.org/downloads/raspbian/

```
sudo apt-get update && sudo apt-get install git cmake libopencv-dev libusb-1.0-0-dev
```

Why the dependencies?
* git: for cloning this repo
* cmake: for easier build process (auto-generate make files).
* libusb-1.0-0-dev: for connecting to the Seek Thermal camera
* libopencv-dev: for connecting to the regular webcam.


**Download, compiling and running.**

```
git clone git://github.com/PhroZenOne/heatseeker.git
cd heatseeker
mkdir build
cd build
cmake ../
make 

sudo ./heatseeker
```

You need to run sudo on heatseeker as the camera by default is not mounted with correct user rights.

*** TODO *** 

* Figure out how to remove terminal
* Threading for both cams for performance boost.
* Possible to use build in hw for better camera performance?
* Better fragement shader algoritm.
* Manual camera adjustment (preferably touch control)
* Save recording system (with sound)

*** Wishlist ***

* Move frame correction system to shader, possible?
* Atomatic camera adjustment (give it three bright points with lighters and let it adjust)
* Keyboard input for shutdown and blending of ir adjustment.
