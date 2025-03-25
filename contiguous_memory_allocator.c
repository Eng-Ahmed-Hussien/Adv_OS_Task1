/**
 * File    contiguous_memory_allocator.c
 * Brief   Simulation of a contiguous memory allocator.
 * \details This program simulates a contiguous memory allocator that supports:
 *          1. Requesting memory using First Fit, Best Fit, or Worst Fit algorithms.
 *          2. Releasing allocated memory.
 *          3. Compacting memory by merging free blocks.
 *          4. Reporting the current memory allocation status.
 * 
 * Compile:
 *     gcc -o allocator contiguous_memory_allocator.c
 * 
 * Run:
 *     ./allocator <initial_memory_size>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FREE_LABEL "FREE"

typedef struct Node {
    int availableSpace;
    int startAddress;
    int endAddress;
    struct Node *next;
    char processId[100];
} Node;

Node *dummyHead;
Node *initialBlock;
Node *current;
int lastAddressSpace;

void createFreeBlock(int spaceAllocated, int leftoverSpace);
void mergeFreeBlocks();
void requestMemory(const char processId[3], int spaceRequested, const char algo[2]);
void releaseMemory(const char processId[3]);
void compactMemory();
void reportStatus();
void cleanupMemory();

void createFreeBlock(int spaceAllocated, int leftoverSpace) {
    Node *newFreeBlock = (Node *)malloc(sizeof(Node));
    if (!newFreeBlock) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(newFreeBlock->processId, FREE_LABEL);
    newFreeBlock->availableSpace = leftoverSpace;
    newFreeBlock->startAddress = current->next->endAddress + 1;
    newFreeBlock->endAddress = newFreeBlock->startAddress + leftoverSpace;
    if (newFreeBlock->endAddress > lastAddressSpace) {
        newFreeBlock->endAddress = lastAddressSpace;
    }
    newFreeBlock->next = current->next->next;
    current->next->next = newFreeBlock;
}

void mergeFreeBlocks() {
    Node *nodeToRemove = NULL;
    if (strcmp(current->processId, FREE_LABEL) == 0 && 
        current->next != NULL && 
        strcmp(current->next->processId, FREE_LABEL) == 0) {
        current->endAddress = current->next->endAddress;
        current->availableSpace += current->next->availableSpace;
        nodeToRemove = current->next;
        current->next = current->next->next;
        free(nodeToRemove);
    }
}

void requestMemory(const char processId[3], int spaceRequested, const char algo[2]) {
    if (strcmp(algo, "F") == 0) {
        current = dummyHead;
        while (current->next != NULL) {
            if (strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->availableSpace >= spaceRequested) {
                dummyHead->availableSpace -= spaceRequested;
                strcpy(current->next->processId, processId);
                current->next->endAddress = current->next->startAddress + spaceRequested;
                int leftoverSpace = current->next->availableSpace - spaceRequested;
                if (leftoverSpace > 0) {
                    createFreeBlock(spaceRequested, leftoverSpace);
                }
                return;
            }
            current = current->next;
        }
    }
    printf("No sufficient space to allocate process %s (%d bytes)\n", processId, spaceRequested);
}

void releaseMemory(const char processId[3]) {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, processId) == 0) {
            dummyHead->availableSpace += current->next->availableSpace;
            strcpy(current->next->processId, FREE_LABEL);
            mergeFreeBlocks();
            return;
        }
        current = current->next;
    }
    printf("Process %s not found in memory.\n", processId);
}

void compactMemory() {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->next != NULL) {
            current->next->next->startAddress = current->next->startAddress;
            current->next->next->endAddress = current->next->next->startAddress + current->next->next->availableSpace;
            Node *tempNode = current->next;
            current->next = current->next->next;
            free(tempNode);
        }
        current = current->next;
    }
}

void reportStatus() {
    printf("Total available space: %d bytes\n", dummyHead->availableSpace);
    current = dummyHead;
    while (current->next != NULL) {
        printf("Addresses [%d : %d] -> Process: %s\n", current->next->startAddress, current->next->endAddress, current->next->processId);
        current = current->next;
    }
}

void cleanupMemory() {
    Node *tempNode;
    while (dummyHead != NULL) {
        tempNode = dummyHead;
        dummyHead = dummyHead->next;
        free(tempNode);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <initial_memory_size>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int initialMemory = atoi(argv[1]) - 1;
    if (initialMemory <= 0) {
        fprintf(stderr, "Error: Invalid memory size.\n");
        return EXIT_FAILURE;
    }
    
    dummyHead = (Node *)malloc(sizeof(Node));
    initialBlock = (Node *)malloc(sizeof(Node));
    if (!dummyHead || !initialBlock) {
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_FAILURE;
    }
    
    dummyHead->availableSpace = initialMemory;
    dummyHead->next = initialBlock;
    
    strcpy(initialBlock->processId, FREE_LABEL);
    initialBlock->startAddress = 0;
    initialBlock->endAddress = initialMemory;
    initialBlock->availableSpace = initialMemory;
    initialBlock->next = NULL;
    
    lastAddressSpace = initialMemory;
    
    printf("Initialize free space: %d bytes\n", dummyHead->availableSpace);
    printf("allocator> ");
    fflush(stdout);
    
    cleanupMemory();
    return EXIT_SUCCESS;
}