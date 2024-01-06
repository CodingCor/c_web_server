#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

const unsigned int BACKLOG_QUEUE_SIZE = 5; 
const unsigned int RESPONSE_SIZE = 8000;

const char* response = 
    "HTTP/1.1 200 OK\r\n"
    "\r\n"
;

int main(void){
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    int openedfd = 0;
    int filefd = 0;

    sockaddr_in address = {
        AF_INET,
        htons(8080),
        {INADDR_ANY},
        {0} // padding
    }; 

    if(bind(socketfd, (sockaddr*)&address, sizeof(address)) == -1){
        printf("Error while binding to and adress. Error Number: %i", errno);
        return 1;
    }

    if(listen(socketfd, BACKLOG_QUEUE_SIZE) == -1){
        printf("Error listening to the specifed port");
        return 1;
    }

    while(1){
        printf("Waiting for connection...");
        openedfd = accept(socketfd, 0, 0);

        if(openedfd == -1){
            printf("Connection was refused");
            continue;
        }

        printf("Connection opened" );

        char buffer[RESPONSE_SIZE] = "";
        recv(openedfd, buffer, RESPONSE_SIZE, 0);
        printf("recv: %s", buffer);

        send(openedfd, response, strlen(response), 0);

        filefd = fopen("index.html", "r");

        close(openedfd);
    }

    return 0;
}

// TODO: read the complete request 
// currently a fixed size of 8000 bytes is read from the request
// a http request can be longer than 8000 bytes and should be read to completion
// if the request is read in multiple chunks the request should be concatenated
// make a linked list of read chunks
