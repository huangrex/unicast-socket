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
//argv1 is port number
//argv2 is file name
int main(int argc, char*argv[]){
    int sockfd, newsockfd;
    char sendbuff[BUFFSIZE+10], recbuff[BUFFSIZE+10];
    struct sockaddr_in client_addr, server_addr;
    int portnu = atoi(argv[1]);
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("failed on socket");
        exit(0);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portnu);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //bind 
    
    if( bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("failed on binding\n");
        exit(1);
    }
   
    
    while(1){
         //listen
        listen(sockfd, 5);
        socklen_t clilen = sizeof(client_addr);
        newsockfd = accept(sockfd, (struct sockaddr*)&client_addr, &clilen);
        if(newsockfd < 0){
            perror("filed on building newsock\n");
            exit(1);
        }
        
        //write file name
    
        int w1 = write(newsockfd, argv[2], strlen(argv[2]));
        if(w1 < 0){
            perror("failed to send file name\n");
            exit(1);
        }
        
        //send file size
        FILE *in;
        in = fopen(argv[2], "rb");
        struct stat s;
        stat(argv[2], &s);
        long long int sz = s.st_size;
        
        write(newsockfd, (char*)&sz, sizeof(sz) );
        //send file
        int check = 0; // the datagram size
        int count  = 0;
        printf("%lld\n", sz);
        while(1){
            
            bzero(sendbuff, sizeof(sendbuff));
            
            check = fread(sendbuff, 1, BUFFSIZE, in);
            
            
            if(check == 0)break;
            
            write(newsockfd, sendbuff, check);
            
        }
        printf("mission complete\n");
        close(newsockfd);
        fclose(in);
    }
    return 0;
}