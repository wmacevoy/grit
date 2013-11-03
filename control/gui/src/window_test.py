#!/usr/bin/env/python

import pyzeromqSub
import sys
import re
import pygtk, gtk
import gobject
#import zmq
import time
from random import randint
pygtk.require('2.0')

textFile = "/home/mojavaton/Documents/src/python/readTest.txt"
servoTemp = "/home/mojavaton/trunk/servos.log"
tempFile = "/home/mojavaton/Documents/src/python/servoTemp.txt"
redLight = "/home/mojavaton/Downloads/icon_red_light.png"
greenLight = "/home/mojavaton/Downloads/icon_green_light.png"
array =  { "servos[11]":54, "servos[12]":87, "servos[13]":64, "servos[14]":75, "servos[15]":100, "servos[16]":120, "servos[17]":98, "servos[18]":80, "servos[19]":100 }

class Base:
    def __init__(self):
        self.win()
        self.greenIcon.set_from_file(greenLight)
        self.greenIcon.hide()
        self.redIcon.set_from_file(redLight)
        self.redIcon.hide()

    def requestZMQ():
        pyzeromqSub.doStuff()
        
    #read in a text file and output
    def readFile(self,widget):
        inFile = open(textFile)
        text = inFile.read()
        print text
        self.label2.set_label(str(text))
        inFile.close()

    #kills the process when window is closed
    def destroy(self,widget):
        print "Window terminated"
        gtk.main_quit()

    def temp_check(self,widget):
        #pyzeromqSub.request()
        new_array = pyzeromqSub.doStuff(array)
        #a = [59, 40, 65, 100, 200, 120, 87, 54, 110]
        #randomNum = randint(0,len(a)-1)
        #num = a[randomNum]
        for i in new_array:
            #printTemp = "Temperature: %d* " % (num)
            print i
            #self.label3.set_text(pyzeromqSub.request())
            #print printTemp
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
        self.window.resize(600,700)
        self.window.set_title("Mojavaton Project")
        self.window.set_position(gtk.WIN_POS_CENTER)
        
        #buttons 1-4
        self.button1 = gtk.Button("Close")
        self.button1.connect("clicked",self.destroy)
        self.button2 = gtk.Button("Get file")
        self.button2.connect("clicked",self.readFile)
        self.button3 = gtk.Button("Temperature")
        self.button3.connect("clicked",self.temp_check)
        self.button4 = gtk.Button("Send info")
        #self.button4.connect("clicked",self.subscriber)

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
        fixed.put(self.button1,273,10)
        fixed.put(self.button2,5,10)
        fixed.put(self.button3,70,10)
        fixed.put(self.button4,173,10)
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
