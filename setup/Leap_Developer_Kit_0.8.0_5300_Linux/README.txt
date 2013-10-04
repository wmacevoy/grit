Linux FAQ

Q. Which distros of Linux does Leap Motion support today?
A. Ubuntu 12.04 LTS, Ubuntu 12.10, or later

Q. How do I install the Leap Motion package?
A. Run:
     sudo dpkg --install Leap-0.8.0-x64.deb

Q. What if I'm on 32-bit Ubuntu?
A. Run:
     sudo dpkg --install Leap-0.8.0-x86.deb

Q. How do I install the 32-bit Leap Motion software on 64-bit Ubuntu?
A. If you have some reason for doing this, for Ubuntu 12.10 and older please
   prepare your system with the following command:
     sudo apt-get install ia32-libs
   For Ubuntu 13.04 and newer, the convenience alias ia32-libs has been
   removed and you must explicitly install:
     sudo apt-get install libc:i386 libX11-6:i386 libgl1:i386 libglu1:i386 \
       libxi6:i386 libxrender1:i386 libsm6:i386 libfontconfig:i386 sni-qt:i386 \
       libfreetype6:i386 libxinerama1:i386 libasound2:i386

Q. How do I uninstall?
   Run:
     sudo dpkg -r leap
   Or if you installed the 32-bit package on a 64-bit system:
     sudo dpkg -r leap:i386

Q. Which dependency packages are required?
A. Mesa OpenGL and libXxf86vm to name a couple. dpkg or your package manager
   of choice will report any missing dependencies.

A. How do I install the missing dependencies?
Q. If you have a mostly complete development environment, you might get lucky
   and not require any additional libraries. Otherwise, you can manually
   install with commands such as:
     sudo apt-get install libgl1-mesa-dri libgl1-mesa-glx
   For a package manager that can automatically install dependencies, try
   gdebi.

Q. I get a long list of warnings when installing through the Ubuntu Software
   Center GUI.
A. The GUI installer will run Lintian and report warnings such as recommending
   against placing the binaries under /usr/local/bin. You may click the option
   to proceed anyway. Alternatively if you use dpkg from the command-line it
   will not run Lintian and will not display these warnings.

Q. How can I install on Fedora?
A. For Fedora 16 and later, first download alien (a package converter written
   in Perl) in .tar.gz format from http://packages.debian.org/unstable/source/alien
   and unpack. Next, run:
     sudo alien/alien.pl --to-rpm --scripts Leap-0.8.0-x64.deb
     sudo rpm -i --nodeps leap-0.8.0-2.x86_64.rpm

Q. How do I uninstall on Fedora?
A. Run:
     rpm -ev leap-0.8.0-2-x86_64.rpm

Q. How do I start the Leap Motion pipeline?
A. Run:
     leapd
   and also:
     LeapControlPanel
   From there you can right-click the tray icon to get a drop-down menu with
   various options.

Q. Where is the rest of the Linux documentation?
A. For our initial Linux releases, most of our notes regarding Linux are in this
   README.

Q. Which window managers are supported?
A. GNOME with Unity, the default for recent releases of Ubuntu. You should
   interact with your desktop locally as it is unlikely to work through VNC or
   any other remote desktop connection.

Q. Can I use the Leap Motion Linux SDK inside a virtual machine?
A. As of today this does not work well. Oracle VirtualBox does not have fully
   functional USB emulation in 64-bit mode. Even in 32-bit, you USB
   authentication is not reliable and you may require special OpenGL drivers
   to run the Visualizer.

Q. The LeapControlPanel seems to be running but no tray icon appears.
A. For Ubuntu 12.10 and earlier, the Unity desktop environment uses a whitelist
   to avoid intrusive tray icons. Add LeapControlPanel to the whitelist with:
     gsettings set com.canonical.Unity.Panel systray-whitelist \
       "$(gsettings get com.canonical.Unity.Panel systray-whitelist | sed "s/]$/, 'LeapControlPanel']/")"
   If your system tray whitelist becomes messed up over time, you may reset it
   with:
     gsettings reset com.canonical.Unity.Panel systray-whitelist

Q. When I run LeapControlPanel I get an error about mismatching Qt versions.
A. We don't yet understand why this happens on some machines but not others.
   One possibility is that one of your Qt plugins is trying to run one of
   the system Qt libraries. To diagnose this, run strace on LeapControlPanel.
   Once you find the offending library, try temporarily moving it from
   /usr/lib/x86_64-linux-gnu/libQt* files to a different location.

Q. The log reports that no device is connected but I have the Leap Motion
   device plugged in?
A. First, attempt the basic troubleshooting steps just like on Windows or Mac
   OS X. Check your cable, do not connect through a USB hub, verify that other
   devices work through that same USB socket. If you can install Windows on
   the same system, verify that the Windows version of the Leap Motion software
   works on this system.

Q. The log still reports that no device is connected, and the problem seems
   specific to Linux.
A. The package sets up udev rules such that root access should not be required
   to run leapd, but you must ensure that you are a member of the plugdev group.
   Try:
     sudo usermod -a -G plugdev $USER
   Be sure to exit all active sessions as $USER then log in again to use the
   updated permissions.

Q. The log still reports that no device is connected!
A. There could be any number of permissions problems remaining. Try:
     sudo leapd

Q. Fails to launch the recalibration or screen locator applications from the GUI.
A. For debugging purposes you may run any of the helper applications directly
   from the command-line, e.g.
     Visualizer
     Recalibrate
     ScreenLocator

Q. leapd was working before, but reports no data now.
A. One common pitfall is running more than one instance of leapd at a time.
   Run:
     ps -A | grep leapd
   and kill off any rogue processes.

Q. Sample.java compiles, but when attempting to run it, I get an error about
   unable to load libLeapJava.so
A. Java seems to have trouble searching for native libraries without some
   guidance. Try:
     LD_LIBRARY_PATH=. java -classpath .:LeapJava.jar Sample

Q. How do I build and run Sample.cs?
   For the C# example, first install mono:
     sudo apt-get install mono-gmcs
   From there, building Sample.cs should be the same as on Mac OS X.

Q. How do I build and run Sample.cpp or Sample.py?
A. The C++ and Python examples work out of the box and the command-line syntax
   is similar to that on Mac OS X.

Q. Why isn't the Screen API working?
A. The Screen API and corresponding ScreenLocator tool are not currently
   supported on Linux. Sorry!

Q. Does OS interaction a.k.a. Leap Legacy work with <insert window manager here>?
A. At this time, OS interaction is not supported on Linux. Sorry!

Q. leapd or Visualizer crashes on exit.
A. This is a known issue and we are investigating.

Q. The sample applications sometimes hang on exit and are unresponsive to
   Ctrl+C or Ctrl+Z.
A. This is a known issue and we are investigating.

Q. How to install on another Linux distro such as CentOS?
A. At this time we are only supporting recent versions of Ubuntu. However, if
   you'd like to port the package to another distro you probably have
   sufficient tools at your disposal. To view the Debian package, unpack it
   with the 'ar' and 'tar' commands and take a look. The 'alien' package
   converter mentioned above may also come in handy.

Q. I have too many issues when trying to run the LeapControlPanel GUI. These
   involve dependencies, window manager issues, Mesa OpenGL problems, or my
   video card driver.
A. You can use leapd without the GUI.
     leapd
   Note that to change any configuration settings you will have to edit
   ~/.Leap\ Motion/config.json
   We do not have any documentation for the various settings, but you could
   figure things out by playing around on Windows.

Q. How do I build the Examples?
A. To build examples such as FingerVisualizer and MotionVisualizer, make sure
   to first install a few libraries:
     sudo apt-get install g++ libGL-dev libGLU-dev libX11-dev libXi-dev \
       libfreetype6-dev libxinerama-dev libxcursor-dev libasound-dev
   From there you should be able to find the appropriate Builds/Linux subdir
   and build with:
     make

Q. Where are the FlockingDemo and GesturesDemo?
A. The Cinder library is not supported under Linux. We would welcome someone
   from the community to port these examples to another framework.

Q. UnitySandbox has its binary, but why isn't the source code in the Linux
   package?
A. Unity 4.0 introduced support for Linux 32-bit and 64-bit build outputs.
   However, this does not mean that the Unity Editor can run under Linux. If
   you would like to modify this code, download the Leap Motion Windows SDK and
   open up UnitySandbox in the Unity 4 Editor.

Q. I get "no such file or directory" when attempting to run the prebuilt
   binaries such as FingerVisualizer on 64-bit Ubuntu.
A. We included 32-bit binaries as the least common denominator across
   32/64-bit hardware and Linux distros. If you have 64-bit Ubuntu 12.10 or older,
   you can set up with:
     sudo apt-get install ia32-libs
   or (for Ubuntu 13.04 and newer):
     sudo apt-get install libX11-6:i386 libgl1:i386 libglu1:i386 libxi6:i386 \
       libfreetype6:i386 libxinerama1:i386 libasound2:i386
   or it may be easiest to build the Examples from source code (see above "How
   do I build the Examples?").

Q. I have a question that is not answered in this README.
A. Please go to the developer portal and post in the forum or questions
   section.

Q. I am a Linux guru and can tell you how to do <insert something here>.
A. Great! Please post your suggestions on the forum.

Q. The Linux version of the Leap Motion SDK is too buggy. Why did you release
   this crap? I hate you.
A. Users in the community have strongly expressed that they would prefer an
   early releases of the Linux SDK compared to nothing at all. We hope that
   through you and other Linux experts in the community that we may quickly
   gather feedback to improve upon the Linux SDK.
