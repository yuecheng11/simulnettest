#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>     
#include <sys/socket.h>
#include <unistd.h>

#define BUFSIZE 666
#define SERV_PORT 8888

int main(int argc, char*argv[])
{
    struct sockaddr_in servaddr;
    char buf[BUFSIZE];
    int sockfd, n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
    servaddr.sin_port = htons(SERV_PORT);
    /*连接*/
    connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    while(fgets(buf, BUFSIZE, stdin) != NULL){
        /*向通信sockfd写入数据buf*/
        write(sockfd, buf, strlen(buf));
        /*接收服务端数据*/
        n = read(sockfd, buf, BUFSIZE);
        if(n == 0){
            printf("服务器已关闭\n");
        }else{
            write(STDOUT_FILENO, buf, n);
        }
    }
    close(sockfd);
    return 0;
}
