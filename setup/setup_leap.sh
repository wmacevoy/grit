#!/bin/bash

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	sudo dpkg -i Leap-0.8.0-x64.deb
else
	sudo dpkg -i Leap-0.8.0-x86.deb
fi
