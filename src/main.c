//
// Created by lukasbt on 10/14/23.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "sort/sort.h"
#include "extract/extract.h"
#include "compress/compress.h"
#include "tree/tree.h"

int main (int argc, char **argv) {
    char *command = NULL;
    char *inputFile = NULL;
    char *outputFile = NULL;
    char *treeFile = NULL; // Only for the 'compress' option
    int bufferSize = 0;
    int opt;

    if (argc < 2) {
        fprintf(stderr, "Missing command (tree, compress, extract, sort).\n");
        exit(EXIT_FAILURE);
    }

    command = argv[1];

    // Validate the command
    if (strcmp(command, "tree") != 0 && strcmp(command, "compress") != 0 &&
        strcmp(command, "extract") != 0 && strcmp(command, "sort") != 0) {
        fprintf(stderr, "Invalid command: %s. Must be one of (tree, compress, extract, sort).\n", command);
        exit(EXIT_FAILURE);
    }

    // validate options
    while ((opt = getopt(argc - 1, argv + 1, "i:o:m:t:")) != -1) {
        if (opt == 'i') {
            inputFile = optarg;
        } else if (opt == 'o') {
            outputFile = optarg;
        } else if (opt == 'm') {
            bufferSize = atoi(optarg);
        } else if (opt == 't') {
            treeFile = optarg;
        } else {
            fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            exit(EXIT_FAILURE);
        }
    }

    if (strcmp(command, "tree") == 0) {
        tree(inputFile, outputFile, bufferSize);
        return 0;
    }

    if (strcmp(command, "compress") == 0) {
        compress(inputFile, outputFile, bufferSize, treeFile);
        return 0;
    }

    if (strcmp(command, "extract") == 0) {
        extract(inputFile, outputFile, bufferSize);
        return 0;
    }

    if (strcmp(command, "sort") == 0) {
        sort(inputFile, outputFile, bufferSize);
        return 0;
    }

    return 0;
}
