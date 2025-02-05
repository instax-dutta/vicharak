# Vicharak Kernel Module

## Overview

Vicharak is a character device kernel module that implements a simple queue for inter-process communication or data buffering within the kernel space. It allows user-space applications to push data into the queue and pop data from it using IOCTL commands. This module is designed for educational purposes and demonstrates basic kernel module development concepts, including character device registration, file operations, IOCTL handling, wait queues, and mutexes.

## Files

- **Makefile**:  Defines the build process for the kernel module and user-space applications. It uses the standard Linux kernel module build system.
- **vicharak.c**:  Contains the source code for the Vicharak kernel module. This file implements the character device driver, including:
    - Device registration and unregistration.
    - File operations (`open`, `release`, `ioctl`).
    - IOCTL commands (`SET_SIZE_OF_QUEUE`, `PUSH_DATA`, `POP_DATA`).
    - A character buffer queue implemented using `kmalloc` and `kfree`.
    - Wait queue for blocking `POP_DATA` operations when the queue is empty.
    - Mutex for protecting the queue data structure from race conditions.
- **configurator.c**: (To be implemented) A user-space program intended to set the size of the queue in the kernel module via the `SET_SIZE_OF_QUEUE` IOCTL command.
- **filler.c**: (To be implemented) A user-space program designed to push data into the Vicharak kernel module's queue using the `PUSH_DATA` IOCTL command.
- **reader.c**: (To be implemented) A user-space program that reads data from the Vicharak kernel module's queue using the `POP_DATA` IOCTL command.

## How to Build

To build the Vicharak kernel module, you need to have the Linux kernel headers installed. Then, simply run the `make` command in the project directory:

```bash
make
```

This will compile the `vicharak.c` kernel module and generate the `vicharak.ko` kernel object file.

To clean the build files, run:

```bash
make clean
```

## How to Use

1. **Load the Kernel Module:**
   Use the `insmod` command to load the `vicharak.ko` kernel module:

   ```bash
   sudo insmod vicharak.ko
   ```

   This will register the character device with the major number allocated dynamically and print the major number to the kernel log. You can check the kernel log using `dmesg`.

2. **Create Device Node:**
   Create a device node in the `/dev` directory using the `mknod` command. You will need the major number obtained from `dmesg` after loading the module. For example, if the major number is 240, use:

   ```bash
   sudo mknod /dev/vicharak c 240 0
   sudo chmod 666 /dev/vicharak # Optional: set permissions for read/write access
   ```

3. **Interact with the Device (User-space programs - to be implemented):**
   - **configurator**: Use this program to set the size of the queue before pushing or popping data.
   - **filler**:  Use this program to push data into the queue. You will need to format the data according to the `struct data` definition in `vicharak.c`.
   - **reader**: Use this program to pop data from the queue. It will read data and display it.

4. **Unload the Kernel Module:**
   When you are finished, unload the kernel module using the `rmmod` command:

   ```bash
   sudo rmmod vicharak
   ```

   This will unregister the character device and clean up the kernel module. Check `dmesg` to confirm successful unregistration.

## IOCTL Commands

The Vicharak kernel module supports the following IOCTL commands, which are defined in `vicharak.c`:

- **`SET_SIZE_OF_QUEUE` (_IOW('a', 'a', int *))**:
  - **Purpose**: Sets the size of the character queue. This command must be called before pushing or popping data to allocate memory for the queue.
  - **Argument**:  An integer pointer to the desired size of the queue.
  - **Example (in user-space program)**:
    ```c
    int size = 10; // Set queue size to 10
    ioctl(fd, SET_SIZE_OF_QUEUE, &size);
    ```

- **`PUSH_DATA` (_IOW('a', 'b', struct data *))**:
  - **Purpose**: Pushes data into the queue.
  - **Argument**: A pointer to a `struct data` structure containing the length of the data and the data itself (max 256 bytes).
  - **`struct data` definition**:
    ```c
    struct data {
        int length;      // Length of the data to be pushed
        char data[256];  // Data buffer (max 256 bytes)
    };
    ```
  - **Example (in user-space program)**:
    ```c
    struct data data_to_push;
    char message[] = "Hello from user space!";
    data_to_push.length = strlen(message) + 1; // Include null terminator
    strcpy(data_to_push.data, message);
    ioctl(fd, PUSH_DATA, &data_to_push);
    ```

- **`POP_DATA` (_IOR('a', 'c', struct data *))**:
  - **Purpose**: Pops data from the queue. If the queue is empty, the process will block until data is available (using a wait queue).
  - **Argument**: A pointer to a `struct data` structure where the popped data will be copied. The `length` field in the returned `struct data` will indicate the length of the popped data.
  - **Example (in user-space program)**:
    ```c
    struct data received_data;
    ioctl(fd, POP_DATA, &received_data);
    printf("Received data: %s\n", received_data.data);
    ```

---

**Note**: This README provides a basic overview of the Vicharak kernel module.  The user-space programs (`configurator.c`, `filler.c`, `reader.c`) are intended to be implemented to fully utilize and test this module.
