#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#include <iostream>

using namespace std;

#define FLAG_a 1
#define FLAG_l 1
#define FLAG_R 1

//void Check_l_flag(int flg[],struct stat statbuf);
bool Check_a_flag(int flg[]);//,dirent* direntp,struct stat statbuf);

void getMonth(int pos)
{
	const string months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	std::cout << ' ' <<  months[pos];
}

void Check_l_flag(int flg[],struct stat statbuf)
{
	if(flg[1])
	{
		if(S_ISDIR(statbuf.st_mode))
			std::cout << 'd';
		else std::cout << '-';
		if(statbuf.st_mode & S_IRUSR)
			std::cout << 'r';
		else std::cout << '-';
		if(statbuf.st_mode & S_IWUSR)
			std::cout << 'w';
		else std::cout << '-';
		if(statbuf.st_mode & S_IXUSR)
			std::cout << 'x';
		else std::cout << '-';
		if(statbuf.st_mode & S_IRGRP)
			std::cout << 'r';
		else std::cout << '-';
		if(statbuf.st_mode & S_IWGRP)
			std::cout << 'w';
		else std::cout << '-';
		if(statbuf.st_mode & S_IXGRP)
			std::cout << 'x';
		else std::cout << '-';
		if(statbuf.st_mode & S_IROTH)
			std::cout << 'r';
		else std::cout << '-';
		if(statbuf.st_mode & S_IWOTH)
			std::cout << 'w';
		else std::cout << '-';
		if(statbuf.st_mode & S_IXOTH)
			std::cout << 'x';
		else std::cout << '-';

		std::cout << ' ' << statbuf.st_nlink;
		
		struct passwd* u_pwd = getpwuid(statbuf.st_uid);
		std::cout << ' ' << u_pwd->pw_name;
		
		struct group* g_pwd = getgrgid(statbuf.st_gid);
		std::cout << ' ' << g_pwd->gr_name;

		std::cout << ' ' << statbuf.st_size;
		
		struct tm* info;
		info = localtime(&statbuf.st_mtime);
		int month = info->tm_mon;
		getMonth(month);
		std::cout << ' ' << info->tm_mday << ' ' 
			<< info->tm_hour << ':' << info->tm_min << ' ';
	}

	return;
}

bool Check_a_flag(int flg[])
{
	if(flg[0])
		return true;;
	return false;
}
/*
void outPutFileNames(char dirName[],int flags[])
{
	cout << "inside outPutFileNames" << endl;
	DIR* dirp = opendir(dirName);
	dirent* direntp;
	struct stat statbuf;
	while((direntp = readdir(dirp)))
	{
		cout << "inside while loop" << endl;
		if(direntp->d_name[0] == '.' && !Check_a_flag(flags))
			continue;
			
		stat(direntp->d_name,&statbuf);
		Check_l_flag(flags,statbuf);
		//outPutFileNames(direntp);
	}
	cout << direntp->d_name << endl;
	closedir(dirp);
	return;
}
*/
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
	dirent* direntp = '\0';
	//outPutFileNames(dirName,flags);
	
	struct stat statbuf;
	while((direntp = readdir(dirp)))
	{
		if(direntp->d_name[0] == '.' && !Check_a_flag(flags))
			continue;
			
		stat(direntp->d_name,&statbuf);
		Check_l_flag(flags,statbuf);
	//	outPutFileNames(direntp);
		cout << direntp->d_name << endl;
	}
	closedir(dirp);

	return 0;
}
