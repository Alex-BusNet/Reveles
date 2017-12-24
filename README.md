Reveles (<i>verb</i>) - Latin; to discover or detect.

# About
Reveles is designed to be an autonomous navigation robot. It will be capable of taking a coordinate provided to it and travel to that location using an onboard GPS sensor, in addition to ultrasonic and passive infrared (PIR) sensors. 

This project contains both a CLion .idea and Qt .pro project file for RevelesCore, and a Qt .pro project file for the GUI. Additionally there are Arduino .ino and Python .py files that are used for device communication and motor control.

This file starts with assuming that the end user is starting from a clean slate (no linux install) and has little experience using linux.

You can skip to [Bringing it all Together](Bringing it all Together) if you have Reveles already set up and built.

## Tools needed: 
*Any linux distro (I did this successfully on Ubuntu 16.04 LTS)
*Qt 5.9.2: https://www.qt.io
*qmake (comes with Qt) 
*linuxdeployqt
*Reveles project (obviously)
*Git

## Setup
### Installing Linux

There are two ways to run linux 1) Virtual Machine (VM) 2) Dual Boot. This guide will walk through a VM install.

1. Download Oracle Virtual Box [here](https://www.virtualbox.org/wiki/Downloads).
2. Download a Linux distribution of your choice. I used [Ubuntu 16.04 LTS](https://www.ubuntu.com/download/desktop).
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

### Installing Git

1. Open a terminal window and check to see if Git is already installed by entering the following into the terminal:
	`git'
> NOTE: Where the terminal is varies by distro. On Ubuntu it can be found by opening the search and typing 'terminal'
> into the text field. Some Debian based distros have the terminal accessible in the taskbar by default. You may want 
> to lock the terminal to the taskbar for later use (right-click icon and select 'Lock to Launcher').

2. If git was not found enter the following command:
	`sudo apt-get install git`

3. (OPTIONAL) You can configure git for easier use with the following commands:
	`git config --global user.email "you@example.com"`
	`git config --global user.name "Your name"`
 
### Installing Qt

All linux distros have a built-in package manager (aptitude on Debian/Ubuntu) which does include a Qt package. However this will install version 5.3.2 which may not compile Reveles correctly. 
Reveles requires at least Qt version 5.7.2 which can be acquired [here](https://www.qt.io/downloads).  

1. The download from Qt's web site will download a .run file. To make this file run as an executable, right-click -> Properties -> Permissions -> 'Allow executing file as program'.
2. Run the installation file the same as you would on windows.
> NOTE: Set the install location somewhere easily accessible such as '/home/<USER NAME>/Qt'. This will make some setup for linuxdeployqt a bit easier for us later.

### Obtaining Reveles
1. Create a folder you can easily find to store the Reveles project (I used '/home/<USER NAME>/Reveles').
2. Inside this folder, open a terminal window (right-click in the window and select 'Open in Terminal')
3. Enter the following command into the terminal to get the Reveles project
	`git clone https://github.com/Alex-BusNet/Reveles.git`

### Obtaining linuxdeployqt
1. Create another folder for the linuxdeployqt project (I used '/home/<USER NAME>/Documents/linuxdeployqt').
2. Download the latest version of linuxdeployqt [here](https://github.com/probonopd/linuxdeployqt/releases).
> NOTE: Be sure to download the .AppImage file and save it to the folder we created in step 1.

## Bringing it all together

1. In QtCreator, open the Reveles.pro file. This will load the entire project.
> If it isn't already, you may want to pin QtCreator to the taskbar for later.

2. Set the build target to 'Release' and build the project.

In the following steps, we will be entering a lot of path names. By default the build folders from Qt have long names; You can change to names of the build folders in the project settings in Qt to something shorter, OR you can copy the 'Reveles' application file and the 'Assets' folder (this folder may still be located with the source files under '/Reveles/RPi/GUI/') to a new empty folder. Either way will work, but for testing purposes later, it may be a better idea to copy the files to a new folder.

3. Open a terminal window where the 'Reveles' application file is. (If you moved it to a new folder, open the terminal there).

Before we can deploy the application we need to set some path variables.

4. If you changed the install location of Qt, then the system will have no idea where the qmake executable is. To fix this we need to enter the following command:
	`export PATH=<PATH TO Qt>/<Qt VERSION>/gcc_64/bin:$PATH`
> Even if you didn't change the install directory, I am not 100% certain the system will know where qmake is, so you may wish to run this command anyways.

5. Test that the system can find qmake by running the command: `qmake -v` in the terminal. Also check the displayed path is correct.
6. (OPTIONAL) If you want to make things a tad easier on yourself. You can repeat the same command for linuxdeployqt. For me this looked like:
	`export PATH=/home/<USER NAME>/Documents/linuxdeployqt:$PATH`

We should now be ready to deploy Reveles

7. In the terminal make sure we are operating in the folder where the _Reveles_ application file is located. This is usually indicated by blue text preceding the cursor. If we are not in the correct folder, we can get there by opening a new window as done previously with the `cd` command. 
> NOTE (1): in Linux, `~` denotes the user's home path ('/home/<USER NAME>').
> NOTE (2): cd means 'Change Directory' to back out of the current folder one level, use the command `cd ..`. You can also jump directly to a folder by 
>	    typing in `cd <PATH>`.

8. To deploy the application run the following command:
	`linuxdeployqt.AppImage Reveles -appimage`
>NOTE: If you skipped step 6, then you will need to give the full path to _linuxdeployqt.AppImage_ in order for this step to work.

If all the path variables are set correctly then the folder where the _Reveles_ application file is should begin to populate with all the dependencies needed to run the application. If done correctly there should be a file called 'Reveles-x86_64.AppImage' located somewhere in your user folder (most likely at '/home/<USER NAME>'. I can't seem to find this file even though the output says it was generated.)

Whether you have the .AppImage file or just the folder we deployed from, you should be able to run Reveles on any linux system with out needing Qt installed. (Needs to be tested).

# Pushing to Reveles Repo

The following command assume you are in the top level folder of where the repository is located on your hard drive. (For me this is /home/<USER NAME>/Reveles/Reveles)

1. To create a commit use:
	`git commit -a` This will ask you to enter a message to go with the commit. (You MUST give a message or it will not commit the files).
2. Push the commit to the repo use:
	`git push origin master`

# Credits
Code base for Trine University ECE Senior Design Project 2017-18.
Created by Frank Hittel, Alisha Johnson, and Alex Portoelse
All rights reserved. &copy; 2017

