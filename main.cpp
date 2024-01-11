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

const unsigned int BACKLOG_QUEUE_SIZE = 5; 
const unsigned int RESPONSE_SIZE = 8000;

enum HTTPMethod{
    GET,
    POST,
};

struct StringNode{
    StringNode*  nextNode;
    char* text;
};

struct HTTPRequest{
    HTTPMethod method;
    char* path;
    char* versionString;
    bool headerRead;
    StringNode* cookies;
};

StringNode* prependToStringList(StringNode* node, char* text);

HTTPRequest parseRequest(char *buffer, unsigned int bufferSize);

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
    int socketfd = 0;
    int openedfd = 0;
    int filefd = 0;
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

        send(openedfd, response, strlen(response), 0);

        filefd = open("index.html", O_RDONLY);
        struct stat fileStat;
        fstat(filefd, &fileStat);
        int fileSize = fileStat.st_size;
        sendfile(openedfd, filefd, 0, fileSize);

        close(openedfd);
    }

    return 0;
}

HTTPRequest parseRequest(char *buffer, unsigned int bufferSize){
    HTTPRequest request = {};
    char* currPosition = buffer;

    while( currPosition <= (buffer + bufferSize)){

        char *endOfLine = currPosition;
        endOfLine = strchr(currPosition, '\n'); 
        if(endOfLine == NULL) break;

        *(endOfLine) = 0;
        if(*(endOfLine-1) == '\r'){
            *(endOfLine-1) = 0;
        }
        //printf("Current Parsed Line: %s\n", currPosition);

        if(currPosition == buffer){ // first line

            char* word = 0;
            word = advanceNextWord(&currPosition, ' ');
            if(word == currPosition) return request;

            if(strcmp(word, "GET")){
                request.method = GET; 
            }else if(strcmp(word, "POST")){
                request.method = POST;
            }
            
            if(word == currPosition) return request;

            word = advanceNextWord(&currPosition, ' ');
            request.path = word;
            
            word = advanceNextWord(&currPosition, ' ');
            request.versionString = word;

            request.headerRead = true;
        }else{ // cookie lines
            request.cookies = prependToStringList(request.cookies, currPosition); 
        }
        currPosition = (endOfLine+1);
    }
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

StringNode* prependToStringList(StringNode* node, char* text){
    StringNode* firstNode = (StringNode*)malloc(sizeof(StringNode));
    firstNode->nextNode = node;
    firstNode->text = text;
    return firstNode;
}

void debugLogRequest(HTTPRequest request){
    printf(
        "Request Header: :\n"
        "method: %i\n"
        "path: %s\n"
        "version: %s\n"
        , request.method ,request.path, request.versionString
    );

    StringNode* node = request.cookies;
    while(node != NULL){
        printf("Cookie: %s\n", node->text);
        node = node->nextNode;
    }
    printf("\n");
}

// TODO: read the complete request 
// currently a fixed size of 8000 bytes is read from the request
// a http request can be longer than 8000 bytes and should be read to completion
// if the request is read in multiple chunks the request should be concatenated
// make a linked list of read chunks
