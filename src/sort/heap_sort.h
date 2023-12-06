//
// Created by lukasbt on 11/27/23.
//

#ifndef PROJECTAD3_HEAP_SORT_H
#define PROJECTAD3_HEAP_SORT_H

#include <stdint.h>
#include <stdio.h>
#include "sort.h"

void heapSort(LineInterval* arr, long n, uint64_t* bits);
uint64_t extractInterval(uint64_t value, int start, int length);
uint64_t getBit(uint64_t value, long bitIndex);

#endif //PROJECTAD3_HEAP_SORT_H
