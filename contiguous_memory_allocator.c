/**
 * \file    contiguous_memory_allocator.c
 * \brief   Simulation of a contiguous memory allocator.
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
 *     On Windows: allocator.exe <initial_memory_size>
 *     On Unix-like systems: ./allocator <initial_memory_size>
 * 
 * If no command-line argument is provided, the program will prompt for the initial memory size.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FREE_LABEL "FREE"   // Label for free memory blocks

// Structure representing a block of memory.
typedef struct Node {
    int availableSpace;      // Size of the block (or free space available)
    int startAddress;        // Starting address of the block
    int endAddress;          // Ending address of the block
    struct Node *next;       // Pointer to the next memory block in the list
    char processId[100];     // Process identifier (or "FREE" for unallocated space)
} Node;

// Global pointers for memory list management.
Node *dummyHead;       // Dummy head node; its availableSpace tracks total free memory.
Node *initialBlock;    // The first block of contiguous memory.
Node *current;         // Temporary pointer used during traversals.
int lastAddressSpace;  // The maximum allowed memory address (initial memory size).

// Function prototypes.
void createFreeBlock(int spaceAllocated, int leftoverSpace);
void mergeFreeBlocks();
int processExists(const char processId[3]);
void allocateFirstFit(const char processId[3], int spaceRequested);
void allocateBestFit(const char processId[3], int spaceRequested);
void allocateWorstFit(const char processId[3], int spaceRequested);
void requestMemory(const char processId[3], int spaceRequested, const char algo[2]);
void releaseMemory(const char processId[3]);
void compactMemory();
void reportStatus();
void cleanupMemory();

// Create a free block for leftover space after an allocation.
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

// Merge adjacent free blocks to maintain continuous free space.
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

// Check if a process is already allocated.
int processExists(const char processId[3]) {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, processId) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// Allocate memory using the First Fit algorithm.
void allocateFirstFit(const char processId[3], int spaceRequested) {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            int leftoverSpace = current->next->availableSpace - spaceRequested;
            current->next->availableSpace = spaceRequested;
            if (leftoverSpace > 0) {
                createFreeBlock(spaceRequested, leftoverSpace);
            }
            return;
        }
        current = current->next;
    }
    printf("No sufficient space to allocate process %s (%d bytes)\n", processId, spaceRequested);
}

// Allocate memory using the Best Fit algorithm.
void allocateBestFit(const char processId[3], int spaceRequested) {
    int smallestFit = INT_MAX;
    current = dummyHead;
    // Find the smallest free block that can fit the requested space.
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested &&
            current->next->availableSpace < smallestFit) {
            smallestFit = current->next->availableSpace;
        }
        current = current->next;
    }
    // Allocate in the identified block.
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace == smallestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            int leftoverSpace = current->next->availableSpace - spaceRequested;
            current->next->availableSpace = spaceRequested;
            if (leftoverSpace > 0) {
                createFreeBlock(spaceRequested, leftoverSpace);
            }
            return;
        }
        current = current->next;
    }
    printf("No sufficient space to allocate process %s (%d bytes)\n", processId, spaceRequested);
}

// Allocate memory using the Worst Fit algorithm.
void allocateWorstFit(const char processId[3], int spaceRequested) {
    int largestFit = INT_MIN;
    current = dummyHead;
    // Find the largest free block available.
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested &&
            current->next->availableSpace > largestFit) {
            largestFit = current->next->availableSpace;
        }
        current = current->next;
    }
    // Allocate in the identified block.
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace == largestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            int leftoverSpace = current->next->availableSpace - spaceRequested;
            current->next->availableSpace = spaceRequested;
            if (leftoverSpace > 0) {
                createFreeBlock(spaceRequested, leftoverSpace);
            }
            return;
        }
        current = current->next;
    }
    printf("No sufficient space to allocate process %s (%d bytes)\n", processId, spaceRequested);
}

// Dispatch a memory request based on the chosen algorithm.
// 'F' for First Fit, 'B' for Best Fit, and 'W' for Worst Fit.
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

// Release memory allocated to a process.
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

// Compact memory by merging all adjacent free blocks.
void compactMemory() {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->processId, FREE_LABEL) == 0 &&
            current->next != NULL &&
            strcmp(current->next->processId, FREE_LABEL) == 0) {
            mergeFreeBlocks();
        } else {
            current = current->next;
        }
    }
}

// Report the current status of memory allocation.
void reportStatus() {
    printf("Total available space: %d bytes\n", dummyHead->availableSpace);
    current = dummyHead;
    while (current->next != NULL) {
        printf("Addresses [%d : %d] -> Process: %s\n", 
               current->next->startAddress, 
               current->next->endAddress, 
               current->next->processId);
        current = current->next;
    }
}

// Free all allocated memory blocks.
void cleanupMemory() {
    Node *tempNode;
    while (dummyHead != NULL) {
        tempNode = dummyHead;
        dummyHead = dummyHead->next;
        free(tempNode);
    }
}

int main(int argc, char *argv[]) {
    int initialMemory;
    // Prompt for initial memory size if not provided as a command-line argument.
    if (argc < 2) {
        printf("Enter initial memory size: ");
        if (scanf("%d", &initialMemory) != 1) {
            fprintf(stderr, "Error reading memory size.\n");
            return EXIT_FAILURE;
        }
        // Clear the newline left in the input buffer.
        getchar();
    } else {
        initialMemory = atoi(argv[1]);
    }
    
    // Adjust memory size by subtracting 1 (to account for addressing logic).
    initialMemory -= 1;
    if (initialMemory <= 0) {
        fprintf(stderr, "Error: Invalid memory size.\n");
        return EXIT_FAILURE;
    }
    
    // Initialize the memory list.
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
    
    // Display the initial free memory and the command prompt.
    printf("Initialize free space: %d bytes\n", dummyHead->availableSpace);
    printf("allocator> ");
    fflush(stdout);
    
    // Main command loop.
    char command[128];
    char requestType[5];
    char processId[3];
    int spaceRequested;
    char algoType[2];
    
    while (fgets(command, sizeof(command), stdin) != NULL) {
        // Remove trailing newline.
        command[strcspn(command, "\n")] = '\0';
        
        if (sscanf(command, "%s", requestType) < 1) {
            fprintf(stderr, "Invalid command. Please try again.\n");
            printf("allocator> ");
            continue;
        }
        
        // Exit the loop if the command is "X".
        if (strcmp("X", requestType) == 0) {
            break;
        }
        
        if (strcmp("RQ", requestType) == 0) {
            // Request memory: RQ <ProcessID> <Space> <Algorithm>
            if (sscanf(command, "%s %s %d %s", requestType, processId, &spaceRequested, algoType) != 4) {
                printf("Invalid RQ command format. Use: RQ <ProcessID> <Space> <Algorithm>\n");
            } else {
                requestMemory(processId, spaceRequested, algoType);
            }
        } else if (strcmp("RL", requestType) == 0) {
            // Release memory: RL <ProcessID>
            if (sscanf(command, "%s %s", requestType, processId) != 2) {
                printf("Invalid RL command format. Use: RL <ProcessID>\n");
            } else {
                releaseMemory(processId);
            }
        } else if (strcmp("C", requestType) == 0) {
            // Compact memory.
            compactMemory();
        } else if (strcmp("STAT", requestType) == 0) {
            // Report memory status.
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
