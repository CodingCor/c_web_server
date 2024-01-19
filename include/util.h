#pragma once

#include "cookiemap.h"
#include "http.h"

void printHeaderMapPointer(HeaderMap *map);

void debugLogRequest(HTTPRequest request);

void dumpBuffer(char* buffer, unsigned int bufferSize);

