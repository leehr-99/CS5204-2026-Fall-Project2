# CS5204 - Memory Page Table Walker

## Introduction

In this assignment, you will be developing a page table walk program both in user-space and kernel-space. The primary goal is to develop a comprehensive understanding of virtual memory management by creating a tool that can traverse the page table hierarchy. By building this functionality in both user-space and kernel-space, you will gain insight into the different privileges and constraints of each environment while exploring the fundamental mechanisms that translate virtual addresses into physical addresses.

The goals of this project are:
- To learn basic characteristics of pages in memory system
- To learn how page table is mapped in user-space with `/proc/$pid/pagemap`
- To learn how page table is managed in kernel-space and how it is different with user-space

## Task 1: Background Study [15 Points]

In a modern operating system, physical memory is managed in fixed-size blocks known as pages. To create an isolated and consistent environment for each running program, the OS utilizes virtualization. This gives every process its own private virtual address space, making it seem as though it has the entire memory to itself. These virtual addresses, however, are just an abstraction. To find the actual location of the data in the computer's RAM, the OS maintains a special data structure for each process called a page table. This table acts as a map, translating the virtual addresses used by the process into the real physical addresses. The procedure of navigating this page table to look up the corresponding physical page for a given virtual page is fundamentally what is known as a page table walk.

- [OSTEP: Address Translation](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-mechanism.pdf) [REQUIRED]
- [Examining Process Page Tables](https://docs.kernel.org/admin-guide/mm/pagemap.html) [REQUIRED]
- [Page Tables](https://www.kernel.org/doc/html/latest/mm/page_tables.html#page-tables) [REQUIRED]
- [OSTEP: Paging: Smaller Tables](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-smalltables.pdf) [STRONGLY RECOMMENDED]
- [Memory Allocation Guide](https://www.kernel.org/doc/html/v5.0/core-api/memory-allocation.html) [STRONGLY RECOMMENDED]
- [OSTEP: Memory API](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-api.pdf) [RECOMMENDED]
- [OSTEP: Paging: Introduction](https://pages.cs.wisc.edu/~remzi/OSTEP/vm-paging.pdf) [RECOMMENDED]
- [Linux Kernel Modules](https://linux-kernel-labs.github.io/refs/heads/master/labs/kernel_modules.html) [RECOMMENDED]

As a part of this task, you should take the time to deep dive into memory, address translation, and multi-level page table structure. You are required to read the "OSTEP: Address Translation," "Examining Process Page Tables," and "Page Tables." Then, submit paragraphs of **What is multi-level page table, How it works, How it is managed differently in user-level and kernel-level.** There is no minimum word limit, so it can be brief but it should convey your understanding of page table both in user-level and kernel-level.

For Task 1, you are to provide your answers to the questions above in pdf file format. Remember, conveying your understanding through your answers is most important.

## Task 2: Page Table Walk within Userspace

### Part 1: Using ```/proc/$pid/pagemap``` [25 Points]

In this project, you will be given with the starter codes for page table walker within userspace. As you studied in Task 1, the user-level processes can access their page table via ```/proc/$pid/pagemap```. Therefore, your userspace walker also going to use its own page table with ```/proc/$/pagemap```. Detailed instructions follow.

Write a C program named "memalloc" that allocates a total of 1GB DRAM with ```calloc()``` in the following ways:
1. Using double pointer array to allocate 4KB block each time, loop until it reaches 1GB in total.
2. Using a single pointer to allocate one big 1GB block.

*Note: If your machine does not have enough memory to allocate 1GB, please re-set your program parameter to accommodate your system. For example, if your system can only accommodate 500MB, adjust the total DRAM to 500MB and the calloc arguments accordingly.*

Using ```pagemap.c``` to translate the virtual addresses of your 1GB DRAM in ```memalloc.c``` to the physical addresses. Then, log the result of the translation in the output file with the format of ```VA -> PA``` (e.g. VA=0x295952a0 -> PA=0x25f89e2a0).
1. In the 4KB allocation case, loop through the double pointer array to translate the VA of each 4KB block to its PA.
2. In the 1GB case, the process is quite different:
   - Before performing a page table walk, you have to first "**touch**" pages. Since malloc uses **lazy allocation**, the physical pages are not actually allocated until they are accessed. So, you need to perform a write once every 4KB to trigger a page fault.
   - Now, you can correctly translate 4KB virtual addresses to physical addresses for the 1GB block.

Your C program must be invoked as follows:
```bash
$ ./memalloc [-s <size>] [-o <output>]
```
- **size**: the size that will be passed to one calloc call (4K or 1G).
- **output**: the file that where the "VA -> PA" transition results will be written to.

### Part 2: Analyse Observation [10 Points]

Describe your observations based on the results you obtained. You are free to explain your findings and interpretations in your own words. There is no minimum word limit; however, your response should clearly and accurately convey your understanding in complete sentences.

**Important: Do not provide a short abbreviated answers/statements. Instead, elaborate on your thoughts and ensure that your explanation demonstrates careful reasoning.**

To guide your writing, here are some example questions you may consider. Note that you need not answer all these questions nor are these a complete set of questions.
- Are virtual and physical addresses continuous?
  - If so, what is the offset between them?
- Is the offset the same as the number of bytes passed to calloc?
  - If not, what explains the difference?
- What differences can be observed between the two cases?
- Did you notice any other interesting observations in this task?

Finally, your observations should be submitted as a pdf file and clearly indicate that these results correspond to Task 2.

## Task 3: Page Table Walk within Kernel Module

In this task, you will implement a Linux kernel module, ko5204, to perform various page table walks at the kernel level. To familiarize yourself with Linux kernel modules, please read the following resource: [Linux Kernel Modules](https://linux-kernel-labs.github.io/refs/heads/master/labs/kernel_modules.html)

Your module should expose a ```/proc/cs5204``` interface that allows user-space to communicate with ```ko5204```. The interface must accept write commands such as:
```bash
$ echo "vmalloc" > /proc/cs5204
$ echo "kmalloc" > /proc/cs5204
```

In the kernel, there are two primary memory allocation functions: ```vmalloc``` and ```kmalloc``` (there is also ```alloc_page```, but we will ignore it here). Since these functions allocate memory differently, the methods for walking their corresponding page tables also differ. Your task is to study these differences and implement page table walkers for both. For reference, see: [Memory Allocation Guide](https://www.kernel.org/doc/html/v5.0/core-api/memory-allocation.html)

**IMPORTANT: Always free allocated memory (vfree/kfree) in the kernel. Memory will not be automatically released when the module exits!**

### Part 1: Using Vmalloc [20 Points]

When your kernel module interface ```/proc/cs5204``` receives the write command ```vmalloc```, it should perform the following steps:
- Allocate 1 MB of kernel memory (4KB * 256) using a single void* array via the ```vmalloc``` function.
- Walk the kernel page table in page-sized steps (4KB).
- Log each virtual-to-physical address translation to ```/var/log/kern.log``` in the format:
- Vmalloc: VA=0xff283b802d92c000 -> PA=0x000000016ddac000
- Free the allocated memory using ```vfree```.

### Part 2: Using Kmalloc [20 Points]

When your kernel module interface ```/proc/cs5204``` receives the write command ```kmalloc```, it should perform the following steps:
- Allocate 1 MB of kernel memory (4KB * 256) using a single void* array via the ```kmalloc``` function.
- Walk the kernel page table in 4KB page-sized steps.
- Log each virtual-to-physical address translation to ```/var/log/kern.log``` in the format:
- Kmalloc: VA=0x00000000b78cfd88 -> PA=0x00000020cebff000
- Free the allocated memory using ```kfree```.

Here are the compile and install module steps:
```bash
$ make
$ insmod ko5204.ko
```
You can check if your module is installed correctly with this command:
```bash
$ lsmod | grep ko5204
```
After you recompile your code, you have to remove the current module first:
```bash
$ rmmod ko5204
```
Then, you can ```make``` and ```insmod``` again.

### Part 3: Analyse Observation [10 Points]

Describe your observations based on the results you obtained. You are free to explain your findings and interpretations in your own words. There is no minimum word limit; however, your response should clearly and accurately convey your understanding in complete sentences.

**Important: Do not provide a short abbreviated answers/statements. Instead, elaborate on your thoughts and ensure that your explanation demonstrates careful reasoning.**

To guide your writing, here are some example questions you may consider. Note that you need not answer all these questions nor are these a complete set of questions.
- Are the virtual and physical addresses continuous in both vmalloc and kmalloc?
   - If they are continuous, what is the offset between them?
- Is the offset the same as the number of bytes passed to memory allocation functions?
   - If not, what factors might explain these differences?
- What differences can you observe between vmalloc and kmalloc?
- What differences can you observe between user-level and kernel-level memory allocation?
- Did you notice any other interesting behaviors during the task?

Finally, your observations should be submitted as a pdf file, and clearly indicate that these results correspond to Task 3.

### Bonus: [5 Points]

- What happens when you try to allocate 1GB of memory in the kernel using vmalloc?
- What happens when you try to allocate 1GB of memory in the kernel using kmalloc?
- Are there any differences between the two cases? If so, why?

Your observations should be submitted as a pdf file and clearly indicate that these results correspond to the Bonus Task.

## Submission

For the submission, you must provide a tarball (.tar.gz) that contains the following:
1. The Makefile and source code for ```memalloc``` (Task 2).
2. The Makefile and source code for ```ko5204``` (Task 3).
3. A **single** PDF file that includes:
   - The write-up for Task 1
   - All observations for Tasks 2 and 3

**Important: Place the Makefile and source code for memalloc and ko5204 in separate directories to avoid filename collisions.**


## Hint

**User-Level Page Walk**
- Be mindful of the numeric unit and base when handling addresses. Using data types that are too small (e.g., int) may cause overflow when working with large numbers.
- You should understand how to access ```/proc/$pid/pagemap``` from user space.
- If helpful, consider visualizing your results to gain clearer insights from your observations.

**Kernel-Level Page Walk**
- No complex algorithms are required. You can import the appropriate **kernel libraries** directly into your module.
- A good starting point is to review kernel source code related to ```vmalloc``` (vmalloc.c), ```kmalloc``` (io.h), virtual addresses, and physical addresses.
- I recommend using [Elixir Bootlin](https://elixir.bootlin.com/linux/v6.14/source/) to browse kernel code. Be sure to match your kernel version, as many APIs and implementations change or are deprecated between releases.

**Kernel Logs**
- Kernel log messages can be found in ```/var/log/kern.log```.
- Use the ```tail``` command with the ```-n``` flag to display the last n lines of the log.
- Alternatively, you can view a larger set of messages using the ```dmesg``` command.









