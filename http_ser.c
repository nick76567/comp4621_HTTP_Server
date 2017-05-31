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


#define BUFFER_SIZE		512
#define REQUEST_SIZE	256
#define HTTP_PORT		80
#define PORT            12345
#define Q_LIMIT			16
#define METHOD			0
#define PATH			1
#define HTTP_VER		2
#define NUM_THREADS		5

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
	char buffer[REQUEST_SIZE], *token;	

	if(read(clifd, buffer, REQUEST_SIZE) <= 0){
		printf("Error in read request\n");
		exit(0);
	}

	//debug
	//printf("%s", buffer);

	token = strtok(buffer, "\n");
	strcpy(buffer, token);
	buffer[strlen(token)] = '\0';

	strcpy(request, buffer);
}


void pharsing_request_first_line(char *request, char **element_in_first_line){
	char *token;
	int i = 0;

	token = strtok(request, " ");

	while(token != NULL){
		element_in_first_line[i] = malloc((strlen(token) + 1) * sizeof(char));
		
		strcpy(element_in_first_line[i++], token);
		token = strtok(NULL, " ");
	}

	element_in_first_line[HTTP_VER][strlen(element_in_first_line[HTTP_VER]) - 1] = '\0';
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
	char tmp_buf[64];

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
	strcat(buffer, "Keep-alive: timeout=15, max=100\r\n");

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
	char gz_fname[] = "tmp.gz";

	if(get_compressed_file(r_bytes, uncompressed_buffer, gz_fname) != 0){
		return -1;
	}

	if((fd = open(gz_fname, O_RDONLY)) < 0){
		printf("Error in get_compressed_buffer: open\n");
		return -1;
	}

	if((new_r_bytes = read(fd, compressed_buffer, r_bytes)) < 0){
		printf("Error in get_compressed_file: open");
		return -1;
	}

	close(fd);
	remove(gz_fname);
	return new_r_bytes;
}

void *request_handler(void *arg){
	char request[REQUEST_SIZE], header_buffer[BUFFER_SIZE];
	char *buffer, *compressed_buffer;
	char *element_in_first_line[3];
	int i, fd, r_bytes, compressed_r_bytes, fsize;
	struct Socket_fd socket_fd;
	socket_fd.servfd = ((struct Socket_fd*)arg)->servfd;
	socket_fd.clifd = ((struct Socket_fd*)arg)->clifd;

	//debug
	int debug_fd;
	char *debug_fname;

	//Get request
	get_request_first_line(socket_fd.clifd, request);
	
	//process request
	pharsing_request_first_line(request, element_in_first_line);


	//do sth based on process result
	get_HTTP_header(element_in_first_line, header_buffer);

	//debug
	printf("%s\n", header_buffer);

	//send back client
	//if(check_content_type(element_in_first_line[PATH], exts) == 9)
	write(socket_fd.clifd, header_buffer, strlen(header_buffer));

	fd = open(element_in_first_line[PATH], O_RDONLY);

	fsize = get_content_length(element_in_first_line[PATH], NULL);
	buffer = malloc(sizeof(char) * fsize);
	compressed_buffer = malloc(sizeof(char) * fsize);

	//debug
/*
	printf("fsize: %d\n", fsize);

	debug_fname = strrchr(element_in_first_line[PATH], '/');
	printf("debug_fname: %s\n", ++debug_fname);
	debug_fd = open(debug_fname, O_CREAT|O_RDWR, 0644);
*/
	while(1){

		r_bytes = read(fd, buffer, fsize);	

		if(get_compressed_buffer(r_bytes, buffer, compressed_buffer) < 0){
			exit(0);
		}

		//debug
		//write(debug_fd, compressed_buffer, compressed_r_bytes);

		if(r_bytes <= 0) {
			break;
		}else{
			write(socket_fd.clifd, compressed_buffer, r_bytes);
		}
		
	}


	sleep(1);
	close(socket_fd.clifd);
	close(fd);

	free(arg);
	free(buffer);
	free(compressed_buffer);
	for(i = 0; i < 3; i++){
		free(element_in_first_line[i]);
	}
	
	pthread_exit(0);
}



int main(){
	struct Socket_fd socket_fd, *socket_fd_ptr;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len = sizeof(struct sockaddr_in);
	pthread_t thread;


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