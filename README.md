# Linux系统学习项目

本项目旨在系统、完整地学习Linux操作系统，内容涵盖基础命令、Shell编程、系统编程、网络编程等模块，适合希望深入理解和掌握Linux的学习者。

## linxu基础

* **Linux 基础环境**

  * 发行版与使用场景：Ubuntu、CentOS、Debian 等
  * 文件系统结构：`/etc`, `/var`, `/usr`, `/proc`, `/sys`, `/dev`, `/home`
  * 文本编辑器：Vim / Emacs / Nano

* **C 编译构建调试工具**

  * 编译与链接过程：`gcc`
  * makefile文档编写: makefile cmake
  * 调试与分析工具：`gdb`, `strace`, `lsof`, `perf`

---

## Linux 基础命令及 Shell

1. **常用命令详解**

   * 文件操作：`ls`, `cp`, `mv`, `rm`, `find`, `tree`
   * 用户、组、权限管理：`useradd`, `groupadd`, `chmod`, `chown`, sudo 配置
   * 进程管理：`ps`, `top`, `htop`, `kill`, `jobs`, `bg`, `fg`
   * 网络命令：`ping`, `ss`, `netstat`, `curl`, `wget`
   * 包管理：`apt`, `yum`, `dnf`, `rpm`, `dpkg`
   * 管道与重定向：`|`, `>`, `>>`, `<`, `2>&1`

2. **Shell 脚本**

   * 基础：变量、条件判断、循环、函数
   * 进阶：数组、正则匹配、环境变量
   * 调试：`set -x`, `set -e`
   * 定时任务：`crontab`, `at`

3. **Shell 常用工具**

   * 文本处理：`awk`, `sed`, `grep`, `cut`, `sort`, `uniq`
   * 日志分析：结合 `grep`/`awk`
   * 批量操作：`xargs`, `parallel`

4. **实战案例与练习**

   * 日志自动归档：按日期切割、压缩
   * 性能监控脚本：CPU/内存/磁盘报警
   * 自动化部署：批量拉取代码、编译、发布

---

## 系统编程

1. **Linux 系统调用基础**

   * 概念：用户态与内核态切换
   * 常用接口：`syscall` 封装

2. **文件 I/O 与进程控制**

   * 打开、读写与定位：`open`, `read`, `write`, `lseek`, `close`
   * 进程创建与执行：`fork`, `vfork`, `execve`, `waitpid`, `exit`

3. **进程间通信（IPC）**

   * 管道 `pipe` / 有名管道 FIFO
   * 共享内存 `shmget`, `shmat`, `shmdt`
   * 消息队列 `msgget`, `msgsnd`, `msgrcv`
   * 信号量 `semget`, `semop`

4. **线程与多线程编程**

   * `pthread_create`, `pthread_join`
   * 同步：互斥锁 `pthread_mutex`、条件变量 `pthread_cond`
   * 线程局部存储（TLS）

5. **信号、定时器与事件处理**

   * 信号处理：`signal`, `sigaction`
   * 定时器：`alarm`, `setitimer`, POSIX 定时器 `timer_create`
   * 事件驱动：`signalfd`, `eventfd`

6. **内存管理**

   * 动态分配：`malloc`, `calloc`, `realloc`, `free`
   * 内存映射：`mmap`, `munmap`
   * 虚拟内存与 `/proc/<pid>/maps`

---

## 网络编程

1. **网络基础知识**

   * OSI/TCP/IP 模型
   * IP 地址、子网掩码、路由
   * DNS、ARP、MAC 地址

2. **Socket 编程**

   * 套接字创建与关闭：`socket`, `bind`, `listen`, `accept`, `connect`, `close`
   * 数据收发：`send`, `recv`, `sendto`, `recvfrom`

3. **TCP/UDP 协议与应用**

   * TCP 特性：流式、可靠性、拥塞控制
   * UDP 特性：无连接、报文传输
   * 应用示例：简易 HTTP 客户端/服务器、DNS 查询工具

4. **多路复用与高性能编程**

   * `select`, `poll`, `epoll` 使用方法与对比
   * 非阻塞 I/O 与边沿/水平触发
   * Reactor 模型与线程池结合

5. **网络安全与加密基础**

   * SSL/TLS 基础：OpenSSL 使用
   * 简单加密：对称加密（AES）、非对称加密（RSA）
   * 身份验证：证书与密钥管理

---

## 嵌入式开发方向

* **交叉编译与环境搭建**

  * 工具链：`arm-linux-gnueabihf-gcc` 等
  * 构建系统：Buildroot / Yocto

* **嵌入式系统架构**

  * 启动流程：Bootloader → Kernel → RootFS → Init
  * 常用组件：BusyBox、systemd（或 init）

* **驱动开发与设备树**

  * 字符设备驱动、Platform 驱动
  * 设备树(Device Tree) 基础
  * 用户空间通信：ioctl、sysfs、procfs

* **外设接口与调试**

  * UART、SPI、I2C、GPIO 调试
  * 串口工具、逻辑分析仪

---

## 服务器开发方向

* **高性能网络服务**

  * Reactor 模型：`epoll` + 线程池
  * 异步框架：muduo 等

* **并发与资源管理**

  * 线程池、连接池
  * 内存池、日志系统

* **存储与持久化**

  * SQLite / MySQL / Redis 接入
  * 日志文件与滚动策略

* **部署与维护**

  * systemd 单元文件
  * CI/CD：Jenkins / GitLab CI

## 容器与虚拟化

* **容器技术**
  * Docker 基础：镜像、容器、网络、存储
  * Kubernetes 入门：Pod、Service、Deployment
  * 容器编排与微服务架构

* **虚拟化**
  * KVM/QEMU 基础
  * 虚拟机管理：virt-manager、libvirt

## 安全与性能调优

* **系统安全**
  * 防火墙配置：iptables、firewalld
  * 安全审计：auditd、SELinux/AppArmor
  * 漏洞扫描与修复

* **性能调优**
  * 系统监控：sar、vmstat、iostat
  * 内核参数调优：sysctl
  * 应用性能分析：perf、ftrace


## 推荐资源

* **书籍**

  * 《UNIX 环境高级编程》
  * 《Linux 多线程服务端编程（muduo）》
  * 《嵌入式 Linux 开发实战》
  * 《深入理解 Linux 内核》

* **在线资源**

  * Linux 源码：[https://github.com/torvalds/linux](https://github.com/torvalds/linux)
  * LearnLinux：[https://linuxjourney.com/](https://linuxjourney.com/)
  * OverTheWire：[https://overthewire.org/](https://overthewire.org/)

## 进度记录
请参考 `projectProgress.md` 文件，实时记录学习进度与心得。

---

欢迎大家一起学习、交流与完善本项目！ 