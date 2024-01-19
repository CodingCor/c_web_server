#include "../include/stringlist.h"

#include <stdlib.h>

StringNode* prependToStringList(StringNode* node, char* text){
    StringNode* firstNode = (StringNode*)malloc(sizeof(StringNode));
    firstNode->nextNode = node;
    firstNode->text = text;
    return firstNode;
}
