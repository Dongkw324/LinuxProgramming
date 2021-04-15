#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <grp.h>
#include <dirent.h>
#include <utime.h>
int main(int argc, char *argv[])
{
	int fd;

	if(argc < 2){
		fprintf(stderr, "touch : 파일 명령어 누락\nTry 'touch --help' for more information\n");
		exit(1);
	}
	
	for(int i=1;i<argc;i++){
		fd = open(argv[i], O_RDWR | O_CREAT, 0664);
		if(utime(argv[i], NULL) == -1){
			fprintf(stderr, "utime error\n");
			exit(1);
		}
		close(fd);
	}

	exit(0);
}

