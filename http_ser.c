#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>

#define BUFFER_SIZE		4096
#define REQUEST_SIZE	512
#define HTTP_PORT		80
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
    {"css", "text/css"},
    {"exe","text/plain" },
    {0,"Type not support."}
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
		printf("%s", tmp_buf);
		if(request[r_bytes++] == '\r') break;
	}
	return r_bytes;
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
	char ext[5];
	int i;

	strcpy(ext, strrchr(fname, '.') + 1);

	for(i = 0; exts[i].ext != NULL; i++){
		if(strcmp(ext, exts[i].ext) == 0) return i;
	}

	return i;
}

void get_content_length(char *fname, char *tmp_buf, int buffer_size){
	struct stat st;
	stat(fname, &st);
	sprintf(tmp_buf, "%ld", (st.st_size + buffer_size));
}

void get_HTTP_header(char **element_in_first_line, char *buffer){
	char tmp_buf[64];
	//HTTP status
	strcat(buffer, element_in_first_line[HTTP_VER]);
	if(check_request_status(element_in_first_line) == 0){
		strcat(buffer, " 200 OK\r\n");
	}else{		
		strcat(buffer, " 404 Not Found\r\n");
	}
	//Content-type:
	strcat(buffer, "Content-type: ");
	strcat(buffer, exts[check_content_type(element_in_first_line[PATH], exts)].filetype);
	strcat(buffer, "\r\n");
	//Content-Length:
	strcat(buffer, "Content-Length: ");
	get_content_length(element_in_first_line[PATH], tmp_buf, strlen(buffer));
	strcat(buffer, tmp_buf);
	strcat(buffer, "\r\n");
	//Connection
	strcat(buffer, "\r\n\r\n");
	
}

void request_handler(int servfd, int clifd){
	char tmp_buf, request[REQUEST_SIZE], buffer[BUFFER_SIZE];
	char *element_in_first_line[3];
	int i, fd;


	//Get request
	if(get_request_first_line(clifd, request) <= 0){
		printf("ERROR: read from request\n");
		exit(0);
	}
	

	//process request
	pharsing_request_first_line(request, element_in_first_line);
	
	//do sth based on process result
	get_HTTP_header(element_in_first_line, buffer);

	//send back client
	write(clifd, buffer, strlen(buffer) + 1);
	

	fd = open(element_in_first_line[PATH], O_RDONLY);
	while(read(fd, &tmp_buf, 1)){

		write(clifd, &tmp_buf, 1);
	}

	close(fd);

	for(i = 0; i < 3; i++){
		free(element_in_first_line[i]);
	}
	
	//debug
	
}

/*
int main(){
	int fd = open("test.txt", O_RDONLY);
	request_handler(fd, 0);
	return 0;
}
*/

int main(){
	int servfd, clifd;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len = sizeof(struct sockaddr_in);
	//
	char buffer;

	if((servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("ERROR: init socket\n");
		exit(0);
	}

	//init_servaddr(&servaddr);
	memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY; /* IP address: 0.0.0.0 */
        servaddr.sin_port = htons(12346);

	if(bind(servfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr)) < 0){
		printf("ERROR: bind\n");
		exit(0);
	}

	if(listen(servfd, Q_LIMIT) < 0){
		printf("ERROR: listen\n");
		exit(0);
	}
	
	while(1){
		if((clifd = accept(servfd, (struct sockaddr *)&cliaddr, &len)) < 0){
			printf("ERROR: accept\n");
			exit(0);
		}

		
		request_handler(servfd, clifd);

		close(servfd);
	}	
	

	return 0;
}
