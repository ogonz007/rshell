compiler = g++
flags = -Wall -Werror -ansi -pedantic


all: rshell 
rshell:
	@$(compiler) $(flags) rshell.cpp -o rshell
	@mkdir bin
	@cp rshell bin
run:
	@bin/rshell

clean: 
	@rm rshell
	@rm -rf bin
	@echo "THIS FOLDER IS CLEAN!!"
