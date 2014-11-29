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
	cout << "@ ";
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
//	int statementPos = 0;
//	bool good = 1;

	while(1)
	{
		printPrompt();
		string input = getCmdLine();
		vector<double> connectors;
		///--- search for connector types ---///
		for(unsigned int i = 0; i < sizeof(delims); i++)
		{
			size_t found = input.find(delims[i]);
			while((found != std::string::npos))
			{
				size_t pos = found + 1;
				//cout << "input.at(" << found << ") = " << input.at(found) << endl;
				if(input.at(found) == ';')
					connectors.push_back(found + (SEMI_CON/10));
				else if(input.at(found) == '&')
					connectors.push_back(found + (AND_CON/10));
				else if(input.at(found) == '|')
					connectors.push_back(found + (OR_CON/10));
				found = input.find(delims[i],pos);
			}
		}
		sort(connectors.begin(),connectors.end());
		for(unsigned int i = 0; i < connectors.size(); i++)
		{
			cout << "connectors[" << i << "] = " << connectors.at(i) << endl;
		}
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
			if(strcmp(cstr,"exit") == 0)
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
			cstr = strtok_r('\0',delims,&saveCstr);
		}
		delete []tmp_cstr;
	}

	return 0;
}
