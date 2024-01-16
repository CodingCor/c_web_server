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

//const unsigned int BACKLOG_QUEUE_SIZE = 5; 
//const unsigned int RESPONSE_SIZE = 8000;

struct StringNode{
    StringNode*  nextNode;
    char* text;
};
StringNode* prependToStringList(StringNode* node, char* text);

struct HeaderEntry{
    char* key;
    char* value;
};

struct HeaderMap{
    unsigned int capacity;
    HeaderEntry* data;
};

HeaderMap createHeaderMap(unsigned int capacity = 10);
void insertCookie(HeaderMap* map, HeaderEntry entry); 
HeaderEntry getCookie(HeaderMap *map, char* key);
unsigned int headerHash(HeaderMap *map, const char *key);
void printHeaderMapPointer(HeaderMap *map);

enum HTTPMethod{
    GET,
    POST,
};

struct HTTPRequest{
    HTTPMethod method;
    char* path;
    char* versionString;
    bool headerRead;
    StringNode* cookies;
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
    HeaderMap map = createHeaderMap();
    printf(
        "Map created: \n"
        "cap: %i\n"
        "data address: %p\n",
        map.capacity, map.data
    );

    insertCookie(&map, {(char*)"Accept-Encoding", (char*)"value1"});
    insertCookie(&map, {(char*)"Accept-Content", (char*)"value2"});
    insertCookie(&map, {(char*)"Content-Length", (char*)"value3"});
    insertCookie(&map, {(char*)"Accept-Types", (char*)"value4"});
    printf("got cookie: %s \n",getCookie(&map, (char*)"cookie").value);
    insertCookie(&map, {(char*)"cookie", (char*)"value5"});
    insertCookie(&map, {(char*)"cookie", (char*)"value6"});

    printHeaderMapPointer(&map);

    printf("got cookie: %s \n",getCookie(&map, (char*)"cookie").value);

    /*
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

        // send the response to the client
        sendResponse(openedfd);

        close(openedfd);
    }

    */

    return 0;
}

HTTPRequest parseRequest(char *buffer, unsigned int bufferSize){
    HTTPRequest request = {};
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
            request.cookies = prependToStringList(request.cookies, line); 
            if(strlen(line) == 0){
                break;
            }
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

    printf("Body: \n");
    printf("%s", request.body);
    printf("\n");
}

HeaderMap createHeaderMap(unsigned int capacity){
    HeaderMap map;
    map.capacity = capacity;
    map.data = (HeaderEntry*)malloc(sizeof(HeaderEntry) * capacity);
    return map;
}

unsigned int headerHash(HeaderMap *map,const char *key){
    unsigned int hash = 1;
    for(unsigned int i = 0; i < strlen(key); i++){
        hash *= key[i];
    }
    return hash % map->capacity;
}

void insertCookie(HeaderMap* map,HeaderEntry entry){
    unsigned int tryAt = headerHash(map, entry.key);
    // check where entry should be 
    if(map->data[tryAt].key == NULL || strlen(map->data[tryAt].key) == 0 || strcmp(map->data[tryAt].key, entry.key) == 0){
        map->data[tryAt].key = entry.key;
        map->data[tryAt].value = entry.value;
        return;
    }
    // check after where entry should be 
    unsigned int i = 0;
    for(i = tryAt; i < map->capacity; i++){
        if(map->data[i].key == NULL || strlen(map->data[i].key) == 0 || strcmp(map->data[tryAt].key, entry.key) == 0){
            map->data[i].key = entry.key;
            map->data[i].value = entry.value;
            return;
        }
    }
    // check from beginning
    for(i = 0; i < tryAt; i++){
        if(map->data[i].key == NULL || strlen(map->data[i].key) == 0 || strcmp(map->data[tryAt].key, entry.key) == 0){
            map->data[i].key = entry.key;
            map->data[i].value = entry.value;
            return;
        }
    }
}

HeaderEntry getCookie(HeaderMap *map, char* key){
    unsigned int tryAt = headerHash(map, key);
    if(key == NULL)  return {};
    // check where entry should be 
    if(strcmp(map->data[tryAt].key, key) == 0){
        return map->data[tryAt];
    }
    // check after where entry should be 
    unsigned int i = 0;
    for(i = tryAt; i < map->capacity; i++){
        if(strcmp(map->data[tryAt].key, key) == 0){
            return map->data[i];
        }
    }
    // check from beginning
    for(i = 0; i < tryAt; i++){
        if(strcmp(map->data[tryAt].key, key) == 0){
            return map->data[i];
        }
    }
    return {};
}

void printHeaderMapPointer(HeaderMap *map){
    for(unsigned int i = 0; i < map->capacity; i++){
        HeaderEntry entry = map->data[i];
        printf("Hash: %i \t", (entry.key == NULL) ? 0 : headerHash(map, entry.key));
        printf("Key PTR: %p \t Value PTR: %p\n", entry.key, entry.value);
    }
}

// TODO: read the complete request 
// currently a fixed size of 8000 bytes is read from the request
// a http request can be longer than 8000 bytes and should be read to completion
// if the request is read in multiple chunks the request should be concatenated
// make a linked list of read chunks
