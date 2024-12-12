
# Operating Systems Project: Multithreading - N Producers M Consumers Problem

## Introduction
This project implements a multithreaded system to analyze the cost of using machines in a processing center. The system employs:
- **Producers**: Extract data from a file and insert it into a circular buffer.
- **Consumers**: Process data from the buffer to calculate and accumulate costs.

The circular buffer is implemented as a queue with no concurrency, and threads are synchronized using mutexes and condition variables.

## Code Overview

### `costCal.c`
- Implements the main functionality of the cost calculation system using threads for producers and consumers.
- **Key Functions:**
  - `producer()`: Reads data and inserts it into the circular buffer.
  - `consumer()`: Processes data from the buffer and calculates costs.
  - `main()`: Initializes threads, manages synchronization, and computes the total cost.

### `queue.c`
- Implements a circular buffer for shared data storage.
- **Key Functions:**
  - `queue_init()`: Initializes the queue.
  - `queue_put()`: Inserts an element into the queue.
  - `queue_get()`: Retrieves an element from the queue.
  - `queue_empty()`: Checks if the queue is empty.
  - `queue_full()`: Checks if the queue is full.
  - `queue_destroy()`: Frees allocated resources.

### `queue.h`
- Defines the structure of the elements and the queue.
- Includes the following:
  - Element structure: Machine type and usage time.
  - Queue structure: Maximum size, current size, head, and tail pointers.

## Project Highlights
- **Multithreading:** Efficiently manages N producers and M consumers.
- **Synchronization:** Uses mutexes and condition variables to avoid race conditions.
- **Circular Buffer:** Implements a queue for shared data storage with wrap-around functionality.
- **Error Handling:** Ensures threads handle edge cases like empty or full queues.

## How It Works
1. Producers read data from an input file and insert it into the circular buffer.
2. Consumers process the data to calculate partial costs.
3. The main thread accumulates partial costs and computes the total.

## Notes
- **Broadcast Wake-Up:** Ensures no consumers are left waiting when the process ends.
- **Memory Management:** Proper allocation and deallocation of resources.

## Instructions
1. Clone the repository.
2. Compile the code using a C compiler with the required libraries for pthreads.
3. Run the executable, providing the necessary input file and parameters.

This project demonstrates the application of multithreading concepts in operating systems, addressing synchronization and resource sharing challenges.
