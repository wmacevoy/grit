#!/usr/bin/env/python

import ctypes
from ctypes import c_uint32, c_uint8, c_int32
import sys
import pygtk, gtk
import gobject
import zmq
import time
import struct

import threading
import time
pygtk.require('2.0')

textFile = "/home/mojavaton/Documents/src/python/readTest.txt"
servoTemp = "/home/mojavaton/trunk/servos.log"
tempFile = "/home/mojavaton/Documents/src/python/servoTemp.txt"
redLight = "/home/mojavaton/Downloads/icon_red_light.png"
greenLight = "/home/mojavaton/Downloads/icon_green_light.png"

class Base:
    #Request temp from ZMQ_REQ
    def temp_REQ(self):
        context = zmq.Context()
        port = '9001'

        req = context.socket(zmq.REQ)
        req.connect("tcp://192.168.2.101: %s" % (port))
        
        temp = []
        for i in range(0,2):
            message = ctypes.c_uint32(1)
            req.send(message)
            receive_msg = req.recv()
            temp.append(map(ord,receive_msg))
        return temp

    #Subscribe sensor from ZMQ_PUB
    def sensor_SUB(self):
        context = zmq.Context()
        port = '5506'
        
        sub = context.socket(zmq.SUB)
        sub.setsockopt(zmq.SUBSCRIBE, '')
        sub.connect("tcp://192.168.2.101: %s" % (port))

        temp = []
        for i in range(0,1):
            print "Receiving msg..\n"
            sensors = sub.recv()
            #for j in range(0,56,4):
            new_Sensors = struct.unpack("<14i",sensors)#sensors[j:j+4])
            list_Sensors = list(new_Sensors)
        return list_Sensors

    def temp_Check():
        temp = temp_REQ()
        return temp

    #kills the process when window is closed
    def destroy(self,widget):
        print "Window terminated"
        gtk.main_quit()

    def press_check1(self):
        leg_dict = {}
        pressures = []

        temps = self.sensor_SUB()
        print temps
        for i in range(1,5):
            leg_dict[i] = temps[-5+i]
            pressures.append(i)
        print leg_dict
        for pressure in pressures:
            sev = 0
            if int(leg_dict[pressure]) < 800:
                sev = 0
            elif int(leg_dict[pressure]) < 900:
                sev = 1
            elif int(leg_dict[pressure]) < 1024:
                sev = 2
            else:
                sev = 3
            try:
                self.pressure_btn[int(pressure)].set_color(gtk.gdk.color_parse(self.colors[sev]))
            except:
                print "not defined %d" % int(pressure)
            else:
                print "defined %d" % int(pressure)

    def temp_check1(self):
        servos_dict = {}
        servos = []

        temps = self.temp_REQ()
        for item in temps:
            for (i, x) in enumerate(item):
                if i%2==0:
                    servos_dict[x] = item[i+1]
                    servos.append(x)
        print servos_dict
        for servo in servos:
            sev = 0
            if int(servos_dict[servo]) > 65:
                sev = 0
            elif int(servos_dict[servo]) > 45:
                sev = 1
            elif int(servos_dict[servo]) > 0:
                sev = 2
            else:
                sev = 3
            try:
                self.color_btn[int(servo)].set_color(gtk.gdk.color_parse(self.colors[sev]))
            except:
                print "not defined %d" % int(servo)
            else:
                print "defined %d" % int(servo)

    def press_check(self,widget):
        leg_dict = {}
        pressures = []

        temps = self.sensor_SUB()
        print temps
        for i in range(1,5):
            leg_dict[i] = temps[-5+i]
            pressures.append(i)
        print leg_dict
        for pressure in pressures:
            sev = 0
            if int(leg_dict[pressure]) < 800:
                sev = 0
            elif int(leg_dict[pressure]) < 900:
                sev = 1
            elif int(leg_dict[pressure]) < 1024:
                sev = 2
            else:
                sev = 3
            try:
                self.pressure_btn[int(pressure)].set_color(gtk.gdk.color_parse(self.colors[sev]))
            except:
                print "not defined %d" % int(pressure)
            else:
                print "defined %d" % int(pressure)
        
    def temp_check(self,widget):
        servos_dict = {}
        servos = []

        temps = self.temp_REQ()
        for item in temps:
            for (i, x) in enumerate(item):
                if i%2==0:
                    servos_dict[x] = item[i+1]
                    servos.append(x)
        print servos_dict
        for servo in servos:
            sev = 0
            if int(servos_dict[servo]) > 65:
                sev = 0
            elif int(servos_dict[servo]) > 45:
                sev = 1
            elif int(servos_dict[servo]) > 0:
                sev = 2
            else:
                sev = 3
            try:
                self.color_btn[int(servo)].set_color(gtk.gdk.color_parse(self.colors[sev]))
            except:
                print "not defined %d" % int(servo)
            else:
                print "defined %d" % int(servo)

    def focus_received(self,widget,data=None):
        self.focused=widget
        #print(widget.get_name())

    class Update(threading.Thread):
        def __init__(self, base_, delay_=2):
            threading.Thread.__init__(self)
            self.base = base_
            self.delay = delay_
        def run(self):
            i = 0
            while self.base.update_bool:
                i += 1
                print "Update %d" % i
                self.base.press_check1()
                self.base.temp_check1()
                time.sleep(self.delay)

    def quit(self,widget):
        print("QUIT")
        self.update_bool = False
        self.update.join()
        gtk.main_quit()

    def __init__(self):
        self.update_bool = True
        builder = gtk.Builder()
        builder.add_from_file("main.xml") 
        self.servos_dict = {}
        self.colors = ['#FF0000','#F9FF00', '#0CFF00', '#000000']

        self.window = builder.get_object("winStatus")
        self.btnTemps = builder.get_object("btnTemps")
        self.btnPressures = builder.get_object("btnPressures")
        self.back = builder.get_object("picBack")
        #self.back.lower()
        #self.parts = builder.get_object("viewParts")
        #self.data = builder.get_object("viewData")
        self.color_btn = [0]*200
        for i in range(0,200):
            self.color_btn[i] = builder.get_object("sig"+str(i))
        self.pressure_btn = [0]*5
        for i in range(1,5):
            self.pressure_btn[i] = builder.get_object("sigL"+str(i))
       # self. = builder.get_object("entry1")
        #self.entry2 = builder.get_object("entry2")
        #self.label1 = builder.get_object("label1")
        #self.about  = builder.get_object("aboutdialog1")
        
        builder.connect_signals(self)
        self.btnTemps.connect("focus-in-event", self.focus_received)
        self.btnPressures.connect("focus-in-event", self.focus_received)
        #self.parts.connect("focus-in-event", self.focus_received)
      #  self.data.connect("focus-in-event", self.focus_received)
        #self.entry2.connect("focus-in-event", self.focus_received)
        self.focused=self.btnTemps
        self.update = self.Update(self,2)
        self.update.start()

if __name__ == "__main__":
    base = Base()
    base.window.show()
    gobject.threads_init()
    gtk.gdk.threads_init()
    gtk.gdk.threads_enter()
    gtk.main()
    #update.join()
    gtk.gdk.threads_leave()
    #update.join()
    
