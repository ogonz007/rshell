#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

using namespace std;

///--- prints the rshell prompt (e.g. $) ---///
void printPrompt()
{
	cout << "$ ";
	return;
}

///--- gets the user input data for command execution ---///
string getCmdLine()
{
	string str;
	getline(cin,str);
	return str;
}

///--- checks if the input command is "exit" ---///
/*bool isExit(char* argu)
{
	if(argu == 'exit')
		return true;
	return false;
}*/


int main()
{
	char* argv[3] = {'\0'};
	string input;
	char* tmp_cstr;
	char* cstr;
	//int cstr_count = 0;

	while(1)
	{
		printPrompt();
		input = getCmdLine();

		///--- parse user input ---///
		tmp_cstr = new char[input.size()+1];
		strcpy(tmp_cstr,input.c_str());

		cstr = strtok(tmp_cstr,";||&&");
		
		while(cstr != '\0')
		{
		//	printf("%s\n",cstr);
			//cstr = strtok(tmp_cstr,";||&&");
			argv[0] = strtok(cstr," ");
			argv[1] = strtok(NULL," ");
			cstr = strtok('\0',";||&&");
			
			/*if(isExit(argv[0]))
			{
				exit(1);
			}*/

			pid_t pid = fork();
			if(pid == -1)
			{
				perror("Error with fork().");
				exit(1);
			}
			else if(pid == 0)
			{
				// child process
				execvp(argv[0],argv);
			//	printf("in child process\n");
			}
			else if(pid > 0)
			{
				// parent process
				if(wait(NULL) == -1)
				{
					perror("error with wait in parent process\n");
					exit(1);
				}
			//	printf("in parent process\n");
			}
		}


	}


	return 0;
}
