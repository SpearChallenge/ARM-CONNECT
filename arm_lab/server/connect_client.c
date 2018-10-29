#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern int client;
int server;
int connect_client(){
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.10");

    struct sockaddr_in client_addr;
    int client_addr_size = sizeof(client_addr);
    if(-1 == (server = socket(AF_INET, SOCK_STREAM, 0))){
        printf("server socket error");
        return -1;
    }
    if(-1 == bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr))){    
        printf("server bind error");
        return -1;
    }
    if(-1 == listen(server, 6)){
        printf("server listen error");
        return -1;
    }
    if(-1 == (client = accept(server, (struct sockaddr*)&client_addr, &client_addr_size))){
        printf("client accept error");
        return -1;
    }
    return 0;
}
