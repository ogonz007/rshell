compiler = g++
flags = -Wall -Werror -ansi -pedantic


all: rshell ls 
rshell:
	@$(compiler) $(flags) rshell.cpp -o rshell
	@$(compiler) $(flags) ls.cpp -o ls
	@$(compiler) $(flags) cp.cpp -o cp
	@mkdir bin
	@cp rshell bin
	@cp ls bin
	@cp cp bin
	@rm rshell
	@rm ls 
	@rm cp
ls:
	@$(compiler) $(flags) ls.cpp -o ls
	@cp ls bin

run: clean rshell
	@bin/rshell
	
clean: 
	@rm -rf bin
