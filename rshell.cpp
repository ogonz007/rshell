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
	char* tmp = 'exit';
	if(!strcmp(argu,tmp))
		return true;
	return false;
}*/


int main()
{
	//char* argv[3] = {'\0'};
	//char** argv;
	//string input;
	//char* tmp_cstr;
	//char* cstr;
	//int cstr_count = 0;

	while(1)
	{
		printPrompt();
		string input = getCmdLine();

		///--- parse user input ---///
		char* tmp_cstr = new char[input.size()+1];
//		cout << "input size = " << input.size() << endl;	
		for(unsigned int i = 0; i < (input.size()+1); i++)
		{
			tmp_cstr[i] = '\0';
		}
		
		strcpy(tmp_cstr,input.c_str());

		//cstr = strtok(tmp_cstr,";||&&");
		//printf("%s\n",cstr);	
		while(tmp_cstr != '\0')
		{
			char* cstr = strtok(tmp_cstr,";||&&");
			
			/*char** argv = new char*[input.size()+1];
			
			for(unsigned int i = 0; i < (input.size()+1); i++)
				argv[i] = '\0';
			
			argv[0] = strtok(cstr," ");
			printf("%s\n",argv[0]);
			
			unsigned int j = 1;
			while(j < (input.size()+1) && cstr != '\0')
			{
				argv[j] = strtok(NULL," ");
				printf("%s\n",argv[j]);
				j++;
			}*/
				
		/*	if(isExit(argv[0]))
			{
				exit(1);
			}
		*/
			cout << "about to fork" << endl;
			pid_t pid = fork();
			if(pid == -1)
			{
				perror("Error with fork().");
				exit(1);
			}
			else if(pid == 0)
			{
				// child process
				printf("in child process\n");

				char** argv = new char*[input.size()+1];
					
				for(unsigned int i = 0; i < (input.size()+1); i++)
				{
					argv[i] = '\0';
				}
				
				argv[0] = strtok(cstr," ");
				printf("%s\n",argv[0]);
				
				unsigned int j = 1;
				cout << "input.size " << input.size() << endl;
				cout << "cstr " << cstr << endl;


				//while((j < (input.size()+1)) && (cstr != '\0'))
				
				while((j < (input.size()+1)) )
				{
					argv[j] = strtok(NULL," ");
					cout << argv[j] << endl;
					cout << "j = " << j << endl;
					cout << input.size() << endl;
					j++;

				}
				cout << "about to execvp" << endl;
				int r = execvp(argv[0],argv);
				if(r == -1)
				{
					perror("error with execvp");
					exit(1);
				}
				delete []argv;
			}
			else if(pid > 0)
			{
				// parent process
				int err = wait(NULL);
				if(err == -1)
				{
					perror("error with wait in parent process\n");
					exit(1);
				}
			}
			tmp_cstr = strtok('\0',";||&&");
		}
		delete []tmp_cstr;
		//cstr = strtok('\0',";||&&");

	}


	return 0;
}
