//
// Created by lukasbt on 11/22/23.
//

#ifndef PROJECTAD3_BIT_INPUT_H
#define PROJECTAD3_BIT_INPUT_H

#include "../extract/extract.h"

typedef struct BitInputHandler {
    uint64_t* buffer;
    size_t size;
    size_t elementsRead;
    int bitsRead;
    FILE* inputStream;
} BitInputHandler;

BitInputHandler createBitInputHandler(FILE* file, size_t size_bytes);
void freeBitInputHandler(BitInputHandler* handler);
void setInputHandlerAt(BitInputHandler* handler, long bytePosition);

uint64_t readNBits(BitInputHandler* handler, int nBits);
int readLength(BitInputHandler* handler);

void printUint64t(uint64_t byte);


#endif //PROJECTAD3_BIT_INPUT_H
