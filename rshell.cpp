#include <sys/types.h>
#include <sys/wait.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <vector>

#define SEMI_CON 1.0
#define AND_CON 2.0
#define OR_CON 3.0

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
		vector<double> connectors(1);
		///--- search for connector types ---///
		for(unsigned int i = 0; i < sizeof(delims); i++)
		{
			size_t found = input.find(delims[i]);
			while((found != std::string::npos))
			{
				size_t pos = found + 1;
				if(input.at(found) == ';')
					connectors.push_back(found + (SEMI_CON/10));
				else if((input.at(found) == '&') && (input.at(found + 1) == '&'))
				{
					connectors.push_back(found + (AND_CON/10));
					pos += 1;
				}
				else if((input.at(found) == '|') && (input.at(found + 1) == '|'))
				{
					connectors.push_back(found + (OR_CON/10));
					pos += 1;
				}
				found = input.find(delims[i],pos);
			}
		}
		sort(connectors.begin(),connectors.end());
	
		///--- parse user input ---///
		char* tmp_cstr = new char[input.size()+1];
		char* saveCstr;
		for(unsigned int i = 0; i < (input.size()+1); i++)
		{
			tmp_cstr[i] = '\0';
		}
		
		strcpy(tmp_cstr,input.c_str());

		char* cstr = strtok_r(tmp_cstr,delims,&saveCstr);
		int statementPos = 0;
		bool good = 1;
		int prev_connector = 0;

		while(cstr != '\0')
		{
			int connector = (connectors.at(statementPos) * 10);
			connector = connector%10;
			if(statementPos == 0)
				good = 1;
			else if(connector == 1)
				good = 1;
			else if((connector == 2) && (prev_connector == 2) && (good == 1))
				good = 1;
			else if((connector == 2) && (prev_connector == 3) && (good == 0))
				good = 1;
			else if((connector == 3) && (prev_connector == 0) && (good == 0))
				good = 1;
			else if((connector == 3) && (prev_connector == 0) && (good == 1))
				good = 0;
			else if((connector == 3) && (prev_connector == 3) && (good == 1))
				good = 0;
			else if((connector == 3) && (prev_connector == 2) && (good == 0))
				good = 1;
			else if((connector == 3) && (prev_connector == 2) && (good == 1))
				good = 0;
			if(good == 1)
			{
				if(strcmp(cstr,"exit") == 0)
				{
					exit(0);
				}
			
				pid_t pid = fork();
				if(pid == -1)
				{
					perror("Error with fork()\n");
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
				
					while((j < (input.size()+1)) )
					{
						argv[j] = strtok(NULL," ");
						j++;
					}
					
					///--- checking for commenting using # sign ---///
					if((*argv[0] == '#'))
					{
						exit(0);;
					}
					
					int r = execvp(argv[0],argv);
					if(r == -1)
					{
						perror("error with execvp\n");
						good = 0;
						exit(1);
					}
					good = 1;
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
			}
			cstr = strtok_r('\0',delims,&saveCstr);
			prev_connector = connector;
			statementPos++;
		}
		delete []tmp_cstr;
	}

	return 0;
}
