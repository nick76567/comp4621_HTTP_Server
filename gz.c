#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <zlib.h>


int get_content_length(char *fname, char *tmp_buf){
	struct stat st;
	stat(fname, &st);
	if(tmp_buf != NULL)
		sprintf(tmp_buf, "%ld", st.st_size);
	return st.st_size;
}

int get_compressed_file(int r_bytes, char *uncompressed_buffer, char* fname){
	gzFile output = gzopen(fname, "wb");
	if(output == NULL){
		printf("Error in gzopen.\n");
		return -1;
	}

	if(gzwrite(output, uncompressed_buffer, r_bytes) == 0 && r_bytes != 0){
		printf("Error in gzwrite.\n");
		return -1;
	}

	gzclose(output);
	return 0;
}
int main(){
	char *buffer;
	char ifname[] = "project_web/megaswitch-nsdi17.pdf";
	char ofname[] = "megaswitch-nsdi17.gz";
	int r_bytes, fd, fsize;

	fd = open(ifname, O_RDONLY);
	fsize = get_content_length(ifname, NULL);
	buffer = malloc(sizeof(char) * fsize);

	while(1){
		r_bytes = read(fd, buffer, fsize);

		printf("r_bytes: %d\n", r_bytes);
		if(r_bytes == 0)break;
		if(get_compressed_file(r_bytes, buffer, ofname) == -1) return 0;

	}

	free(buffer);


	return 0;
}