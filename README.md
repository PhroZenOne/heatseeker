This is the software for the Bad Horse Cavalry Heastseeker. The goal is a cheap but quick way to spot the enemy in the forest with an ir-camera. 

The hardware we are running this on is:

* Seek Thermal compaxt XR ir-camera: http://www.thermal.com/products/compactxr/
* Raspberry Pi B+
* Raspberry Pi 7 inch screen v1.1
* Standard webcam found in a drawer

**Getting it up and running**

The build is running with Lite version of rasbian: 2016-05-10-raspbian-jessie-lite.iso found at  https://www.raspberrypi.org/downloads/raspbian/

```
sudo apt-get update && sudo apt-get install git cmake libopencv-dev libusb-1.0-0-dev
```

Why the dependencies?
* git: for cloning this repo
* cmake: for easier build process (auto-generate make files).
* libusb-1.0-0-dev: for connecting to the Seek Thermal camera
* libopencv-dev: for connecting to the regular webcam.

It also uses raspicam for connecting to the raspberry cam.

Download and install instructions from http://www.uco.es/investiga/grupos/ava/node/40


**Download, compiling and running.**

```
git clone git://github.com/PhroZenOne/heatseeker.git
cd heatseeker

# time to build the heatseeker

mkdir build
cd build
cmake ../
make 

#to start
sudo ./heatseeker 
```

You need to run sudo on heatseeker as the camera by default is not mounted with correct user rights.
We will also use superuser rights to adjust stuff like screen brightness and power.

*** TODO *** 

* Figure out how to remove terminal overlay.
* Threading for both cams for performance boost.
* Possible to use build in hw for better camera performance?
* Better fragement shader algoritm.
* Manual camera adjustment (preferably touch control)
* Save recording system (with sound)
* Autostart and shutdown control
* Keyboard input for shutdown and blending of ir adjustment.

*** Wishlist ***

* Move frame correction system to shader, possible? Probably not worth it as it will be in background thread anyhow.
* Atomatic camera adjustment (give it three bright points with lighters and let it adjust)
* Manual ir-camera calibration (point it to a wall and let that frame be a base)

**Credits**
Lots of code taken from: https://github.com/rzva/ThermalView
