//////////////////////////
//实现一个简易的HTTP服务器
//功能：不管输入什么向客户
//端返回hello world 网页
//传输层使用 TCP 协议
//////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int ServerInit(const char* ip, short port)
{
    int64_t fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = inet_addr(ip);
    
    if(bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        perror("bind");
        return -1;
    }
    return fd;
}

void* Service(void * argc)
{
    int64_t new_sock = (int64_t)argc;
    //1.
    //读取并且解析请求
    //返回hello world
    //解析过程就省略了
    char buf[1024*10] = {0};
    read(new_sock, buf, sizeof(buf) - 1);
    printf("[Request]:%s\n", buf);
    //2.
    //构造一相应字符串，并且写回客户端
    const char* first_line = "HTTP/1.1 200 ok\n";
    const char* blank_line = "\n";
    const char* body = "<html>Made By LiuZhengKai</html>";
    char header[1024*10] = {0};
    sprintf(header, "Content-Type: text/html;\nContent-Length: %lu\n", strlen(body));
    write(new_sock, first_line, strlen(first_line));
    write(new_sock, header, strlen(header));
    write(new_sock, blank_line, strlen(blank_line));
    write(new_sock, body, strlen(body));
    return NULL;
}

int main(int argc, char* argv[])
{
   if(argc < 3)
   {
       printf("Please Input With IP and Port!!\n");
       return 1;
   }
   int64_t listen_sock = ServerInit(argv[1], atoi(argv[2]));
   if(listen_sock < 0)
   {
    printf("ServerInit Error!!\n");
    return 2;
   }
   printf("ServerInit Suceessfully!!\n");
    
   listen(listen_sock, 10);

   while(1){
        struct sockaddr_in client;
        int64_t new_sock;
        socklen_t len = sizeof(client);
        pthread_t tid;
        new_sock = accept(listen_sock, (struct sockaddr*)&client, &len);
        if(new_sock < 0)
        {
            perror("accept");
            continue;
        }
        pthread_create(&tid, NULL, Service, (void *)new_sock);
        pthread_detach(tid); 
   }
}


