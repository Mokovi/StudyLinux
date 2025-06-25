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
   * [函数 `aio_suspend`](#4-函数-aio_suspend)
   * [函数 `io_submit`](#5-函数-io_submit)
   * [函数 `io_getevents`](#6-函数-io_getevents)
4. [零拷贝 I/O](#四零拷贝-io)
   * [零拷贝 I/O 概述](#１-零拷贝-io-概述)
   * [函数 `sendfile`](#２-函数-sendfile)
   * [函数 `mmap()` + `write()`](#３-mmap--write)
   * [函数 `splice`](#４-函数-splice)
   * [函数 `vmsplice`](#５-函数-vmsplice)
5. [进程的概述与进程号](#五进程的概述与进程号)
   * [进程的定义与状态](#1-进程的定义与状态)
   * [进程号与相关函数](#2-进程号与相关函数)
   * [进程的内存布局](#3-进程的内存布局)
6. [进程创建与执行](#六进程创建与执行)
   * [进程创建与执行的基本概念](#1-进程创建与执行的基本概念)
   * [函数 `fork`](#2-函数-fork)
   * [函数 `vfork`](#3-函数-vfork)
   * [函数 `exec` 系列](#4-函数-exec-系列)
   * [函数 `wait` / `waitpid`](#5-函数-wait-waitpid)
   * [函数 `exit` / `_exit`](#6-函数-exit-_exit)
   * [system 函数](#7-system-函数)
   * [进程终止清理 atexit](#8-进程终止清理-atexit)
   * [僵尸进程与孤儿进程](#9-僵尸进程与孤儿进程)
7. [进程属性](#七进程属性)
   * [进程属性的基本概念](#1-进程属性的基本概念)
   * [函数 `setuid` / `setgid`](#2-函数-setuid-setgid)
   * [函数 `nice` / `setpriority`](#3-函数-nice-setpriority)
8. [综合实例](#八综合实例-构建多进程的-web-日志切割守护进程)

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
  * `O_TRUNC`: 如果打开的文件存在，且以写模式打开，则将其内容截断为 0 长度（即清空文件）。
  * `O_APPEND`:  每次对该文件描述符的写操作都会自动追加到文件末尾，即使调用过程中使用了 `lseek()` 更改偏移量，也不会影响写入位置 —— 写操作总是发生在文件末尾
* `mode`：文件权限，如`0644` 
* **返回值**：成功返回文件描述符，失败返回 -1 并设置 `errno`。

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
    close(fd);  // 关闭文件 每个进程的文件描述符数量是有限的
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
* **返回值**：成功返回实际读取的字节数，失败返回 -1 并设置 `errno`。

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
* **返回值**：成功返回实际写入的字节数，失败返回 -1 并设置 `errno`。

#### 使用场景

`write` 函数用于向文件中写入数据。它会将缓冲区中的数据写入文件，并返回实际写入的字节数。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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
* **返回值**：成功返回新的文件偏移量，失败返回 -1 并设置 `errno`。

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
* **返回值**：成功返回 0，失败返回 -1 并设置 `errno`。

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

int fcntl(int fd, int cmd, ...);//...取决于cmd
```

#### 参数说明

* `fd`：文件描述符。

* `cmd`：控制命令，如 `F_SETLK`, `F_SETLKW`。

  * `F_SETLK` — 设置锁（非阻塞）**功能**：尝试给文件加锁，如果无法加锁（例如已有冲突的锁），立即返回错误。**使用场景**：用于检测锁状态或尝试性加锁。

  * `F_SETLKW` — 设置锁（阻塞）**功能**：尝试加锁，如果当前已有冲突锁，则 阻塞等待直到可加锁。**使用场景**：确保一定能加锁的操作（如写日志文件时）

  * ``` c
    struct flock {
        short l_type;    // 锁类型：F_RDLCK（读锁），F_WRLCK（写锁），F_UNLCK（解锁）
        short l_whence;  // 起始位置参考点：SEEK_SET、SEEK_CUR、SEEK_END
        off_t l_start;   // 起始偏移
        off_t l_len;     // 锁定长度（0 表示直到文件末尾）
        pid_t l_pid;     // 拥有该锁的进程（仅用于 F_GETLK）
    };
    ```

* **返回值**：成功返回 0，失败返回 -1 并设置 `errno`。

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

* `operation`：锁操作

  * `LOCK_SH`：共享锁，适用于只读；
  * `LOCK_EX`：独占锁，适用于写入；
  * `LOCK_UN`：解除锁；
  * `LOCK_NB`：配合上面两者使用，表示非阻塞尝试加锁。

  ``` c
  //非阻塞式加锁：如果锁不可用，flock() 会立即返回 -1，并设置 errno = EWOULDBLOCK。
  if (flock(fd, LOCK_EX | LOCK_NB) == -1) {
      perror("flock non-blocking failed");
      return 1;
  }
  ```

  

* **返回值**：成功返回 0，失败返回 -1 并设置 `errno`。

#### 使用场景

`flock` 提供简单的文件锁接口，适用于较为简单的锁定需求。

#### 示例代码

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/file.h>

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

  * ``` c
    struct aiocb {
        int aio_fildes;        // 文件描述符
        off_t aio_offset;      // 读操作的起始偏移（相当于 lseek 的位置）
        volatile void *aio_buf;// 用户提供的读缓冲区（内存必须有效）
        size_t aio_nbytes;     // 要读取的字节数
        struct sigevent aio_sigevent; // 异步操作完成后的通知方式（信号/线程）
        int aio_reqprio;       // 请求优先级（通常为 0）
        // 还有一些内部成员，由系统使用
    };
    ```

* **返回值**：成功返回 0，失败返回 -1 并设置 `errno`。

#### 使用场景

`aio_read` 用于异步读取文件或设备的数据。

#### 示例代码

```c
#include <stdio.h>
#include <fcntl.h>
#include <aio.h>
#include <string.h>
#include <unistd.h>

int main() {
    int fd = open("test.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct aiocb cb;
    char buffer[100];

    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer) - 1;
    cb.aio_offset = 0;

    if (aio_read(&cb) == -1) {
        perror("aio_read");
        return 1;
    }

    // 等待异步读取完成
    while (aio_error(&cb) == EINPROGRESS) {
        usleep(200);
    }

    int ret = aio_return(&cb);
    if (ret > 0) {
        buffer[ret] = '\0';
        printf("Read: %s\n", buffer);
    } else {
        perror("aio_read failed");
    }

    close(fd);
    return 0;
}
```

---

### 3. 函数 `aio_write`

#### 语法

```c
#include <aio.h>

int aio_write(struct aiocb *aiocbp);
```

#### 参数说明

* 同 `aio_read`，传入已填充的 `struct aiocb`


#### 返回值

* 成功：返回 `0`，表示异步写请求提交成功。
* 失败：返回 `-1`，并设置 `errno`。

#### 使用场景

* 在不阻塞当前线程的情况下，将数据异步写入文件或设备。
* 适合需要并发写入日志或大文件传输的场景。

#### 示例代码

```c
#include <stdio.h> 
#include <fcntl.h>
#include <aio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *msg = "Async write example\n";
    struct aiocb cb;

    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes  = fd;
    cb.aio_buf     = (void *)msg;
    cb.aio_nbytes  = strlen(msg);
    cb.aio_offset  = 0;

    if (aio_write(&cb) == -1) {
        perror("aio_write");
        close(fd);
        return 1;
    }

    // 等待完成
    while (aio_error(&cb) == EINPROGRESS) {
        usleep(1000);
    }

    int written = aio_return(&cb);
    if (written >= 0) {
        printf("Wrote %d bytes asynchronously\n", written);
    } else {
        perror("aio_write failed");
    }

    close(fd);
    return 0;
}
```

---

### 4. 函数 `aio_suspend`

#### 语法

```c
#include <aio.h>

int aio_suspend(const struct aiocb *const list[], int nent, const struct timespec *timeout);
```

#### 参数说明

* `list[]`：`struct aiocb *` 指针数组，包含要等待的异步 I/O 请求。
* `nent`：`list` 数组中元素的数量。
* `timeout`：可选 `struct timespec` 指定最大等待时间；传 `NULL` 表示无限等待。

#### 返回值

* 成功：返回 `0`，表示至少一个请求已完成。
* 失败：返回 `-1`，并设置 `errno`（如超时会设置为 `EAGAIN`）。

#### 使用场景

* 等待多个异步读写请求完成，或在超时后继续执行其他逻辑。
* 可用于实现批量 I/O 完成通知与结果收集。

#### 示例代码

```c
#include <stdio.h>
#include <fcntl.h>
#include <aio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

int main(void) {
    int fd = open("multi.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    const char *msgs[] = { "First\n", "Second\n" };
    struct aiocb cbs[2];
    const struct aiocb *list[2];
    struct timespec timeout = { .tv_sec = 5, .tv_nsec = 0 };

    for (int i = 0; i < 2; i++) {
        memset(&cbs[i], 0, sizeof(cbs[i]));
        cbs[i].aio_fildes  = fd;
        cbs[i].aio_buf     = (void *)msgs[i];
        cbs[i].aio_nbytes  = strlen(msgs[i]);
        cbs[i].aio_offset  = i * 8;  // 不同偏移
        list[i] = &cbs[i];

        if (aio_write(&cbs[i]) == -1) {
            perror("aio_write");
            close(fd);
            return 1;
        }
    }

    // 等待任意一个完成或超时
    if (aio_suspend(list, 2, &timeout) == -1) {
        if (errno == EAGAIN)
            printf("Timeout waiting for I/O\n");
        else
            perror("aio_suspend");
    }

    // 检查并返回各自结果
    for (int i = 0; i < 2; i++) {
        int ret = aio_return(&cbs[i]);
        if (ret >= 0)
            printf("Request %d wrote %d bytes\n", i, ret);
        else
            printf("Request %d error: %s\n", i, strerror(errno));
    }

    close(fd);
    return 0;
}
```

---

### 5. 函数 `io_submit`

#### 语法

```c
#include <libaio.h>

int io_submit(io_context_t ctx_id, long nr, struct iocb *iocbpp[]);
```

#### 参数说明

* `ctx_id`：由 `io_setup()` 返回的 I/O 上下文句柄。
* `nr`：要提交的请求数，即 `iocbpp` 数组的长度。
* `iocbpp`：指向 `struct iocb*` 的数组，每项指向一个已初始化的控制块，描述一次读/写操作。

```c
struct iocb {
    int64_t   aio_data;        // 用户自定义数据，会在 io_event 中返回
    uint16_t  aio_lio_opcode;  // 操作类型：IO_CMD_PREAD、IO_CMD_PWRITE
    int       aio_fildes;      // 文件描述符
    uint64_t  aio_buf;         // 用户缓冲区地址（注意对齐）
    uint64_t  aio_nbytes;      // 要读写的字节数
    int64_t   aio_offset;      // 文件偏移
    /* 其他内部字段… */
};
```

#### 返回值

* ≥ 0：成功提交的请求数（应等于 `nr`）。
* -1：失败，并设置 `errno`（如 `EINVAL`、`EAGAIN`）。

---

### 6. 函数 `io_getevents`

#### 语法

```c
#include <libaio.h>

int io_getevents(io_context_t ctx_id,
                 long min_nr,
                 long max_nr,
                 struct io_event *events,
                 struct timespec *timeout);
```

#### 参数说明

* `ctx_id`：与 `io_submit()` 使用同一上下文句柄。
* `min_nr`：最少要获取的完成事件数；若未达，则阻塞（受 `timeout` 限制）。
* `max_nr`：最多获取的事件数（不能超过 `events` 数组大小）。
* `events`：用户分配的 `struct io_event` 数组，用于接收结果：

  ```c
  struct io_event {
      uint64_t data;   // 对应 iocb.aio_data
      uint64_t obj;    // 原始 iocb 指针
      int64_t  res;    // 成功：字节数；失败：负 errno
      int64_t  res2;   // 备用，一般为 0
  };
  ```
* `timeout`：等待超时时间；`NULL` 表示无限等待。

#### 返回值

* ≥ 0：实际返回的事件数（介于 `min_nr` 和 `max_nr` 之间）。
* -1：失败，并设置 `errno`（如 `EINTR`、`EINVAL`）。

---

### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <libaio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define QUEUE_DEPTH 2
#define BUFFER_SIZE 512

int main(void) {
    io_context_t ctx = 0;
    if (io_setup(QUEUE_DEPTH, &ctx) < 0) {
        perror("io_setup");
        return 1;
    }

    int fd = open("data.bin", O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("open");
        io_destroy(ctx);
        return 1;
    }

    // 分配并初始化两个 iocb
    struct iocb cb[QUEUE_DEPTH];
    struct iocb *iocbs[QUEUE_DEPTH];
    void *buf[QUEUE_DEPTH];

    for (int i = 0; i < QUEUE_DEPTH; i++) {
        // 512 字节对齐
        buf[i] = aligned_alloc(BUFFER_SIZE, BUFFER_SIZE);
        if (!buf[i]) {
            perror("aligned_alloc");
            return 1;
        }
        memset(&cb[i], 0, sizeof(cb[i]));
        cb[i].aio_data       = i;                    // 用户标识
        cb[i].aio_lio_opcode = IO_CMD_PREAD;         // 读操作
        cb[i].aio_fildes     = fd;
        cb[i].aio_buf        = (uint64_t)buf[i];
        cb[i].aio_nbytes     = BUFFER_SIZE;
        cb[i].aio_offset     = i * BUFFER_SIZE;      // 不同偏移
        iocbs[i] = &cb[i];
    }

    // 提交两次异步读取
    int submitted = io_submit(ctx, QUEUE_DEPTH, iocbs);
    if (submitted < QUEUE_DEPTH) {
        perror("io_submit");
        goto cleanup;
    }

    // 等待至少 1 个完成，最多获取 2 个
    struct io_event events[QUEUE_DEPTH];
    struct timespec timeout = { .tv_sec = 5, .tv_nsec = 0 };
    int got = io_getevents(ctx, 1, QUEUE_DEPTH, events, &timeout);
    if (got < 0) {
        perror("io_getevents");
        goto cleanup;
    }

    // 处理完成的事件
    for (int i = 0; i < got; i++) {
        struct io_event *ev = &events[i];
        if (ev->res >= 0) {
            printf("Request %llu completed: read %lld bytes at offset %lld\n",
                   (unsigned long long)ev->data,
                   (long long)ev->res,
                   cb[ev->data].aio_offset);
            // 可以在此处使用 buf[ev->data]
        } else {
            printf("Request %llu failed: %s\n",
                   (unsigned long long)ev->data,
                   strerror((int)-ev->res));
        }
    }
    // 释放资源
    for (int i = 0; i < QUEUE_DEPTH; i++) {
        free(buf[i]);
    }
    close(fd);
    io_destroy(ctx);
    return 0;
}
```

### 7. 综合实例
``` c
/**
 * high_perf_log_processor.c
 *
 * 背景：高性能日志处理器，
 *  - 利用 Linux 原生 AIO（libaio）并发读取大日志文件的多个区块
 *  - 对每个区块计算“有效字节数”示例
 *  - 使用 POSIX AIO 将每个区块的处理结果异步写入汇总文件
 *  - 在多次写请求之后，调用 aio_suspend 等待所有写完成
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <libaio.h>
#include <aio.h>

#define QUEUE_DEPTH   4        // 同时提交的原生 AIO 读取请求数
#define CHUNK_SIZE    4096     // 每次读取 4 KB
#define MAX_EVENTS    QUEUE_DEPTH

int main(void) {
    const char *infile  = "app.log";
    const char *outfile = "summary.txt";
    int   infd, outfd, ret;

    // 1. 打开输入/输出文件
    infd = open(infile, O_RDONLY | O_DIRECT);
    if (infd < 0) { perror("open infile"); return 1; }
    outfd = open(outfile, O_CREAT|O_WRONLY|O_TRUNC, 0644);
    if (outfd < 0) { perror("open outfile"); close(infd); return 1; }

    // 2. 设置 libaio 上下文
    io_context_t ctx = 0;
    if (io_setup(QUEUE_DEPTH, &ctx) < 0) {
        perror("io_setup"); goto cleanup;
    }

    // 3. 准备 iocb 和缓冲区
    struct iocb    iocbs[QUEUE_DEPTH];
    struct iocb   *piocbs[QUEUE_DEPTH];
    void          *buffers[QUEUE_DEPTH];
    for (int i = 0; i < QUEUE_DEPTH; i++) {
        // 4KB 对齐
        buffers[i] = aligned_alloc(CHUNK_SIZE, CHUNK_SIZE);
        if (!buffers[i]) { perror("aligned_alloc"); goto cleanup; }
        memset(&iocbs[i], 0, sizeof(iocbs[i]));

        iocbs[i].aio_data       = i;                // 用户标识
        iocbs[i].aio_lio_opcode = IO_CMD_PREAD;     // 读取命令
        iocbs[i].aio_fildes     = infd;
        iocbs[i].aio_buf        = (uint64_t)buffers[i];
        iocbs[i].aio_nbytes     = CHUNK_SIZE;
        iocbs[i].aio_offset     = i * CHUNK_SIZE;   // 每块不同偏移
        piocbs[i] = &iocbs[i];
    }

    // 4. 提交原生 AIO 读取
    ret = io_submit(ctx, QUEUE_DEPTH, piocbs);
    if (ret < QUEUE_DEPTH) {
        perror("io_submit"); goto cleanup;
    }

    // 5. 等待至少一个完成，最多获取 QUEUE_DEPTH 个事件
    struct io_event events[MAX_EVENTS];
    struct timespec timeout = { .tv_sec = 5, .tv_nsec = 0 };
    ret = io_getevents(ctx, 1, MAX_EVENTS, events, &timeout);
    if (ret < 0) {
        perror("io_getevents"); goto cleanup;
    }

    // 6. 针对每个完成的读取结果，启动 POSIX aio_write 写入 summary.txt
    struct aiocb write_cbs[MAX_EVENTS];
    const struct aiocb *write_list[MAX_EVENTS];
    int write_count = 0;

    for (int i = 0; i < ret; i++) {
        struct io_event *ev = &events[i];
        if (ev->res <= 0) {
            fprintf(stderr, "read chunk %llu error: %s\n",
                    (unsigned long long)ev->data,
                    strerror((int)-ev->res));
            continue;
        }

        // 简单“处理”：统计有效字节数
        int bytes = (int)ev->res;
        char result[64];
        int len = snprintf(result, sizeof(result),
                           "Chunk %llu: %d bytes\n",
                           (unsigned long long)ev->data, bytes);

        // 填写 aiocb
        memset(&write_cbs[write_count], 0, sizeof(struct aiocb));
        write_cbs[write_count].aio_fildes  = outfd;
        write_cbs[write_count].aio_buf     = result;
        write_cbs[write_count].aio_nbytes  = len;
        write_cbs[write_count].aio_offset  = 0;  // 追加写
        write_cbs[write_count].aio_sigevent.sigev_notify = SIGEV_NONE; 
        write_list[write_count] = &write_cbs[write_count];
        write_count++;
    }

    // 7. 提交所有 POSIX 异步写请求
    for (int i = 0; i < write_count; i++) {
        if (aio_write((struct aiocb *)write_list[i]) < 0) {
            perror("aio_write");
        }
    }

    // 8. 用 aio_suspend 等待所有写完成
    if (write_count > 0) {
        // 无限等待，直到所有写请求完成
        aio_suspend(write_list, write_count, NULL);
        // 检查结果
        for (int i = 0; i < write_count; i++) {
            int w = aio_return((struct aiocb *)write_list[i]);
            if (w < 0)
                fprintf(stderr, "write %d failed: %s\n", i, strerror(errno));
        }
    }

    printf("All I/O operations completed.\n");

cleanup:
    // 9. 清理资源
    for (int i = 0; i < QUEUE_DEPTH; i++)
        free(buffers[i]);
    close(infd);
    close(outfd);
    io_destroy(ctx);
    return 0;
}
```

---

## 四、零拷贝 I/O

### １. 零拷贝 I/O 概述

#### 概念

零拷贝（Zero-copy I/O）是一种在用户空间与内核空间之间传输数据时，**避免多次数据拷贝**的技术。它通过利用内核直接在缓冲区之间移动数据，减少 CPU 占用、提高吞吐量。

#### 优势

* **减少内存拷贝次数**：传统读写要在内核缓冲区与用户缓冲区之间各拷贝一次；零拷贝可省去至少一次拷贝。
* **降低 CPU 开销**：拷贝操作昂贵，减少后可释放 CPU 资源给业务逻辑。
* **提高 I/O 吞吐**：适合高并发网络或文件传输场景。

#### 典型机制

1. `sendfile()`：文件到套接字
2. `mmap()` + `write()`：文件映射到用户空间
3. `splice()` / `vmsplice()`：管道与文件/套接字间的数据搬运

---

### ２. 函数 `sendfile`

#### 语法

```c
#include <sys/sendfile.h>

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

#### 参数说明

* `out_fd`：目标文件描述符，通常是套接字。
* `in_fd`：源文件描述符，通常是打开的文件。
* `offset`：指向文件偏移量的指针；若为 `NULL`，函数会从当前文件位置读。
* `count`：要传输的字节数。

#### 返回值

* ≥ 0：实际传输的字节数。
* -1：失败，并设置 `errno`。

#### 使用场景

* HTTP 服务器将静态文件直接发送给客户端。
* 文件拷贝到网络或另一文件时提高性能。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int file_fd = open("file.txt", O_RDONLY);
    if (file_fd < 0) {
        perror("open file");
        return 1;
    }

    // 创建 socket 并连接服务器
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(8888),
        .sin_addr.s_addr = inet_addr("127.0.0.1")
    };

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return 1;
    }

    off_t offset = 0;
    struct stat stat_buf;
    fstat(file_fd, &stat_buf);

    ssize_t sent = sendfile(sockfd, file_fd, &offset, stat_buf.st_size);
    if (sent == -1) {
        perror("sendfile");
    } else {
        printf("Sent %zd bytes.\n", sent);
    }

    close(file_fd);
    close(sockfd);
    return 0;
}
```

---

### ３. `mmap()` + `write()`

#### 语法

```c
#include <sys/mman.h>
#include <unistd.h>

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);
int munmap(void *addr, size_t length);
```

#### 参数说明

* `addr`：建议映射地址，通常设为 `NULL`。
* `length`：映射长度。
* `prot`：访问权限，如 `PROT_READ`。
* `flags`：映射方式，如 `MAP_PRIVATE`。
* `fd`：要映射的文件描述符。
* `offset`：文件内偏移。

#### 使用方式

1. 用 `mmap()` 将文件映射到用户空间。
2. 调用 `write(out_fd, addr, length)` 将映射区内容写出。
3. 用 `munmap()` 解除映射。

#### 使用场景

* 小文件或随机访问文件时减少多次 `read()` 调用开销。
* 文件与文件之间或文件与套接字之间传输。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    int fd = open("file.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct stat st;
    fstat(fd, &st);
    size_t filesize = st.st_size;

    // 将文件映射到内存
    char *data = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // 将内容写到标准输出
    if (write(STDOUT_FILENO, data, filesize) != filesize) {
        perror("write");
    }

    munmap(data, filesize);
    close(fd);
    return 0;
}
```

---

### ４. 函数 `splice`

#### 语法

```c
#include <fcntl.h>
#include <unistd.h>

ssize_t splice(int fd_in,  loff_t *off_in,
               int fd_out, loff_t *off_out,
               size_t len, unsigned int flags);
```

#### 参数说明

* `fd_in` / `fd_out`：输入/输出文件描述符，可是管道或文件。
* `off_in` / `off_out`：指向偏移量指针，可为 `NULL` 表示使用文件当前偏移。
* `len`：要移动的数据字节数。
* `flags`：行为标志，如 `SPLICE_F_MOVE`、`SPLICE_F_MORE`。

#### 返回值

* ≥ 0：实际移动的字节数。
* -1：失败，设置 `errno`。

#### 使用场景

* 管道与文件或套接字之间高效搬运数据。
* 构建用户态零拷贝管道链，如：`file → pipe → socket`。

#### 示例代码

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int file_fd = open("file.txt", O_RDONLY);
    if (file_fd < 0) {
        perror("open");
        return 1;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    while (1) {
        ssize_t n = splice(file_fd, NULL, pipefd[1], NULL, 4096, 0);
        if (n == 0) break;
        if (n == -1) {
            perror("splice-read");
            break;
        }

        if (splice(pipefd[0], NULL, STDOUT_FILENO, NULL, n, 0) == -1) {
            perror("splice-write");
            break;
        }
    }

    close(file_fd);
    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}
```

---

### ５. 函数 `vmsplice`

#### 语法

```c
#include <fcntl.h>
#include <unistd.h>

ssize_t vmsplice(int fd, const struct iovec *iov,
                 unsigned long nr_segs, unsigned int flags);
```

#### 参数说明

* `fd`：目标管道的写端文件描述符。
* `iov`：指向 `iovec` 数组，描述要写入的数据缓冲区集合。
* `nr_segs`：`iovec` 数组长度。
* `flags`：如 `SPLICE_F_MORE`。

```c
struct iovec {
    void  *iov_base;    // 缓冲区地址
    size_t iov_len;     // 缓冲区长度
};
```

#### 返回值

* ≥ 0：实际写入管道的字节数。
* -1：失败，设置 `errno`。

#### 使用场景

* 将用户空间内多个不连续缓冲区批量“零拷贝”注入管道。
* 配合 `splice()` 从管道再传输到文件或套接字。

#### 示例代码

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    char buf1[] = "Hello ";
    char buf2[] = "World!\n";
    struct iovec iov[2] = {
        { .iov_base = buf1, .iov_len = strlen(buf1) },
        { .iov_base = buf2, .iov_len = strlen(buf2) }
    };

    // 将用户空间数据写入管道
    ssize_t n = vmsplice(pipefd[1], iov, 2, 0);
    if (n == -1) {
        perror("vmsplice");
        return 1;
    }

    // 将管道数据输出到 stdout
    if (splice(pipefd[0], NULL, STDOUT_FILENO, NULL, n, 0) == -1) {
        perror("splice");
        return 1;
    }

    close(pipefd[0]);
    close(pipefd[1]);
    return 0;
}
```





## 五、进程的概述与进程号

### 1. 进程的定义与状态

**程序与进程的区别：**

* **程序**：是静态的，存放在磁盘上的可执行文件。
* **进程**：是动态的，是运行在内存中的程序的执行实例。

程序是一些指令的有序集合，而进程是程序执行的过程，进程是程序的一次执行过程。进程的状态是变化的，其包括进程的创建、调度和消亡。当程序运行时，其就是进程，程序每运行一次，就会创建一个进程。在 Linux 系统中，进程是管理事务的基本单元。进程拥有自己独立的处理环境和系统资源（处理器、存储器、I/O设备、数据、程序）。

**进程的状态及转换：**

进程整个生命周期可以简单划分为**三种状态**：

* **就绪态**：进程已经具备执行的一切条件，正在等待分配CPU的处理时间。
* **执行态**：该进程正在占用CPU运行。
* **等待态**：进程因不具备某些执行条件而暂时无法继续执行的状态。

**进程三个状态的转换关系：**

引起进程状态转换的具体原因如下：

* **运行态→等待态**：等待使用资源；如等待外设传输；等待人工干预。
* **等待态→就绪态**：资源得到满足；如外设传输结束；人工干预完成。
* **运行态→就绪态**：运行时间片到；出现有更高优先权进程。
* **就绪态→运行态**：CPU 空闲时选择一个就绪进程。

**特殊的进程号：**

在 Linux 系统中进程号由 0 开始。进程号为 0 及 1 的进程由内核创建：

* 进程号为 0 的进程通常是调度进程，常被称为交换进程(swapper)。
* 进程号为 1 的进程通常是 init 进程，init 进程是所有进程的祖先。除调度进程外，在 Linux 下面所有的进程都由进程 init 进程直接或者间接创建。

**进程号(PID)**：标识进程的一个非负整型数。
**父进程号(PPID)**：任何进程(除 init 进程)都是由另一个进程创建，该进程称为被创建进程的父进程，对应的进程号称为父进程号(PPID)。
**进程组号(PGID)**：进程组是一个或多个进程的集合。他们之间相互关联，进程组可以接收同一终端的各种信号，关联的进程有一个进程组号(PGID)。

---

### 2. 进程号与相关函数

#### 语法

```c
#include <sys/types.h>
#include <unistd.h>

pid_t getpid(void);    // 获取当前进程的进程号
pid_t getppid(void);   // 获取当前进程的父进程号
pid_t getpgid(pid_t pid); // 获取指定进程的进程组号
```

#### 参数说明

* `pid`：指定进程的进程号（仅 `getpgid` 函数）。
* **返回值**：成功返回相应的进程号，失败返回 -1 并设置 `errno`。

#### 使用场景

这些函数用于获取进程相关的标识信息，常用于进程管理和调试。

#### 示例代码

```c
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    printf("当前进程号 (PID) = %d\n", getpid());
    printf("父进程号 (PPID) = %d\n", getppid());
    printf("进程组号 (PGID) = %d\n", getpgid(getpid()));
    
    while(1) {
        sleep(1);
    }
    return 0;
}
```

---

### 3. 进程的内存布局

在程序运行时，内存被分为不同的区域，用来存储代码、数据、堆栈等内容。

1. **栈区（Stack）**：用于存储局部变量、函数参数和返回地址，由系统自动分配和释放。
2. **堆区（Heap）**：用于动态分配内存，由程序员手动分配和释放。
3. **BSS 区**：存放未初始化的全局变量和静态变量，程序加载时自动清零。
4. **数据区**：存放已初始化的全局变量和静态变量。
5. **代码区（Text）**：存储程序的机器指令，只读。

**内存布局示意：**

```
高地址
    ↓
  栈区 (Stack)
  堆区 (Heap)
  BSS 区
  数据区 (Data)
  代码区 (Text)
    ↓
低地址
```

---

## 六、进程创建与执行

### 1. 进程创建与执行的基本概念

每个进程都由一个进程号（`pid_t`）标识。进程的创建和执行是操作系统的核心功能。常用函数有 `fork`、`vfork`、`exec`、`wait`、`waitpid`、`exit`。

---

### 2. 函数 `fork`

#### 语法

```c
#include <sys/types.h>
#include <unistd.h>

pid_t fork(void);
```

#### 参数说明

* **返回值**：
  * 父进程中返回子进程的进程号（>0）
  * 子进程中返回0
  * 失败返回-1

#### 使用场景

`fork` 创建一个新的子进程，子进程是父进程的副本。父子进程拥有独立的地址空间，但初始内容相同（写时拷贝）。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid;
    printf("[%d]: Begin!\n", getpid());
    
    pid = fork();
    if (pid < 0) {
        perror("fork 创建进程错误!");
        exit(1);
    }
    
    if (pid == 0) {
        printf("[%d]: 子进程正在工作...\n", getpid());
    } else {
        sleep(1);
        printf("[%d]: 父进程正在工作...\n", getpid());
    }
    
    printf("[%d] End!\n", getpid());
    exit(0);
}
```

#### 父子进程独立地址空间示例

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int x = 10;  // 全局变量

int main() {
    static int y = 10;  // 静态变量
    int z = 10;         // 局部变量
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("创建进程失败");
        return -1;
    }
    
    if (pid > 0) {
        printf("父进程....\n");
        x++; y++; z++;
        printf("x = %d, y = %d, z = %d\n", x, y, z);
    } else {
        sleep(1);
        printf("子进程...\n");
        printf("x = %d, y = %d, z = %d\n", x, y, z);
    }
    
    while (1);
    return 0;
}
```

---

### 3. 函数 `vfork`

#### 语法

```c
#include <sys/types.h>
#include <unistd.h>

pid_t vfork(void);
```

#### 参数说明

* **返回值**：
  * 父进程中返回子进程的进程号（>0）
  * 子进程中返回0
  * 失败返回-1

#### 使用场景

`vfork` 创建子进程，但父子进程共享地址空间，直到子进程调用 exec 或 exit。vfork 后父进程会被挂起，直到子进程退出。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int x = 100;

int main() {
    static int y = 200;
    int z = 300;
    
    pid_t pid = vfork();
    if (pid < 0) {
        perror("创建进程失败!");
        return -1;
    }
    
    if (pid > 0) {
        printf("父进程正在运行中...\n");
        printf("x = %d, y = %d, z = %d\n", x, y, z);
    } else {
        printf("子进程正在运行中...\n");
        x++; y++; z++;
        printf("x = %d, y = %d, z = %d\n", x, y, z);
        exit(0);
    }
    
    while (1);
    return 0;
}
```

#### vfork 与 fork 的区别

| 特性 | fork() | vfork() |
|------|--------|---------|
| 内存空间 | 子进程拥有父进程的完整拷贝（写时拷贝） | 子进程直接共享父进程的地址空间 |
| 性能 | 开销较大，需要复制内存空间 | 性能更好，不需要复制内存空间 |
| 执行顺序 | 父子进程独立执行，谁先执行取决于调度器 | 父进程被阻塞，直到子进程调用 exec() 或 exit() |
| 应用场景 | 适合需要在父子进程中保持独立内存空间的情况 | 适合创建子进程后马上调用 exec() 执行新程序的场合 |

---

### 4. 函数 `exec` 系列

#### 语法

```c
#include <unistd.h>

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execle(const char *path, const char *arg, ..., char * const envp[]);
int execvpe(const char *file, char *const argv[], char *const envp[]);
```

#### 参数说明

* `path/file`：可执行文件的路径或文件名。
* `arg/argv`：传递给新程序的参数。
* `envp`：环境变量数组（仅 e 变体）。
* **返回值**：成功不返回，失败返回 -1 并设置 `errno`。

#### 使用场景

exec 系列函数用新程序替换当前进程映像，不会返回（除非出错）。execl/execv 需要绝对路径，带 p 的可用相对路径。e 变体可自定义环境变量。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fail to fork");
        exit(1);
    } else if (pid > 0) {
        printf("父进程在运行中\n");
        wait(NULL);
        printf("子进程运行结束\n");
    } else {
        printf("子进程运行中\n");
        if (execl("/bin/ls", "ls", "-l", NULL) == -1) {
            perror("fail to execl");
            exit(1);
        }
    }
    return 0;
}
```

#### exec 函数族变体说明

| 函数 | 参数传递方式 | 路径要求 | 环境变量 |
|------|-------------|----------|----------|
| execl | 列表形式 | 绝对路径 | 继承当前环境 |
| execlp | 列表形式 | 相对路径 | 继承当前环境 |
| execv | 数组形式 | 绝对路径 | 继承当前环境 |
| execvp | 数组形式 | 相对路径 | 继承当前环境 |
| execle | 列表形式 | 绝对路径 | 自定义环境 |
| execvpe | 数组形式 | 相对路径 | 自定义环境 |

---

### 5. 函数 `wait` / `waitpid`

#### 语法

```c
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

#### 参数说明

* `pid`：指定的进程或进程组（仅 waitpid）。
  * `pid > 0`：等待进程 ID 等于 pid 的子进程。
  * `pid = 0`：等待同一个进程组中的任何子进程。
  * `pid = -1`：等待任一子进程，此时 waitpid 和 wait 作用一样。
  * `pid < -1`：等待指定进程组中的任何子进程，这个进程组的 ID 等于 pid 的绝对值。
* `status`：保存子进程退出时的状态信息。
* `options`：选项。
  * `0`：同 wait，阻塞父进程，等待子进程退出。
  * `WNOHANG`：没有任何已经结束的子进程，则立即返回。非阻塞形式。
  * `WUNTRACED`：如果子进程暂停了则此函数马上返回，并且不予以理会子进程的结束状态。
* **返回值**：成功返回状态改变了的子进程的进程号，失败返回 -1。

#### 使用场景

wait 等待任一子进程结束，waitpid 可指定等待的子进程。status 可获取子进程退出状态。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("进程创建失败");
        return -1;
    }
    
    if (pid == 0) {
        for (int i = 0; i < 3; i++) {
            printf("子进程\n");
            sleep(1);
        }
        exit(3);
    } else {
        int status = 0;
        wait(&status);
        if (WIFEXITED(status) != 0) {
            printf("子进程返回状态: %d\n", WEXITSTATUS(status));
        }
        printf("父进程\n");
    }
    return 0;
}
```

#### 状态宏说明

* `WIFEXITED(status)`：如果子进程正常终止，取出的字段值非零。
* `WEXITSTATUS(status)`：返回子进程的退出状态，退出状态保存在 status 变量的 8~16 位。

---

### 6. 函数 `exit` / `_exit`

#### 语法

```c
#include <stdlib.h>
void exit(int status);

#include <unistd.h>
void _exit(int status);
```

#### 参数说明

* `status`：退出状态，由父进程通过 wait 函数接收这个状态。一般失败退出设置为非 0，成功退出设置为 0。

#### 使用场景

exit 和 _exit 函数用于退出当前进程。exit 会刷新缓冲区，_exit 不会。一般推荐用 exit。

#### 示例代码

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void fun() {
    printf("BBB\n");
    return;
    printf("CCC\n");  // 不会执行
}

int main() {
    printf("AAA\n");
    fun();
    printf("DDD\n");
    return 0;
}
```

---

### 7. system 函数

#### 语法

```c
#include <stdlib.h>

int system(const char *command);
```

#### 参数说明

* `command`：要执行的命令的字符串。
* **返回值**：
  * 如果 command 为 NULL，则返回非 0，一般为 1。
  * 如果 system() 在调用 /bin/sh 时失败则返回 127，其它失败原因返回 -1。

#### 使用场景

system 用于执行 shell 命令，会创建子进程并等待其结束。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>

int main() {
    system("clear");
    system("ls -l");
    return 0;
}
```

#### system 与 exec 的区别

| 特性 | system() | exec() |
|------|----------|--------|
| 执行方式 | 调用 shell 执行命令，创建子进程 | 直接用新程序替换当前进程 |
| 进程关系 | 创建子进程执行命令，父进程保持不变 | 不创建新进程，替换当前进程 |
| 返回行为 | 执行完命令后返回，返回命令的退出状态 | 不会返回，除非发生错误 |
| 使用场景 | 适合简单调用外部命令，父进程继续执行 | 适合完全替换当前进程执行另一个程序 |

---

### 8. 进程终止清理 atexit

#### 语法

```c
#include <stdlib.h>

int atexit(void (*function)(void));
```

#### 参数说明

* `function`：进程结束前调用的函数入口地址。
* **返回值**：成功返回 0，失败返回非 0。

#### 使用场景

atexit 注册的函数会在进程正常结束前被调用。一个进程中可以多次调用 atexit 函数注册清理函数。正常结束前调用函数的顺序和注册时的顺序相反。

#### 示例代码

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void fun1() { printf("clear fun1...\n"); }
void fun2() { printf("clear fun2...\n"); }
void fun3() { printf("clear fun3...\n"); }

int main() {
    atexit(fun1);
    atexit(fun2);
    atexit(fun3);
    printf("**** **** **** *****\n");
    sleep(3);
    return 0;
}
```

---

### 9. 僵尸进程与孤儿进程

* **僵尸进程**：进程已经结束，但进程的占用的资源未被回收，这样的进程称为僵尸进程。父进程未调用 `wait` 或 `waitpid`函数回收子进程的资源使子进程变为僵尸进程。
* **孤儿进程**：父进程运行结束，但子进程未运行结束的子进程。
* **守护进程**：守护进程是个特殊的孤儿进程，这种进程脱离终端，在后台运行。

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
* **返回值**：成功返回 0，失败返回 -1 并设置 `errno`。

#### 使用场景

这些函数用于改变进程的用户 ID 和组 ID，常用于权限管理。

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

* `inc`：进程优先级的增减值。
* `priority`：进程优先级的设置值。
* **返回值**：成功返回 0，失败返回 -1 并设置 `errno`。

#### 使用场景

这些函数用于调整进程的优先级，nice 用于相对调整，setpriority 用于绝对设置。

---

## 八、综合实例: 构建多进程的 Web 日志切割守护进程

### 背景场景：网站访问日志分片守护服务

在高并发网站中，Web 服务器（如 Nginx）会不断写入访问日志 `access.log`。为了便于分析和防止单个日志文件过大，我们需要：

* 每隔一段时间（如1分钟）将当前日志复制为 `access_时间戳.log`；
* 使用 **子进程并发复制**，避免阻塞主逻辑；
* 使用 **`flock` 加锁** 防止与 Web 写入冲突；
* 使用 **零拷贝 I/O** 提高效率；
* 该守护程序使用 **异步 I/O** 提前预加载部分数据；
* 设置 **低优先级运行**，避免影响主服务器性能；
* 最终作为后台守护进程运行。

---

## 涉及知识点对照表

| 模块      | 使用函数/机制                                   |
| ------- | ----------------------------------------- |
| 文件操作    | `open`, `read`, `write`, `lseek`, `close` |
| 文件锁     | `flock`（避免写读冲突）                           |
| 异步 I/O  | POSIX AIO（日志头异步加载）                        |
| 零拷贝 I/O | `sendfile` 拷贝大文件                          |
| 进程控制    | `fork`, `waitpid`, `exit`                 |
| 进程属性控制  | `nice`, `setuid`, `setsid`（守护化）           |

---

## 示例代码：`log_rotator_daemon.c`

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

## 编译与运行

```bash
gcc -o log_rotator_daemon log_rotator_daemon.c -lrt -laio
sudo ./log_rotator_daemon
```

运行后将自动每分钟生成如 `access_20250618_103001.log` 的副本。

---

## 小结

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