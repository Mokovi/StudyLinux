# Linux C 高级编程学习项目进度

## 项目概述
本项目旨在系统学习 Linux C 高级编程，包括系统调用、进程控制、线程编程、网络编程等核心内容。通过整理学习笔记，建立完整的知识体系。

## 学习进度

### 已完成模块

#### 1. 基础系统调用 (Revision #1)
- ✅ 文件 I/O 基本操作：open, read, write, lseek, close
- ✅ 文件锁机制：fcntl, flock
- ✅ 异步 I/O：aio_read, aio_write, io_submit/io_getevents
- ✅ 零拷贝 I/O：sendfile, splice
- ✅ 进程控制基础：fork, vfork, exec系列, wait/waitpid
- ✅ 进程属性管理：setuid/setgid, nice/setpriority
- ✅ 综合实例：Web日志切割守护进程

#### 2. 线程与多线程编程 (Revision #3)
- ✅ 线程基本概念与创建：pthread_create, pthread_join
- ✅ 线程同步机制：互斥锁、条件变量、读写锁
- ✅ 线程安全与原子操作
- ✅ 线程池设计与实现
- ✅ 线程取消与清理机制
- ✅ 信号量同步原语
- ✅ 综合实例：字符串拼接器

#### 3. 文件 I/O 与进程控制整合 (Revision #4)
- ✅ 以 2_file_IO_processCtl.md 为主线整合 Linux_progress.md 知识点
- ✅ 去重并符合 standard.md 规范
- ✅ 补充进程号相关函数详细说明
- ✅ 完善 vfork 与 fork 的区别对比
- ✅ 增强 exec 函数族变体说明
- ✅ 添加 system 与 exec 的区别对比
- ✅ 统一代码风格和文档格式
- ✅ 完善快速跳转目录

### 进行中模块

#### 4. 网络编程 (计划中)
- 🔄 Socket 编程基础
- 🔄 TCP/UDP 编程
- 🔄 多路复用 (select, poll, epoll)
- 🔄 网络编程综合实例

#### 5. 信号处理 (计划中)
- 🔄 信号概念与分类
- 🔄 信号处理函数
- 🔄 信号集操作
- 🔄 实时信号

#### 6. 进程间通信 (计划中)
- 🔄 管道 (pipe, fifo)
- 🔄 消息队列
- 🔄 共享内存
- 🔄 信号量
- 🔄 Socket 通信

## 文档规范

### 排版标准
- 大章节使用"一、"、"二、"格式编号
- 小节使用"1.1"、"1.2"格式编号
- 每个函数包含：语法、参数说明、使用场景、示例代码
- 统一代码风格和注释格式
- 提供快速跳转目录

### 内容要求
- 理论概念清晰准确
- 示例代码完整可运行
- 错误处理规范
- 综合实例贴近实际应用
- 知识点对比表格化

## 学习资源

### 参考书籍
- 《UNIX环境高级编程》(APUE)
- 《Linux程序设计》
- 《深入理解计算机系统》

### 在线资源
- Linux man pages
- POSIX 标准文档
- Linux 内核文档

## 项目目标

### 短期目标 (1-2个月)
- [x] 完成基础系统调用学习
- [x] 完成线程编程学习
- [x] 整合进程控制知识点
- [ ] 完成网络编程基础
- [ ] 完成信号处理学习

### 中期目标 (3-4个月)
- [ ] 完成进程间通信学习
- [ ] 实现综合项目：多进程Web服务器
- [ ] 性能优化与调试技巧
- [ ] 安全编程实践

### 长期目标 (6个月)
- [ ] 深入理解Linux内核机制
- [ ] 掌握高级调试技术
- [ ] 参与开源项目贡献
- [ ] 编写技术博客分享经验

## 更新日志

### Revision #5 (2025-06-19)
**进程间通信（IPC）编程笔记整理完成**
- 将 3_ipc.md 文档按照 standard.md 规范重新整理为 3_ipc_standard.md
- 涵盖九大IPC机制：匿名管道、有名管道FIFO、POSIX共享内存、System V共享内存、消息队列、信号量、轻量级同步futex、套接字对socketpair、Netlink套接字
- 每个IPC机制包含：概念介绍、函数语法、参数说明、使用场景、示例代码、特性描述、最佳实践、性能优化、调试诊断和常见问题
- 所有示例代码均进行格式化和补充错误处理，确保完整可运行
- 综合实例章节实现多进程系统监控守护进程，集成多种IPC机制展示实际应用
- 详细保留原文技术细节、代码示例和说明，确保内容完整且符合规范
- 形成结构清晰、内容丰富、格式统一的IPC编程笔记

### Revision #4 (2025-06-18)
**文件 I/O 与进程控制整合完成**
- 以 2_file_IO_processCtl.md 为主线，整合 Linux_progress.md 中的进程控制知识点
- 去重并严格按照 standard.md 规范重新组织内容结构
- 补充进程号相关函数的详细说明和示例代码
- 完善 vfork 与 fork 的区别对比表格
- 增强 exec 函数族各变体的详细说明
- 添加 system 与 exec 函数的区别对比
- 统一所有代码风格、注释格式和文档排版
- 完善快速跳转目录，提高文档可读性
- 确保所有示例代码完整可运行，包含错误处理

### Revision #3 (2025-06-17)
**线程与多线程编程笔记整合完成**
- 以 4_thread.md 为主线，整合 thread.md 中的线程知识点
- 补充线程概念、线程与进程比较、多线程应用场景
- 添加线程调度机制、线程取消与清理机制
- 完善信号量同步原语，包含互斥和同步示例
- 新增字符串拼接综合实例（互斥锁和信号量两种实现）
- 统一代码风格和文档格式，符合 standard.md 规范
- 更新快速跳转目录和项目进度记录

### Revision #2 (2025-06-16)
**线程与多线程编程笔记整理完成**
- 按照 standard.md 规范重新组织 4_thread.md 内容
- 统一章节结构、目录格式、函数格式
- 去除重复内容，完善线程池设计和综合实例
- 形成完整的多线程编程指南
- 更新项目进度记录

### Revision #1 (2025-06-15)
**基础系统调用学习完成**
- 完成文件 I/O 基本操作学习
- 完成文件锁机制学习
- 完成异步 I/O 学习
- 完成零拷贝 I/O 学习
- 完成进程控制基础学习
- 完成进程属性管理学习
- 实现 Web 日志切割守护进程综合实例
- 建立项目进度记录文档

## 下一步计划

1. **网络编程模块**：开始 Socket 编程基础学习
2. **信号处理模块**：学习信号概念和处理机制
3. **进程间通信模块**：掌握各种 IPC 机制
4. **综合项目开发**：结合所学知识实现实际应用

## 技术栈

- **编程语言**: C
- **操作系统**: Linux
- **开发工具**: GCC, GDB, Make
- **版本控制**: Git
- **文档格式**: Markdown

---

*最后更新: 2025-06-19*


