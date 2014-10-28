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


int main()
{
	char delims[] = {';','|','&'};
	
		while(1)
		{
			
				printPrompt();
				string input = getCmdLine();

				///--- parse user input ---///
				char* tmp_cstr = new char[input.size()+1];
				char* saveCstr;
				for(unsigned int i = 0; i < (input.size()+1); i++)
				{
					tmp_cstr[i] = '\0';
				}
				
				strcpy(tmp_cstr,input.c_str());
				char* cstr = strtok_r(tmp_cstr,delims,&saveCstr);

				while(cstr != '\0')
				{
					if((strcmp(cstr,"exit") == 0) || strcmp(cstr," exit") == 0)
						{
							exit(0);
						}
				
					pid_t pid = fork();
					if(pid == -1)
					{
						perror("Error with fork().");
						exit(1);
					}
					else if(pid == 0)
					{
						// child process

						char** argv = new char*[input.size()+1];
							
						for(unsigned int i = 0; i < (input.size()+1); i++)
						{
							argv[i] = '\0';
						}
						
						argv[0] = strtok(cstr," ");
				
						unsigned int j = 1;
					
						while((j < (input.size()+1)))
						{
							argv[j] = strtok('\0'," ");
							j++;
						}
						
						///--- checking for commenting using # sign ---///
						/*unsigned int k = 0;
						while(k < (input.size()+1))
						{
							if(*argv[k] == '#')
								*argv[k] = '\0';

								//return 0;
							k++;
						}*/

						if((*argv[0] == '#'))
						{
							return 0;//break;
						}

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
						int err = wait('\0');
						if(err == -1)
						{
							perror("error with wait in parent process\n");
							exit(1);
						}
					}
					cstr = strtok_r('\0',delims,&saveCstr);
				}
				delete []tmp_cstr;
	}

	return 0;
}
