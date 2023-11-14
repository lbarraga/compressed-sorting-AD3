//
// Created by lukasbt on 11/13/23.
//

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "bit_io.h"

BitOutputHandler createOutputHandler(FILE* file, size_t size) {
    BitOutputHandler handler;
    handler.buffer = calloc(size, sizeof(unsigned char));
    handler.size = size;
    handler.bytesWritten = 0;
    handler.bitsWritten = 0;
    handler.outputStream = file;
    return handler;
}

void freeOutputHandler(BitOutputHandler handler) {
    free(handler.buffer);
}

void printByte(unsigned char byte) {
    for (int i = 7; i >= 0; --i) {
        printf("%d", (byte >> i) & 1);
    }
}

// TODO ook een outputNBits die dan de OR over de volledige doet. eigenlijk gewoon volledig veranderen.
void outputBit(BitOutputHandler* handler, uint64_t bit) {
    if (handler->bitsWritten == 8) {
        handler->bytesWritten++;
        handler->bitsWritten = 0;
    }
    if (handler->bytesWritten == handler->size) {
        fwrite(handler->buffer, sizeof(unsigned char), handler->size, handler->outputStream);
        memset(handler->buffer, 0, handler->size);
        handler->bytesWritten = 0;
    }
    handler->buffer[handler->bytesWritten] |= bit << (7 - handler->bitsWritten);
    handler->bitsWritten++;
}

void outputNumber(BitOutputHandler* handler, uint64_t number, int nBits) {
    for (int j = nBits - 1; j >= 0; --j) {
        uint64_t bit = (number >> j) & 1; // Isolate the jth bit from the right (0-based index)
        outputBit(handler, bit);
    }
}

int outputCode(BitOutputHandler* handler, OPC* code) {
    outputNumber(handler, code->code, code->length);
    return code->length;
}

void flushBits(BitOutputHandler* handler) {
    fwrite(handler->buffer, 1, handler->bytesWritten + 1, handler->outputStream);
}
