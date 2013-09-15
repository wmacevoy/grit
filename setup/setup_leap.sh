#!/bin/bash

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	cd ../drivers/Leap_Developer_Kit_0.8.0_5300_Linux
	sudo dpkg -i Leap-0.8.0-x64.deb
else
	cd ../drivers/Leap_Developer_Kit_0.8.0_5300_Linux
	sudo dpkg -i Leap-0.8.0-x86.deb
fi
