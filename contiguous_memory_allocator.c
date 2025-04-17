/**
 * \file    contiguous_memory_allocator.c
 * \brief   Simulates a contiguous memory allocator with allocation strategies.
 *
 * Supports:
 *   - Memory allocation (First Fit, Best Fit, Worst Fit)
 *   - Memory release
 *   - Memory compaction
 *   - Status reporting
 *
 * Compile: gcc -o allocator contiguous_memory_allocator.c
 * Run: ./allocator <initial_memory_size>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FREE_LABEL "FREE"   // Label for free memory blocks

typedef struct Node {
    int availableSpace;   // Size of the block
    int startAddress;     // Start address of the block
    int endAddress;       // End address of the block
    struct Node *next;    // Next block in the list
    char processId[100];  // Process ID or FREE if unallocated
} Node;

// Global memory management variables
Node *dummyHead;      // Tracks total free space
Node *initialBlock;   // Initial free memory block
Node *current;        // Temporary pointer for traversal
int lastAddressSpace; // Maximum address (initial memory size - 1)

/** Creates a new free block for leftover space after allocation. */
void createFreeBlock(int spaceAllocated, int leftoverSpace) {
    Node *newFreeBlock = (Node *)malloc(sizeof(Node));
    if (!newFreeBlock) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(newFreeBlock->processId, FREE_LABEL);
    newFreeBlock->availableSpace = leftoverSpace;
    newFreeBlock->startAddress = current->next->endAddress + 1;
    newFreeBlock->endAddress = newFreeBlock->startAddress + leftoverSpace - 1;
    if (newFreeBlock->endAddress > lastAddressSpace)
        newFreeBlock->endAddress = lastAddressSpace;
    newFreeBlock->next = current->next->next;
    current->next->next = newFreeBlock;
}

/** Merges adjacent free blocks into a single larger block. */
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
/** Merges "all" FREE blocks (even if non‑contiguous) into one big FREE block at the end. */
void mergeAllFreeMemory() {
    int totalFree = 0;
    Node *prev = dummyHead;
    Node *curr = dummyHead->next;
    Node *toRemove;
    while (curr) {
        if (strcmp(curr->processId, FREE_LABEL) == 0) {
            totalFree += curr->availableSpace;
            toRemove = curr;
            curr = curr->next;
            free(toRemove);
            prev->next = curr;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }

    if (totalFree > 0) {
        Node *freeBlock = malloc(sizeof(Node));
        if (!freeBlock) {
            fprintf(stderr, "Error: Memory allocation failed in mergeAllFreeMemory.\n");
            exit(EXIT_FAILURE);
        }
        strcpy(freeBlock->processId, FREE_LABEL);
        freeBlock->availableSpace = totalFree;
        freeBlock->startAddress = lastAddressSpace + 1 - totalFree;
        freeBlock->endAddress   = lastAddressSpace;
        freeBlock->next         = NULL;
        prev->next              = freeBlock;
    }

    dummyHead->availableSpace = totalFree;
}
/** Checks if a process ID is already allocated. Returns 1 if exists, 0 otherwise. */
int processExists(const char processId[3]) {
    current = dummyHead;
    while (current->next) {
        if (strcmp(current->next->processId, processId) == 0)
            return 1;
        current = current->next;
    }
    return 0;
}

/** Allocates memory using First Fit: uses the first suitable free block. */
void allocateFirstFit(const char processId[3], int spaceRequested) {
    current = dummyHead;
    while (current->next) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested - 1;
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

/** Allocates memory using Best Fit: uses the smallest suitable free block. */
void allocateBestFit(const char processId[3], int spaceRequested) {
    int smallestFit = INT_MAX;
    current = dummyHead;
    while (current->next) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested &&
            current->next->availableSpace < smallestFit) {
            smallestFit = current->next->availableSpace;
        }
        current = current->next;
    }
    current = dummyHead;
    while (current->next) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace == smallestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested - 1;
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

/** Allocates memory using Worst Fit: uses the largest suitable free block. */
void allocateWorstFit(const char processId[3], int spaceRequested) {
    int largestFit = INT_MIN;
    current = dummyHead;
    while (current->next) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace >= spaceRequested &&
            current->next->availableSpace > largestFit) {
            largestFit = current->next->availableSpace;
        }
        current = current->next;
    }
    current = dummyHead;
    while (current->next) {
        if (strcmp(current->next->processId, FREE_LABEL) == 0 &&
            current->next->availableSpace == largestFit) {
            dummyHead->availableSpace -= spaceRequested;
            strcpy(current->next->processId, processId);
            current->next->endAddress = current->next->startAddress + spaceRequested - 1;
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

/** Requests memory based on the chosen algorithm (F, B, W). */
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

/** Releases memory allocated to a process and merges free blocks. */
void releaseMemory(const char processId[3]) {
    current = dummyHead;
    while (current->next) {
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

/** Compacts memory by merging adjacent free blocks. */
void compactMemory() {
    mergeFreeBlocks();
    mergeAllFreeMemory();
    printf("Memory compacted successfully.\n");
}

/** Reports current memory allocation status. */
void reportStatus() {
    printf("\n----- Memory Status -----\n");
    printf("Total available space: %d bytes\n", dummyHead->availableSpace);
    current = dummyHead;
    while (current->next) {
        printf("Addresses [%d : %d] -> %s\n",
               current->next->startAddress,
               current->next->endAddress,
               current->next->processId);
        current = current->next;
    }
    printf("-------------------------\n\n");
}

/** Frees all memory blocks before program exit. */
void cleanupMemory() {
    Node *tempNode;
    while (dummyHead) {
        tempNode = dummyHead;
        dummyHead = dummyHead->next;
        free(tempNode);
    }
}

/** Main function: initializes memory and processes user commands. */
int main(int argc, char *argv[]) {
    int initialMemory;
    printf("=== Welcome to the Contiguous Memory Allocator ===\n");

    // Get initial memory size
    if (argc < 2) {
        printf("Enter initial memory size: ");
        if (scanf("%d", &initialMemory) != 1) {
            fprintf(stderr, "Error reading memory size.\n");
            return EXIT_FAILURE;
        }
        getchar(); // Clear input buffer
    } else {
        initialMemory = atoi(argv[1]);
    }

    // Validate and adjust memory size
    initialMemory -= 1;
    if (initialMemory <= 0) {
        fprintf(stderr, "Error: Invalid memory size.\n");
        return EXIT_FAILURE;
    }

    // Initialize memory list
    dummyHead = (Node *)malloc(sizeof(Node));
    initialBlock = (Node *)malloc(sizeof(Node));
    if (!dummyHead || !initialBlock) {
        fprintf(stderr, "Memory allocation error.\n");
        return EXIT_FAILURE;
    }
    dummyHead->availableSpace = initialMemory + 1;
    dummyHead->next = initialBlock;
    strcpy(initialBlock->processId, FREE_LABEL);
    initialBlock->startAddress = 0;
    initialBlock->endAddress = initialMemory;
    initialBlock->availableSpace = initialMemory + 1;
    initialBlock->next = NULL;
    lastAddressSpace = initialMemory;

    printf("\nMemory initialized with %d free bytes.\n", dummyHead->availableSpace);

    // Display commands
    printf("Commands:\n");
    printf("  RQ <ProcessID> <Space> <Algorithm>  (e.g., RQ p1 100 B)\n");
    printf("  RL <ProcessID>                      (Release memory)\n");
    printf("  C                                   (Compact memory)\n");
    printf("  STAT                                (Display memory status)\n");
    printf("  X                                   (Exit)\n\n");

    char command[128], requestType[5], processId[3], algoType[2];
    int spaceRequested;

    while (1) {
        printf("Command > ");
        if (!fgets(command, sizeof(command), stdin))
            break;
        command[strcspn(command, "\n")] = '\0';

        if (sscanf(command, "%s", requestType) < 1) {
            printf("Invalid command. Try again.\n");
            continue;
        }

        if (strcmp("X", requestType) == 0)
            break;

        if (strcmp("RQ", requestType) == 0) {
            if (sscanf(command, "%s %2s %d %1s", requestType, processId, &spaceRequested, algoType) != 4) {
                printf("Usage: RQ <ProcessID> <Space> <Algorithm>\n");
            } else {
                requestMemory(processId, spaceRequested, algoType);
            }
        } else if (strcmp("RL", requestType) == 0) {
            if (sscanf(command, "%s %2s", requestType, processId) != 2) {
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