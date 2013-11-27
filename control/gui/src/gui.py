#!/usr/bin/env python

# example helloworld.py

import pygtk
import gobject
import time
import gtk
import threading
import subprocess

import utilities
import safety

pygtk.require('2.0')
cfg = utilities.Configure()
cfg.path("../../setup");
cfg.load("config.csv");
safetyClient=safety.CreateSafetyClient(cfg.str("gui.safety.publish"),cfg.str("safety.subscribe"),int(cfg.num("safety.rate")));

running = True

class Monitored:
    def __init__(self,value=None):
        self._value=value
        self._lock = threading.RLock()

    @property
    def value(self):
        self._lock.acquire()
        ans = self._value
        self._lock.release()
        return ans

    @value.setter
    def value(self,value):
        self._lock.acquire()
        self._value = value
        self._lock.release()

class Health:
    def __init__(self,container=None):
        self.health = Monitored("unknown")
        self.box = gtk.HBox()
        self.box.show()
        self.label = gtk.Label("Ping")
        self.label.show()
        self.box.pack_start(self.label,False,False,3)
        self.text = gtk.TextView()
        self.text.show()
        self.box.pack_start(self.text,False,False,3)

        self.thread = threading.Thread(target=self.run)
        self.thread.start()

        gobject.timeout_add_seconds(1,self.update)
        if container != None:
            container.add(self.box)

    def run(self):
        global running
        while running:
            hostname = "192.168.2.101"
            try:
                output = subprocess.check_output(["/bin/ping","-w","1","-c","1",hostname],stderr=subprocess.STDOUT)
                if (not 'unreachable' in output):
                    self.health.value = "ping ok"
                else:
                    self.health.value = "ping *fail*"
            except subprocess.CalledProcessError as e:
                self.health.value = "ping *error*"

            time.sleep(1.0)
        print 'run done.'

    def update(self):
        global running
        if running:
            value = self.health.value
            print "health: " + value
            print "safe: " + safety.safe()
            self.text.get_buffer().set_text(value)
            gobject.timeout_add_seconds(1,self.update)

class GUI:
    # This is a callback function. The data arguments are ignored
    # in this example. More on callbacks below.

    def stop(self):
        global running
        running = False
    
    def delete_event(self, widget, event, data=None):
        # If you return FALSE in the "delete_event" signal handler,
        # GTK will emit the "destroy" signal. Returning TRUE means
        # you don't want the window to be destroyed.
        # This is useful for popping up 'are you sure you want to quit?'
        # type dialogs.

        # Change FALSE to TRUE and the main window will not be destroyed
        # with a "delete_event".
        return False

    def destroy(self, widget, data=None):
        self.stop()
        gtk.main_quit()

    def __init__(self):
        # create a new window
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
    
        # When the window is given the "delete_event" signal (this is given
        # by the window manager, usually by the "close" option, or on the
        # titlebar), we ask it to call the delete_event () function
        # as defined above. The data passed to the callback
        # function is NULL and is ignored in the callback function.
        self.window.connect("delete_event", self.delete_event)
    
        # Here we connect the "destroy" event to a signal handler.  
        # This event occurs when we call gtk_widget_destroy() on the window,
        # or if we return FALSE in the "delete_event" callback.
        self.window.connect("destroy", self.destroy)
    
        # Sets the border width of the window.
        self.window.set_border_width(10)

        self.health = Health(self.window)
    
        # This will cause the window to be destroyed by calling
        # gtk_widget_destroy(window) when "clicked".  Again, the destroy
        # signal could come from here, or the window manager.
        #//        self.button.connect_object("clicked", gtk.Widget.destroy, self.window)
    
        # This packs the button into the window (a GTK container).
        # self.window.add(self.button)
    
        # The final step is to display this newly created widget.
        # self.button.show()
    
        # and the window
        self.window.show()

    def main(self):
        # All PyGTK applications must have a gtk.main(). Control ends here
        # and waits for an event to occur (like a key press or mouse event).

        

#            cfg.show();

        gtk.main()

# If the program is run directly or passed as an argument to the python
# interpreter then create a HelloWorld instance and show it
if __name__ == "__main__":
    gui = GUI()
    gui.main()
