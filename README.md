This is the software for the Bad Horse Cavalry Heastseeker. The goal is a cheap but quick way to spot the enemy in the forest with an ir-camera. 

The hardware we are running this on is:

* Seek Thermal compaxt XR ir-camera: http://www.thermal.com/products/compactxr/
* Raspberry Pi B+
* Raspberry Pi 7 inch screen v1.1
* Raspberry Pi camera

A bunch of switches to control screen brightness and other stuff via GPIO.

**Getting it up and running**

The build is running with Lite version of rasbian: 2016-05-10-raspbian-jessie-lite.iso found at  https://www.raspberrypi.org/downloads/raspbian/

```
sudo apt-get update && sudo apt-get install git cmake libopencv-dev libusb-1.0-0-dev fib
```

Why the dependencies?
* git: for cloning this repo
* cmake: for easier build process (auto-generate make files).
* libusb-1.0-0-dev: for connecting to the Seek Thermal camera
* libopencv-dev: for connecting to a regular webcam (in case the raspberry camera fails for some reason).
* fib for changing splash screen.

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

*** Other changes ***

Disable screen blanking in console (screensaver)

edit /etc/kbd/config

BLANK_TIME=0
BLANK_DPMS=off
POWERDOWN_TIME=0

Change default user to root as we need to run everything as superuser anyway:
Edit /etc/systemd/system/autologin@.service and change autologin from pi to root.

*** TODO *** 

* Figure out how to remove terminal overlay.
* Better fragement shader algoritm.
* Manual camera adjustment (preferably touch control)
* Save recording system (with sound)
* Autostart and shutdown control
* GPIO input for switching

*** Wishlist ***

* Atomatic camera adjustment (give it three bright points with lighters and let it adjust)
* Manual ir-camera calibration (point it to a wall and let that frame be a base)

**Credits**
Thermal camera code
Lots of code taken from: https://github.com/rzva/ThermalView
Some graphics
http://robotblogging.blogspot.se/2013/10/an-efficient-and-simple-c-api-for.html
Splashscreen
http://www.edv-huber.com/index.php/problemloesungen/15-custom-splash-screen-for-raspberry-pi-raspbian
