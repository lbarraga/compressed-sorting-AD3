//
// Created by lukasbt on 11/22/23.
//

#ifndef PROJECTAD3_BIT_INPUT_H
#define PROJECTAD3_BIT_INPUT_H

#include "../extract/extract.h"

typedef struct BitInputHandler {
    uint64_t* buffer;
    size_t size;
    size_t elementsToRead;
    int bitsRead;
    FILE* inputStream;
} BitInputHandler;



#endif //PROJECTAD3_BIT_INPUT_H
