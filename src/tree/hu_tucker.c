//
// Created by lukasbt on 10/18/23.
//

#include <malloc.h>
#include "hu_tucker.h"
#include "linkedList/linkedlist.h"

void outputLine(int character, int frequency, int code, int codeLength, FILE* outputFile) {
    fprintf(outputFile, "%d %d ", character, frequency);
    for (int j = codeLength - 1; j >= 0; j--) {
        fprintf(outputFile, "%d", (code >> j) & 1); // bits of code
    }
    fprintf(outputFile, "\n");
}

int generateCode(int prevCode, int prevLength, int length) {
    int lengthDiff = length - prevLength;
    int newCode = prevCode + 1;
    return (lengthDiff >= 0) ? (newCode << lengthDiff) : (newCode >> -lengthDiff);
}

void calculateCodesAndOutput(int charCount, const int* lengths, int* chars, int* freqs, FILE* outputFile) {
    int code = -1;
    int prevLength = 0;
    for (int i = 0; i < charCount; ++i) {
        int length = lengths[i];
        code = generateCode(code, prevLength, length);
        prevLength = length;
        outputLine(chars[i], freqs[i], code, length, outputFile);
    }
}

uint64_t calculatePairCost(Pair* pair, TreeNode* treeNodes) {
    TreeNode first = treeNodes[pair->first];
    TreeNode second = treeNodes[pair->second];
    return first.frequency + second.frequency;
}

void findMCPFromIndex(int beginIndex, uint64_t* minimumCost, Pair* minimumPair, int charCount, TreeNode* terminalArray) {
    for (int i = beginIndex + 1; i < charCount; ++i) {
        Pair pair = {beginIndex, i};
        uint64_t pairCost = calculatePairCost(&pair, terminalArray);
        if (terminalArray[i].type != DELETED && pairCost < *minimumCost) {
            *minimumCost = pairCost;
            minimumPair->first = beginIndex;
            minimumPair->second = i;
        }
        if (terminalArray[i].type == TERMINAL) {
            break;
        }
    }
}


Pair findLMCP(int charCount, TreeNode* terminalArray) {
    uint64_t minimumCost = UINT64_MAX;
    Pair minimumPair;
    for (int i = 0; i < charCount - 1; ++i) {
        if (terminalArray[i].type != DELETED) {
            findMCPFromIndex(i, &minimumCost, &minimumPair, charCount, terminalArray);
        }
    }
    return minimumPair;
}

TreeNode* initTerminalArray(int charCount, const int* freqs) {
    TreeNode* terminalArray = malloc(sizeof(TreeNode) * charCount);
    for (int i = 0; i < charCount; ++i) {
        TreeNode node = {TERMINAL, freqs[i], initLinkedList(i)};
        terminalArray[i] = node;
    }
    return terminalArray;
}

int* calculateLengths(int charCount, int* freqs) {
    int* lengths = calloc(charCount, sizeof(int));
    TreeNode* nodes = initTerminalArray(charCount, freqs);

    for (int _ = 0; _ < charCount - 1; ++_) {
        Pair pair = findLMCP(charCount, nodes);
        TreeNode* first = &nodes[pair.first];
        TreeNode* second = &nodes[pair.second];
        first->frequency += second->frequency;
        first->type = INTERNAL;
        second->type = DELETED;
        addList(&first->dependentChars, second->dependentChars); // concat dependent nodes
        Node* node = first->dependentChars;
        while (node != NULL) {
            lengths[node->data]++; // All dependent nodes are one longer
            node = node->next;
        }
    }
    for (int i = 0; i < charCount; ++i) {
        freeLinkedList(nodes[i].dependentChars);
    }
    free(nodes);
    return lengths;
}

void makeHT_OPC(int* chars, int* freqs, int charCount, FILE* outputFile) {

    int* lengths = calculateLengths(charCount, freqs);
    calculateCodesAndOutput(charCount, lengths, chars, freqs, outputFile);
    free(lengths);

}
