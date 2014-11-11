#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>

#define FLAG_a 1
#define FLAG_l 2
#define FLAG_R 3


using namespace std;

int main(int argc, char** argv)
{
	char dirName[] = ".";
	cout << "argc = " << argc << endl;
	int flags[3] = {0};
	for(int i = 0; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			for(int j = 1; argv[i][j] != 0; j++)
			{
				if(argv[i][j] == 'a')
				{
					flags[i-1] |= FLAG_a;
					cout << "flag = -a" << endl;
				}
				else if(argv[i][j] == 'l')
				{
					flags[i-1] |= FLAG_l;
					cout << "flag = -l" << endl;
				}
				else if(argv[i][j] == 'R')
				{
					flags[i-1] |= FLAG_R;
					cout << "flag = -R" << endl;
				}
			}
		}
	}
	
/******* HARD CODE FLAG POSITION INTO flags[] SO THAT 0 MEANS MISSING
 * THAT FLAG AND 1 MEANS FLAG WAS PASSED IN *******/
	
	DIR* dirp = opendir(dirName);
	dirent* direntp;
	
	struct stat statbuf;

	

	while((direntp = readdir(dirp)))
	{
		stat(direntp->d_name,&statbuf);
		cout << direntp->d_name << endl;
	}
	closedir(dirp);





	return 0;
}
