#pragma once 

#include "cookiemap.h"

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

