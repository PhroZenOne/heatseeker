***This is a non working repo at the moment. (WIP)***

**Credits**
Lots of code taken from: git@github.com:rzva/ThermalView.git


**Getting it up and running**
```
sudo apt-get install build-essential libusb-1.0-0-dev libopencv-dev libglm-dev
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

You may need to run sudo on heatseeker as the camera by default is not mounted with correct user rights.
