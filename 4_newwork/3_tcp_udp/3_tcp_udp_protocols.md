# 三、TCP/UDP 协议与应用

## 目录
- [3.1 传输层协议概述](#31-传输层协议概述)
- [3.2 TCP 协议详解](#32-tcp-协议详解)
- [3.3 TCP 编程实践](#33-tcp-编程实践)
- [3.4 UDP 协议详解](#34-udp-协议详解)
- [3.5 UDP 编程实践](#35-udp-编程实践)
- [3.6 协议选择与性能对比](#36-协议选择与性能对比)
- [3.7 网络编程最佳实践](#37-网络编程最佳实践)

---

## 3.1 传输层协议概述

传输层协议是网络通信的核心，负责端到端的数据传输，主要协议包括TCP和UDP。

#### 传输层职责
- **端到端通信**：提供进程间的可靠通信
- **数据分段**：将应用层数据分割成适合网络传输的片段
- **流量控制**：防止发送方发送过快导致接收方缓冲区溢出
- **拥塞控制**：防止网络拥塞，保证网络稳定性

#### 协议对比
| 特性 | TCP | UDP |
|------|-----|-----|
| 连接方式 | 面向连接 | 无连接 |
| 可靠性 | 可靠传输 | 不可靠传输 |
| 数据顺序 | 保证顺序 | 不保证顺序 |
| 流量控制 | 有 | 无 |
| 拥塞控制 | 有 | 无 |
| 传输效率 | 相对较低 | 较高 |
| 应用场景 | 文件传输、Web浏览 | 实时通信、游戏 |

#### 端口号管理
```c
// 常用端口号
#define HTTP_PORT     80      // HTTP服务
#define HTTPS_PORT    443     // HTTPS服务
#define FTP_PORT      21      // FTP控制
#define SSH_PORT      22      // SSH服务
#define DNS_PORT      53      // DNS服务
#define SMTP_PORT     25      // 邮件发送
#define POP3_PORT     110     // 邮件接收
#define IMAP_PORT     143     // 邮件管理
```

#### 总结
传输层协议为应用层提供通信服务，TCP提供可靠传输，UDP提供高效传输，选择合适协议对应用性能至关重要。

---

## 3.2 TCP 协议详解

TCP（Transmission Control Protocol）是面向连接的可靠传输协议，提供有序、无丢失的数据传输服务。

#### TCP 协议特性
- **面向连接**：通信前需要建立连接，通信结束后释放连接
- **可靠传输**：使用确认机制、重传机制保证数据不丢失
- **有序传输**：保证数据按发送顺序到达
- **流量控制**：使用滑动窗口机制控制发送速率
- **拥塞控制**：防止网络拥塞，包括慢启动、拥塞避免等算法

#### TCP 连接状态
```c
// TCP 连接状态机
CLOSED → LISTEN → SYN_RCVD → ESTABLISHED → CLOSE_WAIT → CLOSED
    ↓        ↓         ↓           ↓           ↓
  SYN_SENT → ESTABLISHED → FIN_WAIT_1 → FIN_WAIT_2 → TIME_WAIT
```

#### TCP 三次握手
```c
// 连接建立过程
客户端 → SYN(seq=x) → 服务端
客户端 ← SYN+ACK(seq=y, ack=x+1) ← 服务端
客户端 → ACK(ack=y+1) → 服务端
```

#### TCP 四次挥手
```c
// 连接释放过程
客户端 → FIN → 服务端
客户端 ← ACK ← 服务端
客户端 ← FIN ← 服务端
客户端 → ACK → 服务端
```

#### TCP 头部结构
```c
struct tcp_header {
    uint16_t source_port;      // 源端口号
    uint16_t dest_port;        // 目标端口号
    uint32_t seq_num;          // 序列号
    uint32_t ack_num;          // 确认号
    uint8_t data_offset;       // 数据偏移
    uint8_t flags;             // 控制标志
    uint16_t window_size;      // 窗口大小
    uint16_t checksum;         // 校验和
    uint16_t urgent_ptr;       // 紧急指针
};
```

#### 总结
TCP协议通过复杂的机制保证数据传输的可靠性，但也会带来一定的性能开销。

---

## 3.3 TCP 编程实践

TCP编程是网络编程中最常用的方式，适用于需要可靠传输的应用场景。

#### TCP 服务端编程流程
```c
socket() → bind() → listen() → accept() → recv()/send() → close()
```

#### TCP 客户端编程流程
```c
socket() → connect() → send()/recv() → close()
```

#### 函数 `connect` 超时设置
#### 语法
```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

#### 参数说明
- `sockfd`：套接字描述符
- `addr`：服务器地址结构指针
- `addrlen`：地址结构长度
- **返回值**：成功返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`connect` 用于客户端主动连接到服务器，建立TCP连接。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int connect_with_timeout(int sockfd, const struct sockaddr *addr, 
                        socklen_t addrlen, int timeout_seconds) {
    int flags, result;
    fd_set write_fds;
    struct timeval timeout;
    
    // 设置非阻塞模式
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    
    // 尝试连接
    result = connect(sockfd, addr, addrlen);
    if (result == 0) {
        // 连接立即成功
        fcntl(sockfd, F_SETFL, flags);  // 恢复阻塞模式
        return 0;
    }
    
    if (errno != EINPROGRESS) {
        fcntl(sockfd, F_SETFL, flags);
        return -1;
    }
    
    // 等待连接完成或超时
    FD_ZERO(&write_fds);
    FD_SET(sockfd, &write_fds);
    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;
    
    result = select(sockfd + 1, NULL, &write_fds, NULL, &timeout);
    if (result == 0) {
        // 超时
        fcntl(sockfd, F_SETFL, flags);
        errno = ETIMEDOUT;
        return -1;
    }
    
    if (result < 0) {
        fcntl(sockfd, F_SETFL, flags);
        return -1;
    }
    
    // 检查连接是否成功
    int error = 0;
    socklen_t len = sizeof(error);
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
        fcntl(sockfd, F_SETFL, flags);
        return -1;
    }
    
    if (error != 0) {
        fcntl(sockfd, F_SETFL, flags);
        errno = error;
        return -1;
    }
    
    fcntl(sockfd, F_SETFL, flags);
    return 0;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    // 带超时的连接
    if (connect_with_timeout(sockfd, (struct sockaddr*)&server_addr, 
                           sizeof(server_addr), 5) == -1) {
        perror("connect failed");
        close(sockfd);
        return 1;
    }
    
    printf("连接成功\n");
    close(sockfd);
    return 0;
}
```

#### 总结
带超时的连接函数可以避免程序长时间阻塞，提高用户体验。

---

#### TCP 数据发送与接收
#### 函数 `send` 完整发送
#### 语法
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

#### 参数说明
- `sockfd`：已连接的套接字描述符
- `buf`：要发送的数据缓冲区
- `len`：数据长度
- `flags`：发送标志
- **返回值**：成功返回发送的字节数，失败返回 -1 并设置 `errno`

#### 使用场景
`send` 用于在已连接的套接字上发送数据，但可能不会发送完整数据。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

ssize_t send_all(int sockfd, const void *buf, size_t len) {
    size_t total_sent = 0;
    const char *ptr = (const char *)buf;
    
    while (total_sent < len) {
        ssize_t sent = send(sockfd, ptr + total_sent, len - total_sent, 0);
        if (sent == -1) {
            if (errno == EINTR) {
                // 被信号中断，继续发送
                continue;
            }
            return -1;  // 其他错误
        }
        if (sent == 0) {
            // 连接已关闭
            break;
        }
        total_sent += sent;
    }
    
    return total_sent;
}

int main() {
    int sockfd;
    char message[] = "Hello, TCP Server!";
    
    // 假设已经建立连接
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ... 连接代码 ...
    
    // 完整发送数据
    ssize_t sent = send_all(sockfd, message, strlen(message));
    if (sent == -1) {
        perror("send_all failed");
    } else {
        printf("发送了 %zd 字节数据\n", sent);
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`send_all` 函数确保数据完整发送，处理了部分发送和信号中断的情况。

---

#### 函数 `recv` 完整接收
#### 语法
```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

#### 参数说明
- `sockfd`：已连接的套接字描述符
- `buf`：接收数据缓冲区
- `len`：缓冲区长度
- `flags`：接收标志
- **返回值**：成功返回接收的字节数，连接关闭返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`recv` 用于从已连接的套接字接收数据，但可能不会接收完整数据。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

ssize_t recv_all(int sockfd, void *buf, size_t len) {
    size_t total_received = 0;
    char *ptr = (char *)buf;
    
    while (total_received < len) {
        ssize_t received = recv(sockfd, ptr + total_received, 
                               len - total_received, 0);
        if (received == -1) {
            if (errno == EINTR) {
                // 被信号中断，继续接收
                continue;
            }
            return -1;  // 其他错误
        }
        if (received == 0) {
            // 连接已关闭
            break;
        }
        total_received += received;
    }
    
    return total_received;
}

int main() {
    int sockfd;
    char buffer[1024];
    
    // 假设已经建立连接
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ... 连接代码 ...
    
    // 完整接收数据
    ssize_t received = recv_all(sockfd, buffer, sizeof(buffer) - 1);
    if (received == -1) {
        perror("recv_all failed");
    } else if (received == 0) {
        printf("连接已关闭\n");
    } else {
        buffer[received] = '\0';
        printf("接收到 %zd 字节数据: %s\n", received, buffer);
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`recv_all` 函数确保数据完整接收，处理了部分接收和信号中断的情况。

---

## 3.4 UDP 协议详解

UDP（User Datagram Protocol）是无连接的不可靠传输协议，提供高效的数据传输服务。

#### UDP 协议特性
- **无连接**：通信前不需要建立连接
- **不可靠传输**：不保证数据到达，不保证数据顺序
- **高效传输**：协议开销小，传输效率高
- **无流量控制**：发送方可以任意速率发送数据
- **无拥塞控制**：不进行网络拥塞控制

#### UDP 头部结构
```c
struct udp_header {
    uint16_t source_port;      // 源端口号
    uint16_t dest_port;        // 目标端口号
    uint16_t length;           // UDP数据包长度
    uint16_t checksum;         // 校验和
};
```

#### UDP 数据包格式
```c
┌─────────────────────────────────────────┐
│ IP 头部                                  │
├─────────────────────────────────────────┤
│ UDP 头部                                 │
├─────────────────────────────────────────┤
│ 数据部分                                 │
└─────────────────────────────────────────┘
```

#### UDP 应用场景
- **实时通信**：语音、视频通话
- **游戏**：在线游戏数据传输
- **DNS查询**：域名解析
- **DHCP**：动态主机配置
- **SNMP**：网络管理

#### 总结
UDP协议简单高效，适用于对实时性要求高、允许少量数据丢失的应用场景。

---

## 3.5 UDP 编程实践

UDP编程相对简单，不需要建立连接，直接发送数据包即可。

#### UDP 服务端编程流程
```c
socket() → bind() → recvfrom()/sendto() → close()
```

#### UDP 客户端编程流程
```c
socket() → sendto()/recvfrom() → close()
```

#### 函数 `sendto` 数据发送
#### 语法
```c
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

#### 参数说明
- `sockfd`：套接字描述符
- `buf`：要发送的数据缓冲区
- `len`：数据长度
- `flags`：发送标志
- `dest_addr`：目标地址结构指针
- `addrlen`：地址结构长度
- **返回值**：成功返回发送的字节数，失败返回 -1 并设置 `errno`

#### 使用场景
`sendto` 用于在无连接套接字上发送数据，适用于UDP等无连接协议。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[] = "Hello, UDP Server!";
    ssize_t bytes_sent;
    
    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    // 发送数据
    bytes_sent = sendto(sockfd, message, strlen(message), 0,
                       (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (bytes_sent == -1) {
        perror("sendto failed");
    } else {
        printf("发送了 %zd 字节数据\n", bytes_sent);
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`sendto` 函数是UDP通信中发送数据的主要接口，每次调用都需要指定目标地址。

---

#### 函数 `recvfrom` 数据接收
#### 语法
```c
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
```

#### 参数说明
- `sockfd`：套接字描述符
- `buf`：接收数据缓冲区
- `len`：缓冲区长度
- `flags`：接收标志
- `src_addr`：用于存储源地址的结构指针（可为 NULL）
- `addrlen`：源地址结构长度的指针（可为 NULL）
- **返回值**：成功返回接收的字节数，失败返回 -1 并设置 `errno`

#### 使用场景
`recvfrom` 用于从无连接套接字接收数据，适用于UDP等无连接协议。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    ssize_t bytes_received;
    char client_ip[INET_ADDRSTRLEN];
    
    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // 绑定套接字
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(sockfd);
        return 1;
    }
    
    printf("UDP服务器开始监听端口 8080\n");
    
    // 接收数据
    bytes_received = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr*)&client_addr, &client_len);
    if (bytes_received == -1) {
        perror("recvfrom failed");
    } else {
        buffer[bytes_received] = '\0';
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("从 %s:%d 接收到 %zd 字节数据: %s\n", 
               client_ip, ntohs(client_addr.sin_port), bytes_received, buffer);
        
        // 发送回复
        char reply[] = "Message received!";
        sendto(sockfd, reply, strlen(reply), 0,
               (struct sockaddr*)&client_addr, client_len);
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`recvfrom` 函数是UDP通信中接收数据的主要接口，可以获取发送方的地址信息。

---

## 3.6 协议选择与性能对比

选择合适的传输协议对应用性能至关重要，需要根据应用需求进行权衡。

#### 协议选择标准
- **可靠性要求**：需要可靠传输选择TCP，允许少量丢失选择UDP
- **实时性要求**：对延迟敏感选择UDP，对延迟不敏感选择TCP
- **数据量大小**：大数据量选择TCP，小数据量选择UDP
- **连接特性**：长连接选择TCP，短连接选择UDP

#### 性能对比测试
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <stdlib.h>

#define TEST_SIZE 1000000  // 1MB数据
#define BUFFER_SIZE 4096

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// TCP性能测试
void tcp_performance_test() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    double start_time, end_time;
    size_t total_sent = 0;
    
    // 创建TCP套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    // 连接服务器
    connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    
    // 发送数据
    start_time = get_time();
    while (total_sent < TEST_SIZE) {
        size_t to_send = (TEST_SIZE - total_sent < BUFFER_SIZE) ? 
                        (TEST_SIZE - total_sent) : BUFFER_SIZE;
        ssize_t sent = send(sockfd, buffer, to_send, 0);
        if (sent > 0) {
            total_sent += sent;
        }
    }
    end_time = get_time();
    
    printf("TCP传输 %zu 字节用时: %.3f 秒, 速率: %.2f MB/s\n", 
           total_sent, end_time - start_time, 
           total_sent / (end_time - start_time) / 1024 / 1024);
    
    close(sockfd);
}

// UDP性能测试
void udp_performance_test() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    double start_time, end_time;
    size_t total_sent = 0;
    
    // 创建UDP套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8081);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    // 发送数据
    start_time = get_time();
    while (total_sent < TEST_SIZE) {
        size_t to_send = (TEST_SIZE - total_sent < BUFFER_SIZE) ? 
                        (TEST_SIZE - total_sent) : BUFFER_SIZE;
        ssize_t sent = sendto(sockfd, buffer, to_send, 0,
                             (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (sent > 0) {
            total_sent += sent;
        }
    }
    end_time = get_time();
    
    printf("UDP传输 %zu 字节用时: %.3f 秒, 速率: %.2f MB/s\n", 
           total_sent, end_time - start_time, 
           total_sent / (end_time - start_time) / 1024 / 1024);
    
    close(sockfd);
}

int main() {
    printf("网络协议性能对比测试\n");
    printf("====================\n\n");
    
    tcp_performance_test();
    udp_performance_test();
    
    return 0;
}
```

#### 总结
UDP通常比TCP有更高的传输速率，但TCP提供可靠传输保证，需要根据应用需求选择合适的协议。

---

## 3.7 网络编程最佳实践

网络编程涉及复杂的系统交互，遵循最佳实践可以提高程序的稳定性和性能。

#### 错误处理
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

// 网络错误处理函数
void handle_network_error(const char *operation, int sockfd) {
    switch (errno) {
        case EINTR:
            printf("操作被信号中断: %s\n", operation);
            break;
        case EAGAIN:
        case EWOULDBLOCK:
            printf("操作会阻塞: %s\n", operation);
            break;
        case ECONNRESET:
            printf("连接被重置: %s\n", operation);
            break;
        case EPIPE:
            printf("管道破裂: %s\n", operation);
            break;
        case ETIMEDOUT:
            printf("操作超时: %s\n", operation);
            break;
        default:
            printf("网络错误 %d: %s\n", errno, operation);
            break;
    }
    
    if (sockfd >= 0) {
        close(sockfd);
    }
}
```

#### 资源管理
```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// 安全的套接字关闭
void safe_close(int *sockfd) {
    if (sockfd && *sockfd >= 0) {
        shutdown(*sockfd, SHUT_RDWR);
        close(*sockfd);
        *sockfd = -1;
    }
}

// 带错误处理的套接字创建
int safe_socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);
    if (sockfd == -1) {
        perror("socket failed");
        return -1;
    }
    
    // 设置套接字选项
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    return sockfd;
}
```

#### 超时处理
```c
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <string.h>

// 带超时的接收函数
ssize_t recv_timeout(int sockfd, void *buf, size_t len, int timeout_seconds) {
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(sockfd, &read_fds);
    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;
    
    int result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
    if (result == 0) {
        errno = ETIMEDOUT;
        return -1;  // 超时
    }
    
    if (result < 0) {
        return -1;  // 错误
    }
    
    return recv(sockfd, buf, len, 0);
}
```

#### 总结
良好的错误处理、资源管理和超时机制是网络编程成功的关键，可以提高程序的健壮性和用户体验。

---

## 综合实例

### 实现目标
创建一个支持TCP和UDP的简单文件传输服务器，客户端可以选择协议类型进行文件传输。

### 涉及知识点
- TCP和UDP协议特性
- 套接字编程基础
- 文件I/O操作
- 多协议支持
- 错误处理和资源管理
- 性能优化

### 代码实现

#### 服务器端代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define TCP_PORT 8080
#define UDP_PORT 8081
#define BUFFER_SIZE 4096

typedef struct {
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    char protocol[10];
} client_info_t;

// TCP文件传输处理
void *handle_tcp_client(void *arg) {
    client_info_t *info = (client_info_t *)arg;
    char buffer[BUFFER_SIZE];
    char filename[256];
    int file_fd;
    ssize_t bytes_received, bytes_read;
    
    // 接收文件名
    bytes_received = recv(info->sockfd, filename, sizeof(filename) - 1, 0);
    if (bytes_received <= 0) {
        goto cleanup;
    }
    filename[bytes_received] = '\0';
    
    printf("TCP客户端请求文件: %s\n", filename);
    
    // 打开文件
    file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        char error_msg[] = "File not found";
        send(info->sockfd, error_msg, strlen(error_msg), 0);
        goto cleanup;
    }
    
    // 发送文件内容
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        if (send(info->sockfd, buffer, bytes_read, 0) == -1) {
            break;
        }
    }
    
    close(file_fd);
    
cleanup:
    close(info->sockfd);
    free(info);
    pthread_detach(pthread_self());
    return NULL;
}

// UDP文件传输处理
void handle_udp_client(int sockfd, struct sockaddr_in *client_addr, socklen_t client_len) {
    char buffer[BUFFER_SIZE];
    char filename[256];
    int file_fd;
    ssize_t bytes_received, bytes_read;
    
    // 接收文件名
    bytes_received = recvfrom(sockfd, filename, sizeof(filename) - 1, 0,
                             (struct sockaddr*)client_addr, &client_len);
    if (bytes_received <= 0) {
        return;
    }
    filename[bytes_received] = '\0';
    
    printf("UDP客户端请求文件: %s\n", filename);
    
    // 打开文件
    file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        char error_msg[] = "File not found";
        sendto(sockfd, error_msg, strlen(error_msg), 0,
               (struct sockaddr*)client_addr, client_len);
        return;
    }
    
    // 发送文件内容
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        if (sendto(sockfd, buffer, bytes_read, 0,
                   (struct sockaddr*)client_addr, client_len) == -1) {
            break;
        }
    }
    
    close(file_fd);
}

int main() {
    int tcp_sockfd, udp_sockfd;
    struct sockaddr_in tcp_addr, udp_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;
    
    // 创建TCP套接字
    tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sockfd == -1) {
        perror("TCP socket failed");
        return 1;
    }
    
    // 设置TCP地址
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_port = htons(TCP_PORT);
    tcp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // 绑定TCP套接字
    if (bind(tcp_sockfd, (struct sockaddr*)&tcp_addr, sizeof(tcp_addr)) == -1) {
        perror("TCP bind failed");
        close(tcp_sockfd);
        return 1;
    }
    
    // 监听TCP连接
    if (listen(tcp_sockfd, 5) == -1) {
        perror("TCP listen failed");
        close(tcp_sockfd);
        return 1;
    }
    
    // 创建UDP套接字
    udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sockfd == -1) {
        perror("UDP socket failed");
        close(tcp_sockfd);
        return 1;
    }
    
    // 设置UDP地址
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_port = htons(UDP_PORT);
    udp_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // 绑定UDP套接字
    if (bind(udp_sockfd, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) == -1) {
        perror("UDP bind failed");
        close(tcp_sockfd);
        close(udp_sockfd);
        return 1;
    }
    
    printf("文件传输服务器启动\n");
    printf("TCP端口: %d, UDP端口: %d\n", TCP_PORT, UDP_PORT);
    
    while (1) {
        fd_set read_fds;
        struct timeval timeout;
        
        FD_ZERO(&read_fds);
        FD_SET(tcp_sockfd, &read_fds);
        FD_SET(udp_sockfd, &read_fds);
        
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int max_fd = (tcp_sockfd > udp_sockfd) ? tcp_sockfd : udp_sockfd;
        
        int result = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (result == -1) {
            perror("select failed");
            break;
        }
        
        if (result == 0) {
            continue;  // 超时
        }
        
        // 处理TCP连接
        if (FD_ISSET(tcp_sockfd, &read_fds)) {
            int client_sockfd = accept(tcp_sockfd, (struct sockaddr*)&client_addr, &client_len);
            if (client_sockfd != -1) {
                client_info_t *info = malloc(sizeof(client_info_t));
                info->sockfd = client_sockfd;
                info->client_addr = client_addr;
                info->client_len = client_len;
                strcpy(info->protocol, "TCP");
                
                pthread_create(&tid, NULL, handle_tcp_client, info);
            }
        }
        
        // 处理UDP数据
        if (FD_ISSET(udp_sockfd, &read_fds)) {
            handle_udp_client(udp_sockfd, &client_addr, client_len);
        }
    }
    
    close(tcp_sockfd);
    close(udp_sockfd);
    return 0;
}
```

### 运行与调试
```bash
# 编译服务器
gcc -o file_server file_server.c -lpthread

# 编译客户端
gcc -o file_client file_client.c

# 运行服务器
./file_server

# 运行客户端（在另一个终端）
./file_client
```

### 常见问题与解答
- **Q: TCP和UDP哪个更适合文件传输？**
- A: TCP更适合文件传输，因为它提供可靠传输保证数据完整性。

- **Q: 如何提高文件传输性能？**
- A: 可以增加缓冲区大小、使用多线程、启用TCP_NODELAY选项等。

- **Q: UDP传输文件时如何处理丢包？**
- A: 需要在应用层实现确认机制和重传逻辑。

- **Q: 如何支持大文件传输？**
- A: 使用分块传输、断点续传、进度显示等技术。

### 参考资料
- 《TCP/IP详解 卷1：协议》
- 《UNIX网络编程 卷1：套接字联网API》
- RFC 793 (TCP协议规范)
- RFC 768 (UDP协议规范) 