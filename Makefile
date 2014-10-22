compiler = g++
flags = -Wall -Werror -ansi -pedantic


all: rshell 
rshell:
	@$(compiler) $(flags) rshell.cpp -o rshell
	@mkdir bin
	@cp rshell bin
run:
	@./rshell.out

clean: 
	@rm rshell
	@echo "THIS FOLDER IS CLEAN!!"
