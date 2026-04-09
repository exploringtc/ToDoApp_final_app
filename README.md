# CYSE 570 Final Project — To-Do List OS Application

**Course:** CYSE 570 - Operating Systems Fundamentals  
**Institution:** George Mason University  
**Semester:** Spring 2026  
**Team:** Mustafa Badaoui & Thomas Cho  
**GitHub Repository:** https://github.com/exploringtc/cyse570finalproject  
**Base OS:** [PeachOS](https://github.com/nibblebits/PeachOS)

---

## Selected Topic

**To-do List OS**

## Detailed Project Description

This project extends the custom kernel by implementing a terminal-based task management application that runs within the OS environment. The user is able to create, access, delete, and save task lists while using a lightweight reversible encryption system to store tasks securely.

The application runs entirely within the program space of the kernel, using system calls, interrupts, and processes to interface with system resources.

The main feature of this system is secure persistence. The task lists are stored on disk using XOR-based reversible encryption, which prevents the encoded data from being read without the correct key. When the system is booted and a file is loaded, the task list is decrypted and loaded back into memory.

---

## Kernel Folder Layout

This is how the project is organized in the kernel folder, specifically under the `programs` directory:

```text
PeachOS/
└── programs/
   └── todo/
      ├── main.c
      ├── todo.c
      ├── todo.h
      ├── syscall.c
      ├── encrypt.c
      ├── disk.c
      ├── disk.h
      └── Makefile
```

---

## Detailed Steps of the Application Operations

### 1. System Startup

- The kernel boots and initializes memory, paging, and process structures.
- The to-do application is loaded from the `/programs` directory.

### 2. User Interface (Terminal-Based)

The user interacts through a single command-line interface:

- `add <task>` -> adds a new task
- `list` -> displays all tasks
- `remove <id>` -> deletes a task
- `save <filename>` -> saves tasks to disk
- `load <filename>` -> loads tasks from disk

### 3. Task Management (In-Memory)

- Tasks are stored in a dynamic in-memory structure, such as an array or linked list.
- Each task has an **ID**, **Description**, and **Status** (complete or incomplete).

### 4. Saving Tasks (Encryption + Disk Write)

When `save` is called:

- Tasks are encoded into a buffer before encryption and storage.
- XOR encryption is applied using a user-provided key.
- Encrypted data is written to disk through a kernel disk driver.

### 5. Loading Tasks (Disk Read + Decryption)

When `load` is called:

- The encrypted file is read from disk.
- XOR decryption is applied using the same key.
- Data is reconstructed into task structures in memory.

---

## Description of the System Call Operation

### Add

`sys_add_task(char *task)` adds a new task to the in-memory task list. The user passes a string, such as a task description. The kernel validates the pointer `char *task`, allocates memory for the new task, assigns a unique ID, and inserts it into a task list such as an array or linked list. This demonstrates safe memory manipulation in kernel space and prevents direct user access to kernel memory.

### List

`sys_list_tasks()` displays all current tasks stored in memory. The kernel iterates through the task list and prints each task in terms of the ID and description. The output is sent to the terminal through the kernel print function. This demonstrates read-only access to kernel-managed data and shows how the kernel safely exposes information to the user.

### Remove

`sys_remove_task(int id)` deletes a task using its unique ID. The kernel searches for tasks with matching IDs. If found, it removes the task from the data structure and frees associated memory. If it is not found, it returns an error message. This demonstrates memory deallocation and safe modification of kernel data structures.

### Save

`sys_save_tasks(char* filename, char* key)` saves the current task list to disk using XOR encryption. The tasks are converted into a byte buffer, XOR encryption is applied using the provided key, and then the kernel calls the disk driver to create or open the file and write the encrypted data.

Encryption logic:

```c
encrypted_data[i] = data[i] ^ key[i % key_length]
```

This demonstrates file I/O through the kernel, basic encryption, and data persistence.

### Load

`sys_load_tasks(char* filename, char* key)` loads and restores tasks from an encrypted file. The kernel reads a file from disk into a buffer, applies XOR decryption using the same key, and reconstructs tasks into memory structures.

Decryption logic:

```c
original_data[i] = encrypted_data[i] ^ key[i % key_length]
```

This demonstrates file reading, data reconstruction, and reversible encryption.

Overall, each system call enforces controlled access to kernel resources, ensuring that user-level programs cannot directly manipulate memory or disk without going through a validated kernel interface.

---

## Description of the Interrupt Operation

### Keyboard Interrupt

1. When the user presses a key, an interrupt is generated.
2. The CPU pauses the current flow.
3. The interrupt handler related to the keyboard is run.
4. Keystrokes are registered and processed.
5. Key characters are moved into the terminal.
6. Control returns to the running application.

---

## Description of the Process Operation Utilizing `fork()`

1. The terminal acts as the parent process.
2. When the application starts, the shell calls `fork()`.
3. A child process is created.
4. The child process runs the application.
5. The parent process either waits for the child or continues handling future shell operations.

---

## Description of the Page Table Directory

When the application starts, the OS maps memory pages for that process. When the user adds tasks, more memory may be used for storing task strings and command input. When a file is opened or saved, temporary buffers are placed in memory as well.

When `fork()` is used, the new process will need its own view of memory and therefore a copy of the address space. This prevents one process from interfering with the memory address space of another in a multitasked environment.

---

## Current Implementation & Future Progress for Phase 3

At this stage, the implementation is around 30 to 40 percent complete. The kernel environment is fully set up and the framework of the to-do app is in the `/programs` directory. The parts of the operating system that have been designed include the task data structure, command parser, system call interface, and XOR encryption algorithm, which is partially written.

For Phase 3, the plan is to complete the application by integrating the system calls and interrupt mechanisms and implementing full disk functionality. The next phase also includes finalizing process management with `fork()`, implementing page handling refinements in the command-line interface, and testing the encryption and decryption workflow for accuracy. The final stage will focus on testing, debugging, and full system integration to ensure all components work cohesively.