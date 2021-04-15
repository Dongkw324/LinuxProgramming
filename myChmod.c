#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
int main(int argc, char *argv[])
{
	if(argc < 2){
		fprintf(stderr,"chmod: 잘못된 연산자\nTry 'chmod --help' for more information\n");
		exit(1);
	}

	if(argc < 3){
		fprintf(stderr, "chmod: '%s' 뒤에 명령어가 빠짐\nTry 'chmod --help' for more information\n", argv[1]);
		exit(1);
	}
	
	for(int i=0;i<strlen(argv[1]);i++){
		
		if(!isdigit(argv[1][i])){
			fprintf(stderr,"chmod: 잘못된 모드 : %s\nTry 'chmod --help' for more information\n", argv[1]); 
			exit(1);
		}
		
	}
	
	int s = atoi(argv[1])/1000;
	int r = (atoi(argv[1])/100)%10;
	int w = (atoi(argv[1])/10)%10;
	int x = atoi(argv[1])%10;

	if(s>7 || r>7 || w>7 || x>7){
		fprintf(stderr, "chmod: 잘못된 모드 : %s\nTry 'chmod --help' for more information\n", argv[1]);
		exit(1);
	}

	int mode = 0;
	sscanf(argv[1], "%o", &mode);

	
	for(int i=2;i<argc;i++){
		if(access(argv[i], R_OK)!=0){
			fprintf(stderr, "'%s'에 접근할 수 없습니다. 그런 파일이나 디렉토리가 없습니다.\n", argv[i]);
			continue;
		}

		if(chmod(argv[i], mode)<0){
			fprintf(stderr, "접근 권한 변경 실패\n");
			exit(1);
		}
	}

	exit(1);
}
	
