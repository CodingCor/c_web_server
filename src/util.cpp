#include "../include/util.h"

#include <stdio.h>

void dumpBuffer(char* buffer, unsigned int bufferSize){
    printf("Buffer Dump of %p of length: %i \n", buffer, bufferSize);
    for(unsigned int i = 0; i < bufferSize; i++){
        printf("%c", buffer[i]);
        }
    printf("\n");
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

void printHeaderMapPointer(HeaderMap *map){
    for(unsigned int i = 0; i < map->capacity; i++){
        HeaderEntry entry = map->data[i];
        printf("Hash: %i \t", (entry.key == NULL) ? 0 : headerHash(map, entry.key));
        printf("Key PTR: %p \t Value PTR: %p\n", entry.key, entry.value);
    }
}
