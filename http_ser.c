#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>

#define BUFFER_SIZE		4096
#define REQUEST_SIZE	512
#define HTTP_PORT		80
#define Q_LIMIT			16

//reference: http://fred-zone.blogspot.hk/2007/09/http-web-server.html
struct {
    char *ext;
    char *filetype;
} extensions [] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
    {"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"exe","text/plain" },
    {0,0}
  };

void init_servaddr(struct sockaddr_in *servaddr){
	servaddr->sin_family = AF_INET;
	servaddr->sin_addr.s_addr = INADDR_ANY;
	servaddr->sin_port = htons(HTTP_PORT);
}

int get_request_first_line(int servfd, char *request){
	int r_bytes = 0;
	char *tmp_buf = request;
	while(1){
		read(servfd, tmp_buf++, 1);
		if(request[r_bytes++] == '\r') break;
	}
	return r_bytes;
}

int get_HTTP_status(char *request, char *buffer){

} 

void get_HTTP_header(char *request, char *buffer){
	//HTTP status
	//Content-Length:
	//Content-type:
	//Connection
}

void request_handler(int servfd, int clifd){
	char request[REQUEST_SIZE], buffer[BUFFER_SIZE];

	//Get request
	if(get_request_first_line(servfd, request) <= 0){
		printf("ERROR: read from request\n");
		exit(0);
	}

	printf("%s\n", request);
	//process request
	//do sth based on process result
	//send back client
}


int main(){
	int servfd, clifd;
	struct sockaddr_in servaddr, cliaddr;

	if( servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("ERROR: init socket\n");
		exit(0);
	}

	init_servaddr(&servaddr);

	if(bind servfd, (struct  sockaddr *)&servaddr, sizeof(struct servaddr)) < 0){
		printf("ERROR: bind\n");
		exit(0);
	}

	if(listen servfd, Q_LIMIT) < 0){
		printf("ERROR: listen\n");
		exit(0);
	}
	
	while(1){
		if((clifd = accept servfd, (struct sockaddr *)&cliaddr, sizeof(struct cliaddr)) < 0){
			printf("ERROR: accept\n");
			exit(0);
		}


	}	
	

	return 0;
}
