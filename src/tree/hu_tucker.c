//
// Created by lukasbt on 10/18/23.
//

#include <malloc.h>
#include "hu_tucker.h"
#include "linkedList/linkedlist.h"

int generateCode(int prevCode, int prevLength, int length) {
    int lengthDiff = length - prevLength;
    int newCode = prevCode + 1;
    return (lengthDiff >= 0) ? (newCode << lengthDiff) : (newCode >> -lengthDiff);
}

TreeNode* initTerminalArray(int charCount, const int* freqs) {
    TreeNode* terminalArray = malloc(sizeof(TreeNode) * charCount);
    for (int i = 0; i < charCount; ++i) {
        TreeNode node = {TERMINAL, NULL, NULL, freqs[i], initLinkedList(i)};
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


int* calculateLengths2(int charCount, int* freqs) {
    int* lengths = calloc(charCount, sizeof(int));
    TreeNode* nodes = initTerminalArray(charCount, freqs);

    for (int _ = 0; _ < charCount - 1; ++_) {
        Pair pair = findLMCP(charCount, nodes);
        TreeNode* first = &nodes[pair.first];
        TreeNode* second = &nodes[pair.second];
        first->frequency += second->frequency;
        first->type = INTERNAL;
        second->type = DELETED;
        addList(&first->dependentChars, second->dependentChars);
        Node* node = first->dependentChars;
        while (node != NULL) {
            lengths[node->data]++;
            node = node->next;
        }
    }
    for (int i = 0; i < charCount; ++i) {
        freeLinkedList(nodes[i].dependentChars);
    }
    free(nodes);
    return lengths;
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

void outputLine(int character, int frequency, int code, int codeLength) {
    printf("%d %d ", character, frequency);
    for (int j = codeLength - 1; j >= 0; j--) {
        printf("%d", (code >> j) & 1); // bits of code
    }
    printf("\n");
}

void output(int charCount, int* lengths, int* chars, int* freqs) {
    int code = -1;
    for (int i = 0; i < charCount; ++i) {
        int length = lengths[i];
        code = generateCode(code, length, lengths[i]);
        outputLine(chars[i], freqs[i], code, length);
    }
}

void makeHT_OPC(int* frequencyTable, FILE* outputFile) {
    int charCount = countChars(frequencyTable);
    int chars[charCount];
    int freqs[charCount];
    fillCharBuffer(chars, freqs, frequencyTable);

    int* lengths = calculateLengths2(charCount, freqs);

    for (int i = 0; i < charCount; ++i) {
        printf("%c: %d\n", chars[i], lengths[i]);
    }

    output(charCount, lengths, chars, freqs);
    free(lengths);

}