//
// Created by lukasbt on 11/22/23.
//

#ifndef PROJECTAD3_BIT_INPUT_H
#define PROJECTAD3_BIT_INPUT_H

#include "../extract/extract.h"
#include <stddef.h>

typedef struct BitInputHandler {
    uint64_t* buffer;
    size_t size;
    size_t elementsRead;
    int bitsRead;
    FILE* inputStream;
    long filePosition; // keep file position to mitigate changes from other input readers
} BitInputHandler;

typedef struct InputHandlerPosition {
    size_t element;
    int bit;
    size_t size;
} InputHandlerPosition;

InputHandlerPosition getInputPosition(BitInputHandler* handler);

BitInputHandler createBitInputHandler(FILE* file, size_t size_bytes);
void freeBitInputHandler(BitInputHandler* handler);
void setAtInputPosition(BitInputHandler* handler, InputHandlerPosition* position);

uint64_t readNBits(BitInputHandler* handler, int nBits);
int readLength(BitInputHandler* handler);

void printUint64t(uint64_t byte);


#endif //PROJECTAD3_BIT_INPUT_H
