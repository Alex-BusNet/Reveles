**Reveles** (<i>verb</i>) - Latin; to discover or detect.

<ul style="list-style-type: none">
	<li><a href="#about">About</a></li>
	<li><a href="#tools-needed">Tools needed</a></li>
	<li><a href="#setup">Setup</a>
		<ul>
			<li><a href="#installing-linux">Installing Linux</a></li>
			<li><a href="#installing-git">Installing Git</a></li>
			<li><a href="#installing-qt-host-pc">Installing Qt</a></li>
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
		</ul>
	</li>
	<li><a href="#updating-the-reveles-repository-with-git">Updating the Reveles Repository with Git</a>
		<ul>
			<li><a href="#tips-on-how-to-avoid-merge-conflicts-with-git">Tips on How to Avoid Merge Conflicts with Git</a></li>
		</ul>
	</li>
	<li><a href="#d-bus">D-Bus</a>
		<ul>
			<li><a href="#making-changes-to-revelesdbus.xml">Making Changes to revelesdbus.xml</a>
				<li><a href="#adding-new-signals-and-methods">Adding New Signals and Methods</a></li>
				<li><a href="#d-bus-standard-type-parameters">D-Bus Standard-Type Parameters</a></li>
				<li><a href="#adding-new-signals-and-methods-with-custom-type-parameters">Adding New Signals and Methods with Custom-Type Parameters</a></li>
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

You can skip to [Bringing it all Together](#bringing-it-all-together "Bringing it all Together") if you have Reveles already set up on a linux system and built.
Required space (Host machine): 10 GB (During setup and build. This can be reduced later.)

## Tools needed 
+ Any linux distro (I did this successfully on Ubuntu 16.04 LTS)
+ [Qt 5.9.2](https://www.qt.io/download-qt-for-application-development "Qt Latest download")
+ Reveles project (obviously)
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
	`sudo apt-get install git`

3. (OPTIONAL) You can configure git for easier use with the following commands:<br>
	`git config --global user.email "you@example.com"`<br>
	`git config --global user.name "Your name"`

### Installing Qt (Host PC)
All linux distros have a built-in package manager (aptitude on Debian/Ubuntu) which does include a Qt package. However this will install Qt version 5.3.2 which may not compile Reveles correctly.
Reveles requires Qt version 5.9 or newer. The latest version can be acquired [here](https://www.qt.io/download-qt-for-application-development "Qt Latest download"). 

1. The download from Qt's web site will download a .run file. To make this file run as an executable,<br>	right-click -> Properties -> Permissions -> 'Allow executing file as program'.

2. Run the installation file the same as you would on Windows.

> NOTE (1): Set the install location somewhere easily accessible such as **/home/USER_NAME/Qt**. This will
> NOTE (2): This is simply to give us the Qt creator. We will configure the IDE later for compiling to the Pi with the proper tools.

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
```
sudo rpi-update

reboot
```

3.**[RPi]** Now we need to configure some dependencies for Qt.<br>
  1. Open the sources list with `sudo nano /etc/apt/sources.list` and uncomment the deb-src line (this should be about the third line of the file).<br>
  2. Save the file (CTRL+X -> y -> ENTER).<br>
  3. Update the system and get some additional libraries.
		```
		sudo apt-get update
		
		sudo apt-get build-dep qt4-x11
		
		sudo apt-get build-dep libqt5gui5		
		
		sudo apt-get install libudev-dev libinput-dev libts-dev libxcb-xinerama0-dev libxcb-xinerama0
		```
		
4.**[RPi]** Prepare the target directory
```
sudo mkdir /usr/local/qt5pi

sudo chown pi:pi /usr/local/qt5pi
```

5.**[Host PC]** Create the working directory
```
mkdir ~/raspi

cd ~/raspi

git clone https://github.com/raspberrypi/tools`
```

6.**[Host PC]** Create a sysroot. This folder contains a copy of files on the pi that we need for compilation.
```
mkdir sysroot sysroot/usr sysroot/opt

rsync -avz pi@<IP address of RPi>:/lib sysroot

rsync -avz pi@<IP address of RPi>:/usr/include sysroot/usr

rsync -avz pi@<IP address of RPi>:/usr/lib sysroot/usr

rsync -avz pi@<IP address of RPi>:/opt/vc sysroot/opt
```
7.**[Host PC]** Adjust symlinks by running the following script:
```
wget https://raw.githubusercontent.com/riscv/riscv-poky/master/scripts/sysroot-relativelinks.py

chmod +x sysroot-relativelinks.py

./sysroot-relativelinks.py sysroot
```
Now we are ready to set up Qt for Raspberry Pi
8.**[Host PC]** Get the qtbase and configure Qt with the following:
```
git clone git://code.qt.io/qt/qtbase.git -b 5.9.4

cd qtbase

./configure -release -opengl es2 -device linux-rasp-pi3-g++ -device-option CROSS_COMPILE=~/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf- -sysroot ~/raspi/sysroot -opensource -confirm-license -make libs -prefix /usr/local/qt5pi -extprefix ~/raspi/qt5pi -hostprefix ~/raspi/qt5

make -jN

make install
```
> NOTE (1):  N is the number of parallel jobs to run (assume 1 per core) You can use this to speed up the build time. e.g. A computer with a quad-core processor would use the command `make -j3`
> 
>NOTE (2): If you fail at any point, you can reset qtbase with `git clean -dfx` while in the qtbase folder. 

9.**[Host PC]** Once `make install` completes, deploy Qt to the Pi:
```
cd ~/raspi

rsync -avz qt5pi pi@r<IP address of Pi>:/usr/local
```
10.**[Host PC]** We can test that everything works by building and running an example. First, build an example:
```
cd ~/raspi/qtbase/examples/opengl/qopenglwidget

~/raspi/qt5/bin/qmake

make

scp qopenglwidget pi@<IP address of Pi>:/home/pi
```

11.**[RPi]** Before we can run the example, we need to configure a few things on the Pi.
```
echo /usr/local/qt5pu/lib | sudo tee /etc/ld.so.conf/d/00-qt5pi.conf

sudo ldconfig
```
12.**[RPi]** We now need to fix some of the EGL/GLES libraries for the Pi.
```
sudo mv /usr/lib/arm-linux-gnueabihf/libEGL.so.1.0.0 /usr/lib/arm-linux-gnueabihf/libEGL.so.1.0.0_backup

sudo mv /usr/lib/arm-linux-gnueabihf/libGLESv2.so.2.0.0 /usr/lib/arm-linux-gnueabihf/libGLESv2.so.2.0.0_backup

sudo ln -s /opt/vc/lib/libEGL.so /usr/lib/arm-linux-gnueabihf/libEGL.so.1.0.0

sudo ln -s /opt/vc/lib/libGLESv2.so /usr/lib/arm-linux-gnueabihf/libGLES.so.2.0.0

sudo ln -s /opt/vc/lib/libEGL.so /opt/vc/lib/libEGL.so.1

sudo ln -s /opt/vc/lib/libGLESv2.so /opt/vc/lib/libGLESv2.so.2
```

13.We should now be able to run the example we built before.
```
cd ~/

./qopenglwidget
```

>NOTE: If the example does not run, see [Troubleshooting](#troubleshooting) for more help

<hr>

## Building Reveles
### Obtaining Reveles
1.This is the simple part. Just clone the repository to your VM.
```
git clone https://github.com/Alex-BusNet/Reveles.git
```

Now just open _Reveles.pro_ in QtCreator.

### Bringing it all Together
1.Open _Reveles.pro_ in QtCreator and click on the "Projects" icon on the left.

2.Click on "Manage Kits"

3.Select "Qt Versions" from the tabs across the top of the window.

4.Chances are, Qt did not find the version we just built. So we will have to add it manually. Click on "Add" and then navigate to `~/raspi/qt5/bin/`.

5.Click on _qmake_ and press "Open".

6.Next, go to the "Compilers" tab and click "Add" -> "GCC" -> "C++". Name this compiler Something easy to find like "G++-ARM".

7.Navigate to `~/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/`

8.Select _arm-linux-gnueabihf-g++_ and press "Open"

9.Check the ABI says arm-linux-generic-elf-32bit. If it says <custom> then click the drop down and select the correct ABI.

10.Go to the "Debuggers" tab and add a new debugger. Name this something like "RPi-Debugger".

11.Go the same folder as the compiler. Select _arm-linux-gnueabihf-gdb_ and press "Open".

12.Click on "Devices" from the options menu and add a new "Generic Linux Device"

13.Give the device a name and configure the IP, username, and password for the device.
>NOTE: The remote device will need SSH enabled on the RPi, and the user being used access the pi will need ownership of the target folder for deploying the application.

14. Once the device is configured, Qt will test the connection. If the test fails, check your IP, username, password, as well as permissions on the RPi. 
>If you are using the user "pi" and are getting an "access denied" check that "pi" is not listed as "Do not allow" in the SSH config.
>Access the SSH config with `sudo nano /etc/ssh/sshd_config`.

15.Go back to "Build & Run" and select the "Kits" tab. Add a new kit and configure it like so:
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

16.Press "Apply" then "OK" the confirm all your changes.

17.On the "Projects" screen, select the Raspberry Pi Kit and select the "Run" option. Check that the "Local File Path" field is not blank.

18. Ensure that the RPi is running and then build and run Reveles.

## Troubleshooting
#### EGLFS not found.
__Error message:__ The application failed to start because it could not find or load the Qt platform plugin "eglfs" in ""
__Solution:__<br>
	`export QT_QPA_PLATFORM_PLUGIN_PATH=/usr/local/qt5/plugins/platforms`<br>
	`export QT_QPA_PLATFORM=eglfs`

#### 0x300b
__Error message:__ Error: could not draw egl surface: Error = 0x300b
__Solution:__<br>
Check first that the system is trying to use `eglfs_brcm` for EGL device integration:<br>
	`export QT_LOGGING_RULES=qt.qpa.*=true`<br>
	`./qopenglwidget`<br>
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
`sudo mkdir -p <directory>/lib/fonts>`<br>
`sudo cp /usr/share/fonts/truetype/dejavu/* <directory>/lib/fonts>`

## Updating the Reveles Repository with Git
The following command assume you are in the top level folder of where the repository is located on your hard drive. (For me this is /home/USER_NAME/Reveles/Reveles)

+ Adding files to staging area: <br>
  + Add all unstaged files: `git add .`<br>
  + Add specific file: `git add <filename>`<br>
  + Add entire directory: `git add <directory name>` <br>

+ To create a commit use:
	`git commit [-a] [-m <message>]`
> NOTE (1): The `-a` flag tells Git to commit everything this is both staged and unstaged. Omit the `-a` flag to commit only staged files. 
> NOTE (2): If `-m` is omitted, or no message is given, Git will prompt you to enter a commit message. If a commit does not have a message with it, it will NOT be pushed to the repository.

+ Push the commit to the repo use:
	`git push origin master`

+ If you wish to see what files are staged or affected by the current commit, use: `git status`.

+ To get the latest updates from the repo to your local working version use: `git pull origin master`
> NOTE: There is another command call `fetch` which will check if there are updates, but will not change the files in your working version.
 

### Tips on How to Avoid Merge Conflicts with Git
1. Before you push new changes to the repository, run the `fetch` command to check if any updates have been made since you last pulled from the repo.

2. If changes have been made since you last pulled, you can save your changes using the `stash` command.
> `Stash` will save a 'snapshot' of your local working version and store it in a different location. This will revert your working version to match the last commit you pulled.
  1. To save your changes use: `git stash -u`
  2. Update your local version.
  3. (OPTIONAL) To see a list of all your stored stashes, use: `git stash list`
> NOTE: stash@{0} is always your most recently created stash.
  4. Reapply your changes by using `git stash pop`.
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

### Making Changes to revelesdbus.xml
#### Adding New Signals and Methods
A signal is formatted like so in the _.xml_ file:

```
// Signal with no parameters
<signal name="<signal name>" />

// Signal with standard-type parameters
<signal name="<signal name>">
	<arg name="<arg name>" type="<variable type>" direction="[in|out]">	
</signal>

// Signal with custom-type parameters
<signal name="<signal name>">
	<annotation name="org.qtproject.QtDBus.QtTypeName.[In0|Out0]" value="<custom type name>" />
	<arg name="<arg name>" type="<variable type>" direction="[in|out]">	
</signal>

```
>NOTE (1): Methods are formatted the same as signals, except the word `signal` is replaced with `method`
>NOTE (2): When adding an annotation for custom-type parameters, using `.Out0` will result in the first `arg` configured as `out` being used as the return type.
>NOTE (3): To give a signal a parameter, declare the argument as `out`. If an annotation node is included in the definition of the signal, be sure to use `.In0` in the annotation name. When we run _qdbusxml2cpp_ we will get a warning about using `In0` and declaring the `arg` as `out`, just ignore these. If you fix them to make _qdbusxml2cpp_ happy, your signals will not be genereated correctly.

#### D-Bus Standard-Type Parameters:
The [D-Bus Specification](dbus.freedesktop.org/doc/dbus-specification.html) outlines the ASCii characters that can be used in the `type` field of the `arg` node. The specification is pretty long and contains a lot of lower-level information that most of us aren't too concerned with. So here's a table of all the type codes:

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

```
struct MyStruct {
	int foo;
	string bar;
};

```

In order to use `MyStruct` on the D-Bus, we have to tell Qt that it is a type that can be sent along the bus. To do this we add the following line:<br>
`Q_DECLARE_METATYPE(MyStruct)`. <br>
We will also need to add `#include <QMetaType>` to to our source file.<br>
If we are using a `struct`, we need to also tell Qt how to marshall and demarshall the datatype. We can do this by adding:

```
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
```
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
```<br>
In our _.xml_ file we would then use our `MyStruct` object like so:
```
<signal name="mySignal">
	<annotation name="org.qtproject.QtDBus.QtTypeName.In0" value="MyStruct" />
	<arg name="ms" type="(xs)" direction="out">	
</signal>
```

### Updating the Interface and Adaptor
If changes are made to _revelesdbus.xml_ the following commands will generate the corresponding _.cpp_ and _.h_ files. The commands will NOT update anyother source file, so make sure any changes to the slots or signals are properly reflected in _revelesgui.cpp_ and _revelescore.cpp_<br>
	```
	export PATH=/path/to/Qt/%VERSION%/gcc_64/bin:$PATH
	
	cd /path/to/reveles/RPi

	qdbusxml2cpp -i Common/datatypes.h -c RevelesDBusAdaptor -a reveles_dbus_adaptor.h:reveles_dbus_adaptor.cpp revelesdbus.xml
	
	cd ../GUI

	qdbusxml2cpp -c RevelesDBusInterface -p reveles_dbus_interface.h:reveles_dbus_interface.cpp -i ../RPi/Common/datatypes.h revelesdbus.xml
	```

## Credits
Code base for Trine University ECE Senior Design Project 2017-18.
Created by Frank Hittel, Alisha Johnson, and Alex Portoelse
All rights reserved. &copy; 2017

Walkthrough written against the following resources:
* [RaspberryPi2EGLFS](https://wiki.qt.io/RaspberryPi2EGLFS)
