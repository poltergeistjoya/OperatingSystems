/*
Submitter: Joya Debi
Group Member: Lucia Rhode
Prof Hakner
Ece357
Program 2 - Recursive Filesystem Lister
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void getTime(struct stat buffer){
	//formats mtime to human readable format
	char *fullTime = ctime(&buffer.st_mtime);
	printf("%.*s ", 12, fullTime+4);
	return; 
}

void getUid(uid_t uid){
	//use getpwuid to get user name of matching uid
	struct passwd *pwd;
	if((pwd = getpwuid(uid)) != NULL){
		printf("%-8s ", pwd->pw_name);
	}
	else{ //if user isn't found, print the uid
		printf("%-8d ", uid);
	}
	return;
}

void getGid(gid_t gid){
	//use getgrgid to get group name of matching gid
	struct group *grp;
	if((grp = getgrgid(gid)) != NULL){
		printf("%-s ",  grp->gr_name);
	}
	else{ //if group isn't found, print the gid
		printf("%-d ", gid);
	}
	return;
}

void printinfo(char *dirPath, struct stat buffer){
	printf("%6llu ", buffer.st_ino); //INODE NUMBER

	blkcnt_t oneKblks = ((buffer.st_blocks)/2); //number 512b blocks/2= 1K BLOCKS
	printf("%4lld ", oneKblks); 

	switch(buffer.st_mode & S_IFMT){ //FILE TYPE
		case S_IFREG:
			printf("-");
			break;
		
		case S_IFDIR:
			printf("d");
			break;

		case S_IFLNK:
			printf("l");
			break;

		case S_IFCHR:
			printf("c");
			break;
	
		case S_IFBLK:
			printf("b");
			break;

		case S_IFIFO:
			printf("p");
			break;

		case S_IFSOCK:
			printf("s");
			break;
		default: //if inode type not defined
			printf("?");
			break;
	}

	printf( (buffer.st_mode & S_IRUSR) ? "r" : "-"); //USER PERMISSIONS
	printf( (buffer.st_mode & S_IWUSR) ? "w" : "-");
	printf( (buffer.st_mode & S_IXUSR) ? "x" : "-");

	printf( (buffer.st_mode & S_IRGRP) ? "r" : "-"); //GROUP PERMISSIONS
	printf( (buffer.st_mode & S_IWGRP) ? "w" : "-");
	printf( (buffer.st_mode & S_IXGRP) ? "x" : "-");

	printf( (buffer.st_mode & S_IROTH) ? "r" : "-"); //OTHERS PERMISSIONS
	printf( (buffer.st_mode & S_IWOTH) ? "w" : "-");
	printf( (buffer.st_mode & S_IXOTH) ? "x " : "- ");
	
	printf("%3hu ", buffer.st_nlink); //NLINK

	getUid(buffer.st_uid); //USER ID

	getGid(buffer.st_gid); //GROUP ID

	printf("%12lld ", buffer.st_size); //FILE SIZE

	getTime(buffer); //LAST MODIFIED TIME

	printf("%s ", dirPath); //PATH
	
	return;
}

int dirinfo(char *dirPath, int firstDir){
	DIR *directory;
	struct dirent *dp;
	struct stat buffer;

	if((directory = opendir(dirPath)) == NULL){
		//this should also catch error message for if the process runs out of fd
		fprintf(stderr, "ERROR: Open directory failure %s: %s\n", dirPath, strerror(errno));
		return -1;
	}
	
	while((dp = readdir(directory)) != NULL){
		char *addPath;
		char newPath[1024] = {0};

		addPath = dp->d_name;
		strcpy(newPath, dirPath);
		strcat(newPath, "/");
		strcat(newPath, addPath);
		
		if(lstat(newPath, &buffer) == -1){
			fprintf(stderr, "ERROR:lstat failure for path  %s: %s\n", addPath, strerror(errno)); //lstat error
			return -1;
		}

		if(firstDir == 0){ //call information for starting directory
			firstDir++;
			printinfo(dirPath, buffer);
			printf("\n");
		}

		if(dp->d_type == DT_DIR){ //call information for directories
			if((strcmp(dp->d_name, ".") == 0) || (strcmp(dp->d_name, "..") == 0)){ //ignore . and ..
				continue;
			}
			printinfo(newPath, buffer);
			printf("\n");
			dirinfo(newPath, firstDir);	
		}
		else if(dp->d_type == DT_LNK) { //call information for symlinks
			char symPath[1024];
			printinfo(newPath, buffer);
			printf("\n");
			if(readlink(newPath, symPath, buffer.st_size +1) < 0){
				fprintf(stderr, "ERROR: failure reading symlink %s: %s\n", newPath, strerror(errno));
				return -1;
			}
			printinfo(newPath, buffer);
			printf("-> %s\n", symPath);	
		}
		else if(dp->d_type == DT_REG){ //call information for regular files
			printinfo(newPath, buffer);
			printf("\n");
		}
		else{
        		printinfo(newPath, buffer);
			printf("\n");
		}	
	}
	return 0;
}

int main(int argc, char *argv[]){
	char dirPath[1024] = {0}; // set array to null
	int firstDir = 0;

	if(argc > 2){ //incorrect format, too many arguments
		printf("ERROR: Incorrect format\n");
		printf("USAGE:\n./find [starting_directory]\n");
		return -1;
	}
	else if(argc < 2){ //no starting directory mentioned, start with root
		strcpy(dirPath, ".");
		dirinfo(dirPath, firstDir);
	}
	else if(argc == 2){ //starting directory specified
		strcpy(dirPath, argv[1]);
		dirinfo(dirPath, firstDir);
	}
	else{ //incorrect format
		printf("ERRROR: Incorrect format\n");
		printf("USAGE:\n./find [starting_directory]\n");
		return -1;
	}	
	return 0;
}