# 一、内存管理完全指南（Memory Management）

## 目录
- [一、基础概念与用法](#一基础概念与用法)
  - [1.1 虚拟内存](#11-虚拟内存)
  - [1.2 进程内存布局](#12-进程内存布局)
- [二、动态分配](#二动态分配)
  - [2.1 malloc/free](#21-mallocfree)
  - [2.2 calloc](#22-calloc)
  - [2.3 realloc](#23-realloc)
- [三、低级接口](#三低级接口)
  - [3.1 brk/sbrk](#31-brksbrk)
- [四、内存映射](#四内存映射)
  - [4.1 mmap/munmap](#41-mmapmunmap)
  - [4.2 mremap](#42-mremap)
  - [4.3 Huge Pages（大页）](#43-huge-pages大页)
- [五、虚拟内存诊断](#五虚拟内存诊断)
  - [5.1 /proc/<pid>/maps](#51-procpidmaps)
  - [5.2 /proc/meminfo](#52-procmeminfo)
- [六、内存屏障与可见性](#六内存屏障与可见性)
  - [6.1 __sync_synchronize](#61-__sync_synchronize)
  - [6.2 C11原子操作](#62-c11-原子操作)
- [七、综合实例](#七综合实例)
- [八、常见问题与排查](#八常见问题与排查)
- [九、参考资料](#九参考资料)

---

# 一、基础概念与用法

## 1.1 虚拟内存（Virtual Memory）
- **概念**：每个进程拥有独立的虚拟地址空间，通过页表（Page Table）映射到物理内存。
- **页表**：多级映射结构，实现虚拟页到物理页的转换。
- **TLB（Translation Lookaside Buffer）**：高速缓存页表条目，加速地址转换。

**总结**：虚拟内存为进程提供隔离和灵活的内存管理，是现代操作系统的基础。

---

## 1.2 进程内存布局（Process Memory Layout）

```
+------------------+
| Arguments / Env  |
+------------------+
| Command-line args|
+------------------+
| Stack Grow ↓     |
|                  |
+------------------+
| Uninitialized BSS|
+------------------+
| Initialized Data |
+------------------+
| Read-Only Data   |
+------------------+
| Text (Code)      |
+------------------+
| NULL             |
+------------------+
```

**总结**：理解进程内存布局有助于定位内存错误和优化程序结构。

---

# 二、动态分配

## 2.1 函数 `malloc` / `free`
#### 语法
```c
#include <stdlib.h>
void *malloc(size_t size);
void free(void *ptr);
```
#### 参数说明
- `size`：分配字节数
- `ptr`：要释放的内存指针
- 返回值：`malloc` 成功返回新内存指针，失败返回 `NULL`；`free` 无返回值

#### 使用场景
堆上动态分配和释放内存，适用于数组、结构体等动态数据结构。

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
int main() {
    int *arr = malloc(10 * sizeof(int));
    if (!arr) {
        perror("malloc failed");
        return 1;
    }
    for (int i = 0; i < 10; i++) arr[i] = i;
    for (int i = 0; i < 10; i++) printf("%d ", arr[i]);
    printf("\n");
    free(arr);
    return 0;
}
```
#### 总结
`malloc`/`free` 是 C 语言动态内存分配的基础，注意内存泄漏和越界。

---

## 2.2 函数 `calloc`
#### 语法
```c
#include <stdlib.h>
void *calloc(size_t nmemb, size_t size);
```
#### 参数说明
- `nmemb`：元素个数
- `size`：每个元素字节数
- 返回值：成功返回新内存指针，失败返回 `NULL`

#### 使用场景
分配并初始化为 0 的内存，适合需要清零的数组。

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
int main() {
    char *buf = calloc(5, sizeof(char));
    if (!buf) {
        perror("calloc failed");
        return 1;
    }
    for (int i = 0; i < 5; i++) printf("%d ", buf[i]);
    printf("\n");
    free(buf);
    return 0;
}
```
#### 总结
`calloc` 适合需要初始化为 0 的内存分配。

---

## 2.3 函数 `realloc`
#### 语法
```c
#include <stdlib.h>
void *realloc(void *ptr, size_t size);
```
#### 参数说明
- `ptr`：原内存块指针（可为 `NULL`）
- `size`：新大小（字节）
- 返回值：成功返回新内存指针，失败返回 `NULL`（原块不变）

#### 使用场景
调整已分配内存块的大小，适合动态扩容。

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
int main() {
    int *p = malloc(5 * sizeof(int));
    if (!p) {
        perror("malloc failed");
        return 1;
    }
    p = realloc(p, 10 * sizeof(int));
    if (!p) {
        perror("realloc failed");
        return 1;
    }
    for (int i = 0; i < 10; i++) p[i] = i;
    for (int i = 0; i < 10; i++) printf("%d ", p[i]);
    printf("\n");
    free(p);
    return 0;
}
```
#### 总结
`realloc` 可动态调整内存大小，注意失败时原指针未释放。

---

# 三、低级接口

## 3.1 函数 `brk` / `sbrk`
#### 语法
```c
#include <unistd.h>
int brk(void *addr);
void *sbrk(intptr_t increment);
```
#### 参数说明
- `addr`：新数据段末尾地址
- `increment`：增加的字节数
- 返回值：`brk` 成功返回 0，失败 -1；`sbrk` 成功返回旧断点，失败 `(void*)-1`

#### 使用场景
直接调整进程堆区，底层内存分配器实现。

#### 示例代码
```c
#include <stdio.h>
#include <unistd.h>
int main() {
    void *old = sbrk(0);
    printf("Old break: %p\n", old);
    if (sbrk(4096) == (void*)-1) perror("sbrk failed");
    printf("New break: %p\n", sbrk(0));
    return 0;
}
```
#### 总结
`brk`/`sbrk` 为历史接口，现代建议用 `mmap`。

---

# 四、内存映射

## 4.1 函数 `mmap` / `munmap`
#### 语法
```c
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
```
#### 参数说明
- `addr`：建议映射地址，`NULL` 由内核选择
- `length`：映射长度（字节，页对齐）
- `prot`：访问权限（`PROT_READ`、`PROT_WRITE` 等）
- `flags`：行为选项（`MAP_SHARED`、`MAP_PRIVATE` 等）
- `fd`：文件描述符（匿名映射填 `-1`）
- `offset`：文件偏移量（页对齐）
- 返回值：成功返回映射起始地址，失败返回 `MAP_FAILED`

#### 使用场景
文件映射、匿名大块内存分配、共享内存等。

#### 示例代码
```c
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    int fd = open("data.bin", O_RDWR);
    if (fd == -1) {
        perror("open failed");
        return 1;
    }
    void *p = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }
    // 使用内存...
    munmap(p, 4096);
    close(fd);
    return 0;
}
```
#### 总结
`mmap` 适合大块内存、文件映射、共享内存等高效场景。

---

## 4.2 函数 `mremap`
#### 语法
```c
#include <sys/mman.h>
void *mremap(void *old_address, size_t old_size, size_t new_size, int flags);
```
#### 参数说明
- `old_address`：旧映射地址
- `old_size`：原映射长度
- `new_size`：新映射长度
- `flags`：可选 `MREMAP_MAYMOVE` 等
- 返回值：成功返回新地址，失败返回 `MAP_FAILED`

#### 使用场景
调整 `mmap` 区域大小或地址。

#### 示例代码
```c
// 仅接口说明，实际用法需结合 mmap
```
#### 总结
`mremap` 用于动态调整映射区，常见于大数据处理。

---

## 4.3 Huge Pages（大页）
- **用途**：减少页表项、提升 TLB 命中率。
- **配置**：内核开启 `HugeTLB`，通过 `mmap` 加 `MAP_HUGETLB` 使用。

**总结**：大页适合高性能场景，但配置复杂，需内核支持。

---

# 五、虚拟内存诊断

## 5.1 /proc/<pid>/maps
#### 说明
- 查看进程映射区域：
```shell
cat /proc/$$/maps
```

**总结**：可用于分析进程内存布局和调试。

---

## 5.2 /proc/meminfo
#### 说明
- 系统内存使用情况：
```shell
cat /proc/meminfo
```

**总结**：用于系统级内存监控和分析。

---

# 六、内存屏障与可见性

## 6.1 `__sync_synchronize`
#### 语法
```c
__sync_synchronize();
```
#### 参数说明
- 无

#### 使用场景
GCC 原生全内存屏障，禁止编译器/CPU 重排序，常用于多线程同步。

#### 示例代码
```c
__sync_synchronize();  // 所有读写前完成，后续才能开始
```
#### 总结
`__sync_synchronize` 保证内存操作顺序，多线程同步常用。

---

## 6.2 C11 原子操作（`<stdatomic.h>`）
#### 语法
```c
#include <stdatomic.h>
atomic_int counter = 0;
atomic_store(&counter, v);
atomic_load(&counter);
atomic_fetch_add(&counter, 1);
atomic_compare_exchange(&counter, ...);
```
#### 参数说明
- `counter`：原子变量
- `v`：写入值

#### 使用场景
多线程下安全操作共享变量。

#### 示例代码
```c
#include <stdatomic.h>
#include <stdio.h>
int main() {
    atomic_int cnt = 0;
    atomic_fetch_add(&cnt, 1);
    printf("cnt = %d\n", atomic_load(&cnt));
    return 0;
}
```
#### 总结
C11 原子操作是现代多线程同步的推荐方式。

---

# 七、综合实例

## 实现目标
主进程产生日志消息，多进程/多线程消费并写入磁盘，追求低延迟、高并发。

## 涉及知识点
- 虚拟内存与进程内存布局
- 动态分配与扩容
- brk/sbrk、mmap/mremap
- Huge Pages
- /proc 诊断
- 内存屏障与原子操作

## 代码实现（`memlog.c`）
```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// 初始缓冲区（日志）大小 1MB
#define LOG_BUF_INIT   (1<<20)
// 扩容时倍增至最大 16MB
#define LOG_BUF_MAX    (16<<20)
// Huge Page 大小（假设为 2MB）
#define HUGEPAGE_SIZE  (2<<20)

static atomic_size_t write_idx = 0, read_idx = 0;

// 打印 /proc/self/maps 和 /proc/meminfo
void dump_proc_info() {
    printf("=== /proc/self/maps ===\n");
    system("sed -n '1,20p' /proc/self/maps");
    printf("    ...\n=== /proc/meminfo (%lld kB total) ===\n",
           (long long)sysconf(_SC_PHYS_PAGES)*sysconf(_SC_PAGESIZE)/1024);
    system("head -n5 /proc/meminfo");
}

// 演示 brk/sbrk 接口
void demo_sbrk() {
    printf("[sbrk] current program break: %p\n", sbrk(0));
    void *p = sbrk(4096);
    printf("[sbrk] increased by 4k, new break: %p\n", sbrk(0));
    // 回退
    sbrk(-4096);
}

// 日志缓冲区管理
void *log_buf = NULL;
size_t buf_size = 0;

// 分配或扩容缓冲区：先尝试用 Huge Pages 映射
void init_log_buf() {
    buf_size = LOG_BUF_INIT;
    // MAP_HUGETLB 可能需要 root & HugeTLBfs 挂载
    log_buf = mmap(NULL, buf_size, PROT_READ|PROT_WRITE,
                   MAP_ANONYMOUS|MAP_SHARED /*|MAP_HUGETLB*/, -1, 0);
    if (log_buf == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    printf("[mmap] log_buf at %p (%zu bytes)\n", log_buf, buf_size);
}

// 扩容：用 mremap 动态调整
void expand_log_buf() {
    if (buf_size >= LOG_BUF_MAX) return;
    size_t new_size = buf_size * 2;
    void *p = mremap(log_buf, buf_size, new_size, MREMAP_MAYMOVE);
    if (p == MAP_FAILED) {
        perror("mremap");
        return;
    }
    log_buf = p;
    printf("[mremap] expanded to %zu bytes at %p\n", new_size, log_buf);
    buf_size = new_size;
}

// 写日志：原子读写下标 + 内存屏障
void write_log(const char *msg) {
    size_t off = atomic_fetch_add(&write_idx, strlen(msg)+1);
    if (off + strlen(msg)+1 > buf_size) {
        expand_log_buf();
        off = atomic_fetch_add(&write_idx, strlen(msg)+1);
    }
    char *p = (char*)log_buf + off;
    // 确保拷贝前有序
    __sync_synchronize();
    strcpy(p, msg);
    // 发布写入
    atomic_thread_fence(memory_order_release);
}

// 模拟消费：读取日志并打印
void consume_logs() {
    size_t r = read_idx;
    size_t w = atomic_load(&write_idx);
    while (r < w) {
        char *p = (char*)log_buf + r;
        printf("[LOG] %s\n", p);
        r += strlen(p)+1;
    }
    read_idx = r;
}

int main() {
    printf("=== Memory Management Demo ===\n");
    dump_proc_info();
    demo_sbrk();

    init_log_buf();

    // 模拟多条日志写入
    write_log("First log entry");
    write_log("Second log entry");
    write_log("重要日志：应用启动完成");

    // 模拟消费
    consume_logs();

    // 再写更多日志，触发扩容
    for (int i = 0; i < 100000; i++) {
        write_log("Heartbeat...");
    }
    printf("[Main] final buffer size: %zu bytes, wrote %zu bytes\n",
           buf_size, atomic_load(&write_idx));

    // 再次打印 maps 及 meminfo
    dump_proc_info();

    // 清理
    munmap(log_buf, buf_size);
    return 0;
}
```

## 运行与调试
- 编译：
  ```bash
  gcc -std=gnu11 -O2 memlog.c -o memlog
  ./memlog
  ```

## 常见问题与解答
| 问题现象         | 可能原因               | 解决方案                             |
| ------------ | ------------------ | -------------------------------- |
| 内存泄漏         | `malloc` 后未 `free` | 使用工具如 `valgrind`                 |
| 段错误          | 访问越界 / 释放后继续使用     | 检查指针有效性                          |
| TLB 未命中      | 内存访问随机分布           | 大页（Huge Pages）                   |
| 映射失败         | 参数错误 / 权限不足 / 文件过大 | 检查 `errno`、调整映射长度                |
| 数据不同步（多线程场景） | 缺少内存屏障 / 原子操作      | 加入 `__sync_synchronize` 或 C11 原子 |

---

# 九、参考资料
- 《UNIX环境高级编程》
- Linux man pages：malloc, mmap, brk, sbrk, stdatomic
- Linux 内核文档
- valgrind 官方文档
