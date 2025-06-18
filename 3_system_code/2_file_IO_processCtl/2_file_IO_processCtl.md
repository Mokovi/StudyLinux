# 文件 I/O 与进程控制 编程笔记

本篇笔记将详细介绍文件 I/O 与进程控制相关的系统调用，从基础到高级，逐一讲解常用函数的概念、语法、参数、使用场景及示例代码。每个部分按函数为单元进行详细讲解，帮助您深入理解每个函数的应用。

## 目录

1. [文件 I/O 基本操作：打开、读写与定位](#一文件-io-基本操作-打开读写与定位)

   * [文件 I/O 基本概念](#1-文件-io-基本概念)
   * [函数 `open`](#2-函数-open)
   * [函数 `read`](#3-函数-read)
   * [函数 `write`](#4-函数-write)
   * [函数 `lseek`](#5-函数-lseek)
   * [函数 `close`](#6-函数-close)
2. [文件锁](#二文件锁)

   * [文件锁的基本概念](#1-文件锁的基本概念)
   * [函数 `fcntl`](#2-函数-fcntl)
   * [函数 `flock`](#3-函数-flock)
3. [异步 I/O](#三异步-io)

   * [异步 I/O 基本概念](#1-异步-io-基本概念)
   * [函数 `aio_read`](#2-函数-aio_read)
   * [函数 `aio_write`](#3-函数-aio_write)
   * [函数 `io_submit` / `io_getevents`](#4-函数-io_submit-io_getevents)
4. [零拷贝 I/O](#四零拷贝-io)

   * [零拷贝 I/O 基本概念](#1-零拷贝-io-基本概念)
   * [函数 `sendfile`](#2-函数-sendfile)
   * [函数 `splice`](#3-函数-splice)
5. [进程的概述](#五进程的概述)
5. [进程创建与执行](#六进程创建与执行)
   * [进程概述] 
   * [进程创建与执行的基本概念](#1-进程创建与执行的基本概念)
   * [函数 `fork`](#2-函数-fork)
   * [函数 `vfork`](#3-函数-vfork)
   * [函数 `execve`](#4-函数-execve)
   * [函数 `waitpid`](#5-函数-waitpid)
   * [函数 `exit`](#6-函数-exit)
6. [进程属性](#七进程属性)

   * [进程属性的基本概念](#1-进程属性的基本概念)
   * [函数 `setuid` / `setgid`](#2-函数-setuid-setgid)
   * [函数 `nice` / `setpriority`](#3-函数-nice-setpriority)
7. [综合实例](#八综合实例)

---

## 一、文件 I/O 基本操作：打开、读写与定位

### 1. 文件 I/O 基本概念

文件 I/O 是操作系统提供的一种机制，允许程序与磁盘上的文件进行交互。文件操作通常包括：打开文件、读取数据、写入数据、定位文件指针以及关闭文件。这些操作通常通过一系列系统调用实现，能够帮助程序高效地进行文件读写。

常用的文件 I/O 函数包括：`open`, `read`, `write`, `lseek`, `close` 等。

---

### 2. 函数 `open`

#### 语法

```c
#include <fcntl.h>

int open(const char *pathname, int flags, mode_t mode);
```

#### 参数说明

* `pathname`：要打开的文件路径。
* `flags`：文件打开模式标志，常见的有：

  * `O_RDONLY`：只读模式。
  * `O_WRONLY`：只写模式。
  * `O_RDWR`：读写模式。
  * `O_CREAT`：如果文件不存在，则创建文件。
  * `O_EXCL`：如果文件已经存在，`open` 调用失败。
* `mode`：文件权限，通常使用 `S_IRUSR`, `S_IWUSR` 等标志。

#### 使用场景

`open` 函数用于打开文件并返回一个文件描述符。它是文件操作的起点，常用于读取、写入或修改文件内容。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    // 打开文件
    int fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    printf("File opened successfully with file descriptor: %d\n", fd);
    close(fd);  // 关闭文件
    return 0;
}
```

---

### 3. 函数 `read`

#### 语法

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);
```

#### 参数说明

* `fd`：文件描述符，通常通过 `open` 函数获得。
* `buf`：数据缓冲区，用于存储读取的数据。
* `count`：要读取的字节数。

#### 使用场景

`read` 函数用于从文件中读取数据。读取的数据会存储在提供的缓冲区中，函数返回实际读取的字节数。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    char buffer[128];
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer)-1);
    if (bytes_read == -1) {
        perror("read failed");
        close(fd);
        return 1;
    }

    buffer[bytes_read] = '\0';  // 确保字符串以null结尾
    printf("Read content: %s\n", buffer);
    close(fd);
    return 0;
}
```

---

### 4. 函数 `write`

#### 语法

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);
```

#### 参数说明

* `fd`：文件描述符。
* `buf`：要写入的数据缓冲区。
* `count`：要写入的字节数。

#### 使用场景

`write` 函数用于向文件中写入数据。它会将缓冲区中的数据写入文件，并返回实际写入的字节数。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("example.txt", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    const char *data = "Hello, World!";
    ssize_t bytes_written = write(fd, data, strlen(data));
    if (bytes_written == -1) {
        perror("write failed");
        close(fd);
        return 1;
    }

    printf("Written content: %s\n", data);
    close(fd);
    return 0;
}
```

---

### 5. 函数 `lseek`

#### 语法

```c
#include <unistd.h>

off_t lseek(int fd, off_t offset, int whence);
```

#### 参数说明

* `fd`：文件描述符。
* `offset`：偏移量，表示从某个位置的字节数。
* `whence`：起始位置，有以下几种：

  * `SEEK_SET`：从文件开头。
  * `SEEK_CUR`：从当前位置。
  * `SEEK_END`：从文件末尾。

#### 使用场景

`lseek` 用于调整文件的读写指针，允许我们在文件中随机访问特定位置。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("example.txt", O_RDWR);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    // 将文件指针移动到文件开头
    off_t new_offset = lseek(fd, 0, SEEK_SET);
    if (new_offset == -1) {
        perror("lseek failed");
        close(fd);
        return 1;
    }

    printf("File pointer moved to offset: %ld\n", new_offset);
    close(fd);
    return 0;
}
```

---

### 6. 函数 `close`

#### 语法

```c
#include <unistd.h>

int close(int fd);
```

#### 参数说明

* `fd`：文件描述符。

#### 使用场景

`close` 用于关闭已经打开的文件。当文件操作完成后，调用 `close` 释放资源。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    // 文件操作
    printf("File opened successfully\n");

    // 关闭文件
    if (close(fd) == -1) {
        perror("close failed");
        return 1;
    }
    printf("File closed successfully\n");

    return 0;
}
```

---

## 二、文件锁

### 1. 文件锁的基本概念

文件锁是用于保证在多进程或多线程环境中，多个进程不会同时对同一文件进行修改。常见的文件锁方式有共享锁（读锁）和独占锁（写锁）。通过锁机制，可以避免文件数据的竞争条件，确保文件的正确性和一致性。

---

### 2. 函数 `fcntl`

#### 语法

```c
#include <fcntl.h>

int fcntl(int fd, int cmd, ...);
```

#### 参数说明

* `fd`：文件描述符。
* `cmd`：控制命令，如 `F_SETLK`, `F_SETLKW`。

#### 使用场景

`fcntl` 可以用于设置或获取文件的各种属性，尤其是用于文件锁的设置。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("example.txt", O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    // 设置文件锁
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;  // 锁定整个文件

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl failed");
        close(fd);
        return 1;
    }

    printf("File locked for writing\n");

    close(fd);
    return 0;
}
```

---

### 3. 函数 `flock`

#### 语法

```c
#include <sys/file.h>

int flock(int fd, int operation);
```

#### 参数说明

* `fd`：文件描述符。
* `operation`：锁操作，常见的有 `LOCK_SH`, `LOCK_EX`, `LOCK_UN`。

#### 使用场景

`flock` 提供简单的文件锁接口，适用于较为简单的锁定需求。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    int fd = open("example.txt", O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }

    // 设置文件锁
    if (flock(fd, LOCK_EX) == -1) {
        perror("flock failed");
        close(fd);
        return 1;
    }

    // 文件操作
    write(fd, "Hello, World!", 13);

    // 释放锁
    flock(fd, LOCK_UN);
    close(fd);
    return 0;
}
```

---

## 三、异步 I/O

### 1. 异步 I/O 基本概念

异步 I/O 是指进程可以在发起 I/O 操作后继续执行其他任务，而不会被 I/O 操作阻塞。I/O 操作会在后台执行，完成后通过回调通知进程。这对于高性能、实时系统尤为重要，能够提高应用程序的响应能力。

---

### 2. 函数 `aio_read`

#### 语法

```c
#include <aio.h>

int aio_read(struct aiocb *aiocb);
```

#### 参数说明

* `aiocb`：包含 I/O 操作的相关信息，包括文件描述符、缓冲区、字节数等。

#### 使用场景

`aio_read` 用于异步读取文件或设备的数据。

#### 示例代码

```c
#include <aio.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    struct aiocb cb;
    char buffer[128];
    cb.aio_fildes = open("example.txt", O_RDONLY);
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer);
    cb.aio_offset = 0;

    aio_read(&cb);

    while (aio_error(&cb) == EINPROGRESS);

    printf("Read content: %s\n", (char *)cb.aio_buf);
    close(cb.aio_fildes);
    return 0;
}
```

---

### 3. 函数 `aio_write`

#### 语法

```c
#include <aio.h>

int aio_write(struct aiocb *aiocb);
```

#### 参数说明

* `aiocb`：包含 I/O 操作的相关信息。

#### 使用场景

`aio_write` 用于异步写入数据到文件。

#### 示例代码

```c
#include <aio.h>
#include <unistd.h>
#include <stdio.h>

int main() {
    struct aiocb cb;
    char buffer[] = "Hello, async I/O!";
    cb.aio_fildes = open("example.txt", O_WRONLY | O_CREAT, 0644);
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer) - 1;
    cb.aio_offset = 0;

    aio_write(&cb);

    while (aio_error(&cb) == EINPROGRESS);

    printf("Write completed.\n");
    close(cb.aio_fildes);
    return 0;
}
```

---

### 4. 函数 `io_submit` / `io_getevents`

#### 语法

```c
#include <libaio.h>

int io_submit(io_context_t ctx, long nr, struct iocb **iocbpp);
int io_getevents(io_context_t ctx, long min, long nr, struct io_event *events, struct timespec *timeout);
```

#### 参数说明

* `ctx`：I/O 上下文。
* `nr`：提交的 I/O 操作数量。

#### 使用场景

`io_submit` 用于提交 I/O 操作，而 `io_getevents` 用于获取已完成的 I/O 事件。

---

## 四、零拷贝 I/O

### 1. 零拷贝 I/O 基本概念

零拷贝 I/O 允许数据在文件和网络之间直接传输，而不需要经过用户空间，从而减少内存拷贝操作，提升性能。

---

### 2. 函数 `sendfile`

#### 语法

```c
#include <sys/sendfile.h>

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

#### 参数说明

* `out_fd`：输出文件描述符。
* `in_fd`：输入文件描述符。
* `offset`：文件的偏移量。
* `count`：要传输的字节数。

#### 使用场景

`sendfile` 用于将数据从一个文件描述符传输到另一个文件描述符，常用于高效的文件传输。

#### 示例代码

```c
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int in_fd = open("input.txt", O_RDONLY);
    int out_fd = open("output.txt", O_WRONLY | O_CREAT, 0644);
    off_t offset = 0;
    sendfile(out_fd, in_fd, &offset, 1024);
    close(in_fd);
    close(out_fd);
    return 0;
}
```

---

### 3. 函数 `splice`

#### 语法

```c
#include <fcntl.h>
#include <unistd.h>

int splice(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t len, unsigned int flags);
```

#### 参数说明

* `fd_in`：输入文件描述符。
* `fd_out`：输出文件描述符。
* `len`：要传输的字节数。

#### 使用场景

`splice` 用于高效地将文件内容从一个文件描述符传输到另一个文件描述符。

#### 示例代码

```c
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd_in = open("input.txt", O_RDONLY);
    int fd_out = open("output.txt", O_WRONLY | O_CREAT, 0644);
    splice(fd_in, NULL, fd_out, NULL, 1024, 0);
    close(fd_in);
    close(fd_out);
    return 0;
}
```

---

## 五、进程的概述

### 进程的定义

**1.程序和进程的区别：** 

​	**程序：是静态的，存放在磁盘上的可执行文件** 

​	**进程：是动态的，是运行在内存中的程序的执行实例**

​	程序是一些指令的有序集合，而进程是程序执行的过程，进程是程序的一次执行过程。 进程的状态是变化的，其包括进程的创建、调度和消亡。 当程序运行时，其就是进程，程序每运行一次，就会创建一个进程。在linux系统中，进程是管理事务的基本单元。 进程拥有自己独立的处理环境和系统资源（处理器、存储器、I/O设备、数据、程序）。

**2.进程的状态及转换**

进程整个生命周期可以简单划分为**三种状态**： 

​	**就绪态：** 进程已经具备执行的一切条件，正在等待分配CPU的处理时间。 

​	**执行态：** 该进程正在占用CPU运行。 

​	**等待态：** 进程因不具备某些执行条件而暂时无法继续执行的状态。

**进程三个状态的转换关系**

![](../resource/1.png)

引起进程状态转换的具体原因如下：

​	**运行态→等待态**：等待使用资源；如等待外设传输；等待人工干预。

​	**等待态→就绪态**：资源得到满足；如外设传输结束；人工干预完成。

​	**运行态→就绪态**：运行时间片到；出现有更高优先权进程。

​	**就绪态—→运行态**：CPU 空闲时选择一个就绪进程

### **进程的调度进制：** 

​	多进程不是说一个进程执行完再执行另一个进程，而是交替执行的，一个进程执行一段时间，然后下一个进程在执行一段时间，依次类推，所有进程执行完之后再回到第一个进程继续执行由此类推。在操作系统中，时间片轮转调度（Round Robin Scheduling）、优先级调度（Priority Scheduling）和完全公平调度（Completely Fair Scheduling，CFS）是三种常见的进程调度算法。

#### **时间片轮转调度（Round Robin Scheduling）**

**概述**：  
	时间片轮转调度是一种简单且常用的调度策略，特别适用于多任务环境中的时间共享系统。该策略的核心思想是为每个进程分配一个固定的时间片（Time Slice），当一个进程用完它的时间片后，操作系统会将其挂起，将CPU资源分配给下一个进程。所有的进程按顺序轮流执行，保证每个进程都能获得一定的CPU时间。

**工作原理**：

- 每个进程按照固定的时间片执行，当时间片耗尽时，调度器会将进程切换出去，并将其状态保存，再选择下一个进程执行。
- 如果进程在其时间片内完成了任务，则该进程会被移除调度队列，继续执行下一个任务。
- 如果进程没有在时间片内完成，调度器会将其放回队列末尾，等待下次调度。

**优点**：

- 简单易实现，适用于多任务操作系统。
- 每个进程都可以得到公平的CPU时间。
- 特别适用于交互式任务，避免某些进程独占CPU时间。

**缺点**：

- 如果时间片设置得太小，频繁的上下文切换会增加开销。
- 不考虑进程的优先级，可能导致重要的进程在轮转时被延迟。
- 时间片的选择要合适，过大或过小都会影响系统性能。

**应用场景**：  
适用于一般的桌面操作系统和基于时间共享的系统，如Linux和Windows中的用户进程调度。

#### **优先级调度（Priority Scheduling）**

**概述**：  
优先级调度是一种基于优先级的调度算法。每个进程都被分配一个优先级值，调度器总是选择优先级最高的进程来执行。高优先级进程会优先于低优先级进程执行，直到它完成或者被阻塞为止。优先级可以是静态的（在进程创建时确定），也可以是动态的（在运行过程中根据进程的行为或外部因素调整）。

**工作原理**：

- 每个进程有一个优先级，优先级可以由系统静态设定，或由用户和操作系统动态调整。
- 调度器选择队列中优先级最高的进程执行，如果多个进程优先级相同，则采用其他算法（如时间片轮转）来调度。
- 优先级调度可以是抢占式的，即当一个高优先级进程到达时，会抢占正在运行的低优先级进程；也可以是非抢占式的，即进程执行完一个时间片后才会进行调度。

**优点**：

- 可以确保高优先级的进程能及时得到执行，适合有不同重要性的任务。
- 对于实时任务或需要快速响应的进程非常有用。

**缺点**：

- **饥饿问题（Starvation）**：低优先级的进程可能长时间得不到执行，特别是在高优先级进程一直占用CPU时。
- 不适用于所有任务，因为它忽视了进程的执行时间和公平性。
- 动态优先级调整可能增加系统的复杂性。

**应用场景**：

- 适用于实时操作系统或任务需要严格响应时间的系统。
- 比如嵌入式系统中的任务调度和硬实时系统。

#### **完全公平调度（CFS，Completely Fair Scheduling）**

**概述**：  
完全公平调度（CFS）是Linux操作系统默认的调度算法，旨在为每个进程提供一个“公平”的CPU时间。与时间片轮转调度不同，CFS不使用固定的时间片，而是使用进程的“虚拟运行时间”（`virtual runtime`，简称`vruntime`）来决定哪个进程应该运行。CFS的目标是让每个进程尽可能公平地获得CPU时间，而不是通过固定的时间片来分配资源。

**工作原理**：

- 每个进程都有一个`vruntime`值，表示该进程消耗的CPU时间。当一个进程运行时，它的`vruntime`会增加。
- CFS通过一个红黑树（Red-Black Tree）来管理可执行的进程，树的根节点是`vruntime`最小的进程，这意味着它会被优先执行。
- 进程的`vruntime`会根据其实际运行时间动态调整，长时间运行的进程会让出CPU，短时间运行的进程会被优先调度。
- CFS没有固定的时间片，而是基于进程的`vruntime`来决定调度顺序，进程越公平，`vruntime`的增加就越慢，调度时得到的CPU时间就越多。

**优点**：

- **公平性**：CFS最大程度地保证了进程之间的公平性，每个进程根据其CPU消耗时间来动态调整调度顺序。
- **灵活性**：相比时间片轮转调度，CFS具有更高的灵活性，可以动态调整每个进程的CPU时间分配，避免了不必要的频繁上下文切换。
- **低延迟**：对交互式进程提供较低的调度延迟，因为它总是优先调度`vruntime`较小的进程。

**缺点**：

- 复杂性较高，调度机制依赖于红黑树的实现，可能会增加系统的开销。
- 对于一些实时应用来说，CFS可能不能提供足够的响应保证，因为它更注重公平性而非严格的实时性。

**应用场景**：

- 适用于需要平衡性能和公平性的多任务操作系统，如Linux。CFS是Linux的默认调度算法，适合各种通用计算任务。
- 不适用于实时性要求较高的任务。

##### CFS实例

​	为了更好地说明 **完全公平调度（CFS）** 的工作原理，我们可以通过一个更完善的例子来展示进程如何基于 `vruntime` 值进行调度，并解释不同优先级的进程如何影响 `vruntime` 的增长速率。

假设场景：

- 系统中有三个进程：`进程 A`、`进程 B` 和 `进程 C`。
- 进程 A 是高优先级进程，`权重 = 1024`。
- 进程 B 是中等优先级进程，`权重 = 512`。
- 进程 C 是低优先级进程，`权重 = 256`。

所有进程的初始 `vruntime` 为 0。

在此例中，我们假设每个进程的 **执行时间** 都为 5 毫秒。

步骤 1：进程 A 执行

- 初始时，`进程 A` 的 `vruntime = 0`，`进程 B` 和 `进程 C` 的 `vruntime = 0`。

- CFS 会选择 `进程 A` 执行，因为它的 `vruntime` 最小。

- 进程 A 执行了 5 毫秒。由于 `进程 A` 的权重为 1024，它的 `vruntime` 增加的速率较慢。假设 `vruntime` 增加的计算公式是：

  ```
  vruntime 增加量 = 执行时间 × 权重因子 / 权重
  ```

  假设权重因子为 1，进程 A 的 `vruntime` 增加量是：

  ```
  vruntime_A = 5 × 1 / 1024 ≈ 0.00488
  ```

  进程 A 执行完后，`vruntime_A` 的新值为：

  ```
  vruntime_A = 0 + 0.00488 = 0.00488
  ```

  所以，`进程 A` 执行完后的 `vruntime = 0.00488`。

步骤 2：进程 C 执行

- 当前时刻，`进程 A` 的 `vruntime = 0.00488`，`进程 B` 的 `vruntime = 0`，`进程 C` 的 `vruntime = 0`。

- CFS 会选择 `进程 C` 执行，因为它的 `vruntime` 最小（与进程 B 相同，选择顺序可能不同，但我们假设先执行 C）。

- 进程 C 执行了 5 毫秒。由于 `进程 C` 的权重为 256，它的 `vruntime` 增加量会比进程 A 大。计算过程如下：

  ```
  vruntime_C = 5 × 1 / 256 ≈ 0.01953
  ```

  进程 C 执行完后的 `vruntime` 为：

  ```
  vruntime_C = 0 + 0.01953 = 0.01953
  ```

  所以，`进程 C` 执行完后的 `vruntime = 0.01953`。

步骤 3：进程 B 执行

- 当前时刻，`进程 A` 的 `vruntime = 0.00488`，`进程 B` 的 `vruntime = 0`，`进程 C` 的 `vruntime = 0.01953`。

- CFS 会选择 `进程 B` 执行，因为它的 `vruntime` 最小。

- 进程 B 执行了 5 毫秒。由于 `进程 B` 的权重为 512，它的 `vruntime` 增加量比进程 A 小，但比进程 C 大。计算过程如下：

  ```
  vruntime_B = 5 × 1 / 512 ≈ 0.00977
  ```

  进程 B 执行完后的 `vruntime` 为：

  ```
  vruntime_B = 0 + 0.00977 = 0.00977
  ```

  所以，`进程 B` 执行完后的 `vruntime = 0.00977`。

步骤 4：再次调度

此时，进程 A、B 和 C 的 `vruntime` 分别为：

- `进程 A`：`vruntime = 0.00488`
- `进程 B`：`vruntime = 0.00977`
- `进程 C`：`vruntime = 0.01953`

接下来，CFS 会选择 **`vruntime` 最小的进程** 来执行，这个进程是 `进程 A`，它的 `vruntime` 最小。然后调度会继续按照这种方式进行，直到所有进程完成执行。

**总结**：

1. **权重（优先级）决定了 `vruntime` 的增长速率**：
   - 高优先级的进程（权重大）其 `vruntime` 增长较慢，因此可以更长时间占用 CPU。
   - 低优先级的进程（权小）其 `vruntime` 增长较快，因此调度时优先级较低。

2. **进程调度顺序是基于 `vruntime` 的**，`vruntime` 越小的进程越容易被调度执行，CFS 会按照进程的 `vruntime` 值维护一个红黑树（基于红黑树来管理所有可执行的进程）。

3. **公平性**：CFS 会确保所有进程在理论上能够获得相同的 CPU 时间，通过动态调整 `vruntime`，使得长期运行的进程逐渐让出 CPU 给其他进程。

这种调度方式与传统的基于时间片的调度算法（如时间片轮转）不同，它根据每个进程的实际执行情况来调整调度策略，确保了调度的公平性。

### **进程控制块**

​	**进程控制块**就是用于保存一个进程信息的结构体，又称之为**PCB**。OS是根据PCB来对并发执行的进程进行控制和管理的。系统在创建一个进程的时候会开辟 一段内存空间存放与此进程相关的PCB数据结构。 PCB是操作系统中最重要的记录型数据结构。PCB中记录了用于描述进程进展情况及控制进程运行所需的全部信息。 

​	PCB是进程存在的唯一标志，在Linux中PCB存放在task_struct结构体中。

``` bash
/usr/src
$ sudo apt-get install ctags
sudo ctags -R

查看结构体  vim -t task_struct
```

​	调度数据 (进程的状态,标记优先级,调度策略等)

​	时间数据: 创建该进程的时间,在用户态的运行时间,在内核态的运行时间.

​	文件数据： 文件描述符表,内存数据,进程标识(进程号)

## 六、进程创建与执行

### 1. 进程创建与执行的基本概念

进程创建和执行是操作系统的核心功能。`fork` 创建子进程，`execve` 执行新程序，`waitpid` 等待子进程结束，\`


exit\` 退出当前进程。通过这些函数，程序可以并发执行任务，提高效率。

---

### 2. 函数 `fork`

#### 语法

```c
#include <unistd.h>

pid_t fork(void);
```

#### 参数说明

* 无。

#### 使用场景

`fork` 用于创建一个新的进程，新进程是父进程的副本。

#### 示例代码

```c
#include <unistd.h>
#include <stdio.h>

int main() {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        printf("Child process\n");
    } else {
        printf("Parent process\n");
    }
    return 0;
}
```

---

### 3. 函数 `vfork`

#### 语法

```c
#include <unistd.h>

pid_t vfork(void);
```

#### 参数说明

* 无。

#### 使用场景

`vfork` 用于创建一个新的进程，但是在子进程执行之前，父进程会被暂时挂起，适用于创建新进程后立即执行新程序的场景。

---

### 4. 函数 `execve`

#### 语法

```c
#include <unistd.h>

int execve(const char *pathname, char *const argv[], char *const envp[]);
```

#### 参数说明

* `pathname`：要执行的程序路径。
* `argv`：传递给新程序的参数。
* `envp`：传递给新程序的环境变量。

#### 使用场景

`execve` 用于加载并执行一个新的程序，替换当前进程映像。

---

### 5. 函数 `waitpid`

#### 语法

```c
#include <sys/wait.h>

pid_t waitpid(pid_t pid, int *status, int options);
```

#### 参数说明

* `pid`：要等待的进程 ID。
* `status`：存储子进程的状态。
* `options`：等待选项，通常为 `0`。

---

### 6. 函数 `exit`

#### 语法

```c
#include <stdlib.h>

void exit(int status);
```

#### 参数说明

* `status`：进程的退出状态。

---

## 七、进程属性

### 1. 进程属性的基本概念

进程属性控制进程的运行优先级、用户身份等。常见的属性控制函数有 `setuid`/`setgid`、`nice`/`setpriority` 等。

---

### 2. 函数 `setuid` / `setgid`

#### 语法

```c
#include <unistd.h>

int setuid(uid_t uid);
int setgid(gid_t gid);
```

#### 参数说明

* `uid`：新设置的用户 ID。
* `gid`：新设置的组 ID。

---

### 3. 函数 `nice` / `setpriority`

#### 语法

```c
#include <unistd.h>
#include <sys/resource.h>

int nice(int inc);
int setpriority(int which, int who, int priority);
```

#### 参数说明

* `inc` / `priority`：进程优先级的增减或设置值。

---

## 八、综合实例: 构建多进程的 Web 日志切割守护进程

### 🎯 背景场景：网站访问日志分片守护服务

在高并发网站中，Web 服务器（如 Nginx）会不断写入访问日志 `access.log`。为了便于分析和防止单个日志文件过大，我们需要：

* 每隔一段时间（如1分钟）将当前日志复制为 `access_时间戳.log`；
* 使用 **子进程并发复制**，避免阻塞主逻辑；
* 使用 **`flock` 加锁** 防止与 Web 写入冲突；
* 使用 **零拷贝 I/O** 提高效率；
* 该守护程序使用 **异步 I/O** 提前预加载部分数据；
* 设置 **低优先级运行**，避免影响主服务器性能；
* 最终作为后台守护进程运行。

---

## 📘 涉及知识点对照表

| 模块      | 使用函数/机制                                   |
| ------- | ----------------------------------------- |
| 文件操作    | `open`, `read`, `write`, `lseek`, `close` |
| 文件锁     | `flock`（避免写读冲突）                           |
| 异步 I/O  | POSIX AIO（日志头异步加载）                        |
| 零拷贝 I/O | `sendfile` 拷贝大文件                          |
| 进程控制    | `fork`, `waitpid`, `exit`                 |
| 进程属性控制  | `nice`, `setuid`, `setsid`（守护化）           |

---

## 🧪 示例代码：`log_rotator_daemon.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <errno.h>
#include <aio.h>
#include <signal.h>

// 日志路径
#define LOG_FILE "access.log"
#define ROTATE_INTERVAL 60  // 秒
#define HEADER_PRELOAD 128  // 异步预加载长度

// 守护进程初始化
void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0); // 父进程退出

    setsid(); // 新会话
    chdir("/");
    umask(0);
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

// 构造目标文件名
void build_filename(char *buffer, size_t len) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(buffer, len, "access_%Y%m%d_%H%M%S.log", tm);
}

// 异步预读日志头部
void preload_header(const char *filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) return;

    struct aiocb cb;
    memset(&cb, 0, sizeof(cb));
    char *buf = malloc(HEADER_PRELOAD);
    cb.aio_fildes = fd;
    cb.aio_buf = buf;
    cb.aio_nbytes = HEADER_PRELOAD;
    cb.aio_offset = 0;

    aio_read(&cb);
    aio_suspend((const struct aiocb *const[]){ &cb }, 1, NULL);
    aio_return(&cb); // 清除状态
    free(buf);
    close(fd);
}

// 执行日志切割
void rotate_log() {
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程拷贝
        int in_fd = open(LOG_FILE, O_RDONLY);
        if (in_fd < 0) exit(1);
        flock(in_fd, LOCK_SH); // 共享锁防止写冲突

        char filename[64];
        build_filename(filename, sizeof(filename));
        int out_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (out_fd < 0) {
            close(in_fd);
            exit(1);
        }

        struct stat st;
        fstat(in_fd, &st);
        sendfile(out_fd, in_fd, NULL, st.st_size);

        flock(in_fd, LOCK_UN);
        close(in_fd);
        close(out_fd);
        exit(0);
    }
}

// 主守护逻辑
int main() {
    daemonize();
    nice(10);  // 降低优先级

    while (1) {
        preload_header(LOG_FILE);  // 异步加载
        rotate_log();              // 子进程拷贝
        sleep(ROTATE_INTERVAL);    // 等待下次
    }

    return 0;
}
```

---

## 🧪 编译与运行

```bash
gcc -o log_rotator_daemon log_rotator_daemon.c -lrt -laio
sudo ./log_rotator_daemon
```

运行后将自动每分钟生成如 `access_20250618_103001.log` 的副本。

---

## ✅ 小结

| 知识点               | 应用方式         |
| ----------------- | ------------ |
| `open/read/write` | 文件打开读取       |
| `flock`           | 并发进程日志文件共享锁  |
| `aio_read`        | 异步加载提高响应     |
| `sendfile`        | 零拷贝提升大文件性能   |
| `fork/waitpid`    | 并发切割子进程管理    |
| `nice`            | 优先级调整避免干扰主业务 |
| `setsid`          | 守护进程脱离终端     |

---