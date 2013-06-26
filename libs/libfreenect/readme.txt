sudo apt-get install cmake freeglut3 freeglut3-dev binutils-gold libxmu-dev libxi-dev libusb-1.0
/bin/rm -rf build
mkdir build
cd build
cmake ..
make
sudo make install

