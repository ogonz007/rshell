#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

#include <iostream>

#define FLAG_a 1
#define FLAG_l 1
#define FLAG_R 1

void Check_l_flag(int flg[],struct stat statbuf);
void Check_a_flag(int flg[],dirent* direntp,struct stat statbuf);


void Check_l_flag(int flg[],struct stat statbuf)
{
	if(flg[1])
	{
		

		///--- checking is DIR ---///
		if(S_ISDIR(statbuf.st_mode))
			std::cout << 'd';
		else std::cout << '-';
		
		///--- checking owner perms ---///


	}

	return;
}

bool Check_a_flag(int flg[])
{
	if(flg[0])
		return true;;
	return false;
}



using namespace std;

int main(int argc, char** argv)
{
	char dirName[] = ".";
	int flags[3] = {0};
	
	for(int i = 0; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			for(int j = 1; argv[i][j] != 0; j++)
			{
				if(argv[i][j] == 'a')
					flags[0] |= FLAG_a;
				else if(argv[i][j] == 'l')
					flags[1] |= FLAG_l;
				else if(argv[i][j] == 'R')
					flags[2] |= FLAG_R;
			}
		}
	}
	
	DIR* dirp = opendir(dirName);
	dirent* direntp;
	
	struct stat statbuf;

	while((direntp = readdir(dirp)))
	{
		if(direntp->d_name[0] == '.' && !Check_a_flag(flags))
			continue;
			
		stat(direntp->d_name,&statbuf);
		//Check_a_flag(flags,direntp,statbuf);
		//Check_l_flag(flags,statbuf);
		cout << ' ' << direntp->d_name << endl;
	}
	closedir(dirp);





	return 0;
}
