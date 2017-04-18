#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFSIZE 666
#define SERV_PORT 8888

int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr;
    char buf[BUFSIZE];
    int sockfd, n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);
    /*连接服务器*/
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    while(fgets(buf, BUFSIZE, stdin) != NULL){
        write(sockfd, buf, strlen(buf));
        /*接收服务端数据*/
        n = read(sockfd, buf, BUFSIZE);
        if(n == 0){
            printf("server closed\n");
        }else{
            /*打印输出buf*/
            write(STDOUT_FILENO, buf, n);
        }
    }

    close(sockfd);
    return 0;
}
