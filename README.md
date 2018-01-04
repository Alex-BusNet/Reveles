**Reveles** (<i>verb</i>) - Latin; to discover or detect.

<ul>
	<li><a href="#about">About</a></li>
	<li><a href="#tools-needed">Tools needed</a></li>
	<li><a href="#setup">Setup</a>
		<ul>
			<li><a href="#installing-linux">Installing Linux</a></li>
			<li><a href="#installing-git">Installing Git</a></li>
			<li><a href="#installing-qt">Installing Qt</a></li>
		</ul>
	</li>
	<li><a href="#building-reveles">Building Reveles</a>
		<ul>
			<li><a href="#obtaining-reveles">Obtaining Reveles</a></li>
			<li><a href="#obtaining-linuxdeployqt">Obtaining linuxdeployqt</a></li>
			<li><a href="#bringing-it-all-together">Bringing it all Together</a></li>
		</ul>
	</li>
	<li><a href="#updating-the-reveles-repository-with-git">Updating the Reveles Repository with Git</a>
		<ul>
			<li><a href="#tips-on-how-to-avoid-merge-conflicts-with-git">Tips on How to Avoid Merge Conflicts with Git</a></li>
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

## Tools needed
* Any linux distro (I did this successfully on Ubuntu 16.04 LTS)
* [Qt 5.9.2](https://www.qt.io/download-qt-for-application-development "Qt Latest download")
* qmake (comes with Qt) 
* linuxdeployqt
* Reveles project (obviously)
* Git

## Setup
**UNDER CONSTRUCTION**<br>
The following instructions are no longer valid

### Installing Linux
**UNDER CONSTRUCTION**

### Installing Git
**UNDER CONSTRUCTION**

### Installing Qt
Reveles requires Qt version 5.7.2 or newer. The latest version can be acquired [here](https://www.qt.io/download-qt-for-application-development "Qt Latest download"). 

## Building Reveles
**UNDER CONSTRUCTION**

## Updating the Reveles Repository with Git
**THESE INSTRUCTIONS ARE STILL MOSTLY VALID. JUST NOTE THE REPOSITORY PATH GIVEN IS FOR UNIX-BASED SYSTEMS. THE COMMANDS WORK THE SAM E ON WINDOWS-BASED MACHINES**
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


## Credits
Code base for Trine University ECE Senior Design Project 2017-18.
Created by Frank Hittel, Alisha Johnson, and Alex Portoelse
All rights reserved. &copy; 2017
