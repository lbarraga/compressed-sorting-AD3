//
// Created by lukasbt on 10/23/23.
//

#ifndef PROJECTAD3_COMPRESS_FILE_H

#define PROJECTAD3_COMPRESS_FILE_H

#include <stdint-gcc.h>
#include "compress.h"

void compressFile(const char *inputFileName, const char *outputFileName,
                  int bufferSize, OPC** codes, int charCount);

#endif //PROJECTAD3_COMPRESS_FILE_H
