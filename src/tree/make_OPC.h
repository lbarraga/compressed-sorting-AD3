//
// Created by lukasbt on 10/17/23.
//

#ifndef PROJECTAD3_MAKE_OPC_H
#define PROJECTAD3_MAKE_OPC_H

#include <stdio.h>

typedef struct {
    int start;
    int end;
} Range;

typedef struct {
    Range left;
    Range right;
} Division;


void makeOPC(int* frequencyTable, FILE* outputFile);

#endif //PROJECTAD3_MAKE_OPC_H
