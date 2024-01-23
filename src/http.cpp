#include "../include/http.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/sendfile.h>

const char* response = 
    "HTTP/1.1 200 OK\r\n"
    "\r\n"
;

const char* HTTP_VERSION = "HTTP/1.1";

/*
 * this does not check for memory boundaries
 * */
char* advanceNextWord(char **text, char delimiter);

void sendResponse(int fd){
    send(fd, response, strlen(response), 0);

    int filefd = open("index.html", O_RDONLY);
    struct stat fileStat;
    fstat(filefd, &fileStat);
    int fileSize = fileStat.st_size;
    sendfile(fd, filefd, 0, fileSize);
}

void sendHttpResponse(int fd, HTTPResponse response){
    char header[300] = "";
    snprintf(
        header, 300, 
        "%s %i %s\r\n"
        "Content-Length: %i\r\n"
        "Content-Type: text/html\r\n"
        "\r\n", 
        response.versionString, response.statusCode, response.statusMessage, response.bodyLength);
    unsigned int headerSize = strlen(header);
    
    send(fd, header, headerSize, 0);
    send(fd, response.body, response.bodyLength, 0);
}


HTTPRequest parseRequest(char *buffer, unsigned int bufferSize){
    HTTPRequest request = {};
    request.cookies = createHeaderMap();
    char* currPosition = buffer;

    while( currPosition <= (buffer + bufferSize)){

        char *line = advanceNextWord(&currPosition, '\n');
        char *endOfLine = currPosition;
        if(line == currPosition) break;

        if(*(endOfLine-2) == '\r'){
            *(endOfLine-2) = 0;
        }

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

            char *value = line;
            char* key = advanceNextWord(&value, ':');
            if(key == value || value == NULL) break;
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

char* getHttpStatusMessage(unsigned int statusCode){
    if(statusCode == 200) return (char*)"OK";
    if(statusCode == 500) return (char*)"ERROR";
    return (char*) "ERROR";
}

HTTPResponse errorResponse(){
    HTTPResponse response = {};
    response.statusCode = 500;
    response.statusMessage = getHttpStatusMessage(response.statusCode);
    response.versionString = (char*)HTTP_VERSION;
    return response;
}
HTTPResponse handleRequest(HTTPRequest request){
    HTTPResponse response = {};
    if(request.headerRead == false){
        return errorResponse();
    }
    if(strcmp(request.versionString, HTTP_VERSION) != 0){
        return errorResponse();
    }
    
    response.statusCode = 200;
    response.versionString = (char*)HTTP_VERSION;
    response.statusMessage = getHttpStatusMessage(response.statusCode);

    int fd = -1;
    char filename[256] = "";
    if(strcmp(request.path, "/") == 0 || request.path[0] != '/'){
        strcpy(filename, "./index.html");
    }else{
        strcpy(filename, "."); 
        strcat(filename, request.path);
    }
    fd = open(filename, O_RDONLY);

    if(fd == -1) return errorResponse();

    struct stat fileStat;
    fstat(fd, &fileStat);

    unsigned int fileSizeInBytes = fileStat.st_size;

    FILE* file = fdopen(fd, "r");

    char *fileContent = (char*)malloc(fileSizeInBytes);

    fread(fileContent, sizeof(char), fileSizeInBytes, file);
    fclose(file);

    response.body = fileContent;
    response.bodyLength = fileSizeInBytes;

    close(fd);

    return response;
}
