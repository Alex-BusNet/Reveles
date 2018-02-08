**Reveles** (<i>verb</i>) - Latin; to discover or detect.

<ul style="list-style-type: none">
	<li><a href="#about">About</a></li>
	<li><a href="#tools-needed">Tools needed</a></li>
	<li><a href="#setup">Setup</a>
		<ul>
			<li><a href="#installing-linux">Installing Linux</a></li>
			<li><a href="#installing-git">Installing Git</a></li>
			<li><a href="#setting-up-your-working-directories">Setting Up Your Working Directories</a></li>
			<li><a href="#installing-qt-host-pc">Installing Qt</a></li>
			<li><a href="#building-and-configuring-opencv">Building and Configuring OpenCV</a>
				<ul>
					<li><a href="#setup">Setup</a></li>
					<li><a href="#build">Build</a></li>
				</ul>
			</li>
			<li><a href="#installing-wiringpi">Installing WiringPi</a></li>
			<li><a href="#installing-raspicam">Installing Raspicam</a></li>
			<li><a href="#setting-up-the-cross-compiler-tools">Setting Up the Cross Compiler Tools</a></li>
		</ul>
	</li>
	<li><a href="#building-reveles">Building Reveles</a>
		<ul>
			<li><a href="#obtaining-reveles">Obtaining Reveles</a></li>
			<li><a href="#bringing-it-all-together">Bringing it all Together</a></li>
		</ul>
	</li>
	<li><a href="#troubleshooting">Troubleshooting</a>
		<ul>
			<li><a href="#eglfs-not-found">EGLFS not found</a></li>
			<li><a href="#0x300b">Error = 0x300b</a></li>
			<li><a href="#fonts-not-found">Fonts not found</a></li>
			<li><a href="#undefined-reference-glibcxx_3421">Undefined Reference GLIBCXX_3.4.21</a></li>
		</ul>
	</li>
	<li><a href="#updating-the-reveles-repository-with-git">Updating the Reveles Repository with Git</a>
		<ul>
			<li><a href="#tips-on-how-to-avoid-merge-conflicts-with-git">Tips on How to Avoid Merge Conflicts with Git</a></li>
		</ul>
	</li>
	<li><a href="#d-bus">D-Bus</a>
		<ul>
			<li><a href="#making-changes-to-the-xml-d-bus-definition-file">Making Changes to the XML D-Bus Definition File</a>
				<li><a href="#adding-new-signals-and-methods">Adding New Signals and Methods</a></li>
				<li><a href="#d-bus-standard-type-parameters">D-Bus Standard-Type Parameters</a></li>
				<li><a href="#adding-new-signals-and-methods-with-custom-type-objects">Adding New Signals and Methods with Custom-Type Objects</a></li>
			</li>
			<li><a href="#updating-the-interface-and-adaptor">Updating the Interface and Adaptor</a></li>
		</ul>
	</li>
	<li><a href="#credits">Credits</a></li>
</ul>

## About
Reveles is designed to be an autonomous navigation robot. It will be capable of taking a coordinate provided to it and travel to that location using an onboard GPS sensor, in addition to ultrasonic and passive infrared (PIR) sensors. 

This project contains one Qt .pro project file for RevelesCore and the GUI. Additionally there are Arduino .ino and Python .py files that are used for device communication and motor control.

This walkthrough will assume that the end user is starting from a clean slate (no linux install) and has little experience using linux.

> From herein, folders will be denoted in **bold** while filenames will be denoted in _italics_.

You can skip to [Bringing it all Together](#bringing-it-all-together "Bringing it all Together") if you have Reveles already set up on a linux system and built.<br>
Required space (Host machine): 10 GB (During setup and build. This can be reduced later.)

## Tools needed 
> NOTE: We will download each of these when appropriate in the walkthrough. This is just a list of what we will be installing.

+ Any linux distro (I did this successfully on Ubuntu 16.04 LTS)
+ [Latest version of QtCreator](https://www.qt.io/download-qt-for-application-development "Qt Latest download")
+ WiringPi
+ Raspicam
+ Reveles project (obviously)
+ CMake
+ Git
+ A Raspberry Pi running Raspbian Stretch

## Setup
### Installing Linux
There are two ways to run linux 1) Virtual Machine (VM) or 2) Dual Boot. This guide will walk through a VM install.

1. Download Oracle Virtual Box [here](https://www.virtualbox.org/wiki/Downloads "Oracle VirtualBox").
2. Download a Linux distribution of your choice. I used [Ubuntu 16.04 LTS](https://www.ubuntu.com/download/desktop "Ubuntu Desktop").
> NOTE: Save the .iso file to a location you can get to easily. We will need this later.
> NOTE: Ubuntu is a pretty widely used Linux distro and is pretty friendly for beginners.

3. Once both are downloaded, install Oracle Virtual Box and then run it once the installation completes.
4. Select "New" from the toolbar and give the machine a name. Set the type as 'Linux' and set the version to Ubuntu.
> NOTE: The bit type will depend on what you downloaded. The default should be 64-bit.

5. Set how much memory you want the VM to use. This uses the memory built in to your computer so be sure to leave some for your regular OS to run. 2-3 GB should be plenty for Ubuntu.
6. You will need to create a new virtual disk for the VM to run. Give it a name and allocate at least 20 GB to the system. If you don't know what you are doing, you can leave the other settings at their defaults.

At this point you should now be back on the VirtualBox Manager screen. Select your newly created VM and scroll down the info screen to the 'Storage' window.

7. Find the Controller: IDE setting and click on "[Optical Drive] Empty".
8. Navigate to where you saved the linux distro and select it.

You are now ready to load the VM.
9. Click the 'Start' arrow in the toolbar and follow the on screen steps to installing the distro.
> NOTE: During the install, the OS may throw a warning about wiping the hard drive and formatting it. Don't worry about this; The VM acts only within the virtual disk you created in step 6. This will NOT modify any hard drive on your computer.

Once the install completes, the VM will restart. You may be prompted to remove the installation media before restarting. VitrualBox should automatically remove the .iso file. Simply press 'Enter' to continue with the reboot.
<hr>

> The rest of this guide will be working within the Linux VM we just made, and on the Raspberry Pi

### Installing Git
1. Open a terminal window and check to see if Git is already installed by entering the following into the terminal: `git`
> NOTE: Where the terminal is varies by distro. On Ubuntu it can be found by opening the search and typing 'terminal' into the text field. Some Debian based distros have the terminal accessible in the taskbar by default. You may want to lock the terminal to the taskbar for later use (right-click icon and select 'Lock to Launcher').

2. If git was not found enter the following command:
	`$ sudo apt-get install git`

3. (OPTIONAL) You can configure git for easier use with the following commands:<br>
	`$ git config --global user.email "you@example.com"`<br>
	`$ git config --global user.name "Your name"`
	
### Setting Up Your Working Directories
**OPTIONAL** Since there are a lot of parts to this project, it is recommended that you make a dedicated folder for this on your host machine. 

1. **[Host PC]** In your linux distro, you should set up a folder similar to this:
```
/ # root directory
|-home
  |-USER_NAME # the name of the account you are developing Reveles in
    |-CrossDevelopment # Here we will store everything we need for Reveles.
      |-Qt # We will install Qt creator here.
```
> NOTE: We will place other folders inside the `CrossDevelopment` directory later.

2. **[RPi]** You may also choose to create a folder to store some needed files for the Pi.
```
\ # root directory
|-home
  |-pi
    |-Reveles
      |-build # We will place the Reveles executables here
```
> NOTE: We will place additional folders in `Reveles` later.

### Installing Qt (Host PC)
All linux distros have a built-in package manager (aptitude on Debian/Ubuntu) which does include a Qt package. However this will install Qt version 5.3.2 which may not compile Reveles correctly.
Reveles requires Qt version 5.9 or newer. The latest version can be acquired [here](https://www.qt.io/download-qt-for-application-development "Qt Latest download"). 

1. **[Host PC]** The download from Qt's web site will download a .run file. To make this file run as an executable,<br>	right-click -> Properties -> Permissions -> 'Allow executing file as program'.

2. **[Host PC]** Run the installation file the same as you would on Windows.

> NOTE (1): Set the install location to the `~/CrossDevelopment/Qt` folder we made earlier.
> NOTE (2): This is simply to give us the Qt creator. We will configure the IDE later for compiling to the Pi with the proper tools.

### Building And Configuring OpenCV
#### Setup
Revles' object detection system uses the OpenCV framework. These steps will guide you through setting up OpenCV on the RPi and cross compile environment

1. **[RPi]** First we need to set up our directory to work in.<br>
```ShellSession
$ cd
$ mkdir opencv opencv/sources opencv/build opencv/contrib opencv/python
```

2. **[RPi]** Download OpenCV and OpenCV contrib modules<br>
```ShellSession
$ cd opencv/sources
$ wget https://github.com/opencv/opencv/3.4.0.zip
$ unzip 3.4.0.zip
$ cd ../contrib
$ wget https://github.com/opencv/opencv-contrib/master.zip
$ unzip master.zip
```
> At this point, our folder structure should looks like so:
> ```
> ~/Reveles/opencv
>  	    |->build
>  	    |->contrib
>  	    |  |->opencv_contrib-master
>  	    |     |->doc
>  	    |     |->modules
>  	    |     |->samples
>  	    |->python
>  	    |->sources
>     	       |->3.4.0
>        	  |->3rdparty
>        	  |->apps
>        	  |->cmake
>        	  |->CMakeFiles
>        	  |->data
>        	  |->doc
>        	  |->include
>        	  |->modules
>        	  |->platforms
>        	  |->samples
> ```

3. **[RPi]** (optional) We can make the paths easier to work with by copying the subfolders in **contrib** and **sources** up one level. You should end up with something like so:
> ```
> ~/Reveles/opencv
>  	    |->build
>  	    |->contrib
>  	    |  |->doc
>  	    |  |->modules
>  	    |  |->samples
>  	    |->python
>  	    |->sources
>     	       |->3rdparty
>     	       |->apps
>     	       |->cmake
>     	       |->CMakeFiles
>     	       |->data
>     	       |->doc
>     	       |->include
>     	       |->modules
>     	       |->platforms
>     	       |->samples
> ```

4. **[RPi]** Before we can build OpenCV, we need to make sure we have the proper dependencies.
```ShellSession
$ cd ~/Reveles/opencv/build
$ sudo apt-get install cmake build-essential pkg-config
$ sudo apt-get install libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev
$ sudo apt-get install libavcodec-dev libavformat-dev libswscal-dev libv4l-dev
$ sudo apt-get install libxvidcore-dev libx264-dev
$ sudo apt-get install libgtk2.0-dev libgtk-3-dev
$ sudo apt-get install libatlas-base-dev gfortran
$ cd ../python
$ wget https://bootstrap.pypa.io/get-pip.py
$ sudo python get-pip.py
$ sudo python3 get-pip.py
$ pip install numpy
$ cd ../build
```
> NOTE (1): While Reveles does not use Python for it's image processing, it is a good idea to install numpy because OpenCV will look for it during configuration.<br>
> NOTE (2): `python` will run the script for python 2.7, and `python3` will run the script for python 3.6 (or what ever verion of python 3 is installed). I recommend doing both since OpenCV will configure itself for both Python 2 and Python 3.

If you are using Raspbian Stretch, then the native GCC version is 6.3 (as of 1/30/2018). In order to get OpenCV to work with our cross development environment, we need GCC 4.8.

5. **[RPi]** Check the current version of GCC with `$ gcc -v`.<br>
There should be a line similar to this at the bottom of the output: `gcc version 4.8.5 (Raspbian 4.8.5-4)`<br>
> NOTE: Your output will most likely show GCC version 6. My RPi already has been adjusted to use 4.8, which is the output shown above.

If your GCC is not 4.8, we need to get it and set it as the default.
```ShellSession
$ sudo apt-get install -t stretch gcc-4.8* cpp-4.8* g++-4.8* c++-4.8* -y
$ sudo update-alternatives --config gcc
$ sudo update-alternatives --config g++
$ sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 10
$ sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20
$ sudo update-alternatives --install /usr/bin/cc cc /usr/bin/gcc 30
$ sudo update-alternatives --set cc /usr/bin/gcc
$ sudo update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++ 30
$ sudo update-alternatives --set c++ /usr/bin/g++
$ sudo update-alternatives --config gcc
$ sudo update-alternatives --config g++
```

Once again, check your GCC version with `gcc -v`. You should now see the same output as above.

#### Build
We should now be ready to build OpenCV.

1. **[RPi]** Make sure we are in the **build** directory (`~/Reveles/opencv/build`). You can check this with `$ pwd`. 

2. **[RPi]** First we need to configure OpenCV with CMake. There are a few variables we need to pass to CMake in order to get proper configuration. If you setup the directory the same as this tutorial, then you can copy the command exactly as writen. If you did not, match the folder structure, or you are using a user other than "pi", then make sure you change the directories to match your system.
```ShellSession
$ cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local -D OPENCV_EXTRA_MODULES_PATH=/home/pi/opencv/contrib/modules -D INSTALL_PYTHON_EXAMPLES=OFF -D INSTALL_C_EXAMPLES=OFF -D BUILD_EXAMPLES=OFF -D _GLIBCXX_USE_CXX11_ABI=0 -D CMAKE_C_COMPILER=/usr/bin/gcc-D CMAKE_CXX_COMPILER=/usr/bin/g++ ../sources
```
> NOTE (1): The _GLIBCXX_USE_CXX11_ABI flag is for some compatibility fixes. Newer versions of GCC work with the newer standards of C++ (C++14 for GCC 6+) which has brought changes to how some functions are defined in the C++ Standard Library (found in libstdc++11.so on Linux systems). Since we are using GCC 4.8, this flag may be unneeded, but I have included it anyways.<br>

> NOTE (2): The CMAKE_C_COMPILER and CMAKE_CXX_COMPILER flags are also unneeded if you followed all the steps when installing and configuring GCC 4.8, but once again I choose to include it anyway to make sure CMake grabbed to correct GCC version (since GCC 6 is still installed). You could also set the paths to `/usr/bin/gcc-4.8` and `/usr/bin/g++4.8` respectively if you want to be super cautious.<br>

> NOTE (3): If you wish to play with OpenCV in your free time, you can set the `INSTALL_C_EXAMPLES`, `INSTALL_PYTHON_EXAMPLES` and `BUILD_EXAMPLES` flags to `ON`, I have them off to save space.<br>

> NOTE (4): If your are running your Pi in CLI mode, then you may wish to save the CMake output to file so you can review it before building. Simply append `|& tee <filename>` to the CMake command. 

3. **[RPi]** Let CMake run, it will take a couple of minutes. Once it finishes, check the output to make sure everything was found. The output should look like this [file](https://github.com/Alex-BusNet/Reveles/CMake_Config_output.txt) if done correctly (it's long so I'm not going to put it in the README).

4. **[RPi]** Once the output is correct we need to configure the swap file before compiling. By default, the RPi allocates 100 MB by default. This is not enough to compile OpenCV and it will freeze near the end if you forget to change the swap size.
```ShellSession
$ sudo nano /etc/dphys-swapfile

// Comment out the line that says CONF_SWAPSIZE=100 and add CONF_SWAPSIZE=1024 to a line beneath it
// CTRL+X to exit nano. Press Y if it asks to save changes.

$ sudo /etc/init.d/dphys-swapfile stop
$ sudo /etc/init.d/dphys-swapfile start
```

5. **[RPi]** Now we can build OpenCV. Use the command `make -j4` to start building OpenCV. This can take about an hour, be sure to keep an eye on the RPi's temp. If you don't have a fan attached to your Pi, find a fan to blow air over it (or blow on it a couple of times when the thermometer appears; it worked for me)

6. **[RPi]** Once it has completed successfully, install OpenCV with `sudo make install`.

7. **[RPi]** After installing it, run `sudo ldconfig` to make sure all the libraries are linked properly.

8. **[RPi]** **RESET THE SIZE OF YOUR SWAP FILE!** It is highly recommended that you DO NOT leave the swap file at 1024MB. Follow step 4 above to set your swap file back to 100MB.

9. **[RPi]** (Optional) We can check OpenCV works by executing a simple python script:<br>
`$ python`
```python
>>> import cv2
>>> cv2.__version__
>>> exit
```
You should get `3.4.0` as the output.

### Installing WiringPi
In order to access the GPIO from C++, we need to download and build the WiringPi project.

2. **[RPi]** Clone the wiringPi repository to the `Reveles` folder we made earlier.
```ShellSession
$ cd ~/Reveles
$ git clone git://git.drogon.net/wiringPi
```
3. **[RPi]** Build the wiringPi library:
```ShellSession
$ cd wiringPi
$ ./build
```
> The `./build` command will automatically install the libraries and header files into your `/usr/local/` directory.

### Installing Raspicam
If you choose to use the Raspberry Pi Camera module with Reveles, then you will also need to download and build the Raspicam library.

1. **[RPi]** Before we can build Raspicam, we need to install a few dependencies:
```ShellSession
$ sudo apt-get install cmake build-essential pkg-config
```

2. **[RPI]** Clone the Raspicam library to the `Reveles` directory.
```ShellSession
$ cd ~/Reveles
$ git clone https://github.com/cedricve/raspicam
$ cd raspicam
$ mkdir build
$ cd build
$ cmake ..
```

> NOTE: Make sure CMake found the OpenCV libraries. If CMake found the libraries, you should see `CREATE OPENCV_MODULE=1` in the output.

3 **[RPi]** If everything is good with CMake, then you can go ahead and install Raspicam.
```ShellSession
$ make
$ sudo make install
$ sudo ldconfig
```

### Setting Up the Cross Compiler Tools

1.**[RPi]** Before we get started, there are a few settings that need to be configured. Open up the raspberry pi configuration and set the following:
```
Boot to	   = CLI
GPU Memory = 256 MB
SSH	   = enable
```
>Note: You may get a warning saying that the password for user 'pi' has not been changed. You can change this if you wish but it is not critical for this project.

Reboot when prompted to do so.

2.**[RPi]**  If you are running Raspbian Stretch, then you will also need to update the RPi:  
```ShellSession
$ sudo rpi-update
$ reboot
```

3.**[RPi]** Now we need to configure some dependencies for Qt.<br>
  1. Open the sources list with `sudo nano /etc/apt/sources.list` and uncomment the deb-src line (this should be about the third line of the file).<br>
  2. Save the file (CTRL+X -> y -> ENTER).<br>
  3. Update the system and get some additional libraries.
		```ShellSession
		$ sudo apt-get update		
		$ sudo apt-get build-dep qt4-x11		
		$ sudo apt-get build-dep libqt5gui5		
		$ sudo apt-get install libudev-dev libinput-dev libts-dev libxcb-xinerama0-dev libxcb-xinerama0
		```
		
4.**[RPi]** Prepare the target directory
```ShellSession
$ sudo mkdir /usr/local/qt5pi
$ sudo chown pi:pi /usr/local/qt5pi
```

5.**[Host PC]** Create the working directory
```ShellSession
$ mkdir ~/CrossDevelopment/raspi
$ cd ~/CrossDevelopment/raspi
$ git clone https://github.com/raspberrypi/tools`
```

6.**[Host PC]** Create a sysroot. This folder contains a copy of files on the pi that we need for compilation.
```ShellSession
$ mkdir sysroot sysroot/usr sysroot/opt sysroot/usr/local
$ rsync -avz pi@<IP address of RPi>:/lib sysroot
$ rsync -avz pi@<IP address of RPi>:/usr/include sysroot/usr
$ rsync -avz pi@<IP address of RPi>:/usr/lib sysroot/usr
$ rsync -avz pi@<IP address of RPi>:/opt/vc sysroot/opt
$ rsync -avz pi@<IP address of RPi>:/usr/local/include sysroot/usr/local
$ rsync -avz pi@<IP address of RPi>:/usr/local/lib sysroot/usr/local
```

7.**[Host PC]** Adjust symlinks by running the following script:
```ShellSession
$ wget https://raw.githubusercontent.com/riscv/riscv-poky/master/scripts/sysroot-relativelinks.py
$ chmod +x sysroot-relativelinks.py
$ ./sysroot-relativelinks.py sysroot
```

Now we are ready to set up Qt for Raspberry Pi
8.**[Host PC]** Get the qtbase and configure Qt with the following:
```ShellSession
$ git clone git://code.qt.io/qt/qtbase.git -b 5.9.4
$ cd qtbase
$ ./configure -release -opengl es2 -device linux-rasp-pi3-g++ -device-option CROSS_COMPILE=/home/USER_NAME/CrossDevelopment/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf- -sysroot /home/USER_NAME/CrossDevelopment/raspi/sysroot -opensource -confirm-license -make libs -prefix /usr/local/qt5pi -extprefix /home/USER_NAME/CrossDevelopment/raspi/qt5pi -hostprefix /home/USER_NAME/CrossDevelopment/raspi/qt5
$ make -jN
$ make install
```
> NOTE (1):  N is the number of parallel jobs to run (assume 1 per core) You can use this to speed up the build time. e.g. A computer with a quad-core processor would use the command `make -j3`
> 
> NOTE (2): If you fail at any point, you can reset qtbase with `git clean -dfx` while in the qtbase folder.

9.**[Host PC]** Once `make install` completes, deploy Qt to the Pi:
```ShellSession
$ cd ~/CrossDevelopment/raspi
$ rsync -avz qt5pi pi@r<IP address of Pi>:/usr/local
```

10.**[Host PC]** We can test that everything works by building and running an example. First, build an example:
```ShellSession
$ cd ~/CrossDevelopment/raspi/qtbase/examples/opengl/qopenglwidget
$ ~/CrossDevelopment/raspi/qt5/bin/qmake
$ make
$ scp qopenglwidget pi@<IP address of Pi>:/home/pi
```

11.**[RPi]** Before we can run the example, we need to configure a few things on the Pi.
```ShellSession
$ echo /usr/local/qt5pu/lib | sudo tee /etc/ld.so.conf/d/00-qt5pi.conf
$ sudo ldconfig
```

12.**[RPi]** We now need to fix some of the EGL/GLES libraries for the Pi.
```ShellSession
$ sudo mv /usr/lib/arm-linux-gnueabihf/libEGL.so.1.0.0 /usr/lib/arm-linux-gnueabihf/libEGL.so.1.0.0_backup
$ sudo mv /usr/lib/arm-linux-gnueabihf/libGLESv2.so.2.0.0 /usr/lib/arm-linux-gnueabihf/libGLESv2.so.2.0.0_backup
$ sudo ln -s /opt/vc/lib/libEGL.so /usr/lib/arm-linux-gnueabihf/libEGL.so.1.0.0
$ sudo ln -s /opt/vc/lib/libGLESv2.so /usr/lib/arm-linux-gnueabihf/libGLES.so.2.0.0
$ sudo ln -s /opt/vc/lib/libEGL.so /opt/vc/lib/libEGL.so.1
$ sudo ln -s /opt/vc/lib/libGLESv2.so /opt/vc/lib/libGLESv2.so.2
```

13.**[RPI]** We should now be able to run the example we built before.
```ShellSession
$ cd ~/CrossDevelopment/raspi/qtbase/examples/opengl/qopenglwidget
$ ./qopenglwidget
```

>NOTE: If the example does not run, see [Troubleshooting](#troubleshooting) for more help

<hr>

## Building Reveles
### Obtaining Reveles
1. For obtaining Reveles, clone the repository to your VM.
```
$ cd ~/CrossDevelopment
$ git clone https://github.com/Alex-BusNet/Reveles.git
```

Now just open _Reveles.pro_ in QtCreator.

### Bringing it all Together
1. Open _Reveles.pro_ in QtCreator and click on the "Projects" icon on the left.

2. Click on "Manage Kits"

3. Select "Qt Versions" from the tabs across the top of the window.

4. Chances are, Qt did not find the version we just built. So we will have to add it manually. Click on "Add" and then navigate to `~/CrossDevelopment/raspi/qt5/bin/`.

5. Click on _qmake_ and press "Open".

6. Next, go to the "Compilers" tab and click "Add" -> "GCC" -> "C++". Name this compiler Something easy to find like "G++-ARM".

7. Navigate to `~/CrossDevelopment/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/`

8. Select _arm-linux-gnueabihf-g++_ and press "Open"

9. Check the ABI says arm-linux-generic-elf-32bit. If it says <custom> then click the drop down and select the correct ABI.

10. Go to the "Debuggers" tab and add a new debugger. Name this something like "RPi-Debugger".

11. Go the same folder as the compiler. Select _arm-linux-gnueabihf-gdb_ and press "Open".

12. Click on "Devices" from the options menu and add a new "Generic Linux Device"

13. Give the device a name and configure the IP, username, and password for the device.
>NOTE: The remote device will need SSH enabled on the RPi, and the user being used access the pi will need ownership of the target folder for deploying the application.

14. Once the device is configured, Qt will test the connection. If the test fails, check your IP, username, password, as well as permissions on the RPi. 
>If you are using the user "pi" and are getting an "access denied" check that "pi" is not listed as "Do not allow" in the SSH config.
>Access the SSH config with `sudo nano /etc/ssh/sshd_config`.

15. Go back to "Build & Run" and select the "Kits" tab. Add a new kit and configure it like so:
```
Name		 		= Raspberry Pi
File system name 		= (leave blank)
Device type	 		= Generic Linux Device
Device		 		= (Select the Raspberry Pi device we made earlier)
Sysroot				= (browse to ~/raspi/sysroot)
Compiler	 		= C: <No Compiler> C++: <G++-ARM>
Environment	 		= (Do not change)
Debugger	 		= RPi-Debugger
Qt version	 		= (Select the version we manually added)
Qt mkspec	 		= (Leave blank)
CMake Tool	 		= (Leave blank)
CMake generator  		= (Do not change)
Additional Qbs Profile Settings = (Do not change)
```

16. Press "Apply" then "OK" the confirm all your changes.

17. On the "Projects" screen, select the Raspberry Pi Kit and select the "Run" option. Check that the "Local File Path" field is not blank.

18. Ensure that the RPi is running and then build and run Reveles.

## Troubleshooting
#### EGLFS not found.
__Error message:__ The application failed to start because it could not find or load the Qt platform plugin "eglfs" in ""<br>
__Solution:__<br>
	`$ export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/local/qt5/plugins/platforms`<br>
	`$ export QT_QPA_PLATFORM=eglfs`

#### 0x300b
__Error message:__ Error: could not draw egl surface: Error = 0x300b<br>
__Solution:__<br>
Check first that the system is trying to use `eglfs_brcm` for EGL device integration:<br>
	`$ export QT_LOGGING_RULES=qt.qpa.*=true`<br>
	`$ ./qopenglwidget`<br>
Your output should look something like: 
```
	qt.qpa.egldeviceintegration: EGL device integration plugin keys: ("eglfs_brcm", "eglfs_kms")
	qt.qpa.egldeviceintegration: EGL device integration plugin keys (sorted): ("eglfs_brcm", "eglfs_kms")
	qt.qpa.egldeviceintegration: Trying to load device EGL integration "eglfs_brcm"
	qt.qpa.egldeviceintegration: Using EGL device integration "eglfs_brcm"
	Unable to query physical screen size, defaulting to 100 dpi.
	To override, set QT_QPA_EGLFS_PHYSICAL_WIDTH and QT_QPA_EGLFS_PHYSICAL_HEIGHT (in millimeters).
	qt.qpa.input: libinput: input device 'Logitech Optical USB Mouse', /dev/input/event0 is a pointer caps = relative-motion button
	qt.qpa.input: libinput: input device 'Apple Inc. Apple Keyboard', /dev/input/event1 is a keyboard
	qt.qpa.input: libinput: input device 'Apple Inc. Apple Keyboard', /dev/input/event2 is a keyboard
	qt.qpa.input: libinput: input device 'Raspberry Pi Sense HAT Joystick', /dev/input/event3 is a keyboard
```
If the system is not using `eglfs_brcm` then try rerunning step 11 in [Setting Up the Cross Compile Tools](#setting-up-the-cross-compile-tools). If the error persists, you may need to start over.

#### Fonts not found
__Error message:__ QFontDatabase: Could not find font directory <directory>/lib/fonts.<br>
__Solution:__<br>
```ShellSession
$ sudo mkdir -p <directory>/lib/fonts>
$ sudo cp /usr/share/fonts/truetype/dejavu/* <directory>/lib/fonts>
```

#### Undefined Reference GLIBCXX_3.4.21
__Error message:__ `undefined reference to 'std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_create(unsigned int&, unsigned int)@GLIBCXX_3.4.21'*`
> *Exact function may differ

__Solution:__<br>
This error means one of a few things:<br>
1) The version of GCC used to compile OpenCV on the RPi and the version of GCC used to compile Reveles on the Host machine are not the same.<br>
2) Your libstdc++.so the host machine is using is not the same as the one used by the RPi when compiling OpenCV.<br>
This solution addresses the second option:
**[Host PC]**
```ShellSession
$ cd ~/CrossDevelopment/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf/lib
$ ls -l | grep libstdc++.so
```
> One of the files that are returned should be a symbolic link: `libstdc++.so.6 -> libstdc++.so.6.0.19` (The .0.19 is the version that comes with the cross compile tools). Make note of the numbers listed after the .so.6 in the file name.

**[RPi]**: Now we need to find the `libstdc++.so` file on the pi
```ShellSession
$ cd /usr/lib/arm-linux-gnueabihf
$ ls -l | grep libstdc++.so
```
> Once again, we should see a file that is listed as symbolic link pointing to another file. On the original RPi for Reveles this looks like: `libstdc++.so.6 -> libstdc++.so.6.0.22`. Notice how the last number on the file is different from the one on our host machine. We need this version on our host to elimanate the error.

**[Host PC]**
We can fix the linked files like so:
```ShellSession
$ cd ~/CrossDevelopment/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/arm-linux-gnueabihf/lib
$ mv libstdc++.so.6 libstdc++.so.6.bak
$ scp pi@<IP address of RPi>:/usr/lib/arm-linux-gnueabihf/libstdc++.so.6.0.22 ./
$ ln -s libstdc++.so.6.0.22 libstdc++.so.6
```
Reveles should now be able to compile.

	
## Updating the Reveles Repository with Git
The following command assume you are in the top level folder of where the repository is located on your hard drive. (For me this is /home/USER_NAME/Reveles/Reveles)

+ Adding files to staging area: <br>
  + Add all unstaged files: `$ git add .`<br>
  + Add specific file: `$ git add <filename>`<br>
  + Add entire directory: `$ git add <directory name>` <br>

+ To create a commit use:
	`$ git commit [-a] [-m <message>]`
> NOTE (1): The `-a` flag tells Git to commit everything this is both staged and unstaged. Omit the `-a` flag to commit only staged files. 
> NOTE (2): If `-m` is omitted, or no message is given, Git will prompt you to enter a commit message. If a commit does not have a message with it, it will NOT be pushed to the repository.

+ Push the commit to the repo use:
	`$ git push origin master`

+ If you wish to see what files are staged or affected by the current commit, use: `$ git status`.

+ To get the latest updates from the repo to your local working version use: `$ git pull origin master`
> NOTE: There is another command call `fetch` which will check if there are updates, but will not change the files in your working version.
 

### Tips on How to Avoid Merge Conflicts with Git
1. Before you push new changes to the repository, run the `fetch` command to check if any updates have been made since you last pulled from the repo.

2. If changes have been made since you last pulled, you can save your changes using the `stash` command.
> `Stash` will save a 'snapshot' of your local working version and store it in a different location. This will revert your working version to match the last commit you pulled.
  1. To save your changes use: `$ git stash -u`
  2. Update your local version.
  3. (OPTIONAL) To see a list of all your stored stashes, use: `$ git stash list`
> NOTE: stash@{0} is always your most recently created stash.
  4. Reapply your changes by using `$ git stash pop`.
> NOTE (1): Most of the time, you will only need to pass 0 as N in this command.

> NOTE (2): This method is not fool-proof. There can still be merge conflicts if the changes on the repo override code you changed in your working version. At this point, you will have to go through the conflicted files manually and delete the code that should be changed.

> HINT: In each conflicted file, the sections that could not be merged will look like this:
>```
>/*
> * Unaffected code.
> */
><<<<<<< HEAD
>/* 
> * Conflicted code 
> * from repository
> */
>=======
>/* 
> * Conflicted code 
> * from your changes
> */
>>>>>>> <branch name>
>/*
> * Unaffected code.
> */
>```
> From here, simply modify the code as needed by deleting the HEAD or local section, or join the two together. Once you have fixed a conflict, besure to remove the `<<<<<<< HEAD`, `=======`, and `>>>>>>> <branch name>` markers from the file before commiting files.
  5. Once all all the files are conflict free (and correct), you can push your newest changes to the repo.

## D-Bus
The Core and GUI programs communicate along the D-Bus on the service named "com.reveles.core" and through the object "/Core". The definition for the D-Bus interface and adapter is outlined in _revelesdbus.xml_ in the GUI and RPi subprojects. These two files MUST BE THE SAME in order for the two programs to communicate properly. 

### Making Changes to the XML D-Bus Definition file
#### Adding New Signals and Methods
A signal is formatted like so in the _.xml_ file:

```xml
<!-- Signal with no parameters -->
<signal name="<signal name>" />

<!-- Signal with standard-type parameters -->
<signal name="<signal name>">
	<arg name="<arg name>" type="<variable type>" direction="[in|out]">	
</signal>

<!-- Signal with custom-type parameters -->
<signal name="<signal name>">
	<annotation name="org.qtproject.QtDBus.QtTypeName.[In0|Out0]" value="<custom type name>" />
	<arg name="<arg name>" type="<variable type>" direction="[in|out]">	
</signal>
```

>NOTE (1): Methods are formatted the same as signals, except the word `signal` is replaced with `method`<br>
>NOTE (2): When adding an annotation for custom-type parameters, using `.Out0` will result in the first `arg` configured as `out` being used as the return type.<br>
>NOTE (3): To give a signal a parameter, declare the argument as `out`. If an annotation node is included in the definition of the signal, be sure to use `.In0` in the annotation name. When we run _qdbusxml2cpp_ we will get a warning about using `In0` and declaring the `arg` as `out`, just ignore these. If you fix them to make _qdbusxml2cpp_ happy, your signals will not be genereated correctly.

#### D-Bus Standard-Type Parameters:
The [D-Bus Specification](https://dbus.freedesktop.org/doc/dbus-specification.html) outlines the ASCii characters that can be used in the `type` field of the `arg` node. The specification is pretty long and contains a lot of lower-level information that most of us aren't too concerned with. So here's a table of all the type codes:

| D-Bus type | ASCII code | Qt type  |
|:----------:|:----------:|:--------:|
|BYTE        |y           |uchar     |
|BOOLEAN     |b           |bool      |
|INT16       |n           |short     |
|UINT16	     |q           |ushort    |
|INT32       |i           |int       |
|UINT32      |u           |uint      |
|INT64       |x           |qlonglong |
|UINT64      |t           |qulonglong|
|DOUBLE      |d           |double    |
|STRING      |s           |QString   |
|OBJECT\_PATH|o           |QDBusObject\_Path |
|SIGNATURE   |g           |QDBusSignature   |
|VARIANT     |v           |QDBusVariant     |
|Struct      |(\<type(s)\>)|          |
|Array	     |a\<type\>   |          |
|Dictionary  |{\<type(s)\>}|          |

>NOTE: Don't worry too much about OBJECT\_PATH and SIGNATURE types. We will hardly see them in this project.


#### Adding New Signals and Methods with Custom Type Objects
The Qt D-Bus module supports custom types but some additional code is needed.<br>
Say for example we have the following struct:

```c++
struct MyStruct {
	int foo;
	string bar;
};
```

In order to use `MyStruct` on the D-Bus, we have to tell Qt that it is a type that can be sent along the bus. To do this we add the following line:<br>
`Q_DECLARE_METATYPE(MyStruct)`. <br>
We will also need to add `#include <QMetaType>` to to our source file.<br>
If we are using a `struct`, we need to also tell Qt how to marshall and demarshall the datatype. We can do this by adding:

```c++
QDBusArgument &operator<<(QDBusArgument &argument, const MyStruct& ms)
{
    argument.beginStructure();
    argument << ms.foo;
    argument << ms.bar;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MyStruct &ms)
{
    argument.beginStructure();
    argument >> ms.foo;
    argument >> ms.bar;
    argument.endStructure();
    return argument;
}
```

Our source file should now look like so:

```c++
#include <QMetaType>
#include <QtDBus/QDBusArgument>
#include <cstring>

struct MyStruct {
	int foo;
	string bar;
};

Q_DECLARE_METATYPE(MyStruct)

QDBusArgument &operator<<(QDBusArgument &argument, const MyStruct& ms)
{
    argument.beginStructure();
    argument << ms.foo;
    argument << ms.bar;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MyStruct &ms)
{
    argument.beginStructure();
    argument >> ms.foo;
    argument >> ms.bar;
    argument.endStructure();
    return argument;
}
```

<br>
In our _.xml_ file we would then use our `MyStruct` object like so:

```xml
<signal name="mySignal">
	<annotation name="org.qtproject.QtDBus.QtTypeName.In0" value="MyStruct" />
	<arg name="ms" type="(xs)" direction="out">	
</signal>
```

### Updating the Interface and Adaptor
If changes are made to _revelesdbus.xml_ the following commands will generate the corresponding _.cpp_ and _.h_ files. The commands will NOT update anyother source file, so make sure any changes to the slots or signals are properly reflected in _revelesgui.cpp_ and _revelescore.cpp_<br>

```ShellSession
$ export PATH=/path/to/Qt/%VERSION%/gcc_64/bin:$PATH	
$ cd /path/to/reveles/RPi
$ qdbusxml2cpp -i Common/datatypes.h -c RevelesDBusAdaptor -a reveles_dbus_adaptor.h:reveles_dbus_adaptor.cpp revelesdbus.xml
$ cd ../GUI
$ qdbusxml2cpp -c RevelesDBusInterface -p reveles_dbus_interface.h:reveles_dbus_interface.cpp -i ../RPi/Common/datatypes.h revelesdbus.xml
```

## Credits
Code base for Trine University ECE Senior Design Project 2017-18.
Created by Frank Hittel, Alisha Johnson, and Alex Portoelse.<br>
All rights reserved. &copy; 2017

Walkthrough written against the following resources:
* [RaspberryPi2EGLFS](https://wiki.qt.io/RaspberryPi2EGLFS)
* [Using Custom Types with D-Bus](https://techbase.kde.org/Development/Tutorials/D-Bus/CustomTypes)
* [Raspbian Stretch: Install OpoenCV 3 + Python on your Raspberry Pi](https://www.pyimagesearch/com/2017/09/04/raspbian-stretch-install-opencv-3-python-on-your-raspberry-pi)
* [GCC 4.8 on Raspberry Pi Wheezy](https://somewideopenspace.wordpress.com/2014/02/28/gcc-4-8-on-raspberry-pi)
