//
// Created by lukasbt on 11/27/23.
//

#include <stdio.h>
#include <malloc.h>
#include "sort.h"

uint64_t leftShiftWithCarry(uint64_t first, uint64_t second, int amount) {
    return (first << amount) | second >> (64 - amount);
}

uint64_t getFirstNBits(uint64_t value, long n) {
    return value & ((1ULL << n) - 1);
}

uint64_t extractInterval(uint64_t value, int start, int length) {
    return (value << start) >> (64 - length);
}

uint64_t getBits(const uint64_t* bits, long liBitIndex, int canTake) {
    return extractInterval(bits[liBitIndex / 64], (int) liBitIndex % 64, canTake);
}

uint64_t getBit(uint64_t value, long bitIndex) {
    return (value >> (63 - bitIndex)) & 1;
}

int compareLineIntervals(const LineInterval* li1, const LineInterval* li2, const uint64_t* bits) {
    long l1 = li1->start;
    long l2 = li2->start;
    long l1Max = li1->start + li1->length;
    long l2Max = li2->start + li2->length;

    while (l1 < l1Max && l2 < l2Max) {
        uint64_t bit1 = getBit(bits[l1 / 64], l1 % 64);
//        printf("getting bit at index %ld, bit %ld -> %lu\n", l1 / 64, l1 % 64, bit1);
        uint64_t bit2 = getBit(bits[l2 / 64], l2 % 64);
//        printf("getting bit at index %ld, bit %ld -> %lu\n", l2 / 64, l2 % 64, bit2);

        if (bit1 != bit2) {
//            printf("returning %d\n", (int) bit1 - (int) bit2);
            return (int) bit1 - (int) bit2;
        }
//        printf("\n");

        l1++;
        l2++;
    }

    //printf("reached end of while with l1 = %ld (max %ld), l2 = %ld (max %ld)\n", l1, l1Max, l2, l2Max);
    return (l1 == l1Max) - (l2 == l2Max);

}

void swap(long i, long j, LineInterval* arr) {
    LineInterval tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

void heapify(LineInterval* arr, long n, long i, uint64_t* bits) {
    long largest = i;
    long left = 2*i + 1;
    long right = 2*i + 2;

    if (left < n && compareLineIntervals(&arr[largest], &arr[left], bits) < 0)
        largest = left;

    if (right < n && compareLineIntervals(&arr[largest], &arr[right], bits) < 0)
        largest = right;

    if (largest != i) {
        swap(i, largest, arr);
        heapify(arr, n, largest, bits); // TODO kan voor stack problemen zorgen
    }
}

void heapSort(LineInterval* arr, long n, uint64_t* bits) {
    // make a heap of the array by starting with the first non-leaf node. O(n)
    for (long i = n/2 - 1; i >= 0 ; --i)
        heapify(arr, n, i, bits);

    // array is now a max heap. Swap with last element in list, heapify root and repeat. O(n*log(n))
    while (n > 0) {
        swap(0, n - 1, arr); // first element in the heap is the largest.
        heapify(arr, n - 1, 0, bits);
        n -= 1;
    }
}

void testHeap() {

    printf("testing the heap\n");

    uint64_t bits[] = {
            0b1111011101101000000000000000000000000000000000000000000000000000
    };

    LineInterval intervals[] = {
            {.start = 0, .length = 5},
            {.start = 5, .length = 4},
            {.start = 9, .length = 3},
            {.start = 12, .length = 2},
    };

    int n = 4;
    heapSort(intervals, n, bits);

    for (int i = 0; i < n; ++i) {
        printf("{start = %lu, length = %d}\n", intervals[i].start, intervals[i].length);
    }
}

