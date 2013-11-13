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

class Base:
    #Request temp from ZMQ_REQ
    def temp_REQ(self):        
        message = ctypes.c_int32(1)
        self.reqTemp.send(message)
        receive_msg = self.reqTemp.recv()
        temps = struct.unpack("<68i", receive_msg)
        list_Temps = list(temps)

        return list_Temps

    #Subscribe sensor from ZMQ_PUB
    def sensor_SUB(self):
        print "Receiving msg..\n"
        sensors = self.subSensors.recv()
        new_Sensors = struct.unpack("<14i",sensors)
        list_Sensors = list(new_Sensors)

        return list_Sensors

    def temp_Check():
        temp = temp_REQ()
        return temp

    #kills the process when window is closed
    def destroy(self,widget):
	self.reqTemp.close()
	self.subSensors.close()
	self.contextTemp.term()
	self.contextSensors.term();
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
        for i in range(0, (len(temps) - 1), 2):
            servos.append(temps[i])
            servos_dict[temps[i]] = temps[i+1]

	print servos_dict
        hottest = 0
        h_servo = 0
        for servo in servos:
            if int(servos_dict[servo]) > hottest:
                hottest = int(servos_dict[servo])
                h_servo = servo
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
        self.hottest.set_label('Top is %d: %d' %  (h_servo, hottest))
        print  'Top is %d: %d' %  (h_servo, hottest)

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
	self.contextSensors = zmq.Context()
	self.contextTemp = zmq.Context()

	portTemp = '9001'
	portSense = '5506'

        self.reqTemp = self.contextTemp.socket(zmq.REQ)
        self.reqTemp.connect("tcp://192.168.2.101: %s" % (portTemp))
        
        self.subSensors = self.contextSensors.socket(zmq.SUB)
        self.subSensors.setsockopt(zmq.SUBSCRIBE, '')
        self.subSensors.connect("tcp://192.168.2.101: %s" % (portSense))

        self.window = builder.get_object("winStatus")
        #self.btnTemps = builder.get_object("btnTemps")
        #self.btnPressures = builder.get_object("btnPressures")
        self.back = builder.get_object("picBack")
        self.hottest = builder.get_object("lblTop")
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
        
        self.highTemp = gtk.Label("Hottest temp")
        fixed = gtk.Fixed()
        fixed.put(self.highTemp,200,200)
        self.window.add(fixed)

        builder.connect_signals(self)
        #self.btnTemps.connect("focus-in-event", self.focus_received)
        #self.btnPressures.connect("focus-in-event", self.focus_received)
        #self.parts.connect("focus-in-event", self.focus_received)
      #  self.data.connect("focus-in-event", self.focus_received)
        #self.entry2.connect("focus-in-event", self.focus_received)
        #self.focused=self.btnTemps
        self.update = self.Update(self,2)
        self.update.start()

if __name__ == "__main__":
    base = Base()
    base.window.show_all()
    gobject.threads_init()
    gtk.gdk.threads_init()
    gtk.gdk.threads_enter()
    gtk.main()
    #update.join()
    gtk.gdk.threads_leave()
    #update.join()
    
