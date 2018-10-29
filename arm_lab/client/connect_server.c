#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

extern int client;
int connect_server(){
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(10000);
    client_addr.sin_addr.s_addr = inet_addr("192.168.1.10");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;        
    server_addr.sin_port = htons(10000);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.10");
    
    if(-1 == (client = socket(AF_INET, SOCK_STREAM, 0))){
        printf("client socket error");
        return -1;
    }
    if(-1 == bind(client, (struct sockaddr*)&client_addr, sizeof(client_addr))){    
        printf("client bind error");
        return -1;
    }
    if(-1 == connect(client, (struct sockaddr*)&server_addr, sizeof(server_addr))){
        printf("client connect error");
        return -1;
    }
    return 0;
}
