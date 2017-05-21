#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>

#define MAXLINE 100

int main()
{
        int sockfd, n;
        struct sockaddr_in servaddr;
        char writeline[MAXLINE] = {0};
        char recvline;

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
        snprintf(writeline, sizeof(writeline) - 1, "GET test.html HTTP/1.1\r\n");

        /* send the request */
        write(sockfd, writeline, strlen(writeline));

       
        /* read the response */
        while (1) {

                n = read(sockfd, &recvline, 1);

                if (n <= 0) {
                        break;
                } else {
                              /* 0 terminate */
                        printf("%c", recvline);
                }
        }

        /* close the connection */
        close(sockfd);
        return 0;
}