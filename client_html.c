#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#define MAXLINE 100
#define BUFFER_SIZE     4096

int main()
{
        int sockfd, n, fd;
        struct sockaddr_in servaddr;
        char writeline[MAXLINE] = {0};
        char recvline[BUFFER_SIZE];
    

        /* init socket */
        sockfd = socket(AF_INET, SOCK_STREAM, 0);       /* TCP */
        if (sockfd < 0) {
                printf("Error: init socket\n");
                return 0;
        }

        /* init server address (IP : port) */
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(12346);

        /* connect to the server */
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
                printf("Error: connect\n");
        }

        /* constrct the request */
        snprintf(writeline, sizeof(writeline) - 1, "GET project_web/indx.html HTTP/1.1\r\n");

      
        /* send the request */
        write(sockfd, writeline, strlen(writeline));
      
        fd = open("index.html", O_CREAT|O_RDWR, 0644);
        if(fd < 0){
            printf("Error in open\n");
        }
        /* read the response */
        while (1) {        
            n = read(sockfd, recvline, BUFFER_SIZE);
            
            //printf("n: %d\n", n);
            if(n <= 0) break;
            write(fd, recvline, n);
            
        }

        /* close the connection */
        close(sockfd);
        close(fd);
        return 0;
}