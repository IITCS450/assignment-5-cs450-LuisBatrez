For this assignment, I implemented user-level threads in xv6 using cooperative scheduling. Each thread has its own stack and saved stack pointer, and context switching is handled in uswtch.S by saving and restoring registers.

The scheduler uses a round-robin approach where threads switch only when thread_yield() is called. thread_create() sets up a new thread and starts it using a trampoline function. When a thread finishes, it is marked as DONE, and thread_join() waits until the thread completes.

The mutex is implemented using a simple lock variable. Since scheduling is cooperative, threads call thread_yield() if the lock is already taken instead of spinning forever.

For the demo, I used a producer-consumer setup with a bounded buffer. Two producers generate items while one consumer removes them. The program successfully processes all 200 items with no deadlocks or data corruption.

Limitations:
- Maximum of 8 threads
- Fixed stack size for each thread
- Cooperative scheduling only (threads must yield to switch)