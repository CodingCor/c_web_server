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

#include "include/cookiemap.h"

const unsigned int BACKLOG_QUEUE_SIZE = 5; 
const unsigned int RESPONSE_SIZE = 8000;

void dumpBuffer(char* buffer, unsigned int bufferSize);

enum HTTPMethod{
    GET,
    POST,
};

struct HTTPRequest{
    HTTPMethod method;
    char* path;
    char* versionString;
    bool headerRead;
    HeaderMap cookies;
    char *body;
};

HTTPRequest parseRequest(char *buffer, unsigned int bufferSize);

void sendResponse(int fd);

/*
 * this does not check for memory boundaries
 * */
char* advanceNextWord(char **text, char delimiter);

void debugLogRequest(HTTPRequest request);

const char* response = 
    "HTTP/1.1 200 OK\r\n"
    "\r\n"
;

int main(void){
    //HeaderMap map = createHeaderMap();
    //printf(
    //    "Map created: \n"
    //    "cap: %i\n"
    //    "data address: %p\n",
    //    map.capacity, map.data
    //);

    //insertCookie(&map, {(char*)"Accept-Encoding", (char*)"value1"});
    //insertCookie(&map, {(char*)"Accept-Content", (char*)"value2"});
    //insertCookie(&map, {(char*)"Content-Length", (char*)"value3"});
    //insertCookie(&map, {(char*)"Accept-Types", (char*)"value4"});
    //printf("got cookie: %s \n",getCookie(&map, (char*)"cookie").value);
    //insertCookie(&map, {(char*)"cookie", (char*)"value5"});
    //insertCookie(&map, {(char*)"cookie", (char*)"value6"});

    //printHeaderMapPointer(&map);

    //printf("got cookie: %s \n",getCookie(&map, (char*)"cookie").value);

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

        printf("Connection opened\n");

        char buffer[RESPONSE_SIZE] = "";
        recv(openedfd, buffer, RESPONSE_SIZE, 0);
        //printf("recv: %s", buffer);
        HTTPRequest request = parseRequest(buffer, RESPONSE_SIZE);
        debugLogRequest(request);

        dumpBuffer(buffer, RESPONSE_SIZE);

        // send the response to the client
        sendResponse(openedfd);

        close(openedfd);
    }

    return 0;
}

HTTPRequest parseRequest(char *buffer, unsigned int bufferSize){
    HTTPRequest request = {};
    request.cookies = createHeaderMap();
    char* currPosition = buffer;

    while( currPosition <= (buffer + bufferSize)){

        char *endOfLine = currPosition;
        endOfLine = strchr(currPosition, '\n'); 
        if(endOfLine == NULL) break;

        char *line = currPosition; 
        currPosition = (endOfLine+1);

        *(endOfLine) = 0;
        if(*(endOfLine-1) == '\r'){
            *(endOfLine-1) = 0;
        }
        //printf("Current Parsed Line: %s\n", currPosition);

        if(line == buffer){ // first line

            char* word = 0;
            word = advanceNextWord(&line, ' ');
            if(word == line) return request;

            if(strcmp(word, "GET")){
                request.method = GET; 
            }else if(strcmp(word, "POST")){
                request.method = POST;
            }
            
            if(word == line) return request;

            word = advanceNextWord(&line, ' ');
            request.path = word;
            
            word = advanceNextWord(&line, ' ');
            request.versionString = word;

        }else{ // cookie lines
            if(strlen(line) == 0){
                break;
            }
            char* key = line;
            char* value = strchr(line, ':');
            if(value == NULL) break;
            *value = 0;
            value++;
            insertCookie(&request.cookies, {key, value});
        }
    }

    request.body = currPosition;
    request.headerRead = true;
    return request;
}

char* advanceNextWord(char **text, char delimiter){
    char* word = *text;
    char* endOfWord = strchr(*text, delimiter);
    if(endOfWord != NULL){
        *endOfWord = 0;
        *text = (endOfWord + 1);
    }

    return word;
    
} 

void sendResponse(int fd){
    send(fd, response, strlen(response), 0);

    int filefd = open("index.html", O_RDONLY);
    struct stat fileStat;
    fstat(filefd, &fileStat);
    int fileSize = fileStat.st_size;
    sendfile(fd, filefd, 0, fileSize);
}

void debugLogRequest(HTTPRequest request){
    printf(
        "Request Header: :\n"
        "method: %i\n"
        "path: %s\n"
        "version: %s\n"
        , request.method ,request.path, request.versionString
    );

    // print cookie map
    printf("Cookies: \n");
    HeaderMap node = request.cookies;
    for(unsigned int i = 0; i < node.capacity; i++){
        if(node.data[i].key == NULL) continue;
        printf(" %s : %s \n", node.data[i].key, node.data[i].value); 
    }

    printf("Body: \n");
    printf("%s", request.body);
    printf("\n");
}

void dumpBuffer(char* buffer, unsigned int bufferSize){
    printf("Buffer Dump of %p of length: %i \n", buffer, bufferSize);
    for(unsigned int i = 0; i < bufferSize; i++){
        printf("%c", buffer[i]);
        }
    printf("\n");
}

// TODO: read the complete request 
// currently a fixed size of 8000 bytes is read from the request
// a http request can be longer than 8000 bytes and should be read to completion
// if the request is read in multiple chunks the request should be concatenated
// make a linked list of read chunks
