#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


void no_option(char *dir, DIR *dp, struct stat st, struct dirent *d, char *pathname);
void i_option(char *dir, DIR *dp, struct stat st, struct dirent *d, char *pathname);
void l_option(char*, DIR*, struct stat, struct dirent*, char*);
void t_option(char*);
void printStat(char*, struct stat*);
char type(mode_t);
char *perm(mode_t);
int sorttime(const struct dirent **, const struct dirent **);

int main(int argc, char **argv)
{
	DIR *dp;
	char *dir;
	struct stat st;
	struct dirent *d;
	char path[1024];

	if(argc == 1){
		dir=".";
		dp = opendir(dir);

		no_option(dir, dp, st, d, path);
	}else{
		if(strcmp(argv[1], "-l")==0){
			if(argc == 2){
				dir=".";
				dp = opendir(dir);
				l_option(dir, dp, st, d, path);
			}
			else{
				for(int i=2;i<argc;i++){
					dir=argv[i];
					if(stat(dir, &st)<0){
						perror(dir);
						continue;
					}
					else{
						if(S_ISREG(st.st_mode)){
							printStat(dir, &st);
							continue;
						} else if(S_ISDIR(st.st_mode)){
							dp=opendir(dir);
							l_option(dir, dp, st, d, path);
						}
					}
				}
			}
		}
		else if(strcmp(argv[1], "-i")==0){
			if(argc == 2){
				dir=".";
				i_option(dir, dp, st, d, path);
			}else{
				for(int i=2;i<argc;i++){
					dir = argv[i];
					if(stat(dir, &st)<0){
						perror(dir);
						continue;
					}
					else{
						if(S_ISREG(st.st_mode)){
							printf("%ld ", (long)st.st_ino);
							printf("%s   ", dir);
							continue;
						} else if(S_ISDIR(st.st_mode)){
							dp = opendir(dir);
							if(argc>3){
								putchar('\n');
								putchar('\n');
								printf("%s:\n", dir);
							}
							i_option(dir, dp, st, d, path);
						}
					}
				}

			}
		}
		else if(strcmp(argv[1], "-t")==0){
			if(argc==2){
				dir=".";
				t_option(dir);
			} else{
				int min=0;
				for(int i=2;i<argc;i++){
					dir=argv[i];
					if(stat(dir, &st)<0){
						perror(dir);
						continue;
					}
					else{
						if(S_ISREG(st.st_mode)){
							printf("%s\t", dir);
						}
						else if(S_ISDIR(st.st_mode)){
							if(argc>3){
								putchar('\n');
								printf("%s:\n", dir);
							}
							t_option(dir);
						}
					}
				}
			}

		}
		else{
			for(int i=1;i<argc;i++){
				dir = argv[i];
				if(stat(dir, &st)<0){
					perror(dir);
					continue;
				}
				else{
					if(S_ISREG(st.st_mode)){
						printf("%s  ", dir);
					}
					else if(S_ISDIR(st.st_mode)){
						putchar('\n');
						dp = opendir(dir);
						no_option(dir, dp, st, d, path);
					}
				}
			}
		}
		
		putchar('\n');
		exit(0);
	}
}

void no_option(char *dir, DIR *dp, struct stat st, struct dirent *d, char *pathname)
{

	while((d=readdir(dp))!=NULL){
		if(!strncmp(d->d_name,".",1)){
			continue;
		}
		sprintf(pathname, "%s/%s", dir, d->d_name);
		if(lstat(pathname, &st)<0){
			perror(pathname);
			continue;
		}
		printf("%s  ", d->d_name);
	}
	putchar('\n');
	closedir(dp);

}

void l_option(char *dir, DIR *dp, struct stat st, struct dirent *d, char *pathname)
{

	while((d=readdir(dp))!=NULL){
		if(!strncmp(d->d_name,".",1)){
			continue;
		}
		sprintf(pathname, "%s/%s", dir, d->d_name);
		if(lstat(pathname, &st)<0){
			perror(pathname);
			continue;
		}
		printStat(d->d_name, &st);
	}
}

void printStat(char *file, struct stat *st)
{
	printf("%c%s ", type(st->st_mode), perm(st->st_mode));
	printf("%3ld ", st->st_nlink);
	printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name);
	printf("%9ld ", st->st_size);
	printf("%.12s ", ctime(&st->st_mtime)+4);
	printf("%s\n", file);
}

char type(mode_t mode)
{
	if(S_ISREG(mode))
		return('-');
	if(S_ISDIR(mode))
		return('d');
	if(S_ISCHR(mode))
		return('c');
	if(S_ISBLK(mode))
		return('b');
	if(S_ISLNK(mode))
		return('l');
	if(S_ISFIFO(mode))
		return('p');
	if(S_ISSOCK(mode))
		return('s');
}

char* perm(mode_t mode)
{
	static char perms[10]="---------";
	int index = 0;
	for(int i=0;i<3;i++){
		if(mode & (S_IREAD >> i*3)){
			perms[index++]='r';
		}else{
			perms[index++]='-';
		}

		if(mode & (S_IWRITE >> i*3)){
			perms[index++]='w';
		}else{
			perms[index++]='-';
		}

		if(mode & (S_IEXEC >> i*3)){
			perms[index++]='x';
		}else{
			perms[index++]='-';
		}
	}

	if(mode&S_ISUID){
		perms[2]=(mode&S_IXUSR) ? 's' : 'S';
	}

	if(mode&S_ISGID){
		perms[5]=(mode&S_IXGRP) ? 's' : 'S';
	}

	if(mode&S_ISVTX){
		perms[8]=(mode&S_IXOTH) ? 't' : 'T';
	}

	return(perms);
}
void i_option(char *dir, DIR *dp, struct stat st, struct dirent *d, char *pathname)
{
	int cnt = 0;
	dp = opendir(dir);
	while((d=readdir(dp))!=NULL){
		if(!strncmp(d->d_name,".",1)){
			continue;
		}
		sprintf(pathname, "%s/%s", dir, d->d_name);
		if(lstat(pathname, &st)<0){
			perror(pathname);
		}
		if(S_ISREG(st.st_mode)){
			printf("%ld ", (long)st.st_ino);
		}
		else if(S_ISDIR(st.st_mode)){
			printf("%ld ", (long)d->d_ino);
		}
		printf("%9s  ", d->d_name);
		cnt++;
		if(cnt%9==0 && cnt!=0){
			putchar('\n');
		}
	}
	putchar('\n');
}

void t_option(char *dir)
{

	struct dirent **fileList=NULL;
	int num_Cnt = 0;

	num_Cnt = scandir(dir, &fileList, NULL, sorttime);

	for(int i=0;i<num_Cnt;i++){
		if(!strncmp(fileList[i]->d_name,".",1)){
			continue;
		}
		printf("%s ", fileList[i]->d_name);
	}
	putchar('\n');
}

int sorttime(const struct dirent **d1, const struct dirent **d2)
{
	struct stat buf1, buf2;
	stat((*d1)->d_name, &buf1);
	stat((*d2)->d_name, &buf2);

	if(buf1.st_mtime<=buf2.st_mtime){

		if(buf1.st_mtime==buf2.st_mtime){
			return strcasecmp((*d1)->d_name, (*d2)->d_name);
		}
		else{
			return 1;
		}
	}
	else{
		return -1;
	}
}
