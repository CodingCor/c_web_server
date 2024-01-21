#pragma once

#include "cookiemap.h"

//static const char* HTTP_VERSION = "HTTP/1.1";


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

struct HTTPResponse{
    char* versionString;
    unsigned int statusCode; 
    char* statusMessage;
    char* body;
    unsigned int bodyLength;
};

char* getHttpStatusMessage(unsigned int statusCode);

HTTPRequest parseRequest(char *buffer, unsigned int bufferSize);

void sendResponse(int fd);
void sendHttpResponse(int fd, HTTPResponse response);

HTTPResponse handleRequest(HTTPRequest request);


