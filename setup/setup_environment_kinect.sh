#!/bin/bash

echo "blacklist gspca_kinect" |sudo tee -a /etc/modprobe.d/blacklist.conf
modprobe -r gspca_kinect
modprobe -r gspca_main
cp 51-kinect.rules /etc/udev/rules.d/
/etc/init.d/udev restart

