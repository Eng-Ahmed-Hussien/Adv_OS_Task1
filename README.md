# Contiguous Memory Allocator 🚀

Welcome to the **Contiguous Memory Allocator**—an interactive C-based simulation that brings the magic of memory management to life! Whether you're a student, a coder, or just curious, dive into this dynamic tool and explore how operating systems juggle memory like cosmic wizards. Ready to allocate, release, and compact your way to mastery? Let’s go! 🌟

## Features That Shine ✨

- **Memory Allocation Magic** 🎩  
  Choose your strategy and watch memory blocks come to life:  
  - **First Fit (F):** Snags the first block that fits—like finding a parking spot fast!  
  - **Best Fit (B):** Picks the coziest block to minimize waste—efficiency at its finest!  
  - **Worst Fit (W):** Grabs the biggest block, leaving room for future adventures!  

- **Memory Release**  
  Free up space and watch adjacent blocks merge into a seamless galaxy of free memory.

- **Memory Compaction**  
  Sweep away fragmentation by fusing free blocks into one tidy, contiguous expanse.

- **Status Reporting** 
  Peek into the memory universe with detailed snapshots of allocated and free blocks.

- **Interactive Command Loop**
  Engage with the allocator in real-time—works flawlessly on Windows, Linux, or macOS!

## What’s Next on the Horizon?

- **Wait Queue:** Park processes in line when memory’s tight.  
- **Code Polish:** Streamline the spells with modular magic.  
- **Smarter Merging:** Enhance release and compaction for a smoother cosmic dance.

## Compilation Station🔧

### Windows Warriors

Grab [MinGW](http://www.mingw.org/) or use Visual Studio’s Command Prompt:  

```bash
gcc -o allocator.exe contiguous_memory_allocator.c
```

### Linux & macOS Mavericks
  
Unleash GCC with ease:  

```bash
gcc -o allocator contiguous_memory_allocator.c
```

<div style="background: #e1f5fe; border-left: 6px solid #0288d1; padding: 15px; border-radius: 5px; margin: 15px 0; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); transition: transform 0.3s ease;color:#322;">
  <strong>Pro Tip:</strong> Ensure your compiler is ready—GCC is your trusty wand for this adventure!
</div>

## How to Launch Your Journey ⚡

### Start the Engine  

- **With a Size Boost:** Feed it memory at launch:  
  **Windows:**

  ```bash
  allocator.exe 1000
  ```  

  **Linux/macOS:**

  ```bash
  ./allocator 1000
  ```  

  *(Sets up 999 bytes of memory fun—accounting for a sneaky -1 adjustment!)*  

- **Prompt Style:** Let it ask you:

```terminal
  ./allocator
  Enter initial memory size: 1000
```

### Command Your Universe

At the `Command >` prompt, wield these powers:  

- **RQ `<ProcessID>` `<Space>` `<Algorithm>`**  
  Summon memory for a process (e.g., `RQ p1 100 B`).  
  - `<ProcessID>`: A snappy 2-char name (e.g., `p1`).  
  - `<Space>`: Bytes you crave.  
  - `<Algorithm>`: `F`, `B`, or `W`.  

- **RL `<ProcessID>`**  
  Liberate a process’s memory (e.g., `RL p1`).  

- **C**  
  Compact the cosmos—merge free blocks!  

- **STAT**  
  Reveal the memory map in all its glory.  

- **X**  
  Exit the adventure gracefully.

## Example Usage Time

Here’s a cosmic session to spark your imagination:

```terminal
Command > RQ p1 100 B
Allocation Successful! Process p1 allocated using Best Fit. Block: [0 : 99]
Command > STAT
----- Memory Status -----
Total available space: 899 bytes
Addresses [0 : 99] -> p1
Addresses [100 : 999] -> FREE
-------------------------
Command > RQ p2 50 F
Allocation Successful! Process p2 allocated using First Fit. Block: [100 : 149]
Command > RL p1
Memory released for process p1.
Command > STAT
----- Memory Status -----
Total available space: 949 bytes
Addresses [0 : 99] -> FREE
Addresses [100 : 149] -> p2
Addresses [150 : 999] -> FREE
-------------------------
Command > C
Memory compacted successfully.
Command > STAT
----- Memory Status -----
Total available space: 949 bytes
Addresses [0 : 949] -> FREE
Addresses [950 : 999] -> p2
-------------------------
Command > X
Exiting allocator. Goodbye!
```

### What Happened?

- **RQ p1 100 B:** Best Fit snagged 100 bytes for `p1`.  
- **RQ p2 50 F:** First Fit grabbed 50 bytes for `p2`.  
- **RL p1:** Freed `p1`’s space.  
- **C:** Merged free blocks into one stellar chunk.  

<div style="background: #e1f5fe; border-left: 6px solid #0288d1; padding: 15px; border-radius: 5px; margin: 15px 0; box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1); transition: transform 0.3s ease; color:#322;">
  <strong>Fun Fact:</strong> The allocator adjusts memory size by -1 internally—999 bytes from 1000 keeps the math tight!
</div>

## Team Members

- **[Eng/ Youssif Seliem]** :
  - **Role** : Supervisor | Adv OS Assistant Lecturer
- **Ahmed Hussien El Sayed** :
  - **Role** : Project Lead | Web Developer
  - **Academic ID**: 15210408
- **Eslam Sa3d Gom3a Mohamed** :
  - **Role** : Python Developer | Documentation Specialist
  - **Academic ID**: 
- **Asmaa Emad Abd-El3lem** : 15210066
  - **Role** : Developer | Code Tester
  - **Academic ID**: 15210075
- **Eman Ayman Mabrouk** :
  - **Role** : Developer | Documentation Specialist
  - **Academic ID**: 15210086
- **Mostafa......** :
  - **Role** : Tester| Documentation Specialist
  - **Academic ID**:

## Why You’ll Love This 🌟

This isn’t just a program—it’s a playground! Explore memory allocation, tackle fragmentation, and master compaction, all with a sleek, cross-platform vibe. Whether you’re on Windows, Linux, or macOS, this tool invites you to play, learn, and conquer the art of memory management.  

## Live Demo Time!

<a href="https://eng-ahmed-hussien.github.io/Memory-Allocator/" style="display: inline-block; padding: 10px 20px; background: #26a69a; color: white; border-radius: 5px; text-align: center; text-decoration: none; margin: 5px; transition: background 0.3s ease;">Go To Life Simulator 🚀</a>