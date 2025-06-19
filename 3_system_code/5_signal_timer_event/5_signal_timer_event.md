# 一、信号、定时器与事件处理

## 目录
- [一、信号机制](#一信号机制)
  - [1.1 信号概念与分类](#11-信号概念与分类)
  - [1.2 信号的产生方式](#12-信号的产生方式)
  - [1.3 信号的默认处理方式](#13-信号的默认处理方式)
  - [1.4 信号处理函数 signal/sigaction](#14-信号处理函数-signalsigaction)
  - [1.5 信号集与信号阻塞](#15-信号集与信号阻塞)
  - [1.6 信号相关常用函数](#16-信号相关常用函数)
- [二、定时器](#二定时器)
  - [2.1 alarm](#21-alarm)
  - [2.2 setitimer](#22-setitimer)
  - [2.3 POSIX定时器 timer_create](#23-posix定时器-timer_create)
  - [2.4 timerfd_create](#24-timerfd_create)
- [三、事件驱动机制](#三事件驱动机制)
  - [3.1 signalfd](#31-signalfd)
  - [3.2 eventfd](#32-eventfd)
  - [3.3 inotify](#33-inotify)
- [四、综合实例](#四综合实例)
- [五、常见问题与调试](#五常见问题与调试)

---

# 一、信号机制

## 1.1 信号概念与分类
信号（Signal）是 Linux 中唯一的异步进程间通信机制，是软件层面对中断机制的模拟。信号可用于进程间通知、事件传递、异常处理等。

- 每个信号以 `SIG` 开头，均有唯一编号。
- 信号分为标准信号和实时信号（`SIGRTMIN`~`SIGRTMAX`，支持排队和携带数据）。

| 信号    | 值   | 说明                   | 默认处理方式 |
| ------- | ---- | ---------------------- | ------------ |
| SIGKILL | 9    | 强制终止进程           | 终止         |
| SIGSTOP | 19   | 停止进程               | 停止         |
| SIGINT  | 2    | Ctrl+C 终端中断        | 终止         |
| SIGQUIT | 3    | Ctrl+\ 终端退出        | 终止         |
| SIGTSTP | 20   | Ctrl+Z 终端挂起        | 停止         |
| SIGCONT | 18   | 继续已停止进程         | 继续         |
| SIGALRM | 14   | alarm定时器超时        | 终止         |
| SIGPIPE | 13   | 管道破裂               | 终止         |
| SIGCHLD | 17   | 子进程状态改变         | 忽略         |
| SIGUSR1 | 10   | 用户自定义信号1        | 忽略         |
| SIGUSR2 | 12   | 用户自定义信号2        | 忽略         |

**总结**：信号机制是 Linux 进程间异步通信的基础，掌握信号编号、默认处理方式及常见信号的作用是后续编程的前提。

---

## 1.2 信号的产生方式
- 用户终端输入（如 Ctrl+C、Ctrl+Z 等）
- 硬件异常（如除零、非法访问）
- 软件异常（如定时器超时）
- 系统调用 `kill`/`raise`/`abort` 等
- shell 命令 `kill` 发送

**总结**：信号可由多种途径产生，既有用户主动触发，也有系统自动生成。

---

## 1.3 信号的默认处理方式
- 终止进程
- 忽略信号
- 停止进程
- 继续进程

部分信号（如 SIGKILL、SIGSTOP）不可捕获、不可忽略。

**总结**：理解信号的默认处理方式有助于合理设计信号响应逻辑。

---

## 1.4 信号处理函数

### 1.4.1 signal
#### 语法
```c
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```
#### 参数说明
- `signum`：信号编号
- `handler`：处理方式（`SIG_IGN` 忽略，`SIG_DFL` 默认，自定义函数）
- 返回值：成功返回上一次处理器指针，失败返回 `SIG_ERR`

#### 使用场景
适用于简单信号处理，兼容性好，但功能有限。

#### 示例代码
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
void handler(int sig) {
    printf("Caught signal %d\n", sig);
}
int main() {
    signal(SIGINT, handler);
    while (1) pause();
    return 0;
}
```

#### 总结
`signal` 适合简单场景，部分系统实现不一致，推荐用 `sigaction`。

---

### 1.4.2 sigaction
#### 语法
```c
#include <signal.h>
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```
#### 参数说明
- `signum`：信号编号
- `act`：新处理方式
- `oldact`：保存旧处理方式
- 返回值：0 成功，-1 失败

#### struct sigaction
```c
struct sigaction {
    void (*sa_handler)(int);     // 传统处理器
    void (*sa_sigaction)(int, siginfo_t*, void*); // 带信息处理器
    sigset_t sa_mask;            // 信号处理期间阻塞的信号集
    int sa_flags;                // 行为标志
};
```

#### 使用场景
需要阻塞掩码、带信号信息、精细控制时。

#### 示例代码
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
void handler(int sig) {
    printf("Received signal %d\n", sig);
}
int main() {
    struct sigaction act = {0};
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
    while (1) pause();
}
```

#### 总结
`sigaction` 功能强大，推荐用于所有信号处理。

---

## 1.5 信号集与信号阻塞

### 1.5.1 信号集操作
#### 语法
```c
#include <signal.h>
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);
```
#### 参数说明
- `set`：信号集指针
- `signum`：信号编号
- 返回值：0 成功，-1 失败

#### 示例代码
```c
#include <signal.h>
#include <stdio.h>
int main() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    if (sigismember(&set, SIGINT)) {
        printf("SIGINT在信号集中\n");
    }
    return 0;
}
```

#### 总结
信号集用于批量管理信号，常用于阻塞集操作。

---

### 1.5.2 信号阻塞集与 sigprocmask
#### 语法
```c
#include <signal.h>
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```
#### 参数说明
- `how`：`SIG_BLOCK` 添加，`SIG_UNBLOCK` 移除，`SIG_SETMASK` 替换
- `set`：要操作的信号集
- `oldset`：保存原阻塞集
- 返回值：0 成功，-1 失败

#### 示例代码
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigprocmask(SIG_BLOCK, &set, NULL); // 阻塞SIGINT
    // ...
    sigprocmask(SIG_UNBLOCK, &set, NULL); // 解除阻塞
    return 0;
}
```

#### 总结
信号阻塞集可临时屏蔽信号，常用于临界区保护。

---

## 1.6 信号相关常用函数

### 1.6.1 kill
#### 语法
```c
#include <sys/types.h>
#include <signal.h>
int kill(pid_t pid, int sig);
```
#### 参数说明
- `pid`：目标进程/进程组
- `sig`：信号编号
- 返回值：0 成功，-1 失败

#### 使用场景
向指定进程/组发送信号。

#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
int main() {
    pid_t pid = fork();
    if (pid > 0) {
        while (1) {
            printf("父进程\n");
            sleep(1);
        }
    } else {
        sleep(3);
        kill(getppid(), SIGINT);
    }
    return 0;
}
```

#### 总结
`kill` 可用于进程间通信、终止进程等。

---

### 1.6.2 raise
#### 语法
```c
#include <signal.h>
int raise(int sig);
```
#### 参数说明
- `sig`：信号编号
- 返回值：0 成功，非0 失败

#### 使用场景
进程自发信号。

#### 示例代码
```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
int main() {
    int count = 0;
    while (1) {
        printf("aaaa\n");
        sleep(1);
        if (++count == 5) raise(SIGINT);
    }
}
```

#### 总结
`raise` 常用于自触发信号。

---

### 1.6.3 pause
#### 语法
```c
#include <unistd.h>
int pause(void);
```
#### 参数说明
- 无
- 返回值：有信号产生时返回 -1

#### 使用场景
阻塞等待信号。

#### 示例代码
```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
int main() {
    signal(SIGINT, SIG_IGN);
    printf("等待信号...\n");
    pause();
    return 0;
}
```

#### 总结
`pause` 常用于信号驱动型程序。

---

### 1.6.4 sigsuspend
#### 语法
```c
#include <signal.h>
int sigsuspend(const sigset_t *mask);
```
#### 参数说明
- `mask`：临时信号掩码
- 返回值：-1

#### 使用场景
原子性等待信号。

#### 示例代码
```c
#include <signal.h>
#include <unistd.h>
int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    sigsuspend(&mask);
    return 0;
}
```

#### 总结
`sigsuspend` 常用于安全等待信号。

---

### 1.6.5 sigwaitinfo
#### 语法
```c
#include <signal.h>
int sigwaitinfo(const sigset_t *set, siginfo_t *info);
```
#### 参数说明
- `set`：等待的信号集
- `info`：信号信息
- 返回值：信号编号

#### 使用场景
同步等待信号并获取详细信息。

#### 示例代码
```c
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);
    siginfo_t info;
    sigwaitinfo(&set, &info);
    printf("Received signal %d from pid %d\n", info.si_signo, info.si_pid);
    return 0;
}
```

#### 总结
`sigwaitinfo` 适合多线程/同步场景。

---

# 二、定时器

## 2.1 alarm
#### 语法
```c
#include <unistd.h>
unsigned int alarm(unsigned int seconds);
```
#### 参数说明
- `seconds`：定时时间（秒）
- 返回值：上一个定时器剩余时间或 0

#### 使用场景
简单秒级定时，触发 `SIGALRM`。

#### 示例代码
```c
#include <stdio.h>
#include <unistd.h>
int main() {
    alarm(5);
    pause();
    return 0;
}
```

#### 总结
`alarm` 简单易用，精度有限。

---

## 2.2 setitimer
#### 语法
```c
#include <sys/time.h>
int setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
```
#### 参数说明
- `which`：定时器类型（`ITIMER_REAL` 等）
- `new_value`：新定时参数
- `old_value`：保存旧参数
- 返回值：0 成功，-1 失败

#### 使用场景
高精度、周期性定时。

#### 示例代码
```c
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
void handler(int sig) {
    printf("定时器到期\n");
}
int main() {
    signal(SIGALRM, handler);
    struct itimerval timer = {
        .it_value = {2, 0},
        .it_interval = {1, 0}
    };
    setitimer(ITIMER_REAL, &timer, NULL);
    while (1) pause();
}
```

#### 总结
`setitimer` 适合高精度定时。

---

## 2.3 POSIX 定时器 timer_create
#### 语法
```c
#include <time.h>
int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);
int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);
```
#### 参数说明
- `clockid`：时钟类型
- `sevp`：到期通知方式
- `timerid`：定时器 ID
- `flags`：行为标志
- `new_value`：新定时参数
- `old_value`：保存旧参数
- 返回值：0 成功，-1 失败

#### 使用场景
纳秒级高精度定时，支持信号通知。

#### 示例代码
```c
#include <stdio.h>
#include <signal.h>
#include <time.h>
void handler(int sig, siginfo_t *si, void *uc) {
    printf("Timer expired\n");
}
int main() {
    struct sigaction sa = {0};
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigaction(SIGRTMIN, &sa, NULL);
    struct sigevent sev = {0};
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    timer_t timerid;
    timer_create(CLOCK_REALTIME, &sev, &timerid);
    struct itimerspec ts = {
        .it_value = {2, 0},
        .it_interval = {1, 0}
    };
    timer_settime(timerid, 0, &ts, NULL);
    while (1) pause();
}
```

#### 总结
`timer_create` 适合高精度、复杂定时需求。

---

## 2.4 timerfd_create
#### 语法
```c
#include <sys/timerfd.h>
int timerfd_create(int clockid, int flags);
int timerfd_settime(int fd, int flags, const struct itimerspec *new_value, struct itimerspec *old_value);
```
#### 参数说明
- `clockid`：时钟类型
- `flags`：行为标志
- `fd`：定时器文件描述符
- `new_value`：新定时参数
- `old_value`：保存旧参数
- 返回值：文件描述符/0 成功，-1 失败

#### 使用场景
与 `epoll` 等事件机制集成。

#### 示例代码
```c
#include <stdio.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdint.h>
int main() {
    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec ts = {
        .it_value = {2, 0},
        .it_interval = {1, 0}
    };
    timerfd_settime(fd, 0, &ts, NULL);
    while (1) {
        uint64_t expirations;
        read(fd, &expirations, sizeof(expirations));
        printf("Timer expired %llu times\n", expirations);
    }
}
```

#### 总结
`timerfd_create` 适合事件驱动架构。

---

# 三、事件驱动机制

## 3.1 signalfd
#### 语法
```c
#include <sys/signalfd.h>
int signalfd(int fd, const sigset_t *mask, int flags);
```
#### 参数说明
- `fd`：原文件描述符，-1 新建
- `mask`：关注的信号集
- `flags`：行为标志
- 返回值：文件描述符/0 成功，-1 失败

#### 使用场景
信号事件与 `select/epoll` 集成。

#### 示例代码
```c
#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
int main() {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    int fd = signalfd(-1, &mask, 0);
    struct signalfd_siginfo si;
    read(fd, &si, sizeof(si));
    printf("Signal from pid: %d\n", si.ssi_pid);
    return 0;
}
```

#### 总结
`signalfd` 适合统一事件驱动模型。

---

## 3.2 eventfd
#### 语法
```c
#include <sys/eventfd.h>
int eventfd(unsigned int initval, int flags);
```
#### 参数说明
- `initval`：初始值
- `flags`：行为标志
- 返回值：文件描述符/0 成功，-1 失败

#### 使用场景
线程/进程间事件通知。

#### 示例代码
```c
#include <sys/eventfd.h>
#include <unistd.h>
#include <stdint.h>
int main() {
    int efd = eventfd(0, 0);
    uint64_t val = 1;
    write(efd, &val, sizeof(val));
    read(efd, &val, sizeof(val));
    return 0;
}
```

#### 总结
`eventfd` 适合高效事件通知。

---

## 3.3 inotify
#### 语法
```c
#include <sys/inotify.h>
int inotify_init();
int inotify_add_watch(int fd, const char *pathname, uint32_t mask);
int inotify_rm_watch(int fd, int wd);
```
#### 参数说明
- `fd`：inotify 实例描述符
- `pathname`：监听路径
- `mask`：事件掩码
- `wd`：watch 描述符
- 返回值：文件描述符/0 成功，-1 失败

#### 使用场景
文件系统事件通知。

#### 示例代码
```c
#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    int fd = inotify_init();
    int wd = inotify_add_watch(fd, "/tmp", IN_CREATE | IN_DELETE);
    char buf[4096];
    int len = read(fd, buf, sizeof(buf));
    struct inotify_event *event = (struct inotify_event*) buf;
    printf("File event: %s\n", event->name);
    return 0;
}
```

#### 总结
`inotify` 适合文件变更监控。

---

# 四、综合实例

## 实现目标
开发一个 Linux 多任务后台守护进程，具备以下功能：
- 使用信号机制管理任务终止与重启（如 SIGTERM, SIGUSR1）；
- 使用 POSIX 定时器定期执行资源监控；
- 使用 signalfd/eventfd/inotify 统一处理事件；
- 实现高并发、低延迟、可扩展的事件驱动式服务；
- 输出监控信息到日志，支持动态加载配置文件。

## 涉及知识点
- 信号注册、默认处理、信号集、sigaction、自定义信号响应
- setitimer/timer_create/timerfd_create 定时任务
- signalfd 管理异步信号、eventfd 任务间通知、inotify 动态监听
- poll 统一事件调度

## 代码实现
```cpp
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/eventfd.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <time.h>

int main() {
    // === 一、信号处理 ===
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGUSR1);
    sigaddset(&sigset, SIGHUP);
    sigprocmask(SIG_BLOCK, &sigset, nullptr);  // 阻塞信号

    int sfd = signalfd(-1, &sigset, 0);  // signalfd 替代传统 signal

    // === 二、定时器：timerfd 监控每 5 秒触发 ===
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    itimerspec its = { {5, 0}, {5, 0} };  // 每 5 秒周期
    timerfd_settime(tfd, 0, &its, nullptr);

    // === 三、eventfd：模拟任务完成通知 ===
    int efd = eventfd(0, 0);

    // === 三、inotify：监听配置文件变化 ===
    int ifd = inotify_init1(IN_NONBLOCK);
    int wd = inotify_add_watch(ifd, "/etc/mydaemon.conf", IN_MODIFY);

    // === 事件统一 poll 调度 ===
    struct pollfd fds[4] = {
        {sfd, POLLIN, 0},  // signal
        {tfd, POLLIN, 0},  // timer
        {efd, POLLIN, 0},  // event
        {ifd, POLLIN, 0},  // inotify
    };

    std::cout << "[Daemon] 正在运行，等待事件...\n";

    while (true) {
        int ret = poll(fds, 4, -1);
        if (ret == -1) {
            perror("poll");
            break;
        }

        // === 信号事件处理 ===
        if (fds[0].revents & POLLIN) {
            struct signalfd_siginfo fdsi;
            read(sfd, &fdsi, sizeof(fdsi));
            if (fdsi.ssi_signo == SIGTERM) {
                std::cout << "[Daemon] 收到 SIGTERM，优雅退出\n";
                break;
            } else if (fdsi.ssi_signo == SIGUSR1) {
                std::cout << "[Daemon] 收到 SIGUSR1，立即执行任务！\n";
            } else if (fdsi.ssi_signo == SIGHUP) {
                std::cout << "[Daemon] 收到 SIGHUP，重新加载配置\n";
            }
        }

        // === 定时任务处理 ===
        if (fds[1].revents & POLLIN) {
            uint64_t expirations;
            read(tfd, &expirations, sizeof(expirations));
            std::cout << "[Timer] 周期性任务执行 (" << expirations << " 次)\n";
        }

        // === 任务事件通知处理 ===
        if (fds[2].revents & POLLIN) {
            uint64_t value;
            read(efd, &value, sizeof(value));
            std::cout << "[Event] 有任务完成通知，value=" << value << "\n";
        }

        // === 配置文件变更处理 ===
        if (fds[3].revents & POLLIN) {
            char buf[1024];
            read(ifd, buf, sizeof(buf));  // 简化处理
            std::cout << "[Config] 配置文件发生变更，建议重新加载！\n";
        }
    }

    close(sfd); close(tfd); close(efd); close(ifd);
    return 0;
}
```

## 运行与调试
- 编译：
  ```bash
  g++ monitor_daemon.cpp -o monitor_daemon
  ./monitor_daemon &
  ```
- 发送信号：
  ```bash
  kill -USR1 <pid>     # 执行任务
  kill -HUP <pid>      # 重新加载配置
  kill -TERM <pid>     # 安全退出
  ```
- 模拟配置变更：
  ```bash
  echo "reload" >> /etc/mydaemon.conf
  ```
- 模拟子任务完成：
  ```bash
  echo 1 > /proc/<pid>/fd/<efd_fd>
  ```

## 常见问题与解答
- Q: 为什么要用 signalfd 而不是传统 signal？
  A: signalfd 可与 poll/epoll 集成，统一事件驱动模型，便于高并发场景。
- Q: timerfd 与 setitimer 有何区别？
  A: timerfd 可与事件机制集成，适合多任务/多定时场景。
- Q: inotify 监听不到文件变化？
  A: 检查监听路径、事件掩码和权限，确保配置文件存在。

## 参考资料
- 《UNIX环境高级编程》
- man 手册：signal、sigaction、timerfd、eventfd、inotify
- Linux 内核文档

---
