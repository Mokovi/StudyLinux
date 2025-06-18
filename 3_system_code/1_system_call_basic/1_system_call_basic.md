# 1. Linux 系统调用基础编程笔记

本篇笔记将详细介绍 Linux 系统调用的基础知识，涵盖用户态与内核态切换、常用接口封装、错误处理机制以及调试和跟踪工具。每个部分都会详细讲解概念、函数语法、输入输出参数定义，并给出相应的使用代码和使用场景。

## 目录

1. [用户态与内核态切换](#1-用户态与内核态切换)
2. [常用接口：syscall 封装](#2-常用接口syscall-封装)
3. [错误处理：errno 机制及 perror/strerror](#3-错误处理errno-机制及-perrorstrerror)
4. [调试与跟踪：strace、ltrace](#4-调试与跟踪straceltrace)
5. [综合实例](#5-综合实例)

---

## 1. 用户态与内核态切换

### 1.1 概念

在 Linux 操作系统中，CPU 的执行模式有两种：**用户态**和**内核态**。

* **用户态**：这是普通应用程序运行的模式。操作系统通过系统调用将应用程序的请求转交给内核。应用程序在用户态下执行时不能直接访问硬件和内核资源。
* **内核态**：这是操作系统核心代码执行的模式。在内核态下，操作系统可以直接访问硬件和管理系统资源。只有内核代码可以在内核态下执行。

当用户程序需要访问内核资源（例如文件操作、网络通讯等）时，必须通过**系统调用**将控制权从用户态切换到内核态，执行完系统调用后再切换回用户态。

### 1.2 相关函数

在用户程序中，使用 `syscall()` 来发起系统调用，从而完成用户态与内核态之间的切换。

```c
#include <unistd.h>

long syscall(long number, ...);
```

#### 输入输出参数：

* `number`：系统调用编号，指定要执行的系统调用。
* 后续参数：根据系统调用的不同而变化。例如，对于文件操作，可能需要传入文件路径、权限等。

### 1.3 使用场景

任何需要操作系统提供底层服务的程序（如文件读写、网络操作等）都会通过系统调用进行。

---

## 2. 常用接口：syscall 封装

### 2.1 概念

`syscall()` 函数是系统调用的底层接口。虽然在大多数应用中，通常使用标准 C 库函数（如 `open()`, `read()`, `write()`）来进行系统调用，但这些库函数最终都会调用 `syscall()` 来与内核交互。

### 2.2 语法

```c
#include <unistd.h>

long syscall(long number, ...);
```

#### 输入输出参数：

* `number`：指定要执行的系统调用编号。
* 后续参数：根据系统调用的不同而不同，具体参数会依赖于你要调用的系统功能。

### 2.3 示例代码

```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = syscall(SYS_open, "test.txt", O_RDONLY);
    if (fd == -1) {
        perror("Open failed");
        return 1;
    }
    printf("File opened with fd: %d\n", fd);
    close(fd);
    return 0;
}
```

### 2.4 使用场景

`syscall()` 适用于需要精确控制系统调用或不依赖于 C 标准库的场景。通常，系统调用提供了比标准库函数更直接的控制。

---

## 3. 错误处理：errno 机制及 perror/strerror

### 3.1 概念

在 Linux 中，系统调用和库函数通常通过返回值来指示是否发生错误。若发生错误，返回值通常是 `-1`，并设置 `errno` 变量来指示具体的错误类型。`errno` 是一个全局变量，它存储着上一次系统调用或库函数调用时的错误码。

### 3.2 常见错误处理函数

* **perror**：将 `errno` 所指示的错误信息打印到标准错误输出。
* **strerror**：根据 `errno` 的值返回一个描述错误的字符串。

### 3.3 语法

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

void perror(const char *s);
char *strerror(int errnum);
```

#### 输入输出参数：

* `s`：用户定义的字符串，作为错误消息前缀。
* `errnum`：错误码，通常为 `errno`。

### 3.4 示例代码

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main() {
    int fd = open("non_existent_file.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    close(fd);
    return 0;
}
```

或者使用 `strerror`：

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main() {
    FILE *file = fopen("non_existent_file.txt", "r");
    if (file == NULL) {
        printf("Error: %s\n", strerror(errno));
        return 1;
    }
    fclose(file);
    return 0;
}
```

### 3.5 使用场景

每当系统调用或库函数返回错误时，应该检查 `errno` 并使用 `perror` 或 `strerror` 来获得详细的错误信息。

---

## 4. 调试与跟踪：strace、ltrace

### 4.1 概念

* **strace**：是一个强大的调试工具，用于跟踪系统调用和信号。它可以显示程序运行时与内核的交互细节（例如，文件操作、内存分配等）。
* **ltrace**：类似于 `strace`，但是它追踪的是用户级的库函数调用，而不是系统调用。

### 4.2 使用方法

#### 4.2.1 strace 示例

```bash
strace -o output.txt ./your_program
```

该命令将会跟踪 `your_program` 的所有系统调用，并将输出记录到 `output.txt` 文件中。

#### 4.2.2 ltrace 示例

```bash
ltrace -o output.txt ./your_program
```

这个命令会输出 `your_program` 所调用的所有库函数。

### 4.3 使用场景

`strace` 和 `ltrace` 可以帮助开发者跟踪程序在执行时与操作系统或库的交互，定位问题，或查看程序是否在按预期进行系统调用。

---

好的，下面提供一个**更贴近实际使用场景**的综合实例：

---

## 5. 综合实例：构建简易的系统日志记录工具 `mini_logger`

### 🧩 实际背景场景

**场景**：在服务器或嵌入式系统中，需要对程序运行中的关键事件进行**日志记录**。这要求日志记录系统：

1. 不依赖高层库函数，适合在资源受限或早期启动阶段使用；
2. 能记录时间戳、日志级别和消息；
3. 使用系统调用实现文件操作（如写入日志文件）；
4. 遇到错误能进行清晰报告（`errno`, `perror`, `strerror`）；
5. 能被 `strace`/`ltrace` 调试追踪。

---

### ✨ 功能目标

实现一个命令行工具：

```bash
./mini_logger info "Service started"
```

运行后将日志写入 `/var/log/mini_logger.log`，日志格式如下：

```
[INFO] 2025-06-18 10:23:14 Service started
```

---

### 📘 涉及知识点

| 模块      | 使用知识点                                            |
| ------- | ------------------------------------------------ |
| 用户态与内核态 | `syscall` 接口触发内核写日志                              |
| 系统调用封装  | `SYS_open`, `SYS_write`, `SYS_close`, `SYS_time` |
| 错误处理    | `errno`, `strerror`, `perror`                    |
| 调试      | 可通过 `strace`, `ltrace` 跟踪分析                      |

---

### 🧪 代码实现（mini\_logger.c）

```c
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define LOG_PATH "/var/log/mini_logger.log"

// 错误输出
void report_error(const char *msg) {
    dprintf(STDERR_FILENO, "%s: %s\n", msg, strerror(errno));
}

// 获取格式化时间字符串
void get_time_str(char *buffer, size_t size) {
    time_t t;
    syscall(SYS_time, &t);
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &tm);
}

// 拼接日志内容：[LEVEL] TIMESTAMP MESSAGE\n
void build_log_entry(const char *level, const char *message, char *out, size_t size) {
    char time_str[64];
    get_time_str(time_str, sizeof(time_str));
    snprintf(out, size, "[%s] %s %s\n", level, time_str, message);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        dprintf(STDERR_FILENO, "Usage: %s <LEVEL> <MESSAGE>\n", argv[0]);
        return 1;
    }

    const char *level = argv[1];
    const char *msg   = argv[2];
    char log_entry[512];

    build_log_entry(level, msg, log_entry, sizeof(log_entry));

    // 打开日志文件（追加写入）
    int fd = syscall(SYS_open, LOG_PATH, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        report_error("open log file");
        return 2;
    }

    // 写入日志
    ssize_t len = strlen(log_entry);
    if (syscall(SYS_write, fd, log_entry, len) != len) {
        report_error("write log");
        syscall(SYS_close, fd);
        return 3;
    }

    syscall(SYS_close, fd);
    return 0;
}
```

---

### 🧪 编译与调试

```bash
gcc -o mini_logger mini_logger.c
sudo ./mini_logger info "System startup complete"
```

**跟踪调试**：

```bash
strace ./mini_logger warn "Something unusual happened"
ltrace ./mini_logger error "Disk almost full"
```

---

### ✅ 示例输出（在 `/var/log/mini_logger.log`）

```
[INFO] 2025-06-18 10:23:14 System startup complete
[WARN] 2025-06-18 10:23:20 Something unusual happened
[ERROR] 2025-06-18 10:24:05 Disk almost full
```

---
