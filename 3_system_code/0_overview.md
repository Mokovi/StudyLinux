## 系统编程 (System Programming)

1. **Linux 系统调用基础**  
   *English: System Call Basics*  
   * 概念：用户态与内核态切换  
   * 常用接口：`syscall` 封装  
   * 错误处理：`errno` 机制及 `perror`/`strerror`  
   * 调试与跟踪：`strace`、`ltrace`  

2. **文件 I/O 与进程控制**  
   *English: File I/O & Process Control*  
   * 打开、读写与定位：`open`, `read`, `write`, `lseek`, `close`  
   * 文件锁：`fcntl(F_SETLK/F_SETLKW)`, `flock`  
   * 异步 I/O：POSIX AIO (`aio_read`, `aio_write`)、`io_submit`/`io_getevents` (libaio)  
   * 零拷贝 I/O：`sendfile`, `splice`
   * 进程创建与执行：`fork`, `vfork`, `clone`, `execve`, `waitpid`, `exit`  
   * 进程属性：`setuid`/`setgid`, `nice`/`setpriority`  

3. **进程间通信（IPC）**  
   *English: Inter-Process Communication*  
   * 匿名管道 `pipe` / 有名管道 FIFO  
   * POSIX 共享内存：`shm_open`, `mmap` + `MAP_SHARED`  
   * System V 共享内存：`shmget`, `shmat`, `shmdt`  
   * 消息队列：`msgget`, `msgsnd`, `msgrcv`  
   * 信号量：System V `semget`, `semop`；POSIX `sem_open`, `sem_post`, `sem_wait`  
   * 轻量级同步：`futex`  
   * 套接字对：`socketpair(AF_UNIX)`  
   * Netlink 套接字：`socket(AF_NETLINK)`  

4. **线程与多线程编程**  
   *English: Threading & Multithreading*  
   * POSIX 线程：`pthread_create`, `pthread_join`, `pthread_detach`  
   * 同步原语：互斥锁 `pthread_mutex`、读写锁 `pthread_rwlock`、条件变量 `pthread_cond`  
   * 线程局部存储（TLS）：`pthread_key_create`, `pthread_setspecific`  
   * 线程取消与清理：`pthread_cancel`, `pthread_cleanup_push/pop`  
   * CPU 亲和性：`pthread_setaffinity_np`  
   * 线程池设计：任务队列 + 工作线程模型  

5. **信号、定时器与事件处理**  
   *English: Signals, Timers & Event Handling*  
   * 信号处理：`signal`, `sigaction`, 实时信号 (`SIGRTMIN`–`SIGRTMAX`)、`sigqueue`  
   * 阻塞/等待信号：`sigprocmask`, `sigsuspend`, `sigwaitinfo`  
   * 定时器：`alarm`, `setitimer`, POSIX 定时器 `timer_create`, `timerfd_create`  
   * 事件驱动：`signalfd`, `eventfd`, `inotify`（文件系统事件监控）  

6. **内存管理**  
   *English: Memory Management*  
   * 动态分配：`malloc`, `calloc`, `realloc`, `free` 与实现原理  
   * 低级接口：`brk`/`sbrk`  
   * 内存映射：`mmap`, `munmap`, `mremap`, Huge Pages  
   * 虚拟内存：页表、TLB、`/proc/<pid>/maps`, `/proc/meminfo`  
   * 内存屏障与可见性：`__sync_synchronize`, C11 原子操作  

7. **系统信息与诊断**  
   *English: System Information & Diagnostics*  
   * 系统调用查询：`uname`, `sysconf`, `getrusage`  
   * `/proc` 和 `/sys` 文件系统  
   * 性能分析：`perf`, `gprof`, `valgrind`  
