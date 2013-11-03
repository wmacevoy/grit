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

    def requestZMQ(self,widget):
        context = zmq.Context()
        port = '9001'

        req = context.socket(zmq.REQ)
        req.connect("tcp://192.168.2.101: %s" % (port))
        
        for i in range(0,2):
            message = ctypes.c_uint32(1)
            req.send(message)
            receive_msg = req.recv()
            print map(ord,receive_msg)
            
    def getInt(x):
        return x

    def sensor(self,widget):
        context = zmq.Context()
        port = '5506'
        
        sub = context.socket(zmq.SUB)
        sub.setsockopt(zmq.SUBSCRIBE, '')
        sub.connect("tcp://192.168.2.101: %s" % (port))

        for i in range(0,1):
            print "Receiving msg..\n"
            sensors = sub.recv()
            #for j in range(0,56,4):
            newSensors = struct.unpack("<14i",sensors)#sensors[j:j+4])
            listSensors = list(newSensors)
            print listSensors

    #kills the process when window is closed
    def destroy(self,widget):
        print "Window terminated"
        gtk.main_quit()

    def temp_check(self,widget):
        new_array = pyzeromqSub.doStuff(array)
        for i in new_array:
            print i
            if num >= 80:
                self.redIcon.show()
                self.greenIcon.hide()
            else:
                self.greenIcon.show()
                self.redIcon.hide()   
        time.sleep(2)

    def win(self):
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", lambda q: gtk.main_quit())
        self.window.resize(150,730)
        self.window.set_title("Mojavaton Project")
        self.window.set_position(gtk.WIN_POS_CENTER)
        
        #buttons 1-4
        self.button1 = gtk.Button("Close")
        self.button1.connect("clicked",self.destroy)
        self.button2 = gtk.Button("Sensor")
        self.button2.connect("clicked",self.sensor)
        self.button3 = gtk.Button("Temperature")
        self.button3.connect("clicked",self.requestZMQ)
        
        #labels 1-4
        self.label1 = gtk.Label(" ")
        self.label2 = gtk.Label("This is where the temp goes")
        self.label3 = gtk.Label("This is label 3")
        self.label4 = gtk.Label(" ")
        
        self.redIcon = gtk.Image()
        self.greenIcon = gtk.Image()
        self.greenIcon.set_pixel_size(20)

        #self.greenIcon.set_from_file('/home/mojavaton/Downloads/icon_green_light.png')

        fixed = gtk.Fixed()
        fixed.put(self.button1, 123,10)
        fixed.put(self.button2, 300,10)
        fixed.put(self.button3, 10,10)
        fixed.put(self.label2, 10, 40)
        fixed.put(self.label3, 10, 80)
        fixed.put(self.greenIcon, 400, 60)
        fixed.put(self.redIcon, 400, 60)

        self.window.add(fixed)
        self.window.show_all()

    def main(self):
        gtk.main()

if __name__ == "__main__":
    base = Base()
    base.main()
