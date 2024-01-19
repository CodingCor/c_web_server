#include "../include/http.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

const char* response = 
    "HTTP/1.1 200 OK\r\n"
    "\r\n"
;

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
