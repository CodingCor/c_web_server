#pragma once

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
