#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <vector>

#define SEMI_CON 1.0
#define AND_CON 2.0
#define OR_CON 3.0

#define STD_IN 4.0
#define STD_OUT 5.0
#define APPEND 6.0

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
	char symbols[] = {';','|','&','>','<'};

	while(1)
	{
		printPrompt();
		string input = getCmdLine();
		vector<double> connectors(1);
		///--- search for connector types ---///
		for(unsigned int i = 0; i < sizeof(symbols); i++)
		{
			size_t found = input.find(symbols[i]);
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
				else if((input.at(found) == '>') && (input.at(found + 1) != '>'))
					connectors.push_back(found + (STD_OUT/10));
				else if((input.at(found) == '>') && (input.at(found + 1) == '>'))
				{
					connectors.push_back(found + (APPEND/10));
					pos += 1;
				}
				else if((input.at(found) == '<'))
					connectors.push_back(found + (STD_IN/10));
				found = input.find(symbols[i],pos);
			}
		}
		sort(connectors.begin(),connectors.end());
		
		/*for(unsigned int i = 0; i < connectors.size(); i++)
			cout << "connectors.at(" << i << ") = " << connectors.at(i) << endl;*/

		///--- parse user input ---///
		char* tmp_cstr = new char[input.size()+1];
		char* saveCstr;
		for(unsigned int i = 0; i < (input.size()+1); i++)
		{
			tmp_cstr[i] = '\0';
		}
		
		strcpy(tmp_cstr,input.c_str());

		char* cstr = strtok_r(tmp_cstr,delims,&saveCstr);
		//cout << "saveCstr = " << *saveCstr << endl;
		int statementPos = 0;
		bool good = 1;
		int prev_connector = 0;

		while(cstr != '\0')
		{
			bool in = 0;
			bool out = 0;
			bool app = 0;
			//cout << "right before connecotrs.at(statementPos)" << endl;
			//cout << "statementPos = " << statementPos << endl;
			//cout << "connectors.size() = " << connectors.size() << endl;
			int connector = (connectors.at(statementPos) * 10);
			connector = connector%10;
			if(statementPos == 0)
				good = 1;
			///--- checking for conditionals ---///
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

			unsigned int size_compare = statementPos + 1;
			///--- checking for io rediredtion ---///
			if(connectors.size() > size_compare)
			{
				int redir_connector = (connectors.at(size_compare) * 10);
				redir_connector = redir_connector%10;
				//cout << "redir_connector = " << redir_connector << endl;
				if((redir_connector == 4))
					in = 1;
				if((redir_connector == 5))
					out = 1;
				if(connectors.size() > (size_compare+1))
				{
					int redir_connector2 = (connectors.at(size_compare+1) * 10);
					redir_connector2 = redir_connector2%10;

					if((redir_connector == 4) && (redir_connector2 == 5))
					{
						in = 1;
						out = 1;
					}
				}
				if((redir_connector == 6))
					app = 1;
				cout << "in = " << in << endl;
				cout << "out = " << out << endl;
			}
			if(good)
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
					/****** child process ******/
					
					char** argv = new char*[input.size()+1];
					char** saveInput = new char*[input.size()+1];
					//strcpy(saveInput,cstr);
					for(unsigned int i = 0; i < (input.size()+1); i++)
					{
						argv[i] = '\0';
					}
					
					argv[0] = strtok(cstr," ");
					//cout << "argv[0] = " << argv[0] << endl;
					/*for(unsigned int i = 0; saveInput[i] != '\0'; i++)	
						cout << "saveInput[" << i << "] = " << saveInput[i] << endl;*/
					unsigned int j = 1;
				
					while((j < (input.size()+1)) )
					{
						argv[j] = strtok(NULL," ");
						j++;
					}
					for(unsigned int i = 0; argv[i] != '\0'; i++)
						saveInput[i] = argv[i];
					if(in)
					{
						//cout << "argv[1] = " << argv[1] << endl;
						//cout << "argv[2] = " << argv[2] << endl;
						unsigned int counter = 0;
						while(*argv[counter] != '<')
						{
							counter++;
						}
						
						string redir_input = argv[counter+1];
						cout << "redir_input = " << redir_input << endl;
						argv[counter] = '\0';
						argv[counter+1] = '\0';
						//string redir_input = argv[2];
						//cout << "redir_input = " << redir_input << endl;

						int fd_in = open(redir_input.c_str(),O_RDONLY,0);
						if(fd_in == -1)
						{perror("open failed\n"); exit(1);}
						
						//int dup_err = dup2(fd_in,STDIN_FILENO);
						int dup_err = dup2(fd_in,0);
						if(dup_err == -1)
						{perror("dup2 failed\n"); exit(1);}
						int close_err = close(fd_in);
						if(close_err == -1)
						{perror("close failed\n"); exit(1);}
						//argv[1] = '\0';
						//argv[2] = '\0';
						//argv[3] = '\0';
						in = 0;
					}

					if(out)
					{
						unsigned int counter = 0;
						while(*argv[counter] != '>')
						{
							counter++;
						}
						
						string redir_output = argv[counter+1];
						cout << "redir_output = " << redir_output << endl;
						argv[counter] = '\0';
						argv[counter+1] = '\0';

						for(unsigned int i = 0; argv[i] != '\0'; i++)
							cout << "argv[" << i << "] = " << argv[i];
						
						cout << "redir_output = " << redir_output << endl;

						int fd_out = creat(redir_output.c_str(),0644);
						if(fd_out == -1)
						{perror("creat failed\n"); exit(1);}
						
						int dup_err2 = dup2(fd_out,STDOUT_FILENO);
						if(dup_err2 == -1)
						{perror("dup2 failed\n"); exit(1);}
						
						int close_err2 = close(fd_out);
						if(close_err2 == -1)
						{perror("close failed\n"); exit(1);}
											
						out = 0;
					}

					if(app)
					{
						cout << "app = " << app << endl;
						app = 0;
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
					delete []saveInput;
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
			in = 0;
			out = 0;
			app = 0;
			cstr = strtok_r('\0',delims,&saveCstr);
			prev_connector = connector;
			statementPos++;
		}
		delete []tmp_cstr;
	}

	return 0;
}
