<style>
  body {
    font-family: 'Segoe UI', 'Helvetica Neue', Arial, sans-serif;
    background: #fbfbfb;
    color: #2d2d2d;
    padding: 20px;
    line-height: 1.6;
  }
  h1, h2, h3, h4 {
    color: #2c3e50;
    border-bottom: 2px solid #2980b9;
    padding-bottom: 5px;
  }
  pre, code {
    font-family: 'Consolas', 'Courier New', monospace;
    background: #f0f0f0;
    color: #2d2d2d;
    padding: 10px;
    border-radius: 4px;
    overflow-x: auto;
  }
  .terminal {
    background: #1e1e1e;
    color: #dcdcdc;
    font-family: 'Consolas', 'Courier New', monospace;
    padding: 10px;
    border-radius: 4px;
  }
  a {
    color: #2980b9;
    text-decoration: none;
  }
  a:hover {
    text-decoration: underline;
  }
  .note {
    background:rgb(18, 176, 255);
    border-left: 5px solid #3498db;
    padding: 10px;
    margin: 10px 0;
  }
</style>

# Contiguous Memory Allocator 🌟 

Welcome to the **Contiguous Memory Allocator** project! This simulation in C demonstrates how operating systems manage memory by allocating contiguous blocks. Enjoy an interactive, cross-platform journey into dynamic memory allocation.

## Features ✨

- **Dynamic Memory Request:**  
  Allocate memory blocks using three elegant strategies:
  - **First Fit:** Uses the first block that is large enough.
  - **Best Fit:** Selects the smallest block that fits to reduce waste.
  - **Worst Fit:** Chooses the largest block, leaving a generous free space.

- **Memory Release:**  
  Free allocated memory and merge adjacent free blocks to create larger continuous free space.

- **Memory Compaction:**  
  Compact memory by merging contiguous free blocks, reducing fragmentation.

- **Status Reporting:**  
  Visualize the current memory layout, including both allocated and free blocks.

- **Interactive Command Loop:**  
  Run the program continuously and interact via commands. No matter your operating system, the instructions below will help you get started.

## Future Enhancements 🔮

- **Wait Queue:** Add a queue for processes waiting for memory.
- **Code Refactoring:** Modularize repeated code segments.
- **Enhanced Merging:** Improve the release and compaction functions for smoother operation.

## Compilation 🔨

### On Windows

Use a compiler like [MinGW](http://www.mingw.org/) or Visual Studio's Developer Command Prompt. For example, with MinGW:

```bash
gcc -o allocator.exe contiguous_memory_allocator.c
```

### On Linux/macOS

Simply use GCC:

```bash
gcc -o allocator contiguous_memory_allocator.c
```

## Usage ⚡

### Running the Program

- **With a Command-line Argument:**  
  Supply the initial memory size directly.

  **Windows Example:**

  ```bash
  allocator.exe 1000
  ```

  **Linux/macOS Example:**

  ```bash
  ./allocator 1000
  ```

  *(This initializes the memory with 999 bytes after internal adjustments.)*

- **Without a Command-line Argument:**  
  The program will prompt you to enter the initial memory size:

  ```terminal
  allocator.exe
  Enter initial memory size: 1000
  ```

  or

  ```terminal
  ./allocator
  Enter initial memory size: 1000
  ```

### Commands

At the `allocator>` prompt, use these commands:

- **RQ `<ProcessID>` `<Space>` `<Algorithm>`**  
  Request a memory block for a process.  
  - `<ProcessID>`: A 3-character identifier.
  - `<Space>`: Amount of memory requested.
  - `<Algorithm>`: Allocation strategy (`F` for First Fit, `B` for Best Fit, `W` for Worst Fit).
  
  **Example:**

  ```terminal
  RQ p1 100 B
  ```

- **RL `<ProcessID>`**  
  Release memory allocated to a process.
  
  **Example:**

  ```terminal
  RL p1
  ```

- **C**  
  Compact memory by merging adjacent free blocks.

- **STAT**  
  Display the current memory status.

- **X**  
  Exit the program.

## 💡 Example Session

Below is an example session, styled for terminal output, that works across platforms:

```terminal
C:\Adv_OS_Task1>allocator.exe
Enter initial memory size: 1000
Initialize free space: 999 bytes
allocator> RQ p1 100 B
allocator> STAT
Total available space: 899 bytes
Addresses [0 : 100] -> Process: p1
Addresses [101 : 999] -> Process: FREE
allocator> RQ p2 50 F
allocator> STAT
Total available space: 849 bytes
Addresses [0 : 100] -> Process: p1
Addresses [101 : 150] -> Process: p2
Addresses [151 : 999] -> Process: FREE
allocator> RL p1
allocator> STAT
Total available space: 949 bytes
Addresses [0 : 100] -> Process: FREE
Addresses [101 : 150] -> Process: p2
Addresses [151 : 999] -> Process: FREE
allocator> C
allocator> STAT
Total available space: 949 bytes
Addresses [0 : 150] -> Process: FREE
Addresses [151 : 999] -> Process: FREE
allocator> X
```

In this session:

- **RQ p1 100 B:** Allocates 100 bytes for process "p1" using Best Fit.
- **STAT:** Displays the updated memory layout.
- **RQ p2 50 F:** Allocates 50 bytes for process "p2" using First Fit.
- **RL p1:** Releases memory allocated to "p1".
- **C:** Compacts memory by merging free blocks.
- **X:** Exits the program.

## 🎉 Conclusion

This simulation offers an engaging, cross-platform way to explore contiguous memory allocation, fragmentation, and memory compaction. Whether you're on Windows, Linux, or macOS, dive in and enjoy the art of memory management with clarity and style!
