***This is a non working repo at the moment. (WIP)***

**Credits**
Lots of code taken from: https://github.com/rzva/ThermalView


**Getting it up and running**
```
sudo apt-get install build-essential cmake libusb-1.0-0-dev libopencv-dev
```

Download, compiling and running.

```
git clone git@github.com:PhroZenOne/heatseeker.git
cd heatseeker
mkdir build
cd build
cmake ../
make 

./heatseeker #to start
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