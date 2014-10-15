#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string>
#include <iostream>

using namespace std;


int main()
{
	char* argv[3];
	argv[0] = new char[3];
	
	//strcpy(argv[0],"ls");	
	execvp("ls",argv);

	int ret;

	//char* cmd[] = { "ls", "-l", (char *)0};
	//execvp("ls",cmd);

	cout << endl << "rshell is working!" << endl;
	return 0;
}





