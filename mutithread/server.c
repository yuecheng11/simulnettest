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
#define MAX 200 //本例线程最大个数设为200

/*与服务端通信的客户端地址及通信文件描述符的信息结构*/
struct s_info{
    struct sockaddr_in cliaddr;
    int connfd;
};

void *work(void *arg)
{
    int n, i;
    struct s_info *ts = (struct s_info*)arg;
    char buf[BUFSIZE];
    char str[INET_ADDRSTRLEN];
    /*线程资源回收*/
    pthread_detach(pthread_self());
    while(1){
        /*读取数据*/
        n = read(ts->connfd, buf, BUFSIZE);
        if(n == 0 ){
            printf("另一端已关闭\n");
            break;
        }
        printf("从地址%s端口%d接收到数据\n", 
                inet_ntop(AF_INET, &(*ts).cliaddr.sin_addr, str, sizeof(str)),
                ntohs((*ts).cliaddr.sin_port));

        for(i = 0; i<n; i++)
            /*小写转为大写*/
            buf[i] = toupper(buf[i]);
        /*将buf写入文件描述符ts->connfd*/
        write(ts->connfd, buf, n);
    }
    close(ts->connfd);
    return;
}

int main(void)
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    int i = 0;
    pthread_t tid;/*线程id*/

    struct s_info ts[MAX];
    /*创建通信socket*/
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    /*socket绑定服务端地址*/
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    /*监听*/
    listen(listenfd, 20);
    printf("等待连接：\n");
    while(1){
        cliaddr_len = sizeof(cliaddr);
        /*接收客户端请求*/
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddr_len);
		if(connfd < 0)
		{
			perror("accept error");
			continue;
		}
        ts[i].cliaddr = cliaddr;
        ts[i].connfd = connfd;
        /*创建线程处理客户端请求*/
        pthread_create(&tid, NULL, work, (void*)&ts[i]);
        i++;
    }
    return 0;
}
