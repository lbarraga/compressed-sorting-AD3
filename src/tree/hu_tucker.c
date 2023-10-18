//
// Created by lukasbt on 10/18/23.
//

#include <malloc.h>
#include "hu_tucker.h"

TreeNode* initTerminalArray(int charCount, const int* freqs) {
    TreeNode* terminalArray = malloc(sizeof(TreeNode) * charCount);
    for (int i = 0; i < charCount; ++i) {
        terminalArray[i].left = NULL;
        terminalArray[i].right = NULL;
        terminalArray[i].type = TERMINAL;
        terminalArray[i].frequency = freqs[i];
    }
    return terminalArray;
}

int calculatePairCost(Pair* pair, TreeNode* treeNodes) {
    TreeNode first = treeNodes[pair->first];
    TreeNode second = treeNodes[pair->second];
    return first.frequency + second.frequency;
}

void findMCPFromIndex(int beginIndex, int* minimumCost, Pair* minimumPair, int charCount, TreeNode* terminalArray) {
    for (int i = beginIndex + 1; i < charCount; ++i) {
        Pair pair = {beginIndex, i};
        int pairCost = calculatePairCost(&pair, terminalArray);
        if (terminalArray[i].type != DELETED && (*minimumCost == -1 || pairCost < *minimumCost)) {
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
    int minimumCost = -1;
    Pair minimumPair;
    for (int i = 0; i < charCount - 1; ++i) {
        if (terminalArray[i].type != DELETED) {
            findMCPFromIndex(i, &minimumCost, &minimumPair, charCount, terminalArray);
        }
    }
    printf("found %d and %d!\n", minimumPair.first, minimumPair.second);
    return minimumPair;
}

TreeNode combineTerminalArray(int charCount, TreeNode* nodes) {
    for (int _ = 0; _ < charCount - 1; ++_) {
        Pair pair = findLMCP(charCount, nodes);
        TreeNode first = nodes[pair.first];
        TreeNode second = nodes[pair.second];
        TreeNode newNode = {INTERNAL, &first, &second, first.frequency + second.frequency};
        nodes[pair.first] = newNode;
        nodes[pair.second].type = DELETED;
    }
    return nodes[0];
}

int countChars(const int * frequencyTable) {
    int amount = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            amount++;
        }
    }
    return amount;
}

void fillCharBuffer(int* charBuffer, int* freqs, const int* frequencyTable) {
    int index = 0;
    for (int i = 0; i < 128; i++) {
        if (frequencyTable[i] != 0) {
            charBuffer[index] = i;
            freqs[index] = frequencyTable[i];
            index++;
        }
    }
}

void makeHT_OPC(int* frequencyTable, FILE* outputFile) {
    int charCount = countChars(frequencyTable);
    int chars[charCount];
    int freqs[charCount];
    fillCharBuffer(chars, freqs, frequencyTable);

    TreeNode* terminalArray = initTerminalArray(charCount, freqs);

    TreeNode tree = combineTerminalArray(charCount, terminalArray);
    free(terminalArray);
    printf("%d\n", tree.frequency);

}



