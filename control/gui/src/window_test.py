#!/usr/bin/env/python

import ctypes
from ctypes import c_uint32, c_uint8, c_int32
import sys
import pygtk, gtk
import gobject
import zmq
import time
import struct
pygtk.require('2.0')

textFile = "/home/mojavaton/Documents/src/python/readTest.txt"
servoTemp = "/home/mojavaton/trunk/servos.log"
tempFile = "/home/mojavaton/Documents/src/python/servoTemp.txt"
redLight = "/home/mojavaton/Downloads/icon_red_light.png"
greenLight = "/home/mojavaton/Downloads/icon_green_light.png"

class Base:
    def __init__(self):
        self.win()
        self.greenIcon.set_from_file(greenLight)
        self.greenIcon.hide()
        self.redIcon.set_from_file(redLight)
        self.redIcon.hide()
    
    #Request temp from ZMQ_REQ
    def temp_REQ():
        context = zmq.Context()
        port = '9001'

        req = context.socket(zmq.REQ)
        req.connect("tcp://192.168.2.101: %s" % (port))
        
        for i in range(0,2):
            message = ctypes.c_uint32(1)
            req.send(message)
            receive_msg = req.recv()
            temp = map(ord,receive_msg)
        return temp

    #Subscribe sensor from ZMQ_PUB
    def sensor_SUB():
        context = zmq.Context()
        port = '5506'
        
        sub = context.socket(zmq.SUB)
        sub.setsockopt(zmq.SUBSCRIBE, '')
        sub.connect("tcp://192.168.2.101: %s" % (port))

        for i in range(0,1):
            print "Receiving msg..\n"
            sensors = sub.recv()
            #for j in range(0,56,4):
            new_Sensors = struct.unpack("<14i",sensors)#sensors[j:j+4])
            list_Sensors = list(new_Sensors)
        return list_Sensors

    def temp_Check():
        temp = temp_REQ()
        print temp

    #kills the process when window is closed
    def destroy(self,widget):
        print "Window terminated"
        gtk.main_quit()
        
    #Creates the window GUI
    def win(self):
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", lambda q: gtk.main_quit())
        self.window.resize(250,400)
        self.window.set_title("Temperature & Sensors")
        self.window.set_position(gtk.WIN_POS_CENTER)
        
        #buttons 1-3
        self.button1 = gtk.Button("Close")
        self.button1.connect("clicked",self.destroy)
        self.button2 = gtk.Button("Sensor")
        self.button2.connect("clicked",self.sensor_SUB)
                
        #labels 1-4
        self.label1 = gtk.Label("Label 1")
        
        #lights
        self.redIcon = gtk.Image()
        self.greenIcon = gtk.Image()
        self.greenIcon.set_pixel_size(20)

        fixed = gtk.Fixed()
        fixed.put(self.button1, 123,10)
        fixed.put(self.button2, 300,10)
        fixed.put(self.label1, 10, 40)
        fixed.put(self.greenIcon, 400, 60)
        fixed.put(self.redIcon, 400, 60)

        self.window.add(fixed)
        self.window.show_all()

    def main(self):
        gtk.main()

if __name__ == "__main__":
   # base = Base()
   # base.main()
    temp_Check()
