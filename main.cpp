#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include <pthread.h>

#include "include/cookiemap.h"
#include "include/http.h"
#include "include/util.h"

const unsigned int BACKLOG_QUEUE_SIZE = 5; 
const unsigned int REQUEST_SIZE = 8000;

void* handleConnection(void* openedfd);

int main(void){
    int socketfd = 0;
    int openedfd = 0;
    sockaddr_in address = {}; 

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if(socketfd == -1){
        printf("Error while creating socket\n");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr = {INADDR_ANY};

    if(bind(socketfd, (sockaddr*)&address, sizeof(address)) == -1){
        printf("Error while binding to and adress. Error Number: %i\n", errno);
        return 1;
    }

    if(listen(socketfd, BACKLOG_QUEUE_SIZE) == -1){
        printf("Error listening to the specifed port\n");
        return 1;
    }

    while(1){
        printf("Waiting for connection...\n");
        openedfd = accept(socketfd, 0, 0);

        if(openedfd == -1){
            printf("Connection was refused\n");
            continue;
        }

        pthread_t thread; 
        pthread_create(&thread, NULL, handleConnection, (void*)&openedfd);
    }

    pthread_exit(NULL);

    return 0;
}

void* handleConnection(void* vargs){
    unsigned int *openedfd = (unsigned int*)vargs;
    printf("Connection opened in thread %li \n", pthread_self());
    char *buffer  = (char*)malloc(REQUEST_SIZE);
    recv(*openedfd, buffer, REQUEST_SIZE, 0);
    HTTPRequest request = parseRequest(buffer, REQUEST_SIZE);

    HTTPResponse responseFromRequest = handleRequest(request);

    //debugLogResponse(responseFromRequest);

    sendHttpResponse(*openedfd, responseFromRequest);

    free(buffer);
    free(request.cookies.data);
    free(responseFromRequest.body);
    close(*openedfd);

    sleep(10);
    return NULL;
}
// TODO: read the complete request 
// currently a fixed size of 8000 bytes is read from the request
// a http request can be longer than 8000 bytes and should be read to completion
// if the request is read in multiple chunks the request should be concatenated
// make a linked list of read chunks
