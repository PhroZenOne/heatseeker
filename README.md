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
sudo apt-get update && sudo apt-get install git cmake libopencv-dev libusb-1.0-0-dev
```

Why the dependencies?
* git: for cloning this repo
* cmake: for easier build process (auto-generate make files).
* libusb-1.0-0-dev: for connecting to the Seek Thermal camera
* libopencv-dev: for connecting to a regular webcam (in case the raspberry camera fails for some reason).

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

set

BLANK_TIME=0
BLANK_DPMS=off
POWERDOWN_TIME=0

also add the following line to /root/.bashrc for autostart and no screensaver stuff.

/usr/bin/setterm -blank 0 -powerdown 0 -powersave off
cd /root/heatseeker/build && ./heatseeker



Change default user to root as we need to run everything as superuser anyway:
Edit /etc/systemd/system/autologin@.service and change autologin from pi to root.

Added the disable_splash=1 in /boot/config.txt to get rid of rainbow start.

Added logo.nologo to /boot/cmdline.txt to get rid of rasbian logo

*** Must TODO *** 

* Camera modes
* GPIO input for switching camera modes
* Better fragement shader algoritm.
* Shutdown control
* GPIO input for switching brightness

*** Wishlist ***

* Manual camera adjustment (preferably touch control)
	Maybe this can help http://ozzmaker.com/programming-a-touchscreen-on-the-raspberry-pi/
* Atomatic camera adjustment (give it three bright points with lighters and let it adjust)
* Manual ir-camera calibration (point it to a wall and let that frame be a base)
* Save recording system (with sound)
	Maybe this can help http://robotblogging.blogspot.se/2013/10/gpu-accelerated-camera-processing-on.html


**Credits**
Thermal camera code
Lots of code taken from: https://github.com/rzva/ThermalView

Some graphics
http://robotblogging.blogspot.se/2013/10/an-efficient-and-simple-c-api-for.html


