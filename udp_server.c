#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
///////////////////////////////
#define PORT     8080
#define MAXLINE 1024
//////////////////////////////
int randIsBiggerThanX(double x){
    double randomNum= rand()%10000/10000.0;//random number between 0-1
     if(randomNum<x){
        return 0;
     }
     else return 1;
}

//////////////////////////////
int main(int argc, char *argv[]){
    int sockfd,boolIsThisTheFirstTimeForA=1,boolIsThisTheFirstTimeForC=1;
    char chatServerBuffer[MAXLINE];
    char chatSendingBuffer[MAXLINE];
    double x = atof(argv[1]);//this is the X value we got from the user
    //this struct represents the basic information about the socket connection of both of server and client side
    struct sockaddr_in serverAddr,clientAddr, clientAddrOfA,clientAddrOfC;
    //making sure both server and client 'sockaddr_in' structs are zeroed
    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddrOfA, 0, sizeof(clientAddr));
    memset(&clientAddrOfA, 0, sizeof(clientAddrOfA));
    memset(&clientAddrOfC, 0, sizeof(clientAddrOfC));
///////////////////////////////

   //this will create the socket file descriptor {which is just an int value} - which will allow us to access it {the socket} from the code by addressing this int value
   //AF_INET=> Ip V4 socket, SOCK_DGRAM=> UDP type socket
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { //in case socket command will return negative --> value it means the method failed
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("server's socket created successfully\n");
    // Filling information about the address we will "listen to" as a server
    serverAddr.sin_family= AF_INET; // this means that the address is IP V4 type
    serverAddr.sin_addr.s_addr = INADDR_ANY; // this means that we'll 'work' with all the address of the machine
    serverAddr.sin_port = htons(PORT); // h-to-ns => "translate" the byte order of this short to Big Endian, and use it as the port

    // Bind the socket with the server address we built=> the fd of the socket we created, the serverAddr struct we configured casted to sockaddr, and the length of this struct
    // if return value of 'bind' is negative => the method failed
    if ( bind(sockfd, (const struct sockaddr *)&serverAddr,
            sizeof(serverAddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("the server is now 'listening' to the socket\n");


    int len,n;
    len = sizeof(clientAddr);
    printf("waiting for both node A and C to connect the chat\n");
    while(boolIsThisTheFirstTimeForA==1||boolIsThisTheFirstTimeForC==1){
      n = recvfrom(sockfd, (char *)chatServerBuffer, MAXLINE,
                  0, ( struct sockaddr *) &clientAddr,
                  &len);
      chatServerBuffer[n] = '\0'; //indicating the end of this msg recived
      if(chatServerBuffer[0]=='A'){//in case we got the data from node A
         if(boolIsThisTheFirstTimeForA){//if this is the first time we got a msg from node A -> save its network values
            clientAddrOfA.sin_family = AF_INET;
            clientAddrOfA.sin_addr.s_addr = clientAddr.sin_addr.s_addr;
            clientAddrOfA.sin_port = clientAddr.sin_port;
            boolIsThisTheFirstTimeForA = 0;
            printf("node A is connected\n");
         }
      }
      else{
         if(boolIsThisTheFirstTimeForC){//if this is the first time we got a msg from node C -> save its network values
            clientAddrOfC.sin_family = AF_INET;
            clientAddrOfC.sin_addr.s_addr = clientAddr.sin_addr.s_addr;
            clientAddrOfC.sin_port = clientAddr.sin_port;
            boolIsThisTheFirstTimeForC = 0;
            printf("node C is connected\n");
         }
    }
    }
    fflush(stdout);
    for(;;){
    memset(chatServerBuffer, 0, sizeof(chatServerBuffer));
    memset(chatSendingBuffer, 0, sizeof(chatSendingBuffer));
    memset(&clientAddr,0,sizeof(clientAddr));
    printf("waiting for data\n");
    //this will BLOCK the code, waiting for data to be recived,input=> the socket fd, the buffer to which the method will allocate the data, the maximum size of data we can read in once, 'flags' which
    // will be just zeroed, a pointer to the struct we want to be filled with 'senders' net information and the size of this struct
    n = recvfrom(sockfd, (char *)chatServerBuffer, MAXLINE,
                  0, ( struct sockaddr *) &clientAddr,
                  &len);
    if(strcasecmp(chatServerBuffer,"exit")==0){
       printf("exit request has been recived ---->shutting down :)\n");
       close(sockfd);
       return 0;//'exit' the main i.e. shut down the server
    }
    chatServerBuffer[n] = '\0'; //indicating the end of this msg recived
    if(clientAddr.sin_port==clientAddrOfA.sin_port){//in case we got the data from node A
       printf("node A: %s\n", chatServerBuffer);
       if(randIsBiggerThanX(x)){//check if a random number is between 0 to 1 is bigger than X -> if it is send the msg to node C if its not, print a mg and start a new iteration
          strcpy( chatSendingBuffer,chatServerBuffer);
          sendto(sockfd, (char*)chatSendingBuffer,n,0,( struct sockaddr *) &clientAddrOfC,len);//send to node c
          printf("msg sent to node C\n");
          fflush(stdout);
       }
       else{
             printf("random number generated was bigger than X, msg from A dropped\n");
             strcpy( chatSendingBuffer,"msg from A dropped by server!!!!!");
             sendto(sockfd, (char*)chatSendingBuffer,n,0,( struct sockaddr *) &clientAddrOfC,len);//send to node c
             
       }
    }
    else{//in case we got the data from node C
       printf("node C: %s\n", chatServerBuffer);
       strcpy( chatSendingBuffer,chatServerBuffer);
       sendto(sockfd, (char*)chatSendingBuffer,n,0,( struct sockaddr *) &clientAddrOfA,len);//send to node a
       printf("msg sent to node A\n");
       fflush(stdout);
    }
    }


}

