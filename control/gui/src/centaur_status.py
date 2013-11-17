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
	portSense = '5506'

	contextSensors = zmq.Context()
	subSensors = contextSensors.socket(zmq.SUB)
	subSensors.setsockopt(zmq.HWM, 1)
        subSensors.setsockopt(zmq.SUBSCRIBE, '')
        subSensors.connect("tcp://192.168.2.101:%s" % (portSense))

        print "Receiving msg..\n"
        sensors = subSensors.recv()
        new_Sensors = struct.unpack("<14i",sensors)
        list_Sensors = list(new_Sensors)

	subSensors.close()
	contextSensors.term();
        return list_Sensors

    #kills the process when window is closed
    def destroy(self,widget):
	self.reqTemp.close()
	self.contextTemp.term()
        print "Window terminated"
        gtk.main_quit()

    def press_check1(self):
        leg_dict = {}
        pressures = []

        sensors = self.sensor_SUB()
        print sensors
        for i in range(1,5):
            leg_dict[i] = sensors[-5+i]
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
                self.tooltips.set_tip(self.pressure_btn[int(pressure)], "Leg: %d | Pressure: %s" % (int(pressure),leg_dict[pressure]))
            except:
                print "not defined %d" % int(pressure)
           
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
                self.tooltips.set_tip(self.color_btn[int(servo)], "Servo: %s | Temp: %s" % (int(servo),servos_dict[servo]))
            except:
                print "not defined %d" % int(servo)
           
        self.hottest.set_label('Top is %d: %d' %  (h_servo, hottest))
        print 'Top is %d: %d' %  (h_servo, hottest)

    def press_check(self,widget):
        leg_dict = {}
        pressures = []

        sensors = self.sensor_SUB()
        print sensors
        for i in range(1,5):
            leg_dict[i] = sensors[-5+i]
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
                   
    def temp_check(self,widget):
        servos_dict = {}
        servos = []

        temps = self.temp_REQ()
        for i in range(0, (len(temps) - 1), 2):
            servos.append(temps[i])
            servos_dict[temps[i]] = temps[i+1]

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
           
    def focus_received(self,widget,data=None):
        self.focused=widget
        #print(widget.get_name())

    class Update(threading.Thread):
        def __init__(self, base_, delay_=2):
            threading.Thread.__init__(self)
            self.base = base_
            self.delay = delay_
        
        #temperature thread
        def run(self):
            i = 0
            while self.base.update_bool:
                i += 1
                print "Update %d" % i
                self.base.press_check1()
                self.base.temp_check1()
                time.sleep(self.delay)
            
    #stops thread and close window
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

	self.contextTemp = zmq.Context()
	portTemp = '9001'
        self.reqTemp = self.contextTemp.socket(zmq.REQ)
        self.reqTemp.connect("tcp://192.168.2.101:%s" % (portTemp))

        self.window = builder.get_object("winStatus")
        self.back = builder.get_object("picBack")
        self.hottest = builder.get_object("lblTop")
       
        #create tooltip object
        self.tooltips = gtk.Tooltips()
        
        #temp btn array
        self.color_btn = [0]*200
        for i in range(0,200):
            self.color_btn[i] = builder.get_object("sig"+str(i))
       
        #pressure btn array
        self.pressure_btn = [0]*5
        for i in range(1,5):
            self.pressure_btn[i] = builder.get_object("sigL"+str(i))
        
        #hottest temp servo object
        self.highTemp = gtk.Label("Hottest temp")
        fixed = gtk.Fixed()
        fixed.put(self.highTemp,200,200)
        self.window.add(fixed)

        builder.connect_signals(self)

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
    
