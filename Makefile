compile: rshell.cpp
	g++ rshell.cpp -Wall -Werror -ansi -pedantic -o rshell
run: rshell
	@./rshell
clean: rshell
	rm rshell
