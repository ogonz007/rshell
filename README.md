rshell
======

Licensing Information: READ LICENSE
------

Project source can be downloaded from https://github.com/ogonz007/rshell.git
------

Authors & Contributor List
------
Oscar Gonzalez
Mike Izbicki

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
~~~~
/tests:

exec.script


How to run file
---------------

Once you have clones the directory to your local machine, follow the directions below:

	1. cd into rshell directory
	2. call make
	3. cd into bin
	4. call rshell

Program Overview:
	This program was written to implement a very simple version of a command shell called rshell.
	Rshell can support simple command inputs similar to those used in BASH command shell.
	It makes commands call through the use of the execvp function and uses a built in exit command
	to shutdown the program.

Bugs:
	1. exit some times requires for it to be types twice before the program truly exits.
	2. cd will give an error
	3. || and && operators were not implemented 
