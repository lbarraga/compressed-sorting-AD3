//
// Created by lukasbt on 10/23/23.
//

#include <stdint-gcc.h>

#ifndef PROJECTAD3_COMPRESS_FILE_H
#define PROJECTAD3_COMPRESS_FILE_H

void compressFile(const char *inputFileName, const char *outputFileName, int bufferSize,
                  int* chars, unsigned char** codes);

#endif //PROJECTAD3_COMPRESS_FILE_H
