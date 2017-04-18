#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFSIZE 666
#define SERV_PORT 8888

int main()
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    char buf[BUFSIZE];
    char str[INET_ADDRSTRLEN];
    int i, n;
    pid_t pid;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);  //打开一个网络通讯端口，分配一个文件描述符listenfd

    bzero(&servaddr, sizeof(servaddr));          //清空服务端套接字
    servaddr.sin_family = AF_INET;               //地址采用IPv4地址 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//地址从主机字节顺序转换成网络字节顺序
    servaddr.sin_port = htons(SERV_PORT);        //端口号从主机字节顺序转换成网络字节顺序
    /*将文件描述符listenfd和服务器地址绑定在一起*/
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    /*声明listenfd处于监听状态,链接个数限制为5*/
    listen(listenfd, 20);
    printf("等待连接....\n");

    while(1){
        cliaddr_len = sizeof(cliaddr);
        /*connfd文件描述符用于和客户端通信，服务端调用accept()接受连接*/
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddr_len);
		if(connfd < 0)
		{
			perror("connd error");
			continue;
		}
        pid = fork();
        if(pid == 0){  //创建一个子进程处理数据
            close(listenfd);//关闭子进程端的监听        
            while(1){
                /*从connfd文件描述符指向的文件读取数据*/
                n  = read(connfd, buf, BUFSIZE);
                if(n == 0){
                    printf("另一端已关闭\n");
                    break;
                }
                /*服务端打印*/
                printf("从地址%s 端口号%d 接收到数据\n", 
                        (char*)inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
                        ntohs(cliaddr.sin_port));

                /*将buf转化为大写后写入connfd*/        
                for(i = 0; i < n;i++ ){
                    buf[i] = toupper(buf[i]);
                }    
                write(connfd, buf, n);
            }    
            close(connfd);//处理完毕关闭连接
            exit(0);
        }else if(pid > 0){ //父进程
			printf("in main process\n");
			close(connfd);//任务交由子进程处理，父进程关闭
        }else{
            printf("fork err\n");
        }

        if(n == 0 ||  n == -1){
            break;
        }
    }
    return 0;
}
