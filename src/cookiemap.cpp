#include "../include/cookiemap.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
