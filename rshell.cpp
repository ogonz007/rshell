#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string>
#include <vector>
#include <pwd.h>

#define SEMI_CON 1.0
#define AND_CON 2.0
#define OR_CON 3.0

#define STD_IN 4.0
#define STD_OUT 5.0
#define APPEND 6.0
#define PIPE 7.0
#define AMP_CON 8.0

pid_t id;

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
	struct passwd* userName;
	userName = getpwuid(getuid());
	if(userName == '\0')
	{perror("getpwuid failed"); exit(1);}

	char hostName[32];
	if((gethostname(hostName,sizeof(hostName))) == -1)
	{perror("gethostname failed"); exit(1);}
	cout << userName->pw_name << "@" << hostName << ":~" << currDir << "$ ";
	return;
}

///--- gets the user input data for command execution ---///
string getCmdLine()
{
	string str;
	getline(cin,str);
	return str;
}

static void handler(int signum)
{
	signal(SIGINT,handler);
	return;
}

static void z_handler(int signum)
{
	if(id)
		kill(id,SIGSTOP);
	return;
}

int main()
{
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	struct sigaction sa_z;
	sa_z.sa_handler = z_handler;
	sigemptyset(&sa_z.sa_mask);
	sa_z.sa_flags = SA_RESTART;

	if((sigaction(SIGINT,&sa,NULL)) == -1)
	{perror("sigaction failed");exit(1);}

	if((sigaction(SIGTSTP,&sa_z,NULL)) == -1)
	{perror("sigaction failed");exit(1);}


	string currDir = "";
	char delims[] = {';','|','&'};
	char symbols[] = {';','|','&','>','<'};

	///--- PARSING THE PATH ---///
	char* tmpPath = getenv("PATH");
	if(tmpPath == '\0')
	{perror("getenv failed"); exit(1);}
	char pathDelims[] = {':'};

	char** pathList = new char*[20];
	for(unsigned int i = 0; i < 20; i++)
		pathList[i] = '\0';

	pathList[0] = strtok(tmpPath,pathDelims);
	for(unsigned int i = 0; i < 20; i++)
		pathList[i] = strtok(NULL,pathDelims);
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
				else if((input.at(found) == '&') && (((found + 1) == input.size()) || (input.at(found + 1) == '&')))
				{
					connectors.push_back(found + (AND_CON/10));
					pos += 1;
				}
				else if((input.at(found) == '&') && ((found + 1) == input.size()))
				{
					connectors.push_back(found + (AMP_CON/10));
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
		///--- done searching for connector types ---///
	
		///--- parse user input ---///
		char* tmp_cstr = new char[input.size()+1];
		char* saveCstr = '\0';
		for(unsigned int i = 0; i < (input.size()+1); i++)
		{
			tmp_cstr[i] = '\0';
		}
		
		strcpy(tmp_cstr,input.c_str());

		char* cstr = '\0'; 
		cstr = strtok_r(tmp_cstr,delims,&saveCstr);
		int statementPos = 0;
		bool good = 1;
		int prev_connector = 0;

		while(cstr != '\0')
		{
			//cout << "good = " << good << endl;
			bool in = 0;
			bool out = 0;
			bool app = 0;
			bool pip = 0;
			bool cd_found = 0;

			///--- checking for conditionals ---///
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
			else if((connector == 7))
				pip = 1;
			else if((connector == 8))
				cout << "found &" << endl;

			///--- checking for io rediredtion ---///
			unsigned int size_compare = statementPos + 1;
			if(connectors.size() > size_compare)
			{
				int redir_connector = (connectors.at(size_compare) * 10);
				redir_connector = redir_connector%10;

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
			}

			if(good)
			{
				///--- handle exit ---///
				if(strcmp(cstr,"exit") == 0)
				{
					exit(0);
				}

				///--- setting commands in argv ---///
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

				///--- handle cd ---///
				if(strcmp(argv[0],"cd") == 0)
				{
					cd_found = 1;
					
					if((argv[1] == '\0') || (*argv[1] == '~'))
					{
						char* homePath = getenv("HOME");
						if(homePath == '\0')
						{perror("getenv failed");exit(1);}

						currDir = homePath;
						if(chdir(currDir.c_str()) != 0)
						{perror("chdir failed1");exit(1);}
					}

					else
					{
						string newPath = currDir;
						newPath = newPath + "/" + argv[1];
						
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
								if(newBuf == '\0')
								{perror("getcwd failed");exit(1);}

								currDir = newBuf;
							}

							else if((S_ISDIR(stat_buf.st_mode) == false))
							{
								cout << "cd: " << argv[1] << ": Not a directory" << endl;
							}
						}
						else
						{perror("stat failed");exit(1);}


					}
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
					id = pid;
					if(pid == -1)
					{
						perror("Error with fork()\n");
						exit(1);
					}
					else if(pid == 0)
					{
						/****** child process ******/
						
						if(in)
						{
							unsigned int counter = 0;
							while(*argv[counter] != '<')
							{
								counter++;
							}
							
							string redir_input = argv[counter+1];
							argv[counter] = argv[counter+2];
							argv[counter+1] = argv[counter+3];

							int fd_in = open(redir_input.c_str(),O_RDONLY,0);
							if(fd_in == -1)
							{perror("open failed\n"); exit(1);}
							
							int dup_err = dup2(fd_in,0);
							if(dup_err == -1)
							{perror("dup2 failed\n"); exit(1);}
							int close_err = close(fd_in);
							if(close_err == -1)
							{perror("close failed\n"); exit(1);}
							
							in = 0;
						}

						if(out)
						{
							unsigned int counter2 = 0;
							while(*argv[counter2] != '>')
							{
								counter2++;
							}
							//cerr << "testing >" << argv[counter2][0] << argv[counter2][1] << endl;
							if((argv[counter2][0] == '>') && (argv[counter2][1] != '>')){
							
							string redir_output = argv[counter2+1];
							//cout << "redir_output = " << redir_output << endl;
							argv[counter2] = '\0';
							argv[counter2+1] = '\0';

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
						}

						if(app)
						{
							unsigned int counter3 = 0;
							while(*argv[counter3] != '>')
							{
								//cerr << "argv[" << counter3 << "] = " << argv[counter3] << endl;
								counter3++;
							}
							//cerr << argv[counter3][0] << argv[counter3][1] << endl;
							if((argv[counter3][0] == '>') && (argv[counter3][1] == '>')){
							
							string redir_outAppend = argv[counter3+1];
							//cout << "redir_output = " << redir_output << endl;
							//argv[counter3] = '\0';
							//argv[counter3+1] = '\0';
							//cerr << "argv[" << counter3 << "] = " << argv[counter3] << endl;
							while(argv[counter3] != '\0')
							{
								argv[counter3] = '\0';
								counter3++;
							}

							if(app)
							{
								int fd_outAppend = open(redir_outAppend.c_str(),O_WRONLY | O_APPEND);
								if(fd_outAppend == -1)
								{perror("open failed\n"); exit(1);}
							
								int dup_err2 = dup2(fd_outAppend,STDOUT_FILENO);
								if(dup_err2 == -1)
								{perror("dup2 failed\n"); exit(1);}
							
								int close_err2 = close(fd_outAppend);
								if(close_err2 == -1)
								{perror("close failed\n"); exit(1);}
							}					
							app = 0;}
						}

						///--- checking for commenting using # sign ---///
						if((*argv[0] == '#'))
						{
							exit(0);
						}

						if(pip)
						{
							int dup2_err = dup2(fd[1],1);
							if(dup2_err == -1)
							{perror("dup2 failed"); exit(1);}

							int close_err = close(fd[0]);
							if(close_err == -1)
							{perror("close failed"); exit(1);}
						}
						
						int r = 0;
						for(unsigned int counter = 0; counter < 17; counter++)
						{
							string tmpPathList = pathList[counter];
							string tmpCmd = argv[0];
							tmpPathList = tmpPathList + "/" + tmpCmd;
							//cout << "tmpPathList = " << tmpPathList << endl;
							r = execv(tmpPathList.c_str(),argv);
						}
						if(r == -1)
						{
							//good = 0;
							perror("error with execv");
						}
						good = 0;
						//cout << "good after error = " << good << endl;
						delete []argv;
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
						
						int id_stat;
						int err = waitpid(id,&id_stat,WUNTRACED);
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
			}
			//cout << "good outside = " << good << endl;
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
