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
#include <time.h>


#define BUFFER_SIZE		512
#define REQUEST_SIZE	256
#define PORT            12345
#define Q_LIMIT			16
#define METHOD			0
#define PATH			1
#define HTTP_VER		2

//reference: http://fred-zone.blogspot.hk/2007/09/http-web-server.html
struct extensions{
    char *ext;
    char *filetype;
} exts [] = {
	{"pdf", "application/pdf"},
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"css", "text/css"  },
    {"js",  "text/js"   },
    {"exe","text/plain" },
    {0,		"image/gif"}
  };

struct Socket_fd{
	int clifd;
	int servfd;
};

void get_request_first_line(int clifd, char *request){
	char *buffer, *token;
	buffer = malloc(sizeof(char) * (REQUEST_SIZE + 1));
	memset(buffer, 0, REQUEST_SIZE + 1);	

	if(read(clifd, buffer, REQUEST_SIZE) < 0){
		printf("Error in read request\n");
		exit(0);
	}


	token = strtok(buffer, "\n");
	strcpy(buffer, token);
	buffer[strlen(token) - 1] = '\0';

	strcpy(request, buffer);


	free(buffer);


}


void pharsing_request_first_line(char *request, char **element_in_first_line){
	char *token;
	int i = 0;

	token = strtok(request, " ");

	while(token != NULL){


		element_in_first_line[i] = malloc((strlen(token) + 1) * sizeof(char));
		memset(element_in_first_line[i], 0, (strlen(token) + 1) * sizeof(char));
		
		strcpy(element_in_first_line[i], token);
		i++;
		token = strtok(NULL, " ");
	}

}

int check_request_status(char **element_in_first_line){
	if(strcmp(element_in_first_line[METHOD], "GET") != 0) return -1;
	if(access(element_in_first_line[PATH], F_OK) == -1) return -1;
	if((strcmp(element_in_first_line[HTTP_VER], "HTTP/1.1") != 0) &&
		(strcmp(element_in_first_line[HTTP_VER], "HTTP/1.0") != 0)) return -1;

	return 0;
}


int check_content_type(char *fname, struct extensions *exts){
	char ext[5], *tmp;
	int i;

	tmp = strrchr(fname, '.');
	if(tmp != 0)
		strcpy(ext, ++tmp);

	for(i = 0; exts[i].ext != NULL; i++){
		if(strcmp(ext, exts[i].ext) == 0) return i;
	}

	return i;
}

int get_content_length(char *fname, char *tmp_buf){
	struct stat st;
	stat(fname, &st);
	if(tmp_buf != NULL)
		sprintf(tmp_buf, "%ld", st.st_size);
	return st.st_size;
}

void get_HTTP_header(char **element_in_first_line, char *buffer){
	char tmp_buf[256] = {0};
	

	//HTTP status
	strcat(buffer, element_in_first_line[HTTP_VER]);
	if(check_request_status(element_in_first_line) == 0){
		strcat(buffer, " 200 OK\r\n");
	}else{		
		strcat(buffer, " 404 Not Found\r\n");
		strcpy(element_in_first_line[PATH], "project_web/not_found.html");
	}



	//Content-type:
	strcat(buffer, "Content-type: ");
	strcat(buffer, exts[check_content_type(element_in_first_line[PATH], exts)].filetype);
	strcat(buffer, "\r\n");



	//Content-Length:
	strcat(buffer, "Content-Length: ");
	get_content_length(element_in_first_line[PATH], tmp_buf);
	strcat(buffer, tmp_buf);
	strcat(buffer, "\r\n");


	//Connection
	strcat(buffer, "Connection: keep-alive\r\n");


	//gzip
	strcat(buffer, "Content-Encoding: gzip\r\n");

	strcat(buffer, "\r\n");

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

int get_compressed_buffer(int r_bytes, char *uncompressed_buffer, char *compressed_buffer){
	int fd, new_r_bytes;

	char gz_fname[32];
	sprintf(gz_fname, "tmp_%d.gz",  (int)pthread_self());

	if(get_compressed_file(r_bytes, uncompressed_buffer, gz_fname) != 0){
		return -1;
	}

	if((fd = open(gz_fname, O_RDONLY)) < 0){
		printf("Error in get_compressed_buffer: open\n");
		return -1;
	}


	if((new_r_bytes = read(fd, compressed_buffer, r_bytes)) < 0){
		printf("Error in get_compressed_file: open %d", new_r_bytes);
		return -1;
	}

	close(fd);
	remove(gz_fname);

	return new_r_bytes;
}

int get_random_num(int fsize){
	if(fsize <= 0)return -1;

	const int RAN_MAX = fsize / 2;
	int res;

	srand(time(NULL));
	return res = 10 + (rand() % RAN_MAX);
}

int get_chunked_buffer(char *chunked_buffer, char *uncompressed_buffer, int chunked_size, int total_sent_bytes, int fsize){
	if(total_sent_bytes == fsize) return -1;

	if(fsize < total_sent_bytes + chunked_size) chunked_size = fsize - total_sent_bytes;

	strncpy(chunked_buffer, &uncompressed_buffer[total_sent_bytes], chunked_size);

	return total_sent_bytes + chunked_size;
}

void chunked_transfer(char *uncompressed_buffer, int fsize, int clifd){
	int chunked_size, w_bytes, total_sent_bytes = 0;
	char *compressed_chunked_buffer, *chunked_buffer, *tmp_buffer;

	while(1){
		//get random number < sending buffer
		if((chunked_size = get_random_num(fsize)) == -1){
			printf("Error in chunked size.\n");
			exit(0);
		}

		compressed_chunked_buffer = malloc(sizeof(char) * chunked_size * 2);
		chunked_buffer = malloc(sizeof(char) * chunked_size * 2);
		tmp_buffer = malloc(size(char) * (1 + chunked_size));

		memset(compressed_chunked_buffer, 0,  chunked_size * 2);
		memset(chunked_buffer, 0, chunked_size * 2);
		memset(tmp_buffer, 0, 1 + chunked_size);


		//copy the string from the previous end pt in compressed_buffer to the pos of random number
		if((total_sent_bytes = get_chunked_buffer(tmp_buffer, uncompressed_buffer, chunked_buffer, total_sent_bytes) == -1){
			return;
		}
		//modify the the string and send
		sprintf(chunked_buffer, "%X\r\n", chunked_size);
		strcat(chunked_buffer, tmp_buffer);
		strcat(chunked_buffer, "\r\n");

		if((w_bytes = get_compressed_buffer(strlen(chunked_buffer), chunked_buffer, compressed_chunked_buffer)) == -1){
			exit(0);
		}
		
		write(clifd, compressed_chunked_buffer, w_bytes);

		free(compressed_chunked_buffer);
		free(chunked_buffer);
		free(tmp_buffer);
	}

}

void *request_handler(void *arg){
	char *request, *header_buffer;
	char *buffer, *compressed_buffer;
	char *element_in_first_line[3];
	int i, fd, r_bytes, compressed_r_bytes, fsize;
	struct Socket_fd socket_fd;
	socket_fd.servfd = ((struct Socket_fd*)arg)->servfd;
	socket_fd.clifd = ((struct Socket_fd*)arg)->clifd;

	request = malloc(sizeof(char) * (REQUEST_SIZE + 1));
	header_buffer = malloc(sizeof(char) * (BUFFER_SIZE + 1));
	memset(request, 0, sizeof(char) * (REQUEST_SIZE + 1));
	memset(header_buffer, 0, sizeof(char) * (BUFFER_SIZE+ 1));


	//Get request
	get_request_first_line(socket_fd.clifd, request);
	
	//process request
	pharsing_request_first_line(request, element_in_first_line);

	//do sth based on process result
	get_HTTP_header(element_in_first_line, header_buffer);



	//send back client
	write(socket_fd.clifd, header_buffer, strlen(header_buffer));


	fd = open(element_in_first_line[PATH], O_RDONLY);

	fsize = get_content_length(element_in_first_line[PATH], NULL);
	buffer = malloc(sizeof(char) * (1 + fsize));
	compressed_buffer = malloc(sizeof(char) * (1 + fsize));

	memset(buffer, 0, (1 + fsize) * sizeof(char));
	memset(compressed_buffer, 0, (1 + fsize) * sizeof(char));


	while(1){

		r_bytes = read(fd, buffer, fsize);	

		if(r_bytes <= 0) {
			break;
		}


		if((compressed_r_bytes = get_compressed_buffer(r_bytes, buffer, compressed_buffer)) < 0){
			exit(0);
		}



		if(fsize > 40960) r_bytes = compressed_r_bytes;
		compressed_r_bytes = write(socket_fd.clifd, compressed_buffer, r_bytes);

	}


	sleep(1);
	close(socket_fd.clifd);
	close(fd);

	free(arg);
	free(buffer);
	free(compressed_buffer);
	free(request);
	free(header_buffer);
/*
	for(i = 0; i < 3; i++){
		free(element_in_first_line[i]);
	}
*/	
	pthread_detach(pthread_self());
}



int main(){
	struct Socket_fd socket_fd, *socket_fd_ptr;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len = sizeof(struct sockaddr_in);
	pthread_t thread;
	int i = 0;


	if((socket_fd.servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("ERROR: init socket\n");
		exit(0);
	}

	memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT);

	if(bind(socket_fd.servfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0){
		printf("ERROR: bind\n");
		exit(0);
	}

	if(listen(socket_fd.servfd, Q_LIMIT) < 0){
		printf("ERROR: listen\n");
		exit(0);
	}
	
	while(1){
		socket_fd_ptr = malloc(sizeof(struct Socket_fd));
		

		if((socket_fd.clifd = accept(socket_fd.servfd, (struct sockaddr *)&cliaddr, &len)) < 0){
			printf("ERROR: accept\n");
			exit(0);
		}

		*socket_fd_ptr = socket_fd;

		if(pthread_create(&thread, NULL, request_handler, (void *)socket_fd_ptr) != 0){
			printf("ERROR: pthread_create\n");
			exit(0);
		}

	}	
	
	return 0;
}