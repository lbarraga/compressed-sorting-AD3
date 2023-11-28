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

int compareLineIntervals(const LineInterval* li1, const LineInterval* li2, const uint64_t* bits) {
    long l1 = li1->start;
    long l2 = li2->start;
    long li1BitStop = li1->start + li1->length;
    long li2BitStop = li2->start + li2->length;

    // TODO niet moeilijk doen en gewoon bit per bit kijken
}

void swap(long i, long j, LineInterval* arr) {
    LineInterval tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
}

void heapify(LineInterval* arr, long n, long i) {
//    long largest = i;
//    long left = 2*i + 1;
//    long right = 2*i + 2;
//
//    if (left < n && arr[largest] < arr[left])   largest = left;
//    if (right < n && arr[largest] < arr[right]) largest = right;
//
//    if (largest != i) {
//        swap(i, largest, arr);
//        heapify(arr, n, largest);
//    }
}

void heapSort(LineInterval* arr, long n) {
    // make a heap of the array by starting with the first non-leaf node. O(n)
    for (long i = n/2 - 1; i >= 0 ; --i)
        heapify(arr, n, i);

    // array is now a max heap. Swap with last element in list, heapify root and repeat. O(n*log(n))
    while (n > 0) {
        swap(0, n - 1, arr); // first element in the heap is the largest.
        heapify(arr, n - 1, 0);
        n -= 1;
    }
}

void printArray(int arr[], long n) {
    for (int i = 0; i < n; ++i)
        printf("%d ", arr[i]);
    printf("\n");
}

void testHeap() {
    int arr[] = {
        10, 9, 8, 7, 6, 5, 4, 3, 2, 1
    };

    int* m = malloc(sizeof(int) * 10);
    for (int i = 0; i < 10; ++i) {
        m[i] = arr[i];
    }

    long n = sizeof(arr) / sizeof(arr[0]);
    printf("n = %ld, n/2 - 1 = %ld\n", n, n/2 - 1);

    heapSort(m, n);

    printf("Sorted array is \n");
    printArray(m, n);
}

