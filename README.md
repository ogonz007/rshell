rshell ls & cp
==============

Licensing Information: READ LICENSE
------

Project source can be downloaded from https://github.com/ogonz007/rshell.git
------

Authors & Contributor List
------
Oscar Gonzalez

File List
------
~~~~
.:

Makefile

LICENSE

README.md

./src

./tests
~~~~
/src:

rshell.cpp
ls.cpp
cp.cpp
~~~~
/tests:

exec.script
ls.script
cp.script
io_redir.script

How to run file
---------------

Once you have clones the directory to your local machine, follow the directions below:
	For rshell:
		cd rshell
		make
		bin/rshell
	For ls:
		cd rshell
		make
		bin/ls
	For cp:
		cd rshell
		make
		bin/cp

Rshell Program Overview:
	This program was written to implement a very simple version of a command shell called rshell.
	Rshell can support simple command inputs similar to those used in BASH command shell.
	It makes commands call through the use of the execvp function and uses a built in exit command
	to shutdown the program. This shell can also handle I/O redirection.

Rshell Bugs:
	1. cd will give an error
	2. >> and piping are not currently working correctly 


ls Program Overview:
	This program was written to implement the ls funtion found in unix and linux BASH command shells.
	As this is a simple implementation of ls, it can only support -a, -l, and -R flags.

ls Bugs:
	1. ls output is not properly formatted (ie. non-alphabetical, all output in one column)
	2. -R flag is not working


cp Program OverView:
	This program was meant to implement a simple version of the cp function in unix and linux BASH.
	This version does copying of files in three different ways if a flag is passed in but chooses
	the most efficent way if no flag is passed in.

cp Bugs:
	1. If no output is added, but a flags is passed for using a method of copying,
		the program interprets the flag name as the output file. 
	2. The method flag must be added in the thrid position. Will not work if in any other position other than last
