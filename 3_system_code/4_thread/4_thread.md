# 线程与多线程编程笔记 (Threading & Multithreading)

## 🧭 快速跳转目录

1. [POSIX 线程基础](#一posix-线程基础)
   - [1.1 概念介绍](#11-概念介绍)
   - [1.2 函数 `pthread_create`](#12-函数-pthread_create)
   - [1.3 函数 `pthread_join`](#13-函数-pthread_join)
   - [1.4 函数 `pthread_detach`](#14-函数-pthread_detach)
   - [1.5 线程属性管理](#15-线程属性管理)
   - [1.6 线程调度机制](#16-线程调度机制)
   - [1.7 函数 `pthread_exit`](#17-函数-pthread_exit)

2. [线程同步原语](#二线程同步原语)
   - [2.1 概念介绍](#21-概念介绍)
   - [2.2 互斥锁 `pthread_mutex_t`](#22-互斥锁-pthread_mutex_t)
   - [2.3 读写锁 `pthread_rwlock_t`](#23-读写锁-pthread_rwlock_t)
   - [2.4 条件变量 `pthread_cond_t`](#24-条件变量-pthread_cond_t)
   - [2.5 信号量 `sem_t`](#25-信号量-sem_t)

3. [线程局部存储（TLS）](#三线程局部存储tls)
   - [3.1 概念介绍](#31-概念介绍)
   - [3.2 函数 `pthread_key_create`](#32-函数-pthread_key_create)
   - [3.3 函数 `pthread_setspecific`/`pthread_getspecific`](#33-函数-pthread_setspecificpthread_getspecific)

4. [线程取消与清理](#四线程取消与清理)
   - [4.1 概念介绍](#41-概念介绍)
   - [4.2 函数 `pthread_cancel`](#42-函数-pthread_cancel)
   - [4.3 线程取消状态](#43-线程取消状态)
   - [4.4 线程取消点](#44-线程取消点)
   - [4.5 线程取消类型](#45-线程取消类型)
   - [4.6 函数 `pthread_cleanup_push`/`pthread_cleanup_pop`](#46-函数-pthread_cleanup_pushpthread_cleanup_pop)

5. [CPU 亲和性](#五cpu-亲和性)
   - [5.1 概念介绍](#51-概念介绍)
   - [5.2 函数 `pthread_setaffinity_np`/`pthread_getaffinity_np`](#52-函数-pthread_setaffinity_nppthread_getaffinity_np)

6. [线程池设计](#六线程池设计)
   - [6.1 概念介绍](#61-概念介绍)
   - [6.2 线程池结构设计](#62-线程池结构设计)
   - [6.3 线程池接口实现](#63-线程池接口实现)

7. [综合实例：日志聚合与异步写入服务](#七综合实例日志聚合与异步写入服务)

8. [综合实例：字符串拼接系统](#八综合实例字符串拼接系统)

---

## 一、POSIX 线程基础

### 1.1 概念介绍

POSIX 线程（Pthreads）是 POSIX 标准定义的线程接口，提供了创建、管理和同步线程的完整 API。线程是进程内的执行单元，共享进程的内存空间，但拥有独立的执行栈。

**线程的基本概念**：
- 每个进程都有自己的数据段、代码段和堆栈段，进程在创建、切换、撤销操作时需要较大的系统开销
- 为了减少系统开销，从进程中演化出了线程
- 线程存在于进程中，共享进程的资源
- 线程是进程中独立控制流（具有独立执行的一条路径），由环境（包括寄存器组和程序计数器）和一系列的执行指令组成

**核心概念**：
- **线程 ID**：每个线程有唯一的标识符
- **线程函数**：线程执行的入口函数
- **线程属性**：可配置的线程特性（栈大小、调度策略等）
- **线程状态**：可连接（joinable）或分离（detached）

#### 线程和进程的比较

**调度**：
- **线程**是CPU调度和分派的基本单位，通常不独立拥有系统资源（仅有必要的程序计数器、寄存器和栈），但可以访问其所属进程的资源
- **进程**是系统中程序执行和资源分配的基本单位，拥有独立的地址空间和资源，调度时需要加载和切换完整的CPU环境

**系统开销**：
- 线程属于同一进程，共享地址空间和资源，线程间同步和通信更高效，系统开销较小
- 进程切换需要保存当前进程的CPU状态并加载新进程的状态，还涉及内存管理，系统开销相对较大
- 线程切换只需保存和恢复少量的寄存器信息，不涉及内存地址空间的切换，因此更轻量

**并发性**：
- 进程之间可以并发执行，每个进程都是独立的任务
- 线程也可以并发执行，且多个线程可以在同一进程中共享数据和资源，更高效地实现并发任务

**总结**：
- 线程通常被称为**轻量级的进程**
- 一个进程可以创建多个线程，这些线程共享同一进程的资源
- 进程切换涉及用户空间的切换，开销较大
- 线程共享进程的地址空间，切换时无需切换这些资源，效率更高
- 线程和进程的调度机制类似，都是通过轮询机制实现来回切换

#### 多线程的应用场景

1. **多任务程序的设计**：将复杂任务分解为多个子任务并行执行
2. **并发程序设计**：提高程序的并发处理能力
3. **网络程序设计**：提高网络的利用效率，处理多个并发连接
4. **数据共享**：同一进程中的不同线程共享进程的数据空间，方便线程间数据共享
5. **多CPU系统**：在多个CPU系统中，可以实现真正的并行处理

### 1.2 函数 `pthread_create`

#### 语法
```c
#include <pthread.h>

int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg);
```

#### 参数说明
- `thread`：输出参数，返回新线程的 ID
- `attr`：线程属性对象，传 `NULL` 则使用默认属性
- `start_routine`：线程主函数，签名为 `void* func(void*)`
- `arg`：传递给线程主函数的参数，可为任意指针
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
`pthread_create` 用于创建新线程，是多线程编程的基础，常用于并发处理、异步操作等场景。

#### 示例代码
```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void* worker(void* arg) {
    printf("子线程正在运行中\n");
    return NULL;
}

int main(void) {
    printf("主线程正在执行中\n");
    
    pthread_t thread;
    if (pthread_create(&thread, NULL, worker, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    // 进程结束后，进程中所有的线程都会强制退出
    while(1);
    return 0;
}
```

#### 总结
`pthread_create` 是线程创建的核心函数，注意检查返回值和及时管理线程资源。与 `fork` 不同，`pthread_create` 创建的线程从指定的处理函数开始运行，该函数运行完后线程也就退出。线程依赖进程存在，如果创建线程的进程结束了，那么线程也就结束了。**链接时要加上参数 `-lpthread`**。

### 1.3 函数 `pthread_join`

#### 语法
```c
#include <pthread.h>

int pthread_join(pthread_t thread, void **retval);
```

#### 参数说明
- `thread`：要等待的线程 ID
- `retval`：输出参数，接收线程函数的返回值指针；可为 `NULL`
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
`pthread_join` 用于等待线程结束并获取返回值，确保线程资源正确回收。

#### 示例代码
```c
void* compute(void* arg) {
    int *res = malloc(sizeof(int));
    *res = 123;
    return res;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, compute, NULL);

    void *ret;
    pthread_join(tid, &ret);
    printf("Got result: %d\n", *(int*)ret);
    free(ret);
    return 0;
}
```

#### 总结
`pthread_join` 是线程同步的重要手段，注意处理返回值和内存管理。

### 1.4 函数 `pthread_detach`

#### 语法
```c
#include <pthread.h>

int pthread_detach(pthread_t thread);
```

#### 参数说明
- `thread`：要分离的线程 ID
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
`pthread_detach` 将线程设置为分离状态，线程结束后自动回收资源，适用于不需要获取返回值的场景。

#### 示例代码
```c
pthread_t tid;
pthread_create(&tid, NULL, worker, NULL);
pthread_detach(tid);
// 无需 pthread_join，线程结束时自动释放
```

#### 总结
分离线程简化了资源管理，但无法获取返回值，适用于后台任务。

### 1.5 线程属性管理

#### 语法
```c
#include <pthread.h>

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
```

#### 参数说明
- `attr`：线程属性对象
- `detachstate`：分离状态（`PTHREAD_CREATE_JOINABLE` 或 `PTHREAD_CREATE_DETACHED`）
- `stacksize`：栈大小（字节）
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
线程属性用于配置线程的栈大小、分离状态、调度策略等特性。

#### 示例代码
```c
pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
pthread_attr_setstacksize(&attr, 1024 * 1024);  // 1MB 栈

pthread_t tid;
pthread_create(&tid, &attr, worker, NULL);
pthread_attr_destroy(&attr);
```

#### 总结
合理配置线程属性可以优化性能和资源使用，注意及时销毁属性对象。

### 1.6 线程调度机制

#### 概念介绍
线程调度机制决定了多个线程如何分配CPU时间片。POSIX线程库使用抢占式调度，线程可以在任何时候被中断，让其他线程运行。

**调度特点**：
- 线程处理函数是并行执行的，是来回交替执行
- 普通函数是一定按照先后调用函数顺序执行
- 线程调度由操作系统内核负责，程序员无法直接控制

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* fun1(void* arg) {
    printf("子线程A正在运行中\n");
    sleep(1);
    printf("---------------A--------------\n");
    return NULL;
}

void* fun2(void* arg) {
    printf("子线程B正在运行中\n");
    sleep(1);
    printf("---------------B--------------\n");
    return NULL;
}

int main(void) {
    printf("主线程main正在运行中\n");
    
    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, fun1, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    if (pthread_create(&thread2, NULL, fun2, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    while(1);
    return 0;
}
```

#### 线程处理函数传参
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int n = 200;  // 全局变量，线程间共享

void* fun1(void* arg) {
    printf("子线程A n=%d\n", n);
    n++;
    int k = *(int*)arg;
    printf("子线程A K=%d\n", k);
    *(int*)arg = 555;
    return NULL;
}

void* fun2(void* arg) {
    sleep(1);
    printf("子线程B n=%d\n", n);
    int k = *(int*)arg;
    printf("子线程B K=%d\n", k);
    return NULL;
}

int main(void) {
    printf("主线程main正在运行中\n");
    int k = 100;
    
    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, fun1, (void*)&k) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    if (pthread_create(&thread2, NULL, fun2, (void*)&k) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    while(1);
    return 0;
}
```

#### 总结
线程调度是操作系统自动管理的，程序员可以通过合理的程序设计来影响线程的执行顺序，但不能直接控制调度。

### 1.7 函数 `pthread_exit`

#### 语法
```c
#include <pthread.h>

void pthread_exit(void *retval);
```

#### 参数说明
- `retval`：当前线程的退出状态值，可以被调用 `pthread_join` 函数的线程接收到
- **返回值**：无

#### 使用场景
`pthread_exit` 用于退出正在执行的线程，可以在线程函数的任意位置调用。

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* fun1(void* arg) {
    static char buffer[] = "this is a book";
    printf("子线程正在运行中...\n");
    
    for (int k = 0; k < 10; k++) {
        if (k == 5) {
            // 退出当前线程
            pthread_exit(buffer);
        }
        printf("==========================\n");
        sleep(1);
    }
    return NULL;
}

int main(void) {
    printf("主线程正在运行中...\n");
    
    pthread_t thread;
    if (pthread_create(&thread, NULL, fun1, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    char *str;
    pthread_join(thread, (void**)&str);
    printf("str====%s\n", str);
    printf("进程要退出了....\n");
    return 0;
}
```

#### 总结
`pthread_exit` 允许线程在任意位置退出，并可以传递退出状态。一个进程中的多个线程共享该进程的数据段，因此线程退出后所占用的资源并不会自动释放，需要结合态通过 `pthread_join` 函数或分离态自动释放。

---

## 二、线程同步原语

### 2.1 概念介绍

线程同步原语用于协调多个线程对共享资源的访问，防止数据竞争和不一致。主要包括互斥锁、读写锁、条件变量等。

**同步问题**：
- **竞态条件**：多个线程同时访问共享数据
- **死锁**：线程相互等待对方释放资源
- **活锁**：线程不断重试但无法取得进展

### 2.2 互斥锁 `pthread_mutex_t`

#### 语法
```c
#include <pthread.h>

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
```

#### 参数说明
- `mutex`：互斥锁对象
- `attr`：锁属性，`NULL` 表示默认属性
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
互斥锁提供对共享资源的互斥访问，是最基本的同步原语。

#### 示例代码
```c
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int shared_counter = 0;

void* increment(void* arg) {
    for (int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&lock);
        shared_counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Final counter: %d\n", shared_counter);
    
    pthread_mutex_destroy(&lock);
    return 0;
}
```

#### 总结
互斥锁简单有效，但要注意避免死锁和保持锁的粒度适中。

### 2.3 读写锁 `pthread_rwlock_t`

#### 语法
```c
#include <pthread.h>

int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

#### 参数说明
- `rwlock`：读写锁对象
- `attr`：锁属性，`NULL` 表示默认属性
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
读写锁支持多读单写，适用于读多写少的场景，提高并发性能。

#### 示例代码
```c
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int data = 0;

void* reader(void* arg) {
    pthread_rwlock_rdlock(&rwlock);
    printf("Reader: data = %d\n", data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* writer(void* arg) {
    pthread_rwlock_wrlock(&rwlock);
    data++;
    printf("Writer: data = %d\n", data);
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}
```

#### 总结
读写锁在读多写少场景下性能优于互斥锁，但实现复杂度更高。

### 2.4 条件变量 `pthread_cond_t`

#### 语法
```c
#include <pthread.h>

int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
```

#### 参数说明
- `cond`：条件变量对象
- `mutex`：关联的互斥锁
- `attr`：条件变量属性，`NULL` 表示默认属性
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
条件变量用于线程间的等待-通知机制，常与互斥锁配合使用。

#### 示例代码
```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int ready = 0;

void* producer(void* arg) {
    pthread_mutex_lock(&mutex);
    ready = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* consumer(void* arg) {
    pthread_mutex_lock(&mutex);
    while (!ready) {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("Data is ready!\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}
```

#### 总结
条件变量解决了互斥锁无法实现的等待机制，是复杂同步的基础。

### 2.5 信号量 `sem_t`

#### 语法
```c
#include <semaphore.h>

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_getvalue(sem_t *sem, int *sval);
```

#### 参数说明
- `sem`：信号量对象
- `pshared`：是否在线程间或者进程间共享
  - `0`：线程间共享
  - `1`：进程间共享
- `value`：信号量的初始值
- `sval`：保存信号量值的地址
- **返回值**：成功返回 0，失败返回 -1

#### 使用场景
信号量广泛用于进程或线程间的同步和互斥。信号量本质上是一个非负的整数计数器，它被用来控制对公共资源的访问。在编程中可根据操作信号量的结果判断是否对公共资源具有访问的权限，当信号量值大于0时，可以访问，否则将阻塞。

信号量又称为PV操作，PV原语是对信号量的操作：
- 一次P操作使信号量sem减1
- 一次V操作使信号量sem加1
- 对于P操作，如果信号量的sem值为小于等于0，则P操作会阻塞，如果信号量的值大于0，才可以执行P操作进行减1

**应用场景**：
1. **互斥**：几个进程或线程往往只设置一个信号量
2. **同步**：往往会设置多个信号量，并且安排不同的初始值，来实现它们之间的执行顺序

#### 示例代码

**示例1**：信号量实现互斥
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// 创建一个信号量
sem_t sem;

void print(char *str) {
    // 执行P操作
    // 由于信号量实现互斥，信号量的初始值为1，则两个线程执行P操作，
    // 先执行P操作的线程继续执行，后执行P操作先阻塞等待
    sem_wait(&sem);
    
    while(*str) {
        putchar(*str);
        fflush(stdout);
        str++;
        sleep(2);
    }
    
    // 实现V操作
    sem_post(&sem);
}

void* fun1(void* arg) {
    char *p1 = "hello";
    print(p1);
    return NULL;
}

void* fun2(void* arg) {
    char *p2 = "world";
    print(p2);
    return NULL;
}

int main(void) {
    // 初始化信号量
    sem_init(&sem, 0, 1);
    
    pthread_t t1, t2;
    if (pthread_create(&t1, NULL, fun1, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    if (pthread_create(&t2, NULL, fun2, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("\n");
    sem_destroy(&sem);
    return 0;
}
```

**示例2**：信号量实现同步
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

// 创建两个信号量
sem_t sem_p, sem_v;
char c = 'a';

void* fun1(void* arg) {
    while(1) {
        sem_wait(&sem_v);
        c++;
        sleep(1);
        sem_post(&sem_p);
    }
    return NULL;
}

void* fun2(void* arg) {
    while(1) {
        sem_wait(&sem_p);
        printf("%c", c);
        fflush(stdout);
        sleep(1);
        sem_post(&sem_v);
    }
    return NULL;
}

int main(void) {
    // 初始化信号量
    sem_init(&sem_v, 0, 0);
    sem_init(&sem_p, 0, 1);
    
    pthread_t t1, t2;
    if (pthread_create(&t1, NULL, fun1, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    if (pthread_create(&t2, NULL, fun2, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("\n");
    sem_destroy(&sem_v);
    sem_destroy(&sem_p);
    return 0;
}
```

#### 总结
信号量是强大的同步原语，既可以用于互斥控制，也可以用于线程间的同步协调。合理使用信号量可以避免竞态条件，确保程序的正确性。

---

## 三、线程局部存储（TLS）

### 3.1 概念介绍

线程局部存储（Thread Local Storage, TLS）允许每个线程拥有变量的独立副本，避免全局变量的竞争问题。每个线程对 TLS 变量的修改只影响自己的副本。

**应用场景**：
- 线程特定的错误码
- 线程本地缓存
- 递归函数的上下文信息

### 3.2 函数 `pthread_key_create`

#### 语法
```c
#include <pthread.h>

int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
```

#### 参数说明
- `key`：输出参数，返回 TLS key
- `destructor`：当线程退出且关联值非 NULL 时调用的清理函数
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
创建 TLS key 是使用线程局部存储的第一步，需要为每个 TLS 变量创建对应的 key。

#### 示例代码
```c
pthread_key_t tls_key;

void cleanup_tls(void* value) {
    if (value) {
        free(value);
    }
}

int main(void) {
    pthread_key_create(&tls_key, cleanup_tls);
    // ... 使用 TLS ...
    pthread_key_delete(tls_key);
    return 0;
}
```

#### 总结
TLS key 创建后需要及时删除，清理函数确保资源正确释放。

### 3.3 函数 `pthread_setspecific`/`pthread_getspecific`

#### 语法
```c
#include <pthread.h>

int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
```

#### 参数说明
- `key`：TLS key
- `value`：要设置的值
- **返回值**：`pthread_setspecific` 成功返回 0，失败返回错误码；`pthread_getspecific` 返回当前线程的值

#### 使用场景
设置和获取线程局部存储的值，每个线程可以独立操作自己的副本。

#### 示例代码
```c
pthread_key_t buffer_key;

void* worker(void* arg) {
    // 为当前线程分配缓冲区
    char* buffer = malloc(1024);
    pthread_setspecific(buffer_key, buffer);
    
    // 使用缓冲区
    char* my_buffer = pthread_getspecific(buffer_key);
    sprintf(my_buffer, "Thread %ld", (long)pthread_self());
    printf("%s\n", my_buffer);
    
    // 线程退出时自动调用 cleanup_tls 释放 buffer
    return NULL;
}
```

#### 总结
TLS 提供了线程安全的全局变量替代方案，但要注意内存管理。

---

## 四、线程取消与清理

### 4.1 概念介绍

线程取消允许一个线程请求终止另一个线程的执行。为了确保资源正确释放，需要注册清理函数在取消时执行。

**取消类型**：
- **延迟取消**：在取消点检查取消请求
- **异步取消**：立即响应取消请求（危险）

**线程的取消状态**：线程能不能被取消
**线程取消点**：即线程被取消的地方
**线程的取消类型**：在线程能被取消的状态下，是立马被取消结束，还是执行到取消点的时候被取消结束

### 4.2 函数 `pthread_cancel`

#### 语法
```c
#include <pthread.h>

int pthread_cancel(pthread_t thread);
```

#### 参数说明
- `thread`：要取消的线程 ID
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
请求取消指定线程，实际取消发生在该线程的取消点。`pthread_cancel` 函数的实质是发信号给目标线程，使目标线程退出。此函数只是发送终止信号给目标线程，不会等待取消目标线程执行完才返回。然而发送成功并不意味着目标线程一定就会终止，线程被取消时，线程的取消属性会决定线程能否被取消以及何时被取消。

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* fun1(void* arg) {
    while(1) {
        printf("子线程正在运行中....\n");
        sleep(1);
    }
    return NULL;
}

int main(void) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, fun1, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    sleep(5);
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    printf("进程即将结束\n");
    return 0;
}
```

#### 总结
线程取消需要谨慎使用，确保在取消点检查请求并正确清理资源。

### 4.3 线程取消状态

#### 语法
```c
#include <pthread.h>

int pthread_setcancelstate(int state, int *oldstate);
```

#### 参数说明
- `state`：新的状态
  - `PTHREAD_CANCEL_DISABLE`：不可以被取消
  - `PTHREAD_CANCEL_ENABLE`：可以被取消（默认）
- `oldstate`：保存调用线程原来的可取消状态的内存地址
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
在Linux系统中，线程默认可以被取消，编程时可以通过 `pthread_setcancelstate` 函数设置线程是否可以被取消。

### 4.4 线程取消点

#### 语法
```c
#include <pthread.h>

void pthread_testcancel(void);
```

#### 参数说明
- **返回值**：无

#### 使用场景
线程被取消后，该线程并不是马上终止，默认情况下线程执行到取消点时才被终止，编程时可以通过 `pthread_testcancel` 函数设置线程的取消点。

### 4.5 线程取消类型

#### 语法
```c
#include <pthread.h>

int pthread_setcanceltype(int type, int *oldtype);
```

#### 参数说明
- `type`：取消类型
  - `PTHREAD_CANCEL_ASYNCHRONOUS`：立即取消
  - `PTHREAD_CANCEL_DEFERRED`：不立即取消（默认）
- `oldtype`：保存调用线程原来的可以取消类型的内存地址
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
线程被取消后，该线程并不是马上终止，默认情况线程执行到取消点时才被终止，编程时可以通过 `pthread_setcanceltype` 函数设置线程是否可以立即被取消。

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* func1(void* arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);  // 设置可以取消（默认可以取消）
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); // 不立即取消（默认不立即取消）
    
    printf("子线程进行中1.\n");
    sleep(1);
    printf("子线程进行中2.\n");
    sleep(1);
    printf("子线程进行中3.\n");
    sleep(1);
    printf("子线程进行中4.\n");
    sleep(1);
    pthread_testcancel(); // 检查取消请求
    return NULL;
}

int main(void) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, func1, NULL) != 0) {
        perror("fail to create thread.");
        exit(1);
    }
    
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    printf("进程结束.\n");
    return 0;
}
```

**注意**：输出结果可能与预想的不一致。这是因为在POSIX线程库中，像 `sleep()` 这样的函数内部通常会包含隐式的取消点。这意味着当一个线程在 `sleep()` 调用中等待时，它是可能被取消的。取消点包括但不限于 `sleep()`、`pthread_cond_wait()`、`sem_wait()` 等阻塞调用，也包括显式调用的 `pthread_testcancel()`。

### 4.6 函数 `pthread_cleanup_push`/`pthread_cleanup_pop`

#### 语法
```c
#include <pthread.h>

void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);
```

#### 参数说明
- `routine`：清理函数，签名为 `void func(void*)`
- `arg`：传给清理函数的参数
- `execute`：0 表示弹出清理函数但不执行，非 0 表示执行并弹出

#### 使用场景
注册清理函数，在线程被取消或退出时自动执行，确保资源正确释放。线程可以建立多个清理处理程序，处理程序在栈中，所有它们执行顺序与它们注册时顺序相反。

**注意**：
- 由于这两个函数是宏，因此两个函数必须成对出现并且出现在同一个代码块中，不能跨越函数或者代码块
- `pthread_cleanup_push` 注册的清理函数将在以下情况之一发生时被调用：
  - 线程被取消（例如通过 `pthread_cancel`）
  - 线程正常退出（例如调用 `pthread_exit` 或者函数返回）
  - 显式调用 `pthread_cleanup_pop` 时，如果参数为 1

#### 示例代码

**示例1**：调用 `pthread_exit` 退出
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void myclean(void* arg) {
    printf("myclean ptr=%s\n", (char*)arg);
    free((char*)arg);
}

void* fun1(void* arg) {
    printf("子线程正在运行中....\n");
    char *ptr = NULL;
    ptr = (char*)malloc(100);
    
    pthread_cleanup_push(myclean, (void*)(ptr));
    memset(ptr, 0, 100);
    strcpy(ptr, "this is a book");
    printf("注册清理函数!\n");
    sleep(3);
    pthread_exit(NULL); // 此时退出，其后面的代码不会再起作用，但是由于是正常退出，cleanup函数仍然会调用
    printf("删除清理函数\n");
    pthread_cleanup_pop(1);
    return NULL;
}

int main(void) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, fun1, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    pthread_join(thread, NULL);
    printf("进程即将要结束....\n");
    return 0;
}
```

**示例2**：调用 `pthread_cancel` 退出
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void myclean(void* arg) {
    printf("myclean ptr=%s\n", (char*)arg);
    free((char*)arg);
}

void* fun1(void* arg) {
    printf("子线程正在运行中....\n");
    char *ptr = NULL;
    ptr = (char*)malloc(100);
    
    pthread_cleanup_push(myclean, (void*)(ptr));
    memset(ptr, 0, 100);
    strcpy(ptr, "this is a book");
    printf("注册清理函数!\n");
    sleep(3);
    printf("删除清理函数\n");
    pthread_cleanup_pop(1);
    return NULL;
}

int main(void) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, fun1, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    // 取消线程
    pthread_cancel(thread);
    pthread_join(thread, NULL);
    printf("进程即将要结束....\n");
    return 0;
}
```

**示例3**：正常退出，多个清理函数查看执行顺序
```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

void fun1(void* arg) {
    printf("fun1...\n");
    printf("myclean ptr=%s\n", (char*)arg);
    free((char*)arg);
}

void fun2(void* arg) {
    printf("fun2...\n");
}

void* fun(void* arg) {
    printf("子线程正在运行中....\n");
    char *ptr = NULL;
    ptr = (char*)malloc(100);
    
    pthread_cleanup_push(fun1, (void*)(ptr));
    pthread_cleanup_push(fun2, NULL);
    memset(ptr, 0, 100);
    strcpy(ptr, "this is a book");
    printf("注册清理函数!\n");
    sleep(3);
    printf("删除清理函数\n");
    printf("clearup1...\n");
    pthread_cleanup_pop(1);
    printf("clearup2...\n");
    pthread_cleanup_pop(1);
    return NULL;
}

int main(void) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, fun, NULL) != 0) {
        perror("fail to pthread_create");
        exit(1);
    }
    
    pthread_join(thread, NULL);
    printf("进程即将要结束....\n");
    return 0;
}
```

#### 总结
清理函数是线程取消安全的重要保障，注意 push/pop 必须成对使用。

---

## 五、CPU 亲和性

### 5.1 概念介绍

CPU 亲和性允许将线程绑定到特定的 CPU 核心上，减少线程迁移开销，提高缓存命中率。但过度使用可能导致负载不均衡。

**应用场景**：
- 实时系统
- 高性能计算
- 缓存敏感的应用

### 5.2 函数 `pthread_setaffinity_np`/`pthread_getaffinity_np`

> **注意**：属于 GNU 扩展，非标准 POSIX

#### 语法
```c
#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>

int pthread_setaffinity_np(pthread_t thread,
                           size_t cpusetsize,
                           const cpu_set_t *cpuset);
int pthread_getaffinity_np(pthread_t thread,
                           size_t cpusetsize,
                           cpu_set_t *cpuset);
```

#### 参数说明
- `thread`：目标线程 ID
- `cpusetsize`：`cpu_set_t` 大小，一般为 `sizeof(cpu_set_t)`
- `cpuset`：指定或返回的 CPU 集合
- **返回值**：成功返回 0，失败返回错误码

#### 使用场景
设置和获取线程的 CPU 亲和性，优化特定应用的性能。

#### 示例代码
```c
#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>

void* cpu_bound_task(void* arg) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  // 绑定到 CPU0
    
    pthread_t tid = pthread_self();
    pthread_setaffinity_np(tid, sizeof(cpuset), &cpuset);
    
    // 执行 CPU 密集型任务
    for (long i = 0; i < 1000000000; i++) {
        // 计算密集型操作
    }
    
    return NULL;
}
```

#### 总结
CPU 亲和性可以提升性能，但需要谨慎使用，避免影响系统调度。

---

## 六、线程池设计

### 6.1 概念介绍

线程池是一种预先创建多个线程的技术，用于处理大量短期任务，避免频繁创建和销毁线程的开销。线程池包含任务队列和工作线程集合。

**优势**：
- 减少线程创建/销毁开销
- 控制并发线程数量
- 提供任务排队机制

### 6.2 线程池结构设计

#### 数据结构
```c
typedef struct {
    void (*function)(void *);  // 任务函数
    void *arg;                 // 任务参数
} ThreadTask;

typedef struct {
    pthread_t *threads;            // 工作线程数组
    int thread_count;              // 线程池大小

    ThreadTask *task_queue;        // 环形队列存储任务
    int queue_capacity;            // 队列容量
    int queue_head, queue_tail;    // 队头、队尾索引
    int queue_size;                // 队列当前任务数

    pthread_mutex_t queue_lock;    // 保护队列的互斥锁
    pthread_cond_t  queue_not_empty; // 队列非空条件变量
    pthread_cond_t  queue_not_full;  // 队列非满条件变量

    bool shutdown;                 // 线程池关闭标志
} ThreadPool;
```

### 6.3 线程池接口实现

#### 创建线程池
```c
ThreadPool* threadpool_create(int num_threads, int queue_capacity) {
    ThreadPool *pool = malloc(sizeof(ThreadPool));
    if (!pool) return NULL;
    
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    pool->task_queue = malloc(queue_capacity * sizeof(ThreadTask));
    if (!pool->threads || !pool->task_queue) {
        free(pool->threads);
        free(pool->task_queue);
        free(pool);
        return NULL;
    }
    
    pool->thread_count = num_threads;
    pool->queue_capacity = queue_capacity;
    pool->queue_head = pool->queue_tail = pool->queue_size = 0;
    pool->shutdown = false;
    
    pthread_mutex_init(&pool->queue_lock, NULL);
    pthread_cond_init(&pool->queue_not_empty, NULL);
    pthread_cond_init(&pool->queue_not_full, NULL);
    
    // 创建工作线程
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, worker, pool);
    }
    
    return pool;
}
```

#### 工作线程函数
```c
void* worker(void *arg) {
    ThreadPool *pool = (ThreadPool*)arg;
    while (1) {
        pthread_mutex_lock(&pool->queue_lock);
        
        while (pool->queue_size == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->queue_not_empty, &pool->queue_lock);
        }
        
        if (pool->shutdown && pool->queue_size == 0) {
            pthread_mutex_unlock(&pool->queue_lock);
            break;
        }
        
        // 取任务
        ThreadTask task = pool->task_queue[pool->queue_head];
        pool->queue_head = (pool->queue_head + 1) % pool->queue_capacity;
        pool->queue_size--;
        
        pthread_cond_signal(&pool->queue_not_full);
        pthread_mutex_unlock(&pool->queue_lock);
        
        // 执行任务
        task.function(task.arg);
    }
    return NULL;
}
```

#### 提交任务
```c
int threadpool_submit(ThreadPool *pool, void (*function)(void*), void *arg) {
    pthread_mutex_lock(&pool->queue_lock);
    
    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->queue_lock);
        return -1;
    }
    
    while (pool->queue_size == pool->queue_capacity) {
        pthread_cond_wait(&pool->queue_not_full, &pool->queue_lock);
    }
    
    // 入队
    pool->task_queue[pool->queue_tail] = (ThreadTask){ .function=function, .arg=arg };
    pool->queue_tail = (pool->queue_tail + 1) % pool->queue_capacity;
    pool->queue_size++;
    
    pthread_cond_signal(&pool->queue_not_empty);
    pthread_mutex_unlock(&pool->queue_lock);
    return 0;
}
```

#### 关闭线程池
```c
void threadpool_shutdown(ThreadPool *pool) {
    pthread_mutex_lock(&pool->queue_lock);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->queue_not_empty);
    pthread_mutex_unlock(&pool->queue_lock);
    
    // 等待所有线程退出
    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    
    // 销毁同步原语
    pthread_mutex_destroy(&pool->queue_lock);
    pthread_cond_destroy(&pool->queue_not_empty);
    pthread_cond_destroy(&pool->queue_not_full);
    
    // 释放内存
    free(pool->threads);
    free(pool->task_queue);
    free(pool);
}
```

---

## 七、综合实例：日志聚合与异步写入服务

### 实现目标
构建一个多线程日志服务，支持多个业务线程异步提交日志，通过线程池批量写入磁盘，避免日志写入阻塞业务逻辑。

### 涉及知识点
- 线程池设计与实现
- 条件变量与互斥锁
- 批量 I/O 操作
- 资源管理与清理
- 优雅关闭机制

### 代码实现

#### 日志条目结构
```c
typedef struct {
    char *message;           // 日志内容
    size_t length;           // 日志长度
} LogEntry;
```

#### 日志工作线程
```c
#define BATCH_SIZE     32
#define FLUSH_INTERVAL 1 // 秒

void* log_worker(void *arg) {
    ThreadPool *pool = (ThreadPool*)arg;
    LogEntry batch[BATCH_SIZE];
    int count = 0;
    time_t last_flush = time(NULL);

    while (1) {
        pthread_mutex_lock(&pool->queue_lock);

        // 等待新日志或超时
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += FLUSH_INTERVAL;
        
        while (pool->queue_size == 0 && !pool->shutdown) {
            pthread_cond_timedwait(&pool->queue_not_empty, &pool->queue_lock, &ts);
        }

        // 批量取出日志
        while (pool->queue_size > 0 && count < BATCH_SIZE) {
            batch[count++] = pool->task_queue[pool->queue_head];
            pool->queue_head = (pool->queue_head + 1) % pool->queue_capacity;
            pool->queue_size--;
        }

        pthread_cond_signal(&pool->queue_not_full);
        pthread_mutex_unlock(&pool->queue_lock);

        // 批量写入日志
        if (count > 0) {
            FILE *fp = fopen("app.log", "a");
            if (fp) {
                for (int i = 0; i < count; i++) {
                    fwrite(batch[i].message, 1, batch[i].length, fp);
                    fwrite("\n", 1, 1, fp);
                    free(batch[i].message);
                }
                fflush(fp);
                fclose(fp);
                count = 0;
                last_flush = time(NULL);
            }
        }

        // 退出条件
        if (pool->shutdown && pool->queue_size == 0) {
            break;
        }
    }
    return NULL;
}
```

#### 提交日志接口
```c
int submit_log(ThreadPool *pool, const char *msg) {
    size_t len = strlen(msg);
    char *copy = malloc(len + 1);
    if (!copy) return -1;
    
    memcpy(copy, msg, len + 1);
    LogEntry entry = { .message = copy, .length = len };
    
    return threadpool_submit(pool, (void (*)(void *))log_worker, entry);
}
```

#### 主程序
```c
int main(void) {
    // 创建日志线程池
    ThreadPool *logPool = threadpool_create(2, 1024);
    if (!logPool) {
        fprintf(stderr, "Failed to create log pool\n");
        return 1;
    }

    // 模拟业务线程产生日志
    for (int i = 0; i < 10000; i++) {
        char buf[128];
        snprintf(buf, sizeof(buf), "[%d] RequestID=%d, status=OK", 
                 (int)time(NULL), i);
        submit_log(logPool, buf);
        
        if (i % 1000 == 0) {
            printf("Submitted %d logs\n", i);
        }
    }

    // 等待日志写入
    sleep(3);

    // 优雅关闭
    threadpool_shutdown(logPool);
    printf("Log service shutdown complete\n");
    return 0;
}
```

### 运行与调试
```bash
# 编译
gcc -o log_service log_service.c -lpthread

# 运行
./log_service

# 查看日志文件
tail -f app.log
```

### 常见问题与解答
- **Q: 为什么使用批量写入？**
  A: 减少系统调用次数，提高 I/O 效率，降低磁盘碎片。

- **Q: 如何保证日志不丢失？**
  A: 使用条件变量确保线程池关闭前所有日志都被处理。

- **Q: 定时刷盘的作用是什么？**
  A: 防止日志在队列中堆积过久，确保日志及时写入磁盘。

### 参考资料
- POSIX Threads Programming: https://computing.llnl.gov/tutorials/pthreads/
- Linux 线程编程指南
- 《UNIX 环境高级编程》第 11 章

---

## 八、综合实例：字符串拼接系统

### 实现目标
构建一个多线程字符串拼接系统，使用互斥锁和信号量两种不同的同步方式实现字符串的有序拼接，展示不同同步原语的使用方法和特点。

### 涉及知识点
- 线程创建与管理
- 互斥锁同步机制
- 信号量同步机制
- 线程间数据共享
- 资源管理与清理

### 代码实现

#### 使用互斥锁实现字符串拼接
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

static int currentOrder = 1;
char string[1024];
const unsigned int threadNum = 3;
#define STRLEN 30

typedef struct threadInputPara {
    unsigned int order;
    char str[STRLEN];
    bool finishFlag;
} tpara;

pthread_mutex_t mutex;

void* spliceStr(void* arg) {
    tpara *para = (tpara*)arg;
    
    while (1) {
        pthread_mutex_lock(&mutex);
        if (currentOrder == para->order) {
            if (para->order == 1) {
                strcpy(string, para->str);  // 第一个线程直接拷贝
            } else {
                strcat(string, para->str);  // 之后的线程拼接
            }
            currentOrder++;  // 更新顺序
            pthread_mutex_unlock(&mutex);
            break;  // 跳出循环，完成拼接
        }
        pthread_mutex_unlock(&mutex);
        usleep(100);  // 避免忙等，稍作休眠
    }
    para->finishFlag = true;
    return NULL;
}

int main(void) {
    pthread_mutex_init(&mutex, NULL);
    tpara para[threadNum];
    
    printf("Please input %d strings to splice (mutex version):\n", threadNum);
    int readByte;
    for (int i = 0; i < threadNum; i++) {
        readByte = read(0, (para + i)->str, STRLEN);
        para[i].str[readByte-1] = '\0'; // 将最后一个\n改为\0
        para[i].order = i + 1;
    }
    
    pthread_t thread[threadNum];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); // 设置为分离态
    
    for (int i = 0; i < threadNum; i++) {
        if (pthread_create(thread + i, &attr, spliceStr, (void*)(para + i)) != 0) {
            perror("fail to create thread.");
            exit(1);
        }
    }
    
    pthread_attr_destroy(&attr);
    
    do {
        printf("正在拼接中....\n");
        sleep(1);
    } while (!(para[0].finishFlag) || !(para[1].finishFlag) || !(para[2].finishFlag));
    
    printf("拼接结果: %s\n", string);
    pthread_mutex_destroy(&mutex);
    return 0;
}
```

#### 使用信号量实现字符串拼接
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define STRLEN 30
#define THREADNUM 3 // 线程数量
char string[1024];  // 拼接的最终字符串

typedef struct threadInputPara {
    unsigned int order; // 线程的顺序
    char str[STRLEN];   // 该线程要拼接的字符串
} tpara;

sem_t semaphores[THREADNUM]; // 信号量数组

void* spliceStr(void* arg) {
    tpara *para = (tpara*)arg;

    // 等待当前线程的信号量
    sem_wait(&semaphores[para->order - 1]);

    // 拼接字符串
    if (para->order == 1) {
        strcpy(string, para->str); // 第一个线程直接拷贝
    } else {
        strcat(string, para->str); // 其他线程拼接
    }

    // 唤醒下一个线程
    if (para->order < THREADNUM) {
        sem_post(&semaphores[para->order]);
    }

    return NULL;
}

int main(void) {
    tpara para[THREADNUM];
    pthread_t threads[THREADNUM];

    // 初始化信号量，第一个信号量初始值为1，其他为0
    for (int i = 0; i < THREADNUM; i++) {
        sem_init(&semaphores[i], 0, (i == 0) ? 1 : 0);
    }

    // 输入字符串
    printf("Please input %d strings to splice (semaphore version):\n", THREADNUM);
    for (int i = 0; i < THREADNUM; i++) {
        fgets(para[i].str, STRLEN, stdin);
        para[i].str[strcspn(para[i].str, "\n")] = '\0'; // 去掉换行符
        para[i].order = i + 1;
    }

    // 创建线程
    for (int i = 0; i < THREADNUM; i++) {
        if (pthread_create(&threads[i], NULL, spliceStr, (void*)&para[i]) != 0) {
            perror("fail to create thread");
            exit(1);
        }
    }

    // 等待所有线程完成
    for (int i = 0; i < THREADNUM; i++) {
        pthread_join(threads[i], NULL);
    }

    // 打印拼接结果
    printf("拼接结果: %s\n", string);

    // 销毁信号量
    for (int i = 0; i < THREADNUM; i++) {
        sem_destroy(&semaphores[i]);
    }

    return 0;
}
```

### 运行与调试
```bash
# 编译互斥锁版本
gcc -o string_splice_mutex string_splice_mutex.c -lpthread

# 编译信号量版本
gcc -o string_splice_sem string_splice_sem.c -lpthread

# 运行互斥锁版本
./string_splice_mutex

# 运行信号量版本
./string_splice_sem
```

### 两种实现方式的比较

**互斥锁方式**：
- 使用忙等待机制，线程需要不断检查是否可以执行
- 实现相对简单，但效率较低
- 适合简单的同步需求

**信号量方式**：
- 使用阻塞等待机制，线程在等待时不会消耗CPU
- 实现更加高效，线程按顺序执行
- 适合复杂的同步需求

### 常见问题与解答
- **Q: 为什么需要同步机制？**
  A: 多个线程同时访问共享资源可能导致数据竞争，同步机制确保资源访问的有序性和一致性。

- **Q: 互斥锁和信号量有什么区别？**
  A: 互斥锁主要用于互斥访问，信号量既可以用于互斥也可以用于同步，功能更强大。

- **Q: 如何选择合适的同步原语？**
  A: 简单互斥用互斥锁，复杂同步用信号量，需要等待通知机制用条件变量。

### 参考资料
- POSIX Threads Programming: https://computing.llnl.gov/tutorials/pthreads/
- Linux 线程编程指南
- 《UNIX 环境高级编程》第 11 章

