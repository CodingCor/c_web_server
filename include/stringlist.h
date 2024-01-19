#pragma once

struct StringNode{
    StringNode*  nextNode;
    char* text;
};

StringNode* prependToStringList(StringNode* node, char* text);
