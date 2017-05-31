#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define		WORD_LEN	128
#define		BUF_SIZE	4096
int main(){
	int fd, ofd, r_bytes;
	char fpath[WORD_LEN] = {0}, ofpath[WORD_LEN] = {0}, buf[BUF_SIZE] = {0};

	printf("Enter the fname.\n");
	scanf("%s", fpath);

	printf("Enter the output file.\n");
	scanf("%s", ofpath);

	if((fd = open(fpath, O_RDONLY)) < 0){
		printf("Error open\n");
		return 0;
	}

	if((ofd = open(ofpath, O_CREAT|O_WRONLY)) < 0){
		printf("Error new open\n");
		return 0;
	}

	while(1){
		r_bytes = read(fd, buf, BUF_SIZE);

		if(r_bytes <= 0){
			break;
		}else{
			write(ofd, buf, r_bytes);
		}

	}

	close(fd);
	close(ofd);
	return 0;
}