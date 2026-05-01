I implemented a cooperative user-level threading library in xv6. Each thread has its own stack and saved stack pointer. Context switching is done using an assembly routine (uswtch.S) that saves and restores registers.

The scheduler is round-robin and cooperative. Threads switch only when thread_yield() is called. thread_create() sets up a new stack and starts execution at a trampoline function. When a thread finishes, it is marked DONE and thread_join() waits for completion.

The mutex is implemented using a simple lock variable. Since scheduling is cooperative, threads yield when the lock is unavailable instead of busy waiting.

The producer-consumer demo uses a bounded buffer protected by a mutex. Two producers generate items and one consumer removes them. The program correctly processes 200 items with no deadlock or corruption.

Limitations:
- Maximum 8 threads
- Fixed stack size per thread
- Cooperative scheduling only