# 网络编程知识概览

本笔记文档系统地梳理了网络编程的关键知识点，从基础网络架构到高性能编程模型，再到网络安全实践，旨在为开发者构建完整的网络编程知识体系。

---

## 一、网络基础知识

网络编程的基础在于理解网络通信的底层原理。本节涵盖：

- **OSI / TCP/IP 模型**：分层思想，有助于理解协议间职责划分。
- **IP 地址与子网掩码**：掌握地址划分与网络路由的基本原理。
- **DNS、ARP 与 MAC 地址**：实现主机间通信的关键机制。

---

## 二、Socket 编程

Socket 是网络编程的核心接口。主要包括：

- **套接字的创建与关闭**：
  - 创建与绑定：`socket`, `bind`
  - 服务端监听与连接接收：`listen`, `accept`
  - 客户端连接：`connect`
  - 通信结束释放资源：`close`

- **数据收发接口**：
  - 面向连接：`send`, `recv`
  - 无连接：`sendto`, `recvfrom`

---

## 三、TCP/UDP 协议与应用

深入理解传输层协议特性：

- **TCP 协议**：
  - 面向连接，提供可靠、顺序到达的字节流。
  - 支持拥塞控制与流量控制。

- **UDP 协议**：
  - 无连接，报文传输效率高但不保证可靠性。

- **实践示例**：
  - 简易 HTTP 客户端与服务器实现。
  - 基于 UDP 的 DNS 查询工具。

---

## 四、多路复用与高性能编程

为应对高并发需求，掌握 I/O 多路复用技术至关重要：

- **系统调用接口**：
  - `select`：兼容性广，性能有限。
  - `poll`：支持更多文件描述符。
  - `epoll`：Linux 下高性能 I/O 多路复用方案。

- **触发模式**：
  - 水平触发（Level Trigger）与边沿触发（Edge Trigger）机制。

- **并发模型**：
  - 基于 Reactor 模型的事件驱动架构。
  - 配合线程池提升吞吐能力。

---

## 五、网络安全与加密基础

网络通信不仅要高效，还需保障数据安全：

- **SSL/TLS 概述**：
  - 掌握加密通信协议基础。
  - 了解 OpenSSL 工具的基本用法。

- **加密技术**：
  - 对称加密（如 AES）：适合快速加密大数据。
  - 非对称加密（如 RSA）：用于密钥交换与身份验证。

- **身份认证机制**：
  - X.509 证书体系。
  - 公钥与私钥的管理方法。

