//
// Created by lukasbt on 10/18/23.
//

#include <malloc.h>
#include "hu_tucker.h"

void generateCode(int prevCode, int length) {
    // 1. throw away any final 1's
    // 2. convert the last 0 to a 1
    int newCode = prevCode + 1;
    // 3. add additional 0's if necessary to make the length of the word right.
    
}

void calculateLengthsHelp(int* currentChar, TreeNode* currentNode, int currentLength, int* lengths) {
    if (currentNode->left == NULL) {
        lengths[*currentChar] = currentLength;
        (*currentChar)++;
        return;
    }
    calculateLengthsHelp(currentChar, currentNode->left, currentLength + 1, lengths);
    calculateLengthsHelp(currentChar, currentNode->right, currentLength + 1, lengths);
}

int* calculateLengths(int charCount, TreeNode* subOptimal) {
    int* lengths = malloc(sizeof(int) * charCount);
    int currentChar = 0;
    calculateLengthsHelp(&currentChar, subOptimal, 0, lengths);
    return lengths;
}

TreeNode* initTerminalArray(int charCount, const int* freqs) {
    TreeNode* terminalArray = malloc(sizeof(TreeNode) * charCount);
    for (int i = 0; i < charCount; ++i) {
        TreeNode node = {TERMINAL, NULL, NULL, freqs[i]};
        terminalArray[i] = node;
    }
    return terminalArray;
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

TreeNode* copyTreeNode(TreeNode* treeNode) {
    TreeNode* treeNodeCopy = malloc(sizeof(TreeNode));
    TreeNode n = {treeNode->type, treeNode->left, treeNode->right, treeNode->frequency};
    *treeNodeCopy = n;
    return treeNodeCopy;
}

TreeNode combineTerminalArray(int charCount, TreeNode* nodes) {
    for (int _ = 0; _ < charCount - 1; ++_) {
        Pair pair = findLMCP(charCount, nodes);
        TreeNode* first = copyTreeNode(&nodes[pair.first]);
        TreeNode* second = copyTreeNode(&nodes[pair.second]);
        TreeNode newNode = {INTERNAL, first, second, first->frequency + second->frequency};
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

    int* lengths = calculateLengths(charCount, &tree);
    free(lengths);
    free(terminalArray);

    printf("%lu\n", tree.frequency);

}