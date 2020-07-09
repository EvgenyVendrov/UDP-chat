#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
/////////////////////////
#define PORT     8080 
#define MAXLINE 1024 
/////////////////////////
int main(int argc, char *argv[]) { 
    int sockfd; 
    char nodeCBuffer[MAXLINE]; 

/////////////////////////
    struct sockaddr_in  serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
/////////////////////////

 

   // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 

printf("socket created!\n");


// Filling server information 
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_port = htons(PORT); 
    serverAddr.sin_addr.s_addr = INADDR_ANY; 

printf("socket connected!\n");

//connecting (logiclly only of course as we work with UDP socket) to the server  
    int n, len=sizeof(serverAddr); 
    strcpy(nodeCBuffer,"C");  
    sendto(sockfd, (const char *)nodeCBuffer, strlen(nodeCBuffer), 
        0, (const struct sockaddr *) &serverAddr,  
            len); 
    fflush(stdout);
    printf(" 'connect C' was sent to server\n"); 



    for(;;){ 
    printf("please enter your msg\n");
    scanf(" %[^\n]s", nodeCBuffer);
    sendto(sockfd,&nodeCBuffer,sizeof(nodeCBuffer),0,(struct sockaddr *)&serverAddr,
                  len);
    fflush(stdout);
   printf("sent msg to server: '%s'\n",nodeCBuffer);
   n = recvfrom(sockfd, (char *)nodeCBuffer, MAXLINE,  
                0, (struct sockaddr *) &serverAddr, 
                &len); 
    nodeCBuffer[n] = '\0'; 
    printf("Server : %s\n", nodeCBuffer);
}

}
