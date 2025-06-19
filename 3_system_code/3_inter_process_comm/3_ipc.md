# 进程间通信（IPC）编程笔记

## 📖 快速跳转目录

### 一、匿名管道 `pipe`
1.1 [概念介绍](#11-概念介绍)
1.2 [函数 `pipe`](#12-函数-pipe)
1.3 [管道特性与机制](#13-管道特性与机制)
1.4 [最佳实践指南](#14-最佳实践指南)
1.5 [高级应用模式](#15-高级应用模式)

### 二、有名管道 FIFO
2.1 [概念介绍](#21-概念介绍)
2.2 [函数 `mkfifo`](#22-函数-mkfifo)
2.3 [FIFO特性与机制](#23-fifo特性与机制)
2.4 [最佳实践指南](#24-最佳实践指南)
2.5 [高级应用模式](#25-高级应用模式)

### 三、POSIX 共享内存
3.1 [概念介绍](#31-概念介绍)
3.2 [函数 `shm_open`](#32-函数-shm_open)
3.3 [函数 `mmap`](#33-函数-mmap)
3.4 [共享内存特性](#34-共享内存特性)
3.5 [最佳实践指南](#35-最佳实践指南)

### 四、System V 共享内存
4.1 [概念介绍](#41-概念介绍)
4.2 [函数 `shmget`](#42-函数-shmget)
4.3 [函数 `shmat`](#43-函数-shmat)
4.4 [函数 `shmctl`](#44-函数-shmctl)
4.5 [System V特性](#45-system-v特性)

### 五、消息队列
5.1 [概念介绍](#51-概念介绍)
5.2 [System V消息队列](#52-system-v消息队列)
5.3 [POSIX消息队列](#53-posix消息队列)
5.4 [消息队列特性](#54-消息队列特性)
5.5 [最佳实践指南](#55-最佳实践指南)

### 六、信号量
6.1 [概念介绍](#61-概念介绍)
6.2 [System V信号量](#62-system-v信号量)
6.3 [POSIX信号量](#63-posix信号量)
6.4 [信号量特性](#64-信号量特性)
6.5 [最佳实践指南](#65-最佳实践指南)

### 七、轻量级同步 `futex`
7.1 [概念介绍](#71-概念介绍)
7.2 [函数 `futex`](#72-函数-futex)
7.3 [futex特性](#73-futex特性)
7.4 [最佳实践指南](#74-最佳实践指南)

### 八、套接字对 `socketpair`
8.1 [概念介绍](#81-概念介绍)
8.2 [函数 `socketpair`](#82-函数-socketpair)
8.3 [socketpair特性](#83-socketpair特性)
8.4 [最佳实践指南](#84-最佳实践指南)

### 九、Netlink 套接字
9.1 [概念介绍](#91-概念介绍)
9.2 [Netlink套接字创建](#92-netlink套接字创建)
9.3 [Netlink特性](#93-netlink特性)
9.4 [最佳实践指南](#94-最佳实践指南)

### 十、综合实例
10.1 [多进程系统监控守护进程](#101-多进程系统监控守护进程)

---

## 一、匿名管道 `pipe`

### 1.1 概念介绍

匿名管道是Linux系统中最基本的进程间通信机制，用于具有共同祖先的进程间进行单向字节流通信。管道在内存中创建，不占用文件系统空间，具有高效、简单的特点。

**核心特性**：
- 单向通信：单个管道只能单向传输数据
- 血缘要求：只能用于父子或兄弟进程
- 字节流：无消息边界，需要应用层协议处理
- 自动销毁：所有进程关闭描述符后自动清理

### 1.2 函数 `pipe`

#### 语法
```c
#include <unistd.h>
int pipe(int pipefd[2]);
```

#### 参数说明
- `pipefd[2]`：返回的管道文件描述符数组
  - `pipefd[0]`：读端，用于从管道读取数据
  - `pipefd[1]：写端，用于向管道写入数据
- **返回值**：成功返回0，失败返回-1并设置`errno`

#### 使用场景
`pipe`用于创建匿名管道，是父子进程间通信的基础机制，常用于命令管道、进程间数据传递等场景。

#### 示例代码
```c
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int fd[2];
    
    // 创建管道
    if (pipe(fd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    
    if (pid == 0) {  // 子进程
        close(fd[1]);   // 关闭写端
        char buf[64];
        ssize_t n = read(fd[0], buf, sizeof(buf));
        if (n > 0) {
            printf("Child received: %.*s\n", (int)n, buf);
        }
        close(fd[0]);
    } else {            // 父进程
        close(fd[0]);   // 关闭读端
        const char* msg = "Hello from parent";
        write(fd[1], msg, strlen(msg));
        close(fd[1]);
        wait(NULL);      // 等待子进程
    }
    
    return 0;
}
```

#### 总结
`pipe`是进程间通信的基础，注意及时关闭未使用的描述符，避免资源泄漏。

### 1.3 管道特性与机制

#### 阻塞行为
| 场景 | 读端行为 | 写端行为 |
|------|----------|----------|
| 管道空 | 阻塞等待数据 | 正常写入 |
| 管道满 | 正常读取 | 阻塞等待空间 |
| 所有写端关闭 | 返回EOF | - |
| 所有读端关闭 | - | 触发SIGPIPE |

#### 原子性保证
- 单次写入≤`PIPE_BUF`（通常4096字节）时保证原子性
- 多进程同时写入时，小数据包不会交叉混合

#### 容量限制
- 默认缓冲区大小：通常4KB-64KB
- 可通过`fcntl`调整：`fcntl(fd, F_SETPIPE_SZ, size)`

### 1.4 最佳实践指南

#### 描述符管理
```c
int fd[2];
pipe(fd);

if (fork() == 0) {   // 子进程
    close(fd[1]);    // 立即关闭未使用的写端
    // 使用读端...
} else {             // 父进程
    close(fd[0]);    // 立即关闭未使用的读端
    // 使用写端...
}
```

#### 错误处理
```c
// 检查系统调用
if (pipe(fd) == -1) {
    perror("pipe creation failed");
    exit(EXIT_FAILURE);
}

// 处理部分写入
size_t total = 0;
while (total < len) {
    ssize_t n = write(fd[1], buf + total, len - total);
    if (n == -1) {
        if (errno == EINTR) continue;  // 被信号中断，重试
        perror("write error");
        break;
    }
    total += n;
}
```

#### 信号处理
```c
// 防止SIGPIPE导致进程崩溃
signal(SIGPIPE, SIG_IGN);

// 或自定义处理程序
void sigpipe_handler(int sig) {
    printf("Caught SIGPIPE, cleaning up...\n");
    exit(EXIT_FAILURE);
}
signal(SIGPIPE, sigpipe_handler);
```

### 1.5 高级应用模式

#### 双向通信
```c
int parent_to_child[2], child_to_parent[2];
pipe(parent_to_child);  // 父→子
pipe(child_to_parent);  // 子→父

if (fork() == 0) { 
    close(parent_to_child[1]); 
    close(child_to_parent[0]);
    // 子进程逻辑
} else {
    close(parent_to_child[0]); 
    close(child_to_parent[1]);
    // 父进程逻辑
}
```

#### Shell管道实现
```c
// 模拟 ls | grep .c
int fd[2];
pipe(fd);

if (fork() == 0) {   // grep进程
    dup2(fd[0], STDIN_FILENO);  // 重定向标准输入
    close(fd[1]);
    execlp("grep", "grep", ".c", NULL);
} else {             // ls进程
    dup2(fd[1], STDOUT_FILENO); // 重定向标准输出
    close(fd[0]);
    execlp("ls", "ls", NULL);
}
```

#### 零拷贝传输
```c
// 使用splice()实现高效数据传输
while ((len = splice(input_fd, NULL, fd[1], NULL, 4096, 0)) > 0) {
    splice(fd[0], NULL, output_fd, NULL, len, 0);
}
``` 

---

## 二、有名管道 FIFO

### 2.1 概念介绍

有名管道（FIFO）是一种特殊的文件类型，允许无亲缘关系的进程间进行通信。FIFO在文件系统中以命名文件的形式存在，具有持久性，进程退出后仍然保留。

**核心特性**：
- 持久性：文件系统持久存在，进程退出后不销毁
- 无亲缘要求：任意进程都可以访问
- 命名访问：通过文件路径访问
- 阻塞特性：打开时可能阻塞等待另一端

### 2.2 函数 `mkfifo`

#### 语法
```c
#include <sys/stat.h>
int mkfifo(const char *pathname, mode_t mode);
```

#### 参数说明
- `pathname`：FIFO文件的路径名
- `mode`：文件权限位（如0644）
- **返回值**：成功返回0，失败返回-1并设置`errno`

#### 使用场景
`mkfifo`用于创建有名管道，适用于无亲缘关系进程间的通信，常用于日志收集、进程协同等场景。

#### 示例代码
```c
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 创建FIFO
    if (mkfifo("/tmp/myfifo", 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // 写入端
    int fd = open("/tmp/myfifo", O_WRONLY);
    if (fd == -1) {
        perror("open for write failed");
        exit(EXIT_FAILURE);
    }
    
    const char* msg = "Hello from FIFO writer";
    write(fd, msg, strlen(msg));
    close(fd);
    
    return 0;
}
```

#### 总结
`mkfifo`创建持久化的命名管道，注意权限设置和错误处理。

### 2.3 FIFO特性与机制

#### 阻塞行为
| 打开模式 | 行为特点 |
|----------|----------|
| O_RDONLY | 阻塞直到有写入端打开 |
| O_WRONLY | 阻塞直到有读取端打开 |
| O_NONBLOCK + O_RDONLY | 立即返回 |
| O_NONBLOCK + O_WRONLY | 无读取端时返回ENXIO |

#### 原子性保证
- 单次写入≤`PIPE_BUF`（通常4096字节）时保证原子性
- 多进程同时写入时，小数据包不会交叉混合

#### 生命周期管理
- 创建：`mkfifo()`创建FIFO文件
- 删除：`unlink()`删除FIFO文件
- 持久性：文件系统持久存在

### 2.4 最佳实践指南

#### 安全的FIFO创建
```c
// 检查FIFO是否已存在
if (access("/tmp/myfifo", F_OK) == -1) {
    if (mkfifo("/tmp/myfifo", 0644) == -1) {
        perror("mkfifo failed");
        exit(EXIT_FAILURE);
    }
}

// 设置权限掩码防止意外权限
umask(0);
mkfifo("/tmp/secure_fifo", 0660);  // 实际权限：rw-rw----
```

#### 非阻塞模式处理
```c
int fd = open("/tmp/myfifo", O_RDWR | O_NONBLOCK);
if (fd == -1) {
    if (errno == ENXIO) {
        printf("No reader connected\n");
    } else {
        perror("open failed");
    }
}

// 非阻塞读取
char buf[256];
ssize_t n = read(fd, buf, sizeof(buf));
if (n == -1) {
    if (errno == EAGAIN) {
        // 无数据可读
    } else {
        perror("read error");
    }
}
```

#### 多进程协同机制
```c
// 使用锁文件确保单实例
int lock_fd = open("/tmp/fifo.lock", O_CREAT | O_EXCL, 0644);
if (lock_fd == -1 && errno == EEXIST) {
    fprintf(stderr, "Another instance is running\n");
    exit(EXIT_FAILURE);
}

// 主逻辑...
unlink("/tmp/fifo.lock");  // 退出时删除锁文件
```

### 2.5 高级应用模式

#### 多客户端服务端架构
```c
// 服务端
int main() {
    mkfifo("/tmp/service", 0666);
    int fd = open("/tmp/service", O_RDONLY);
    
    while (1) {
        char req[256];
        if (read(fd, req, sizeof(req)) > 0) {
            // 创建专用响应FIFO
            char resp_fifo[64];
            sprintf(resp_fifo, "/tmp/client_%d", getpid());
            mkfifo(resp_fifo, 0600);
            
            // 处理请求并响应
            int resp_fd = open(resp_fifo, O_WRONLY);
            write(resp_fd, "Response", 9);
            close(resp_fd);
            unlink(resp_fifo);
        }
    }
}
```

#### 日志收集系统
```c
// 日志生产者
void log_message(const char* msg) {
    int fd = open("/tmp/app_log", O_WRONLY | O_NONBLOCK);
    if (fd != -1) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        char log_entry[512];
        snprintf(log_entry, sizeof(log_entry), "[%ld.%06ld] %s\n", 
                 tv.tv_sec, tv.tv_usec, msg);
        write(fd, log_entry, strlen(log_entry));
        close(fd);
    }
}

// 日志消费者
void log_consumer() {
    int fd = open("/tmp/app_log", O_RDONLY);
    char buffer[4096];
    while (1) {
        ssize_t n = read(fd, buffer, sizeof(buffer));
        if (n > 0) {
            fwrite(buffer, 1, n, stdout);
        } else {
            usleep(100000); // 100ms
        }
    }
}
```

#### 跨语言通信
**Python读取端**：
```python
import os

fifo_path = "/tmp/crosslang_fifo"
if not os.path.exists(fifo_path):
    os.mkfifo(fifo_path)

with open(fifo_path, 'r') as fifo:
    while True:
        data = fifo.read()
        if data:
            print(f"Python received: {data}")
```

**C写入端**：
```c
int main() {
    int fd = open("/tmp/crosslang_fifo", O_WRONLY);
    const char* msg = "Hello from C";
    write(fd, msg, strlen(msg));
    close(fd);
    return 0;
}
``` 

---

## 三、POSIX 共享内存

### 3.1 概念介绍

POSIX共享内存是现代Linux系统中推荐的共享内存机制，基于文件系统路径名访问，使用标准文件描述符接口，具有更好的可移植性和安全性。

**核心特性**：
- 基于路径名：使用文件系统路径访问
- 标准接口：使用`shm_open`、`mmap`等标准函数
- 权限控制：支持标准文件权限
- 生命周期：独立于进程存在
- 动态调整：支持运行时调整大小

### 3.2 函数 `shm_open`

#### 语法
```c
#include <sys/mman.h>
#include <fcntl.h>
int shm_open(const char *name, int oflag, mode_t mode);
```

#### 参数说明
- `name`：共享内存对象路径（格式：`/name`）
- `oflag`：标志位组合
  - `O_CREAT`：不存在则创建
  - `O_RDONLY`/`O_RDWR`：读写权限
  - `O_EXCL`：独占创建
- `mode`：权限位（如0666）
- **返回值**：成功返回文件描述符，失败返回-1并设置`errno`

#### 使用场景
`shm_open`用于创建或打开POSIX共享内存对象，适用于需要高性能数据共享的场景，如数据库缓存、图像处理等。

#### 示例代码
```c
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 创建共享内存对象
    int fd = shm_open("/shm_demo", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }
    
    // 设置共享内存大小
    if (ftruncate(fd, 4096) == -1) {
        perror("ftruncate failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // 内存映射
    char *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // 使用共享内存
    sprintf(ptr, "PID %d: Shared Hello", getpid());
    printf("Written to shared memory: %s\n", ptr);
    
    // 清理资源
    munmap(ptr, 4096);
    close(fd);
    shm_unlink("/shm_demo");
    
    return 0;
}
```

#### 总结
`shm_open`创建POSIX共享内存对象，需要配合`ftruncate`和`mmap`使用，注意及时清理资源。

### 3.3 函数 `mmap`

#### 语法
```c
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
```

#### 参数说明
- `addr`：映射地址（通常为NULL，由系统选择）
- `length`：映射长度
- `prot`：保护标志
  - `PROT_READ`：可读
  - `PROT_WRITE`：可写
  - `PROT_EXEC`：可执行
- `flags`：映射标志
  - `MAP_SHARED`：共享映射（必须）
  - `MAP_LOCKED`：锁定内存
- `fd`：文件描述符
- `offset`：文件偏移
- **返回值**：成功返回映射地址，失败返回`MAP_FAILED`

#### 使用场景
`mmap`用于将共享内存对象映射到进程地址空间，是POSIX共享内存的核心操作。

#### 示例代码
```c
// 映射共享内存
char *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
if (ptr == MAP_FAILED) {
    perror("mmap failed");
    exit(EXIT_FAILURE);
}

// 使用映射的内存
strcpy(ptr, "Hello from shared memory");

// 解除映射
munmap(ptr, 4096);
```

### 3.4 共享内存特性

#### 同步机制
```c
// 使用POSIX信号量同步
sem_t *sem = sem_open("/shm_sem", O_CREAT, 0666, 1);

sem_wait(sem);  // 进入临界区
// 访问共享内存...
sem_post(sem);  // 离开临界区
```

#### 动态调整大小
```c
// 扩展共享内存（需要重新映射）
ftruncate(fd, 8192);  // 扩展到8KB
void *new_ptr = mremap(ptr, 4096, 8192, MREMAP_MAYMOVE);
if (new_ptr == MAP_FAILED) {
    perror("mremap failed");
}
```

#### 内存锁定
```c
// 锁定物理内存（避免交换）
mlock(ptr, 4096);  // 锁定物理内存
```

### 3.5 最佳实践指南

#### 安全创建
```c
// 设置共享内存权限
shm_open("/secure_shm", O_CREAT | O_RDWR, 0600);  // 仅所有者可读写

// 使用独占模式创建
if (shm_open("/unique", O_CREAT | O_EXCL | O_RDWR, 0666) == -1) {
    if (errno == EEXIST) {
        fprintf(stderr, "Shared memory already exists\n");
    }
}
```

#### 错误处理
```c
int fd = shm_open("/shm_name", O_CREAT | O_RDWR, 0666);
if (fd == -1) {
    if (errno == EACCES) {
        fprintf(stderr, "Permission denied\n");
    } else if (errno == EEXIST) {
        fprintf(stderr, "Already exists\n");
    } else {
        perror("shm_open failed");
    }
    exit(EXIT_FAILURE);
}
```

#### 资源清理
```c
// 程序退出时清理
void cleanup() {
    munmap(ptr, size);
    close(fd);
    shm_unlink("/shm_name");
}

atexit(cleanup);
```

#### 生产者-消费者模型
```c
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#define SHM_SIZE 4096

struct shm_data {
    sem_t mutex;
    int count;
    char buffer[1024];
};

int main() {
    // 生产者进程
    int fd = shm_open("/prod_cons", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SHM_SIZE);
    struct shm_data *data = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    // 初始化信号量
    sem_init(&data->mutex, 1, 1);
    
    sem_wait(&data->mutex);
    data->count++;
    snprintf(data->buffer, sizeof(data->buffer), "Message %d", data->count);
    sem_post(&data->mutex);
    
    // 消费者进程（另一程序）
    int fd2 = shm_open("/prod_cons", O_RDWR, 0);
    struct shm_data *data2 = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    
    sem_wait(&data2->mutex);
    printf("Received: %s\n", data2->buffer);
    sem_post(&data2->mutex);
    
    // 清理
    munmap(data, SHM_SIZE);
    close(fd);
    shm_unlink("/prod_cons");
    return 0;
}
``` 

---

## 四、System V 共享内存

### 4.1 概念介绍

System V共享内存是传统的IPC共享内存机制，使用键值标识，支持精细的权限控制和特殊IPC功能，适用于遗留系统维护和特殊控制需求。

**核心特性**：
- 键值标识：使用`key_t`键值跨进程标识
- 精细控制：支持`SHM_LOCK`等特殊功能
- 持久性：需显式删除，系统重启后仍存在
- 传统接口：使用专用IPC函数族
- 固定大小：创建后大小不可调整

### 4.2 函数 `shmget`

#### 语法
```c
#include <sys/shm.h>
#include <sys/ipc.h>
int shmget(key_t key, size_t size, int shmflg);
```

#### 参数说明
- `key`：IPC键值（`IPC_PRIVATE`或`ftok`生成）
- `size`：共享内存段大小（字节）
- `shmflg`：标志位组合
  - `IPC_CREAT`：不存在则创建
  - `IPC_EXCL`：独占创建
  - 权限位（如0666）
- **返回值**：成功返回共享内存标识符，失败返回-1并设置`errno`

#### 使用场景
`shmget`用于创建或获取System V共享内存段，适用于需要特殊IPC功能的场景，如内存锁定、精细权限控制等。

#### 示例代码
```c
#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 生成唯一key
    key_t key = ftok("/tmp", 'A');
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    
    // 创建共享内存
    int shmid = shmget(key, 4096, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }
    
    // 附加到进程地址空间
    char *ptr = shmat(shmid, NULL, 0);
    if (ptr == (char*)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    
    // 使用共享内存
    sprintf(ptr, "SysV Hello from PID %d", getpid());
    printf("Written to shared memory: %s\n", ptr);
    
    // 分离共享内存
    shmdt(ptr);
    
    // 删除共享内存
    shmctl(shmid, IPC_RMID, NULL);
    
    return 0;
}
```

#### 总结
`shmget`创建System V共享内存段，需要配合`shmat`和`shmctl`使用，注意及时清理资源。

### 4.3 函数 `shmat`

#### 语法
```c
#include <sys/shm.h>
void *shmat(int shmid, const void *shmaddr, int shmflg);
```

#### 参数说明
- `shmid`：共享内存标识符
- `shmaddr`：指定附加地址（通常为NULL，由系统选择）
- `shmflg`：标志位
  - `SHM_RDONLY`：只读附加
  - `SHM_RND`：对齐地址
- **返回值**：成功返回映射地址，失败返回`(void*)-1`

#### 使用场景
`shmat`用于将共享内存段附加到进程地址空间，是System V共享内存的核心操作。

#### 示例代码
```c
// 附加共享内存
char *ptr = shmat(shmid, NULL, 0);
if (ptr == (char*)-1) {
    perror("shmat failed");
    exit(EXIT_FAILURE);
}

// 使用共享内存
strcpy(ptr, "Hello from System V shared memory");

// 分离共享内存
shmdt(ptr);
```

### 4.4 函数 `shmctl`

#### 语法
```c
#include <sys/shm.h>
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
```

#### 参数说明
- `shmid`：共享内存标识符
- `cmd`：控制命令
  - `IPC_STAT`：获取状态信息
  - `IPC_SET`：设置所有者/权限
  - `IPC_RMID`：标记为删除
  - `SHM_LOCK`：锁定内存
- `buf`：状态结构体指针
- **返回值**：成功返回0，失败返回-1并设置`errno`

#### 使用场景
`shmctl`用于控制共享内存段的属性，如获取状态、设置权限、删除等操作。

### 4.5 System V特性

#### 共享内存状态监控
```c
struct shmid_ds info;
shmctl(shmid, IPC_STAT, &info);

printf("Size: %zu\n", info.shm_segsz);
printf("Last attach: %ld\n", info.shm_atime);
printf("Processes attached: %lu\n", info.shm_nattch);
```

#### 内存锁定（避免交换）
```c
// 需要CAP_IPC_LOCK权限
shmctl(shmid, SHM_LOCK, NULL);  // 锁定整个段
```

#### 安全最佳实践
```c
// 最小权限原则
shmget(key, size, IPC_CREAT | 0640);  // 所有者读写，组用户只读

// 使用IPC_PRIVATE创建私有共享内存
int private_id = shmget(IPC_PRIVATE, size, 0666);

// 及时删除不再使用的共享内存
shmctl(shmid, IPC_RMID, NULL);  // 设置删除标记
```

#### 进程间数据采集系统
```c
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <time.h>

#define MAX_SENSORS 8

struct sensor_data {
    int count;
    float readings[MAX_SENSORS];
    time_t last_update;
};

int main() {
    // 生成唯一key
    key_t key = ftok("/tmp", 'S');
    int shmid = shmget(key, sizeof(struct sensor_data), IPC_CREAT | 0666);
    
    // 采集进程
    if (fork() == 0) {
        struct sensor_data *data = shmat(shmid, NULL, 0);
        while (1) {
            for (int i = 0; i < MAX_SENSORS; i++) {
                data->readings[i] = (float)(rand() % 100);  // 模拟传感器读数
            }
            data->count++;
            data->last_update = time(NULL);
            sleep(1);
        }
    }
    
    // 监控进程
    struct sensor_data *monitor = shmat(shmid, NULL, SHM_RDONLY);
    while (1) {
        printf("Update #%d at %s", monitor->count, ctime(&monitor->last_update));
        for (int i = 0; i < MAX_SENSORS; i++) {
            printf("Sensor %d: %.2f\n", i, monitor->readings[i]);
        }
        sleep(5);
    }
    
    // 清理
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}
``` 

---

## 五、消息队列

### 5.1 概念介绍

消息队列提供有序的、类型化的进程间通信机制，支持不同进程间的结构化消息传递，具有持久性和优先级支持。

**核心特性**：
- 消息持久性：消息在系统重启前一直存在
- 类型过滤：可接收特定类型的消息
- 优先级支持：通过消息类型实现优先级
- 异步通信：发送者和接收者无需同时存在
- 容量限制：受系统参数限制

### 5.2 System V消息队列

#### 语法
```c
#include <sys/msg.h>

// 创建/获取消息队列
int msgget(key_t key, int msgflg);

// 发送消息
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);

// 接收消息
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);

// 控制操作
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```

#### 消息结构定义
```c
struct msgbuf {
    long mtype;       // 必须 > 0 的消息类型
    char mtext[1];    // 实际消息数据（柔性数组）
};

// 实际使用示例：
struct custom_msg {
    long mtype;
    int data_id;
    float value;
    char description[32];
};
```

#### 最小示例
```c
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    // 创建消息队列
    int qid = msgget(0x1234, IPC_CREAT | 0644);
    if (qid == -1) {
        perror("msgget failed");
        return 1;
    }

    // 定义消息结构
    struct {
        long mtype;
        char mtext[32];
    } msg;

    if (fork() == 0) {  // 子进程：接收消息
        msgrcv(qid, &msg, sizeof(msg.mtext), 1, 0);
        printf("Child received: %s\n", msg.mtext);
    } else {            // 父进程：发送消息
        msg.mtype = 1;
        strcpy(msg.mtext, "Hello from parent");
        msgsnd(qid, &msg, sizeof(msg.mtext), 0);
        wait(NULL);
        
        // 清理消息队列
        msgctl(qid, IPC_RMID, NULL);
    }
    return 0;
}
```

### 5.3 POSIX消息队列

#### 语法
```c
#include <mqueue.h>

// 创建队列
mqd_t mq_open(const char *name, int oflag, mode_t mode, struct mq_attr *attr);

// 发送消息
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);

// 接收消息
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);

// 关闭和删除
int mq_close(mqd_t mqdes);
int mq_unlink(const char *name);
```

#### 示例代码
```c
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    // 创建队列
    mqd_t mq = mq_open("/myqueue", O_CREAT | O_RDWR, 0644, NULL);
    if (mq == (mqd_t)-1) {
        perror("mq_open failed");
        exit(1);
    }

    // 发送消息
    struct { int id; float value; } data = {1, 3.14};
    if (mq_send(mq, (char*)&data, sizeof(data), 10) == -1) {
        perror("mq_send failed");
    }

    // 接收消息
    struct mq_attr attr;
    mq_getattr(mq, &attr);
    char buf[attr.mq_msgsize];
    unsigned prio;
    if (mq_receive(mq, buf, sizeof(buf), &prio) == -1) {
        perror("mq_receive failed");
    }

    // 关闭和删除
    mq_close(mq);
    mq_unlink("/myqueue");
    return 0;
}
```

### 5.4 消息队列特性

#### 阻塞行为控制
| 标志 | `msgsnd`行为 | `msgrcv`行为 |
|------|-------------|-------------|
| 0 | 队列满时阻塞 | 无匹配消息时阻塞 |
| IPC_NOWAIT | 队列满时立即返回EAGAIN | 无消息时立即返回ENOMSG |

#### 消息接收模式
```c
// 接收指定类型的消息
msgrcv(qid, &msg, sizeof(msg.mtext), 5, 0); // 只接收类型5

// 接收类型 ≤ N 的最小类型消息
msgrcv(qid, &msg, sizeof(msg.mtext), -3, 0); // 接收类型≤3的最小类型

// 接收任意类型消息
msgrcv(qid, &msg, sizeof(msg.mtext), 0, 0); // 接收队列中第一条消息
```

### 5.5 最佳实践指南

#### 健壮性设计
```c
// 使用ftok生成唯一key
key_t key = ftok("/some/existing/file", 'A');
if (key == -1) {
    perror("ftok failed");
    exit(EXIT_FAILURE);
}

// 创建队列时检查已有队列
int qid = msgget(key, IPC_CREAT | IPC_EXCL | 0644);
if (qid == -1 && errno == EEXIST) {
    qid = msgget(key, 0644);  // 已存在则打开
}
```

#### 消息结构设计
```c
// 使用固定头部+数据体
struct app_msg {
    long mtype;           // 消息类型
    uint16_t version;     // 协议版本
    uint32_t data_len;    // 数据长度
    char payload[];       // 可变长度数据
};

// 发送时动态分配
struct app_msg *msg = malloc(sizeof(*msg) + data_size);
msg->mtype = 2;
msg->version = 1;
msg->data_len = data_size;
memcpy(msg->payload, data, data_size);
msgsnd(qid, msg, sizeof(*msg) + data_size, 0);
free(msg);
```

#### 错误处理
```c
// 带错误检查的发送
if (msgsnd(qid, &msg, msg_size, IPC_NOWAIT) == -1) {
    if (errno == EAGAIN) {
        // 队列满，实现重试或等待
        usleep(100000); // 等待100ms
    } else {
        perror("msgsnd failed");
    }
}

// 安全的接收
ssize_t received = msgrcv(qid, &msg, max_size, msg_type, MSG_NOERROR | IPC_NOWAIT);
if (received == -1) {
    if (errno == ENOMSG) {
        // 无消息，继续处理其他任务
    } else {
        perror("msgrcv failed");
    }
}
```

#### 资源管理
```c
// 程序退出时清理队列
void cleanup(int sig) {
    msgctl(qid, IPC_RMID, NULL);
    exit(0);
}

signal(SIGINT, cleanup);
signal(SIGTERM, cleanup);
``` 

---

## 六、信号量

### 6.1 概念介绍

信号量是一种同步原语，用于控制多个进程或线程对共享资源的访问。信号量维护一个计数器，通过P操作（等待）和V操作（释放）来实现同步。

**核心特性**：
- 计数器机制：维护一个整数值
- P/V操作：P操作减少计数，V操作增加计数
- 阻塞机制：计数为0时P操作阻塞
- 原子性：操作是原子的，不可中断
- 多种类型：System V和POSIX两种实现

### 6.2 System V信号量

#### 语法
```c
#include <sys/sem.h>
#include <sys/ipc.h>

// 创建/获取信号量集
int semget(key_t key, int nsems, int semflg);

// 信号量操作
int semop(int semid, struct sembuf *sops, size_t nsops);

// 控制操作
int semctl(int semid, int semnum, int cmd, ... /* union semun arg */);
```

#### 参数说明
- `key`：IPC键值（`IPC_PRIVATE`或`ftok`生成）
- `nsems`：信号量集中信号量的数量
- `semflg`：标志位组合
  - `IPC_CREAT`：不存在则创建
  - `IPC_EXCL`：独占创建
  - 权限位（如0666）
- **返回值**：成功返回信号量集标识符，失败返回-1并设置`errno`

#### 使用场景
System V信号量用于进程间同步，支持信号量集操作，适用于复杂的同步场景。

#### 示例代码
```c
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // 生成唯一key
    key_t key = ftok("/tmp", 'S');
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }
    
    // 创建信号量集（1个信号量）
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }
    
    // 初始化信号量值为1
    union semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL failed");
        exit(EXIT_FAILURE);
    }
    
    // P操作（获取资源）
    struct sembuf p_op = {0, -1, 0};
    if (semop(semid, &p_op, 1) == -1) {
        perror("semop P failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Critical section entered\n");
    sleep(2);  // 模拟临界区操作
    
    // V操作（释放资源）
    struct sembuf v_op = {0, 1, 0};
    if (semop(semid, &v_op, 1) == -1) {
        perror("semop V failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Critical section exited\n");
    
    // 删除信号量集
    semctl(semid, 0, IPC_RMID);
    
    return 0;
}
```

#### 总结
System V信号量提供复杂的同步机制，支持信号量集操作，适用于需要精细控制的场景。

### 6.3 POSIX信号量

#### 语法
```c
#include <semaphore.h>

// 创建/打开命名信号量
sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);

// 基本操作
int sem_wait(sem_t *sem);    // P操作
int sem_post(sem_t *sem);    // V操作
int sem_trywait(sem_t *sem); // 非阻塞尝试
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

// 清理资源
int sem_close(sem_t *sem);
int sem_unlink(const char *name);
```

#### 参数说明
- `name`：信号量名称（以'/'开头，如"/mysem"）
- `oflag`：标志位
  - `O_CREAT`：不存在则创建
  - `O_EXCL`：排他创建
- `mode`：权限位（如0644）
- `value`：信号量初始值
- **返回值**：成功返回信号量指针，失败返回`SEM_FAILED`

#### 使用场景
POSIX信号量提供更简洁的API，原生支持线程同步，适用于现代Linux应用开发。

#### 示例代码
```c
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // 创建命名信号量
    sem_t *sem = sem_open("/demo_sem", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(EXIT_FAILURE);
    }
    
    // P操作（获取资源）
    if (sem_wait(sem) == -1) {
        perror("sem_wait failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Critical section entered\n");
    sleep(2);  // 模拟临界区操作
    
    // V操作（释放资源）
    if (sem_post(sem) == -1) {
        perror("sem_post failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Critical section exited\n");
    
    // 清理资源
    sem_close(sem);
    sem_unlink("/demo_sem");
    
    return 0;
}
```

### 6.4 信号量特性

#### 操作类型
| 操作类型 | 行为描述 |
|----------|----------|
| P操作 | 减少计数，计数为0时阻塞 |
| V操作 | 增加计数，唤醒等待的进程 |
| 零等待 | 等待计数变为0 |

#### 原子性保证
- 所有P/V操作都是原子的
- 多信号量操作保证原子性
- 支持`SEM_UNDO`机制防止死锁

#### 阻塞行为
```c
// 阻塞等待
sem_wait(sem);

// 非阻塞尝试
if (sem_trywait(sem) == -1 && errno == EAGAIN) {
    // 信号量为0，不阻塞
}

// 限时等待
struct timespec ts;
clock_gettime(CLOCK_REALTIME, &ts);
ts.tv_sec += 5;  // 5秒超时
if (sem_timedwait(sem, &ts) == -1 && errno == ETIMEDOUT) {
    // 超时
}
```

### 6.5 最佳实践指南

#### 错误处理
```c
if (semop(semid, &op, 1) == -1) {
    if (errno == EINTR) {
        // 被信号中断，重试操作
    } else if (errno == EIDRM) {
        // 信号量已被删除
        exit(EXIT_FAILURE);
    }
}
```

#### 资源清理
```c
// System V信号量
semctl(semid, 0, IPC_RMID);

// POSIX信号量
sem_close(sem);
sem_unlink("/sem_name");
```

#### SEM_UNDO机制
```c
struct sembuf op = {0, -1, SEM_UNDO};
// 进程崩溃时自动撤销操作
```

#### 多信号量操作
```c
struct sembuf ops[2] = {
    {0, -1, 0},  // 锁定资源A
    {1, -1, 0}   // 锁定资源B
};
semop(semid, ops, 2);  // 原子操作
```

#### 生产者-消费者模型
```c
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

#define BUFFER_SIZE 10

sem_t *empty, *full, *mutex;
int buffer[BUFFER_SIZE];
int in = 0, out = 0;

void init_semaphores() {
    // 创建命名信号量
    empty = sem_open("/empty", O_CREAT, 0644, BUFFER_SIZE);
    full = sem_open("/full", O_CREAT, 0644, 0);
    mutex = sem_open("/mutex", O_CREAT, 0644, 1);
}

void* producer(void* arg) {
    for (int i = 0; i < 100; i++) {
        sem_wait(empty);   // 等待空槽位
        sem_wait(mutex);   // 进入临界区
        
        buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        printf("Produced: %d\n", i);
        
        sem_post(mutex);   // 离开临界区
        sem_post(full);    // 增加可用项目
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < 100; i++) {
        sem_wait(full);    // 等待可用项目
        sem_wait(mutex);   // 进入临界区
        
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("Consumed: %d\n", item);
        
        sem_post(mutex);   // 离开临界区
        sem_post(empty);   // 增加空槽位
    }
    return NULL;
}

int main() {
    init_semaphores();
    
    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, NULL);
    pthread_create(&cons, NULL, consumer, NULL);
    
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    
    // 清理
    sem_close(empty);
    sem_close(full);
    sem_close(mutex);
    sem_unlink("/empty");
    sem_unlink("/full");
    sem_unlink("/mutex");
    
    return 0;
}
``` 

---

## 七、轻量级同步 `futex`

### 7.1 概念介绍

`futex`（Fast Userspace muTEX）是Linux提供的用户空间低开销锁机制，通过内核辅助实现高效同步。futex采用混合模型：无竞争时在用户空间操作，竞争时陷入内核进行线程调度。

**核心特性**：
- 快速路径：无竞争时在用户空间原子操作完成
- 慢速路径：竞争时陷入内核进行线程调度
- 混合模型：用户空间尝试+内核等待队列
- 高性能：最小化内核介入，减少上下文切换

### 7.2 函数 `futex`

#### 语法
```c
#include <linux/futex.h>
#include <sys/syscall.h>

int futex(int *uaddr, int futex_op, int val,
          const struct timespec *timeout, 
          int *uaddr2, int val3);
```

#### 参数说明
- `uaddr`：指向futex变量的指针（必须对齐到4字节）
- `futex_op`：操作类型和标志位
- `val`：操作相关值（通常用于比较）
- `timeout`：等待超时时间（NULL表示无限等待）
- `uaddr2`：第二个futex地址（REQUEUE操作使用）
- `val3`：操作特定参数
- **返回值**：成功返回0，失败返回-1并设置`errno`

#### 使用场景
`futex`用于实现高效的同步原语，如互斥锁、条件变量等，适用于高并发场景。

#### 示例代码
```c
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>

// 用户空间自旋尝试
#define SPIN_COUNT 100

void futex_lock(atomic_int *futex) {
    int c;
    // 用户空间快速路径
    while ((c = atomic_compare_exchange_strong(futex, 0, 1)) {
        if (c != 2) {
            // 尝试将状态设为"锁定+等待"
            if (atomic_exchange(futex, 2) == 0) 
                return;
        }
        
        // 有限自旋
        for (int i = 0; i < SPIN_COUNT; i++) {
            if (*futex == 0) {
                if (atomic_compare_exchange_strong(futex, 0, 2))
                    return;
            }
            __builtin_ia32_pause(); // CPU暂停指令
        }
        
        // 进入内核等待
        syscall(SYS_futex, futex, FUTEX_WAIT, 2, NULL, NULL, 0);
    }
}

void futex_unlock(atomic_int *futex) {
    // 快速释放锁
    if (atomic_exchange(futex, 0) == 2) {
        // 有等待线程，唤醒一个
        syscall(SYS_futex, futex, FUTEX_WAKE, 1, NULL, NULL, 0);
    }
}

int main() {
    atomic_int lock = ATOMIC_VAR_INIT(0);
    
    futex_lock(&lock);
    printf("Critical section entered\n");
    sleep(1);
    futex_unlock(&lock);
    printf("Critical section exited\n");
    
    return 0;
}
```

#### 总结
`futex`提供高效的同步机制，通过用户空间快速路径和内核慢速路径的混合模型实现高性能。

### 7.3 futex特性

#### 核心操作类型
| 操作 | 值 | 功能 |
|------|----|------|
| FUTEX_WAIT | 0 | 如果`*uaddr == val`，则阻塞线程 |
| FUTEX_WAKE | 1 | 唤醒最多`val`个等待线程 |
| FUTEX_REQUEUE | 3 | 转移等待线程到另一个futex |
| FUTEX_CMP_REQUEUE | 4 | 带条件检查的转移 |
| FUTEX_WAKE_OP | 5 | 唤醒+原子操作 |
| FUTEX_WAIT_BITSET | 9 | 使用bitmask指定等待条件 |

#### 常用标志位
- `FUTEX_PRIVATE_FLAG` (128)：进程内私有futex（性能优化）
- `FUTEX_CLOCK_REALTIME` (256)：使用CLOCK_REALTIME计时
- `FUTEX_BITSET_MATCH_ANY` (~0)：匹配任意等待条件

### 7.4 最佳实践指南

#### 条件变量实现
```c
void futex_wait(atomic_int *futex, int val) {
    syscall(SYS_futex, futex, FUTEX_WAIT, val, NULL, NULL, 0);
}

void futex_wake(atomic_int *futex, int count) {
    syscall(SYS_futex, futex, FUTEX_WAKE, count, NULL, NULL, 0);
}

// 使用示例
atomic_int cond = ATOMIC_VAR_INIT(0);

// 等待线程
if (/* 条件不满足 */) {
    int oldval = cond.load();
    while (!/* 条件检查 */) {
        futex_wait(&cond, oldval);
    }
}

// 通知线程
/* 修改条件 */
futex_wake(&cond, 1);  // 唤醒一个等待线程
```

#### 性能优化技巧
```c
// 私有futex标志
syscall(SYS_futex, futex, FUTEX_WAKE | FUTEX_PRIVATE_FLAG, 1, NULL, NULL, 0);

// 精确唤醒策略
uint32_t bitset = 1 << index;  // 特定条件位
syscall(SYS_futex, futex, FUTEX_WAIT_BITSET, val, timeout, NULL, bitset);
syscall(SYS_futex, futex, FUTEX_WAKE_BITSET, count, NULL, NULL, bitset);
```

#### 自适应自旋策略
```c
// 根据历史等待时间动态调整自旋次数
static __thread int spin_count = 100;

for (int i = 0; i < spin_count; i++) {
    if (*futex == desired_value) return;
    __builtin_ia32_pause();
}

// 更新自旋计数（指数退避）
spin_count = (spin_count < 1000) ? spin_count * 1.5 : 1000;
```

---

## 八、套接字对 `socketpair`

### 8.1 概念介绍

套接字对是创建一对已连接的套接字，支持双向全双工通信。套接字对在内核内部实现，无需网络协议栈，具有零开销本地通信的特点。

**核心特性**：
- 双向全双工通信：创建一对已连接的套接字，支持同时读写
- 零开销本地通信：内核内部实现，无需网络协议栈
- 血缘进程支持：专为父子/同组进程设计
- 自动连接：创建即连接，无需手动`bind`/`connect`
- 类型支持：`SOCK_STREAM`（字节流）和`SOCK_DGRAM`（数据报）

### 8.2 函数 `socketpair`

#### 语法
```c
#include <sys/socket.h>
int socketpair(int domain, int type, int protocol, int sv[2]);
```

#### 参数说明
- `domain`：地址族（通常为`AF_UNIX`）
- `type`：套接字类型（`SOCK_STREAM`或`SOCK_DGRAM`）
- `protocol`：协议类型（通常设为0）
- `sv[2]`：返回的套接字对
- **返回值**：成功返回0，失败返回-1并设置`errno`

#### 使用场景
`socketpair`用于创建进程间通信的套接字对，适用于RPC通信、任务分发等场景。

#### 示例代码
```c
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main() {
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // 子进程
        close(sv[0]);  // 关闭父进程端
        
        // 接收消息
        char buf[128];
        ssize_t n = read(sv[1], buf, sizeof(buf));
        if (n > 0) {
            printf("Child received: %.*s\n", (int)n, buf);
            
            // 发送响应
            const char* reply = "ACK from child";
            write(sv[1], reply, strlen(reply) + 1);
        }
        
        close(sv[1]);
        exit(EXIT_SUCCESS);
    } 
    else {  // 父进程
        close(sv[0]);  // 关闭子进程端
        
        // 发送消息
        const char* msg = "Hello from parent";
        if (write(sv[0], msg, strlen(msg) + 1) == -1) {
            perror("write failed");
        }
        
        // 接收响应
        char reply[128];
        ssize_t n = read(sv[0], reply, sizeof(reply));
        if (n > 0) {
            printf("Parent received: %s\n", reply);
        }
        
        close(sv[0]);
        wait(NULL);
    }
    return 0;
}
```

### 8.3 socketpair特性

#### 典型使用场景
1. **进程间RPC通信**：轻量级远程过程调用
2. **调度器与工作进程**：主进程分发任务，工作进程返回结果
3. **双工管道替代**：替代两个单向管道
4. **线程安全通信**：同一进程内线程间通信

#### 性能对比
| 特性 | `socketpair` | `pipe` |
|------|-------------|--------|
| 通信方向 | 双向 | 单向 |
| 数据类型 | 字节流/数据报 | 字节流 |
| 进程关系 | 父子/同组 | 父子/兄弟 |
| 性能开销 | 极低 | 极低 |
| 典型用途 | RPC、双工通信 | 单向数据流 |

### 8.4 最佳实践指南

#### 多进程服务架构
```c
#define MAX_WORKERS 3

typedef struct {
    int task_id;
    int data;
} Task;

typedef struct {
    int worker_id;
    int result;
} Result;

int main() {
    int worker_sockets[MAX_WORKERS][2];
    
    // 创建工作进程
    for (int i = 0; i < MAX_WORKERS; i++) {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, worker_sockets[i]) == -1) {
            perror("socketpair failed");
            exit(EXIT_FAILURE);
        }
        
        pid_t pid = fork();
        if (pid == 0) {  // Worker进程
            close(worker_sockets[i][0]);  // 关闭父进程端
            
            while (1) {
                Task task;
                ssize_t n = read(worker_sockets[i][1], &task, sizeof(Task));
                if (n <= 0) break;  // EOF或错误
                
                // 处理任务
                Result res = {i, task.data * 2};
                write(worker_sockets[i][1], &res, sizeof(Result));
            }
            close(worker_sockets[i][1]);
            exit(0);
        } else {  // 父进程
            close(worker_sockets[i][1]);  // 关闭子进程端
        }
    }
    
    // 分发任务
    for (int i = 0; i < 10; i++) {
        int worker = i % MAX_WORKERS;
        Task task = {i, i + 1};
        write(worker_sockets[worker][0], &task, sizeof(Task));
        
        Result res;
        read(worker_sockets[worker][0], &res, sizeof(Result));
        printf("Task %d result: %d (worker %d)\n", i, res.result, res.worker_id);
    }
    
    // 清理
    for (int i = 0; i < MAX_WORKERS; i++) {
        close(worker_sockets[i][0]);  // 关闭连接
        wait(NULL);  // 等待子进程退出
    }
    return 0;
}
```

#### 非阻塞模式
```c
// 设置非阻塞标志
fcntl(sv[0], F_SETFL, O_NONBLOCK);
fcntl(sv[1], F_SETFL, O_NONBLOCK);

// 带超时的select
fd_set readfds;
FD_ZERO(&readfds);
FD_SET(sv[0], &readfds);

struct timeval timeout = {.tv_sec = 1, .tv_usec = 0}; // 1秒超时

int ready = select(sv[0] + 1, &readfds, NULL, NULL, &timeout);
if (ready > 0 && FD_ISSET(sv[0], &readfds)) {
    // 数据可读
}
```

---

## 九、Netlink 套接字

### 9.1 概念介绍

Netlink套接字是Linux内核与用户空间通信的专用机制，提供异步事件通知、多播支持和双向通信能力。

**核心特性**：
- 内核-用户空间通信：专用内核通信通道
- 异步事件通知：实时接收内核事件
- 多播支持：同时通知多个应用
- 大数据传输：支持分片传输大消息
- 双向通信：用户可查询，内核可通知
- 协议扩展：支持自定义协议

### 9.2 Netlink套接字创建

#### 语法
```c
#include <linux/netlink.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

#### 参数说明
- `domain`：地址族（`AF_NETLINK`）
- `type`：套接字类型（`SOCK_RAW`或`SOCK_DGRAM`）
- `protocol`：Netlink协议类型
- **返回值**：成功返回套接字描述符，失败返回-1并设置`errno`

#### 使用场景
Netlink套接字用于内核与用户空间的通信，适用于网络配置、系统监控、设备管理等场景。

#### 示例代码
```c
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main() {
    // 创建Netlink套接字
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 配置地址
    struct sockaddr_nl addr;
    memset(&addr, 0, sizeof(addr));
    addr.nl_family = AF_NETLINK;
    addr.nl_pid = getpid();  // 当前进程ID
    addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR; // 订阅事件组

    // 绑定套接字
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // 接收消息
    char buf[4096];
    struct iovec iov = { buf, sizeof(buf) };
    struct sockaddr_nl src_addr;
    struct msghdr msg = {
        .msg_name = &src_addr,
        .msg_namelen = sizeof(src_addr),
        .msg_iov = &iov,
        .msg_iovlen = 1
    };

    printf("Listening for network events...\n");
    
    while (1) {
        ssize_t len = recvmsg(fd, &msg, 0);
        if (len <= 0) {
            perror("recvmsg failed");
            break;
        }

        // 解析Netlink消息头
        struct nlmsghdr *nlh = (struct nlmsghdr *)buf;
        for (; NLMSG_OK(nlh, len); nlh = NLMSG_NEXT(nlh, len)) {
            switch (nlh->nlmsg_type) {
                case RTM_NEWLINK:
                    printf("New network interface detected\n");
                    break;
                case RTM_DELLINK:
                    printf("Network interface removed\n");
                    break;
                case RTM_NEWADDR:
                    printf("New IP address assigned\n");
                    break;
                case RTM_DELADDR:
                    printf("IP address removed\n");
                    break;
                default:
                    printf("Received event: %d\n", nlh->nlmsg_type);
            }
        }
    }

    close(fd);
    return 0;
}
```

### 9.3 Netlink特性

#### 查询网络接口信息
```c
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>

void get_network_interfaces() {
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd == -1) {
        perror("socket failed");
        return;
    }

    // 准备请求消息
    char req_buf[NLMSG_SPACE(sizeof(struct ifinfomsg))];
    struct nlmsghdr *nlh = (struct nlmsghdr *)req_buf;
    
    // 初始化消息头
    nlh->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    nlh->nlmsg_type = RTM_GETLINK;
    nlh->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    nlh->nlmsg_seq = 1;
    nlh->nlmsg_pid = getpid();

    // 发送请求
    struct sockaddr_nl sa;
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    
    struct iovec iov = { req_buf, nlh->nlmsg_len };
    struct msghdr msg = {
        .msg_name = &sa,
        .msg_namelen = sizeof(sa),
        .msg_iov = &iov,
        .msg_iovlen = 1
    };
    
    if (sendmsg(fd, &msg, 0) == -1) {
        perror("sendmsg failed");
        close(fd);
        return;
    }

    // 接收响应
    char resp_buf[4096];
    printf("Network Interfaces:\n");
    printf("-------------------\n");
    
    while (1) {
        iov.iov_base = resp_buf;
        iov.iov_len = sizeof(resp_buf);
        
        ssize_t len = recvmsg(fd, &msg, 0);
        if (len <= 0) break;

        // 解析接口信息
        struct nlmsghdr *resp_nlh = (struct nlmsghdr *)resp_buf;
        for (; NLMSG_OK(resp_nlh, len); resp_nlh = NLMSG_NEXT(resp_nlh, len)) {
            if (resp_nlh->nlmsg_type == RTM_NEWLINK) {
                struct ifinfomsg *ifi = (struct ifinfomsg *)NLMSG_DATA(resp_nlh);
                struct rtattr *rta = IFLA_RTA(ifi);
                int rta_len = IFLA_PAYLOAD(resp_nlh);
                
                char ifname[IF_NAMESIZE] = "unknown";
                
                while (rta_len >= RTA_LENGTH(0)) {
                    if (rta->rta_type == IFLA_IFNAME) {
                        strncpy(ifname, (char *)RTA_DATA(rta), IF_NAMESIZE - 1);
                        break;
                    }
                    
                    rta = RTA_NEXT(rta, rta_len);
                }
                
                printf("Interface: %s (Index: %d)\n", ifname, ifi->ifi_index);
            }
        }
    }
    
    close(fd);
}
```

### 9.4 最佳实践指南

#### 自定义Netlink协议
```c
// 用户空间
#define MY_PROTOCOL 31  // > 16 的自定义协议号

int fd = socket(AF_NETLINK, SOCK_RAW, MY_PROTOCOL);

// 内核模块
#include <linux/netlink.h>

static struct sock *nl_sk;

static void my_nl_recv_msg(struct sk_buff *skb) {
    // 处理接收到的消息
}

static int __init my_module_init(void) {
    struct netlink_kernel_cfg cfg = {
        .input = my_nl_recv_msg,
    };
    
    nl_sk = netlink_kernel_create(&init_net, MY_PROTOCOL, &cfg);
    if (!nl_sk) return -ENOMEM;
    
    return 0;
}
```

#### 大消息分片处理
```c
// 发送大消息
struct nlmsghdr *nlh = create_large_message();
size_t remaining = nlh->nlmsg_len;
char *ptr = (char *)nlh;

while (remaining > 0) {
    size_t chunk = remaining > 4096 ? 4096 : remaining;
    ssize_t sent = send(fd, ptr, chunk, 0);
    if (sent <= 0) break;
    ptr += sent;
    remaining -= sent;
}

// 接收大消息
struct msghdr msg = {0};
struct iovec iov;
char buffer[16384];
iov.iov_base = buffer;
iov.iov_len = sizeof(buffer);

while (1) {
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    
    ssize_t len = recvmsg(fd, &msg, MSG_PEEK);
    if (len <= 0) break;
    
    struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;
    if (!NLMSG_OK(nlh, len)) continue;
    
    // 分配足够空间
    char *full_msg = malloc(nlh->nlmsg_len);
    
    // 读取完整消息
    size_t total = 0;
    while (total < nlh->nlmsg_len) {
        ssize_t n = recv(fd, full_msg + total, nlh->nlmsg_len - total, 0);
        if (n <= 0) break;
        total += n;
    }
    
    process_message((struct nlmsghdr *)full_msg);
    free(full_msg);
}
```

#### 性能优化技巧
```c
// 增大套接字缓冲区
int size = 1024 * 1024;  // 1MB
setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));

// 使用内存映射I/O（Linux 3.10+）
struct nl_mmap_req req = {
    .nm_block_size = 4096,
    .nm_block_nr = 64,
    .nm_frame_size = 1024,
    .nm_frame_nr = 256,
};
setsockopt(fd, SOL_NETLINK, NETLINK_RX_RING, &req, sizeof(req));
setsockopt(fd, SOL_NETLINK, NETLINK_TX_RING, &req, sizeof(req));
```

#### 安全注意事项
1. **权限控制**：
   ```c
   // 内核模块检查权限
   if (!netlink_capable(skb, CAP_NET_ADMIN))
       return -EPERM;
   ```

2. **消息验证**：
   ```c
   // 验证消息长度
   if (nlh->nlmsg_len < sizeof(struct nlmsghdr) || 
       skb->len < nlh->nlmsg_len) {
       return -EINVAL;
   }
   ```

3. **来源验证**：
   ```c
   // 检查发送方PID
   if (nlh->nlmsg_pid != expected_pid) {
       return -EPERM;
   }
   ```

#### 调试与诊断
```bash
# 使用libnl工具
sudo apt install libnl-3-dev libnl-genl-3-dev

# 监听Netlink事件
nlmon -i all

# 解析Netlink消息
tcpdump -i nlmon0 -w netlink.pcap
```

```c
// 内核调试
printk(KERN_INFO "Netlink message received: type=%d, len=%d\n", 
       nlh->nlmsg_type, nlh->nlmsg_len);

// 用户空间调试
printf("Received Netlink message: type=%hu, flags=%hu, seq=%u, pid=%u\n",
       nlh->nlmsg_type, nlh->nlmsg_flags, nlh->nlmsg_seq, nlh->nlmsg_pid);
```

---

## 十、综合实例

### 10.1 多进程系统监控守护进程

#### 实现目标
实现一个守护程序，定期收集网络状态（通过Netlink）、将数据写入POSIX共享内存，并用POSIX信号量同步，客户端通过消息队列请求获取最新数据；父子进程用socketpair传递控制命令，备用日志通过FIFO输出。

#### 涉及知识点
- 匿名管道`pipe`/有名管道FIFO
- POSIX共享内存
- System V消息队列
- POSIX信号量
- 套接字对`socketpair`
- Netlink套接字

#### 代码实现
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <semaphore.h>
#include <string.h>
#include <linux/netlink.h>
#include <time.h>

#define SHM_NAME "/ipc_shm"
#define SEM_NAME "/ipc_sem"
#define MSG_KEY 0x1111
#define FIFO_PATH "/tmp/ipc_fifo"

struct msgbuf { 
    long mtype; 
    char data[128]; 
};

// 共享内存数据结构
struct system_data {
    time_t timestamp;
    int network_interfaces;
    float cpu_usage;
    long memory_usage;
    char status[32];
};

int main() {
    // 1. FIFO 日志初始化
    mkfifo(FIFO_PATH, 0666);
    int fifo_fd = open(FIFO_PATH, O_WRONLY | O_NONBLOCK);

    // 2. POSIX 共享内存+信号量
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(struct system_data));
    struct system_data *shm_ptr = mmap(NULL, sizeof(struct system_data), 
                                       PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);

    // 3. 消息队列
    int msqid = msgget(MSG_KEY, IPC_CREAT | 0666);

    // 4. 父子 socketpair
    int sv[2]; 
    socketpair(AF_UNIX, SOCK_STREAM, 0, sv);

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程：收集网络状态 via Netlink
        close(sv[0]);  // 关闭父进程端
        
        int nl = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (nl != -1) {
            struct sockaddr_nl addr;
            memset(&addr, 0, sizeof(addr));
            addr.nl_family = AF_NETLINK;
            addr.nl_pid = getpid();
            bind(nl, (struct sockaddr*)&addr, sizeof(addr));
            
            // 模拟收集网络数据
            struct system_data data;
            data.timestamp = time(NULL);
            data.network_interfaces = 3;  // 模拟数据
            data.cpu_usage = 45.5;
            data.memory_usage = 1024 * 1024 * 512;  // 512MB
            strcpy(data.status, "normal");
            
            // 写入共享内存
            sem_wait(sem);
            memcpy(shm_ptr, &data, sizeof(data));
            sem_post(sem);
            
            close(nl);
        }
        
        // 通知父进程完成
        write(sv[1], "OK", 2);
        close(sv[1]);
        exit(0);
    } else {
        // 父进程：等待子进程完成
        close(sv[1]);  // 关闭子进程端
        
        char buf[4]; 
        read(sv[0], buf, 2);
        
        // 写日志到 FIFO
        dprintf(fifo_fd, "Child updated shm at %s", ctime(&shm_ptr->timestamp));
        
        // 响应客户端消息
        struct msgbuf msg;
        if (msgrcv(msqid, &msg, sizeof(msg.data), 1, IPC_NOWAIT) >= 0) {
            sem_wait(sem);
            snprintf(msg.data, sizeof(msg.data), 
                     "Status: %s, CPU: %.1f%%, Memory: %ld MB", 
                     shm_ptr->status, shm_ptr->cpu_usage, 
                     shm_ptr->memory_usage / (1024 * 1024));
            sem_post(sem);
            msgsnd(msqid, &msg, strlen(msg.data) + 1, 0);
        }
        
        close(sv[0]);
        wait(NULL);
    }
    
    // 清理资源
    close(fifo_fd);
    munmap(shm_ptr, sizeof(struct system_data));
    close(shm_fd);
    sem_close(sem);
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_NAME);
    msgctl(msqid, IPC_RMID, NULL);
    unlink(FIFO_PATH);
    
    return 0;
}
```

#### 运行与调试
```bash
# 编译
gcc -o ipc_monitor ipc_monitor.c -lrt -lpthread

# 运行
./ipc_monitor

# 查看FIFO日志
cat /tmp/ipc_fifo

# 发送消息队列请求
echo "request" | ipc_send_message
```

#### 系统设计亮点
1. **多种IPC机制集成**：综合使用管道、共享内存、消息队列、信号量、套接字对和Netlink
2. **数据采集与分发分离**：子进程负责数据采集，父进程负责数据分发
3. **同步机制**：使用POSIX信号量保护共享内存访问
4. **异步通信**：消息队列支持客户端异步请求
5. **日志记录**：FIFO提供备用日志通道
6. **资源管理**：程序退出时清理所有IPC资源

#### 常见问题与解答
- **Q**: 为什么使用多种IPC机制？
- **A**: 不同机制适用于不同场景：共享内存用于大数据传输，消息队列用于异步通信，信号量用于同步，管道用于父子进程通信。

- **Q**: 如何处理IPC资源泄漏？
- **A**: 使用`atexit()`注册清理函数，确保程序异常退出时也能清理资源。

- **Q**: 如何提高系统性能？
- **A**: 使用POSIX共享内存减少数据拷贝，使用信号量减少锁竞争，使用非阻塞I/O提高响应性。

#### 参考资料
- 《UNIX网络编程 第2卷：进程间通信》
- Linux man pages: pipe(2), shm_open(3), msgget(2), sem_open(3), socketpair(2), netlink(7)
- POSIX.1-2001标准文档