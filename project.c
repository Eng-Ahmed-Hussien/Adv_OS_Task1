/**
 * \file    contiguous_memory_allocator.c
 * \brief   Simulation of a contiguous memory allocator.
 * \details This program simulates a contiguous memory allocator.
 *          It supports:
 *            1. Requesting a contiguous block of memory using different algorithms (First Fit, Best Fit, Worst Fit)
 *            2. Releasing a contiguous block of memory.
 *            3. Compacting memory by merging free blocks.
 *            4. Reporting current memory allocation status.
 *
 * Compile with:
 *     gcc -o allocator contiguous_memory_allocator.c
 *
 * Usage:
 *     ./allocator <initial_memory_size>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define FREE_LABEL "FREE"  // Label for free memory blocks

typedef struct Node {
    int availableSpace;
    int startAddress;
    int endAddress;
    struct Node *next;
    char processId[100];
} Node;

// Global pointers for our memory list.
Node *dummyHead;       // Dummy head node tracks the overall available memory.
Node *initialBlock;    // The initial block of contiguous memory.
Node *current;         // Temporary pointer for traversals.

int lastAddressSpace;  // Maximum allowed memory address.

// Function prototypes
void createFreeBlock(int spaceAllocated, int leftoverSpace);
void mergeFreeBlocks();
void swapNodes();
int processExists(const char processId[3]);
void allocateFirstFit(const char processId[3], int spaceRequested);
void allocateBestFit(const char processId[3], int spaceRequested);
void allocateWorstFit(const char processId[3], int spaceRequested);
void requestMemory(const char processId[3], int spaceRequested, const char algo[2]);
void releaseMemory(const char processId[3]);
void compactMemory();
void reportStatus();
void cleanupMemory();

void createFreeBlock(int spaceAllocated, int leftoverSpace) {
    // Create a new free block for the leftover space after allocation.
    Node *newFreeBlock = (Node *)malloc(sizeof(Node));
    if (newFreeBlock == NULL) {
        fprintf(stderr, "Memory allocation error in createFreeBlock\n");
        exit(EXIT_FAILURE);
    }

    strcpy(newFreeBlock->processId, FREE_LABEL);
    newFreeBlock->availableSpace = leftoverSpace;

    // Update the allocated block node's available space to the requested size.
    current->next->availableSpace = spaceAllocated;

    newFreeBlock->startAddress = current->next->endAddress + 1;
    newFreeBlock->endAddress = newFreeBlock->startAddress + leftoverSpace;
    if (newFreeBlock->endAddress > lastAddressSpace) {
        newFreeBlock->endAddress = lastAddressSpace;
    }
    newFreeBlock->next = current->next->next;
    current->next->next = newFreeBlock;

    // Adjust addresses of subsequent nodes if they are contiguous.
    if (newFreeBlock->next != NULL) {
        if (newFreeBlock->next->startAddress == newFreeBlock->endAddress + 1) {
            current = newFreeBlock;
            while (current->next != NULL) {
                current->next->startAddress = current->endAddress + 1;
                current->next->endAddress = current->next->startAddress + current->next->availableSpace;
                current = current->next;
            }
        }
    }
}

void mergeFreeBlocks() {
    // Merge adjacent free blocks if they exist.
    Node *nodeToRemove = NULL;

    // Merge with the next free block if current is free.
    if (strcmp(current->processId, FREE_LABEL) == 0 && 
        current->next != NULL && 
        strcmp(current->next->processId, FREE_LABEL) == 0) {
        
        current->endAddress = current->next->endAddress;
        current->availableSpace += current->next->availableSpace;
        nodeToRemove = current->next;
        current->next = current->next->next;
        free(nodeToRemove);
    }
    
    // Alternatively, check if the next node can be merged.
    if (current->next != NULL && current->next->next != NULL &&
        strcmp(current->next->processId, FREE_LABEL) != 0 &&
        strcmp(current->next->next->processId, FREE_LABEL) == 0) {
        
        current->next->next->startAddress = current->next->startAddress;
        current->next->next->availableSpace += current->next->availableSpace;
        nodeToRemove = current->next;
        current->next = current->next->next;
        free(nodeToRemove);
    }
}

void swapNodes() {
    // Swap a free block with the following process block.
    Node *tempNode;
    
    // Adjust addresses for the process block being swapped.
    current->next->next->startAddress = current->next->startAddress;
    current->next->next->endAddress = current->next->next->startAddress + current->next->next->availableSpace;
    
    // Adjust addresses for the free block.
    current->next->startAddress = current->next->next->endAddress + 1;
    current->next->endAddress = current->next->startAddress + current->next->availableSpace;
    
    // Swap the nodes.
    tempNode = current->next->next;
    current->next->next = current->next->next->next;
    tempNode->next = current->next;
    current->next = tempNode;
}

int processExists(const char processId[3]) {
    int exists = 0;
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, processId) == 0) {
            exists = 1;
            break;
        }
        current = current->next;
    }
    return exists;
}

void allocateFirstFit(const char processId[3], int spaceRequested) {
    current = dummyHead;
    int leftoverSpace = 0;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->availableSpace >= spaceRequested) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            leftoverSpace = current->next->availableSpace - spaceRequested;
            if (leftoverSpace > 0) {
                createFreeBlock(spaceRequested, leftoverSpace);
            }
            return;
        }
        current = current->next;
    }
    printf("No sufficient space to allocate process %s (%d bytes)\n", processId, spaceRequested);
}

void allocateBestFit(const char processId[3], int spaceRequested) {
    int smallestFit = INT_MAX;
    int leftoverSpace = 0;
    current = dummyHead;
    // Find the smallest free block that fits.
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->availableSpace >= spaceRequested) {
            if (current->next->availableSpace < smallestFit) {
                smallestFit = current->next->availableSpace;
            }
        }
        current = current->next;
    }
    
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->availableSpace == smallestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            leftoverSpace = current->next->availableSpace - spaceRequested;
            if (leftoverSpace > 0) {
                createFreeBlock(spaceRequested, leftoverSpace);
            }
            return;
        }
        current = current->next;
    }
    printf("No sufficient space to allocate process %s (%d bytes)\n", processId, spaceRequested);
}

void allocateWorstFit(const char processId[3], int spaceRequested) {
    int largestFit = INT_MIN;
    int leftoverSpace = 0;
    current = dummyHead;
    // Find the largest free block.
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->availableSpace >= spaceRequested) {
            if (current->next->availableSpace > largestFit) {
                largestFit = current->next->availableSpace;
            }
        }
        current = current->next;
    }
    
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->availableSpace == largestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            leftoverSpace = current->next->availableSpace - spaceRequested;
            if (leftoverSpace > 0) {
                createFreeBlock(spaceRequested, leftoverSpace);
            }
            return;
        }
        current = current->next;
    }
    printf("No sufficient space to allocate process %s (%d bytes)\n", processId, spaceRequested);
}

void requestMemory(const char processId[3], int spaceRequested, const char algo[2]) {
    if (processExists(processId)) {
        printf("Process %s is already allocated. Try a different ID.\n", processId);
        return;
    }
    if (strcmp(algo, "W") == 0) {
        allocateWorstFit(processId, spaceRequested);
    } else if (strcmp(algo, "B") == 0) {
        allocateBestFit(processId, spaceRequested);
    } else if (strcmp(algo, "F") == 0) {
        allocateFirstFit(processId, spaceRequested);
    } else {
        printf("Invalid algorithm. Use 'F' for First Fit, 'B' for Best Fit, or 'W' for Worst Fit.\n");
    }
}

void releaseMemory(const char processId[3]) {
    if (processExists(processId)) {
        dummyHead->availableSpace += current->next->availableSpace;
        strcpy(current->next->processId, FREE_LABEL);
        mergeFreeBlocks();
        return;
    }
    printf("Process %s not found in memory.\n", processId);
}

void compactMemory() {
    current = dummyHead;
    while (current->next != NULL) {
        if ((strcmp(current->next->processId, FREE_LABEL) == 0 && current->next->next != NULL) ||
            (strcmp(current->next->processId, FREE_LABEL) != 0 && strcmp(current->processId, FREE_LABEL) == 0)) {
            swapNodes();
            current = current->next;
            mergeFreeBlocks();
        } else {
            current = current->next;
        }
    }
}

void reportStatus() {
    current = dummyHead;
    printf("Total available space: %d\n", dummyHead->availableSpace);
    while (current->next != NULL) {
        printf("Addresses [%d : %d] -> Process: %s\n", current->next->startAddress, current->next->endAddress, current->next->processId);
        current = current->next;
    }
}

void cleanupMemory() {
    current = dummyHead;
    Node *tempNode;
    while (current != NULL) {
        tempNode = current;
        current = current->next;
        free(tempNode);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <initial_memory_size>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    char command[128];
    char requestType[5];
    char processId[3];
    int spaceRequested;
    char algoType[2];
    
    int initialMemory = atoi(argv[1]) - 1;
    if (initialMemory <= 0) {
        fprintf(stderr, "Invalid initial memory size provided.\n");
        return EXIT_FAILURE;
    }
    
    // Initialize dummy head and the initial memory block.
    dummyHead = (Node *)malloc(sizeof(Node));
    if (dummyHead == NULL) {
        fprintf(stderr, "Memory allocation error for dummy head.\n");
        return EXIT_FAILURE;
    }
    initialBlock = (Node *)malloc(sizeof(Node));
    if (initialBlock == NULL) {
        fprintf(stderr, "Memory allocation error for initial block.\n");
        free(dummyHead);
        return EXIT_FAILURE;
    }
    
    strcpy(dummyHead->processId, "Dummy");
    dummyHead->startAddress = -1;
    dummyHead->endAddress = -1;
    dummyHead->availableSpace = initialMemory;
    dummyHead->next = initialBlock;
    
    strcpy(initialBlock->processId, FREE_LABEL);
    initialBlock->startAddress = 0;
    initialBlock->endAddress = initialMemory;
    initialBlock->availableSpace = initialBlock->endAddress - initialBlock->startAddress;
    initialBlock->next = NULL;
    
    lastAddressSpace = initialMemory;
    
    printf("allocator> ");
    fflush(stdout);
    
    // Main command loop.
    while (fgets(command, sizeof(command), stdin) != NULL) {
        // Remove trailing newline.
        command[strcspn(command, "\n")] = '\0';
        
        if (sscanf(command, "%s", requestType) < 1) {
            fprintf(stderr, "Invalid command. Please try again.\n");
            printf("allocator> ");
            continue;
        }
        
        if (strcmp("X", requestType) == 0) {
            break;
        }
        
        if (strcmp("RQ", requestType) == 0) {
            if (sscanf(command, "%s %s %d %s", requestType, processId, &spaceRequested, algoType) != 4) {
                printf("Invalid RQ command format. Use: RQ <ProcessID> <Space> <Algorithm>\n");
            } else {
                requestMemory(processId, spaceRequested, algoType);
            }
        } else if (strcmp("RL", requestType) == 0) {
            if (sscanf(command, "%s %s", requestType, processId) != 2) {
                printf("Invalid RL command format. Use: RL <ProcessID>\n");
            } else {
                releaseMemory(processId);
            }
        } else if (strcmp("C", requestType) == 0) {
            compactMemory();
        } else if (strcmp("STAT", requestType) == 0) {
            reportStatus();
        } else {
            printf("Unrecognized command. Valid commands: RQ, RL, C, STAT, X\n");
        }
        
        printf("allocator> ");
        fflush(stdout);
    }
    
    cleanupMemory();
    return EXIT_SUCCESS;
}
