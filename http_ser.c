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


#define BUFFER_SIZE		4096
#define REQUEST_SIZE	512
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

/*
int get_request_first_line(int clifd, char *request){
	int r_bytes = 0, n;
	char *tmp_buf = request;
//debug

	char debug_buf[4096];
	n = read(clifd, debug_buf, 4096);
	debug_buf[n] = 0;
	printf("%s\n", debug_buf);

	while(1){
		//debug
		printf("test\n");

		read(clifd, tmp_buf++, 1);

		//debug
		printf("test2\n");

		if(request[r_bytes++] == '\r') break;
	}
//debug
	
	request[r_bytes] = 0;
	printf("%s\n", request);
	
	return r_bytes;
}
*/
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

void get_content_length(char *fname, char *tmp_buf){
	struct stat st;
	stat(fname, &st);
	sprintf(tmp_buf, "%ld", st.st_size);
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

	strcat(buffer, "\r\n");
}

void *request_handler(void *arg){
	char request[REQUEST_SIZE], buffer[BUFFER_SIZE];
	char *element_in_first_line[3];
	int i, fd, r_bytes;
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
	get_HTTP_header(element_in_first_line, buffer);

	//debug
	printf("%s\n", buffer);

	//send back client
	//if(check_content_type(element_in_first_line[PATH], exts) == 9)
		write(socket_fd.clifd, buffer, strlen(buffer));

	fd = open(element_in_first_line[PATH], O_RDONLY);

	//debug
/*
	debug_fname = strrchr(element_in_first_line[PATH], '/');
	printf("debug_fname: %s\n", ++debug_fname);
	debug_fd = open(debug_fname, O_CREAT|O_RDWR, 0644);
*/
	while(1){
		
		r_bytes = read(fd, buffer, BUFFER_SIZE);	
		//debug
		//write(debug_fd, buffer, r_bytes);

		if(r_bytes <= 0) {
			break;
		}else{
			write(socket_fd.clifd, buffer, r_bytes);
		}
		
	}


	sleep(1);
	close(socket_fd.clifd);
	close(fd);

	free(arg);
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




