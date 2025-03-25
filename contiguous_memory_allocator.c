/**
 * \file    contiguous_memory_allocator.c
 * \brief   Simulation of a contiguous memory allocator.
 *
 * This program simulates a contiguous memory allocator that supports:
 *   - Requesting memory using First Fit, Best Fit, or Worst Fit algorithms.
 *   - Releasing allocated memory.
 *   - Compacting memory by merging adjacent free blocks.
 *   - Reporting the current memory allocation status.
 *
 * Compile:
 *     gcc -o allocator contiguous_memory_allocator.c
 *
 * Run:
 *     On Windows: allocator.exe <initial_memory_size>
 *     On Unix-like systems: ./allocator <initial_memory_size>
 *
 * If no command-line argument is provided, the program prompts for the initial memory size.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FREE_LABEL "FREE"   // Label used for free memory blocks

typedef struct Node {
    int availableSpace;   // Size of this block (allocated or free)
    int startAddress;     // Start address of the block
    int endAddress;       // End address of the block
    struct Node *next;    // Next block in the list
    char processId[100];  // Process ID or FREE if unallocated
} Node;

// Global variables for memory management.
Node *dummyHead;      // Dummy head: tracks total free space
Node *initialBlock;   // The initial free memory block
Node *current;        // Temporary pointer for list traversal
int lastAddressSpace; // Maximum address (initial memory size)

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

/*
 * Creates a new free block for any leftover space after allocation.
 */
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
    if (newFreeBlock->endAddress > lastAddressSpace)
        newFreeBlock->endAddress = lastAddressSpace;
    newFreeBlock->next = current->next->next;
    current->next->next = newFreeBlock;
}

/*
 * Merges adjacent free blocks to form a larger free block.
 */
void mergeFreeBlocks() {
    current = dummyHead;
    while (current && current->next) {
        if (strcmp(current->processId, FREE_LABEL) == 0 &&
            strcmp(current->next->processId, FREE_LABEL) == 0) {
            current->endAddress = current->next->endAddress;
            current->availableSpace += current->next->availableSpace;
            Node *tempNode = current->next;
            current->next = current->next->next;
            free(tempNode);
        } else {
            current = current->next;
        }
    }
}

/*
 * Checks if a process with the given ID is already allocated.
 */
int processExists(const char processId[3]) {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, processId) == 0)
            return 1;
        current = current->next;
    }
    return 0;
}

/*
 * Allocates memory using the First Fit strategy.
 */
void allocateFirstFit(const char processId[3], int spaceRequested) {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            int allocatedStart = current->next->startAddress;
            int allocatedEnd = current->next->endAddress;
            int leftoverSpace = current->next->availableSpace - spaceRequested;
            current->next->availableSpace = spaceRequested;
            if (leftoverSpace > 0)
                createFreeBlock(spaceRequested, leftoverSpace);
            printf("Allocation Successful! Process %s allocated using First Fit. Block: [%d : %d]\n", processId, allocatedStart, allocatedEnd);
            return;
        }
        current = current->next;
    }
    printf("Not enough space to allocate %d bytes for process %s using First Fit.\n", spaceRequested, processId);
}

/*
 * Allocates memory using the Best Fit strategy.
 */
void allocateBestFit(const char processId[3], int spaceRequested) {
    int smallestFit = INT_MAX;
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested &&
            current->next->availableSpace < smallestFit) {
            smallestFit = current->next->availableSpace;
        }
        current = current->next;
    }
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace == smallestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            int allocatedStart = current->next->startAddress;
            int allocatedEnd = current->next->endAddress;
            int leftoverSpace = current->next->availableSpace - spaceRequested;
            current->next->availableSpace = spaceRequested;
            if (leftoverSpace > 0)
                createFreeBlock(spaceRequested, leftoverSpace);
            printf("Allocation Successful! Process %s allocated using Best Fit. Block: [%d : %d]\n", processId, allocatedStart, allocatedEnd);
            return;
        }
        current = current->next;
    }
    printf("Not enough space to allocate %d bytes for process %s using Best Fit.\n", spaceRequested, processId);
}

/*
 * Allocates memory using the Worst Fit strategy.
 */
void allocateWorstFit(const char processId[3], int spaceRequested) {
    int largestFit = INT_MIN;
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested &&
            current->next->availableSpace > largestFit) {
            largestFit = current->next->availableSpace;
        }
        current = current->next;
    }
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace == largestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested;
            int allocatedStart = current->next->startAddress;
            int allocatedEnd = current->next->endAddress;
            int leftoverSpace = current->next->availableSpace - spaceRequested;
            current->next->availableSpace = spaceRequested;
            if (leftoverSpace > 0)
                createFreeBlock(spaceRequested, leftoverSpace);
            printf("Allocation Successful! Process %s allocated using Worst Fit. Block: [%d : %d]\n", processId, allocatedStart, allocatedEnd);
            return;
        }
        current = current->next;
    }
    printf("Not enough space to allocate %d bytes for process %s using Worst Fit.\n", spaceRequested, processId);
}

/*
 * Dispatches a memory request based on the chosen strategy.
 * Valid options: "F" for First Fit, "B" for Best Fit, "W" for Worst Fit.
 */
void requestMemory(const char processId[3], int spaceRequested, const char algo[2]) {
    if (processExists(processId)) {
        printf("Process %s already exists. Choose a different ID.\n", processId);
        return;
    }
    if (strcmp(algo, "W") == 0)
        allocateWorstFit(processId, spaceRequested);
    else if (strcmp(algo, "B") == 0)
        allocateBestFit(processId, spaceRequested);
    else if (strcmp(algo, "F") == 0)
        allocateFirstFit(processId, spaceRequested);
    else
        printf("Invalid algorithm. Use 'F' (First Fit), 'B' (Best Fit), or 'W' (Worst Fit).\n");
}

/*
 * Releases memory allocated to a process.
 */
void releaseMemory(const char processId[3]) {
    current = dummyHead;
    while (current->next != NULL) {
        if (strcmp(current->next->processId, processId) == 0) {
            dummyHead->availableSpace += current->next->availableSpace;
            strcpy(current->next->processId, FREE_LABEL);
            mergeFreeBlocks();
            printf("Memory released for process %s.\n", processId);
            return;
        }
        current = current->next;
    }
    printf("Process %s not found.\n", processId);
}

/*
 * Compacts memory by merging adjacent free blocks.
 */
void compactMemory() {
    mergeFreeBlocks();
    printf("Memory compacted successfully.\n");
}

/*
 * Displays the current memory status.
 */
void reportStatus() {
    printf("\n----- Memory Status -----\n");
    printf("Total available space: %d bytes\n", dummyHead->availableSpace);
    current = dummyHead;
    while (current->next != NULL) {
        printf("Addresses [%d : %d] -> %s\n",
               current->next->startAddress,
               current->next->endAddress,
               current->next->processId);
        current = current->next;
    }
    printf("-------------------------\n\n");
}

/*
 * Frees all allocated memory blocks.
 */
void cleanupMemory() {
    Node *tempNode;
    while (dummyHead != NULL) {
        tempNode = dummyHead;
        dummyHead = dummyHead->next;
        free(tempNode);
    }
}

/*
 * Main function: initializes memory and handles user commands.
 */
int main(int argc, char *argv[]) {
    int initialMemory;
    printf("=== Welcome to the Contiguous Memory Allocator ===\n");
    
    // Get initial memory size from argument or prompt.
    if (argc < 2) {
        printf("Enter initial memory size: ");
        if (scanf("%d", &initialMemory) != 1) {
            fprintf(stderr, "Error reading memory size.\n");
            return EXIT_FAILURE;
        }
        getchar(); // Clear newline from input buffer.
    } else {
        initialMemory = atoi(argv[1]);
    }
    
    // Adjust memory size (subtract 1 for addressing logic).
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
    
    printf("\nMemory initialized with %d free bytes.\n", dummyHead->availableSpace);
    
    // Display command instructions.
    printf("Commands:\n");
    printf("  RQ <ProcessID> <Space> <Algorithm>  (e.g., RQ p1 100 B)\n");
    printf("  RL <ProcessID>                      (Release memory)\n");
    printf("  C                                   (Compact memory)\n");
    printf("  STAT                                (Display memory status)\n");
    printf("  X                                   (Exit)\n\n");
    
    char command[128];
    char requestType[5];
    char processId[3];
    int spaceRequested;
    char algoType[2];
    
    while (1) {
        printf("Command > ");
        if (!fgets(command, sizeof(command), stdin))
            break;
        command[strcspn(command, "\n")] = '\0';  // Remove newline
        
        if (sscanf(command, "%s", requestType) < 1) {
            printf("Invalid command. Try again.\n");
            continue;
        }
        
        if (strcmp("X", requestType) == 0)
            break;
        
        if (strcmp("RQ", requestType) == 0) {
            // Format: RQ <ProcessID> <Space> <Algorithm>
            if (sscanf(command, "%s %s %d %s", requestType, processId, &spaceRequested, algoType) != 4) {
                printf("Usage: RQ <ProcessID> <Space> <Algorithm>\n");
            } else {
                requestMemory(processId, spaceRequested, algoType);
            }
        } else if (strcmp("RL", requestType) == 0) {
            if (sscanf(command, "%s %s", requestType, processId) != 2) {
                printf("Usage: RL <ProcessID>\n");
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
    }
    
    printf("Exiting allocator. Goodbye!\n");
    cleanupMemory();
    return EXIT_SUCCESS;
}
