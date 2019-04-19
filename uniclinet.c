#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <pthread.h>
#define BUFFSIZE 256
//argv1 is ip address
//argv2 is port number
int main(int argc, char *argv[]){
    int sockfd, recfd;
    char sendbuff[BUFFSIZE], recbuff[BUFFSIZE];
    struct sockaddr_in client_addr, server_addr;
    int portnu = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("failed to open socket");
        exit(0);
    }
    else printf("opening datagram socket........OK\n");
    //memset(server_addr, 0, sizeof(server_addr));
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(portnu);
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("connection failed");
        printf("waiting for connection\n");
        while(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)));
    }


    //data name transfer
    socklen_t serlen;
    serlen = sizeof(server_addr); 
    char name[BUFFSIZE];
    memset(name, 0, sizeof(name));
    int r1 = read(sockfd, name, BUFFSIZE);
    if(r1 < 0){
        perror("failed on receive file name");
        exit(0);  
    }
    printf("%s\n", name);
    
    //open outputfile
    FILE *out;
    out = fopen(name, "ab");
    long long int sz;
    int r2 = read(sockfd, (char*)&sz, sizeof(sz));
    if(r2 < 0){
        perror("failed on receive data size");
        exit(0);
    }
    printf("%lld\n", sz);
    int count = 0, check = 0;
    while(1){
        memset(recbuff, 0, sizeof(recbuff));
        check = read(sockfd, recbuff, BUFFSIZE);
        //no data
        if(check == 0)break;
        //data count size
        if(check > 0){
            count += check;
            printf("%d\n", count);
            fwrite(recbuff, sizeof(char), check, out);
            
        }
        
       
    }
    
    printf("mission completed\n");
    fclose(out);
    
    close(sockfd);
    return 0;
}