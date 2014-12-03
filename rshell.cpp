#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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
#define PIPE 7.0

using namespace std;

void getPath(string &currDir)
{
	char buf[1024];
	if(!getcwd(buf,1024))
		perror("getcwd failed");
	currDir = buf;
	return;
}

///--- prints the rshell prompt (e.g. $) ---///
void printPrompt(string &currDir)
{
	//char buf[1024];
	//if(!getcwd(buf,1024))
	//	perror("getcwd failed");
	cout << currDir << " $ ";
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
	string currDir = "";
	char delims[] = {';','|','&'};
	char symbols[] = {';','|','&','>','<'};

	///--- PARSING THE PATH ---///
	char* tmpPath = getenv("PATH");
	char pathDelims[] = {':'};

	char** pathList = new char*[20];
	for(unsigned int i = 0; i < 20; i++)
		pathList[i] = '\0';

	pathList[0] = strtok(tmpPath,pathDelims);
	for(unsigned int i = 0; i < 20; i++)
		pathList[i] = strtok(NULL,pathDelims);
	/*for(unsigned int i = 0; pathList[i] != NULL; i++)
		cout << "pathList[" << i << "] = " << pathList[i] << endl;*/
	///--- PARSING THE PATH DONE ---///

	getPath(currDir);

	while(1)
	{
		printPrompt(currDir);
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
				else if((input.at(found) == '|') && (input.at(found + 1) != '|'))
					connectors.push_back(found + (PIPE/10));
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
			bool pip = 0;
			bool cd_found = 0;
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
			else if((connector == 7))
				pip = 1;

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
				//cout << "in = " << in << endl;
				//cout << "out = " << out << endl;
			}

			if(good)
			{
				if(strcmp(cstr,"exit") == 0)
				{
					exit(0);
				}
				char** argv = new char*[input.size()+1];
				char** saveInput = new char*[input.size()+1];
				//strcpy(saveInput,cstr);
				for(unsigned int i = 0; i < (input.size()+1); i++)
				{
					argv[i] = '\0';
				}
				
				argv[0] = strtok(cstr," ");
				//cout << "argv[0] = " << argv[0] << endl;
				unsigned int j = 1;
				while((j < (input.size()+1)) )
				{
					argv[j] = strtok(NULL," ");
					j++;
				}

				if(strcmp(argv[0],"cd") == 0)
				{
					//cout << "cd found" << endl;
					cd_found = 1;
					
					if(argv[1] == '\0')
					{
						char* homePath = getenv("HOME");
						currDir = homePath;
						if(chdir(currDir.c_str()) != 0)
						{perror("chdir failed1");exit(1);}
						//cout << "currDir = " << currDir << endl;
					}

					else
					{
						//cout << "currDir = " << currDir << endl;
						string newPath = currDir;
						newPath = newPath + "/" + argv[1];// + "/";
						
						struct stat stat_buf;
						if(stat(newPath.c_str(),&stat_buf) != -1)
						{
							if((S_ISDIR(stat_buf.st_mode)) == true)
							{
								newPath = newPath + "/";
								if(chdir(newPath.c_str()) != 0)
								{perror("chdir failed2");exit(1);}

								char newBuf[1024];
								getcwd(newBuf,1024);

								currDir = newBuf;
								//cout << "currDir = " << currDir << endl;
							}

							else if((S_ISDIR(stat_buf.st_mode) == false))
							{
								cout << "cd: " << argv[1] << ": Not a directory" << endl;
							}
						}
						else
						{perror("stat failed");exit(1);}

						/*if(chdir(newPath.c_str()) != 0)
						{perror("chdir failed2");exit(1);}

						char newBuf[1024];
						getcwd(newBuf,1024);

						currDir = newBuf;
						cout << "currDir = " << currDir << endl;*/
					}
					//cout << "currDir = " << currDir << endl;
					//cout << "newBuf = " << newBuf << endl;
					
					//for(unsigned int i = 0; argv[i] != '\0'; i++)	
					//	cout << "argv[" << i << "] = " << argv[i] << endl;

				}
				if(!cd_found)
				{
					int fd[2];
					if(pip)
					{
						int pipe_err = pipe(fd);
						if(pipe_err == -1)
						{perror("pipe failed"); exit(1);}
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
						/*
						char** argv = new char*[input.size()+1];
						char** saveInput = new char*[input.size()+1];
						//strcpy(saveInput,cstr);
						for(unsigned int i = 0; i < (input.size()+1); i++)
						{
							argv[i] = '\0';
						}
						
						argv[0] = strtok(cstr," ");
						//cout << "argv[0] = " << argv[0] << endl;
						for(unsigned int i = 0; saveInput[i] != '\0'; i++)	
							cout << "saveInput[" << i << "] = " << saveInput[i] << endl;
						unsigned int j = 1;
					
						while((j < (input.size()+1)) )
						{
							argv[j] = strtok(NULL," ");
							j++;
						}*/
						/*for(unsigned int i = 0; argv[i] != '\0'; i++)
							saveInput[i] = argv[i];*/
						if(in)
						{
							//cout << "in if(in) \n";
							//cout << "argv[1] = " << argv[1] << endl;
							//cout << "argv[2] = " << argv[2] << endl;
							unsigned int counter = 0;
							while(*argv[counter] != '<')
							{
								counter++;
							}
							
							string redir_input = argv[counter+1];
							//cout << "redir_input = " << redir_input << endl;
							argv[counter] = argv[counter+2];
							argv[counter+1] = argv[counter+3];
							//string redir_input = argv[2];
							//cout << "redir_input = " << redir_input << endl;

							int fd_in = open(redir_input.c_str(),O_RDONLY,0);
							if(fd_in == -1)
							{perror("open failed\n"); exit(1);}
							
							int dup_err = dup2(fd_in,0);
							if(dup_err == -1)
							{perror("dup2 failed\n"); exit(1);}
							int close_err = close(fd_in);
							if(close_err == -1)
							{perror("close failed\n"); exit(1);}
							/*for(unsigned int i = 0; i < input.size(); i++)
							{
								if(argv[i] != '\0')
									cout << "argv[" << i << "] = " << argv[i] << endl;
							}*/
							in = 0;
						}

						if(out)
						{
							//cout << "in if(out) \n";
							unsigned int counter2 = 0;
							while(*argv[counter2] != '>')
							{
								counter2++;
							}
							//cout << "argv[" << counter2 << "] = " << argv[counter2+1] << endl;
							
							string redir_output = argv[counter2+1];
							//cout << "redir_output = " << redir_output << endl;
							argv[counter2] = '\0';
							argv[counter2+1] = '\0';

							/*for(unsigned int i = 0; argv[i] != '\0'; i++)
								cout << "argv[" << i << "] = " << argv[i] << endl;
							*/
							//cout << "redir_output = " << redir_output << endl;

							if(out){
							int fd_out = creat(redir_output.c_str(),0644);
							if(fd_out == -1)
							{perror("creat failed\n"); exit(1);}
							
							int dup_err2 = dup2(fd_out,STDOUT_FILENO);
							if(dup_err2 == -1)
							{perror("dup2 failed\n"); exit(1);}
							
							int close_err2 = close(fd_out);
							if(close_err2 == -1)
							{perror("close failed\n"); exit(1);}
							}					
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

						if(pip)
						{
							int dup2_err = dup2(fd[1],1);
							if(dup2_err == -1)
							{perror("dup2 failed"); exit(1);}

							int close_err = close(fd[0]);
							if(close_err == -1)
							{perror("close failed");exit(1);}
						}
						
						int r = 0;
						for(unsigned int counter = 0; counter < 18; counter++)
						{
							string tmpPathList = pathList[counter];
							string tmpCmd = argv[0];
							tmpPathList = tmpPathList + "/" + tmpCmd;
							//cout << "tmpPathList = " << tmpPathList << endl;
							execv(tmpPathList.c_str(),argv);
						}
						if(r == -1)
						{
							good = 0;
							perror("error with execv");
						}
						delete []argv;
						delete []saveInput;
						exit(1);
						
					}
					else if(pid > 0)
					{
						// parent process
						/*int saveSTDIN = dup(0);
						if(saveSTDIN == -1)
						{perror("dup failed");exit(1);}

						if(pip)
						{
							int dup2_err2 = dup2(fd[0],0);
							if(dup2_err2 == -1)
							{perror("dup2 failed");exit(1);}

							int close_err2 = close(fd[1]);
							if(close_err2 == -1)
							{perror("close failed");exit(1);}
						}*/
						
						int err = wait(NULL);
						if(err == -1)
						{
							perror("error with wait in parent process");
							exit(1);
						}
						/*int exec_err = execvp(argv[0],argv);
						if(exec_err == -1)
						{perror("execvp in parent failed");exit(1);}
					
						int dup2_err3 = dup2(saveSTDIN,0);
						if(dup2_err3 == -1)
						{perror("dup2 failed");exit(1);}*/

					}
				}
				delete []argv;
				delete []saveInput;
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
	for(unsigned int i = 0; i < 20; i++)
		pathList[i] = NULL;
	delete []pathList;
	return 0;
}
