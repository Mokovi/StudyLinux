# 二、Socket 编程

## 目录
- [2.1 Socket 编程概述](#21-socket-编程概述)
- [2.2 套接字创建与绑定](#22-套接字创建与绑定)
- [2.3 服务端监听与连接接收](#23-服务端监听与连接接收)
- [2.4 客户端连接](#24-客户端连接)
- [2.5 数据收发接口](#25-数据收发接口)
- [2.6 套接字选项设置](#26-套接字选项设置)
- [2.7 套接字关闭与资源释放](#27-套接字关闭与资源释放)

---

## 2.1 Socket 编程概述

Socket 是网络编程的核心接口，提供了进程间通信的抽象层，支持不同协议族的网络通信。

#### 基本概念
- **Socket**：网络通信的端点，包含IP地址和端口号
- **协议族**：定义通信协议的类型，如IPv4、IPv6、Unix域套接字等
- **套接字类型**：流套接字（TCP）、数据报套接字（UDP）、原始套接字
- **地址族**：定义地址格式，如AF_INET（IPv4）、AF_INET6（IPv6）

#### Socket 地址结构
```c
// IPv4 地址结构
struct sockaddr_in {
    sa_family_t sin_family;     // 地址族（AF_INET）
    in_port_t sin_port;         // 端口号（网络字节序）
    struct in_addr sin_addr;    // IP地址
    char sin_zero[8];           // 填充字节
};

// 通用地址结构
struct sockaddr {
    sa_family_t sa_family;      // 地址族
    char sa_data[14];           // 地址数据
};
```

#### 网络字节序
```c
// 字节序转换函数
uint16_t htons(uint16_t hostshort);    // 主机字节序转网络字节序（16位）
uint32_t htonl(uint32_t hostlong);     // 主机字节序转网络字节序（32位）
uint16_t ntohs(uint16_t netshort);     // 网络字节序转主机字节序（16位）
uint32_t ntohl(uint32_t netlong);      // 网络字节序转主机字节序（32位）
```

#### 总结
Socket 编程是网络通信的基础，理解其基本概念和地址结构是进行网络编程的前提。

---

## 2.2 套接字创建与绑定

套接字的创建和绑定是网络编程的第一步，为后续的通信建立基础。

#### 函数 `socket`
#### 语法
```c
int socket(int domain, int type, int protocol);
```

#### 参数说明
- `domain`：协议族，常用值：
  - `AF_INET`：IPv4 协议族
  - `AF_INET6`：IPv6 协议族
  - `AF_UNIX`：Unix 域套接字
- `type`：套接字类型：
  - `SOCK_STREAM`：流套接字（TCP）
  - `SOCK_DGRAM`：数据报套接字（UDP）
  - `SOCK_RAW`：原始套接字
- `protocol`：协议类型，通常设为 0（自动选择）
- **返回值**：成功返回套接字描述符，失败返回 -1 并设置 `errno`

#### 使用场景
`socket` 用于创建套接字，是网络通信的起点，所有网络操作都基于套接字描述符进行。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    
    // 创建 TCP 套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    printf("套接字创建成功，描述符: %d\n", sockfd);
    close(sockfd);
    return 0;
}
```

#### 总结
`socket` 函数是网络编程的基础，选择合适的协议族和套接字类型对后续通信至关重要。

---

#### 函数 `bind`
#### 语法
```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

#### 参数说明
- `sockfd`：套接字描述符
- `addr`：指向地址结构的指针
- `addrlen`：地址结构的长度
- **返回值**：成功返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`bind` 用于将套接字绑定到特定的IP地址和端口，服务端程序必须调用此函数。

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
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);                    // 端口号
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);       // 任意地址
    
    // 绑定套接字
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(sockfd);
        return 1;
    }
    
    printf("套接字绑定成功\n");
    close(sockfd);
    return 0;
}
```

#### 总结
`bind` 函数将套接字与地址绑定，是服务端程序必须的步骤，客户端通常不需要显式绑定。

---

## 2.3 服务端监听与连接接收

服务端需要监听连接请求并接受客户端连接，这是建立通信的关键步骤。

#### 函数 `listen`
#### 语法
```c
int listen(int sockfd, int backlog);
```

#### 参数说明
- `sockfd`：已绑定的套接字描述符
- `backlog`：连接队列的最大长度
- **返回值**：成功返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`listen` 用于将套接字设置为监听状态，等待客户端连接请求。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    
    // 开始监听
    if (listen(sockfd, 5) == -1) {
        perror("listen failed");
        close(sockfd);
        return 1;
    }
    
    printf("服务器开始监听端口 8080\n");
    close(sockfd);
    return 0;
}
```

#### 总结
`listen` 函数将套接字设置为被动监听状态，`backlog` 参数影响并发连接处理能力。

---

#### 函数 `accept`
#### 语法
```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

#### 参数说明
- `sockfd`：监听套接字描述符
- `addr`：用于存储客户端地址信息的结构指针（可为 NULL）
- `addrlen`：地址结构长度的指针（可为 NULL）
- **返回值**：成功返回新的套接字描述符，失败返回 -1 并设置 `errno`

#### 使用场景
`accept` 用于接受客户端连接，返回新的套接字描述符用于与客户端通信。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

int main() {
    int sockfd, clientfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN];
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    
    // 开始监听
    if (listen(sockfd, 5) == -1) {
        perror("listen failed");
        close(sockfd);
        return 1;
    }
    
    printf("等待客户端连接...\n");
    
    // 接受连接
    clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    if (clientfd == -1) {
        perror("accept failed");
        close(sockfd);
        return 1;
    }
    
    // 获取客户端IP地址
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    printf("客户端 %s:%d 已连接\n", client_ip, ntohs(client_addr.sin_port));
    
    close(clientfd);
    close(sockfd);
    return 0;
}
```

#### 总结
`accept` 函数是服务端处理客户端连接的核心，每次调用都会返回新的套接字描述符。

---

## 2.4 客户端连接

客户端需要主动连接到服务器，建立通信通道。

#### 函数 `connect`
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
`connect` 用于客户端主动连接到服务器，建立TCP连接或UDP关联。

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
    
    // 设置服务器IP地址
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        close(sockfd);
        return 1;
    }
    
    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
        close(sockfd);
        return 1;
    }
    
    printf("成功连接到服务器\n");
    close(sockfd);
    return 0;
}
```

#### 总结
`connect` 函数是客户端建立连接的关键，连接成功后即可进行数据收发。

---

## 2.5 数据收发接口

数据收发是网络通信的核心功能，支持面向连接和无连接两种模式。

#### 函数 `send`
#### 语法
```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

#### 参数说明
- `sockfd`：已连接的套接字描述符
- `buf`：要发送的数据缓冲区
- `len`：数据长度
- `flags`：发送标志：
  - `MSG_OOB`：发送带外数据
  - `MSG_DONTROUTE`：不经过路由表
  - `MSG_NOSIGNAL`：不产生SIGPIPE信号
- **返回值**：成功返回发送的字节数，失败返回 -1 并设置 `errno`

#### 使用场景
`send` 用于在已连接的套接字上发送数据，适用于TCP等面向连接的协议。

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
    char message[] = "Hello, Server!";
    ssize_t bytes_sent;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
        close(sockfd);
        return 1;
    }
    
    // 发送数据
    bytes_sent = send(sockfd, message, strlen(message), 0);
    if (bytes_sent == -1) {
        perror("send failed");
    } else {
        printf("发送了 %zd 字节数据\n", bytes_sent);
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`send` 函数是TCP通信中发送数据的主要接口，需要注意返回值可能小于请求发送的长度。

---

#### 函数 `recv`
#### 语法
```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

#### 参数说明
- `sockfd`：已连接的套接字描述符
- `buf`：接收数据缓冲区
- `len`：缓冲区长度
- `flags`：接收标志：
  - `MSG_PEEK`：查看数据但不移除
  - `MSG_OOB`：接收带外数据
  - `MSG_WAITALL`：等待接收完整数据
- **返回值**：成功返回接收的字节数，连接关闭返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`recv` 用于从已连接的套接字接收数据，适用于TCP等面向连接的协议。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

int main() {
    int sockfd, clientfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];
    ssize_t bytes_received;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    
    // 开始监听
    if (listen(sockfd, 5) == -1) {
        perror("listen failed");
        close(sockfd);
        return 1;
    }
    
    printf("等待客户端连接...\n");
    
    // 接受连接
    clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
    if (clientfd == -1) {
        perror("accept failed");
        close(sockfd);
        return 1;
    }
    
    // 接收数据
    bytes_received = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received == -1) {
        perror("recv failed");
    } else if (bytes_received == 0) {
        printf("客户端关闭连接\n");
    } else {
        buffer[bytes_received] = '\0';
        printf("接收到 %zd 字节数据: %s\n", bytes_received, buffer);
    }
    
    close(clientfd);
    close(sockfd);
    return 0;
}
```

#### 总结
`recv` 函数是TCP通信中接收数据的主要接口，返回值0表示连接已关闭。

---

#### 函数 `sendto`
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

#### 函数 `recvfrom`
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
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`recvfrom` 函数是UDP通信中接收数据的主要接口，可以获取发送方的地址信息。

---

## 2.6 套接字选项设置

套接字选项用于配置套接字的行为，如超时、缓冲区大小、地址重用等。

#### 函数 `setsockopt`
#### 语法
```c
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

#### 参数说明
- `sockfd`：套接字描述符
- `level`：选项级别：
  - `SOL_SOCKET`：套接字级别选项
  - `IPPROTO_TCP`：TCP协议选项
  - `IPPROTO_IP`：IP协议选项
- `optname`：选项名称
- `optval`：选项值指针
- `optlen`：选项值长度
- **返回值**：成功返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`setsockopt` 用于设置套接字选项，配置套接字的行为特性。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

int main() {
    int sockfd;
    int optval;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置地址重用选项
    optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt SO_REUSEADDR failed");
    } else {
        printf("地址重用选项设置成功\n");
    }
    
    // 设置发送超时
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("setsockopt SO_SNDTIMEO failed");
    } else {
        printf("发送超时设置成功\n");
    }
    
    // 设置接收超时
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("setsockopt SO_RCVTIMEO failed");
    } else {
        printf("接收超时设置成功\n");
    }
    
    // 设置TCP_NODELAY选项（禁用Nagle算法）
    optval = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) == -1) {
        perror("setsockopt TCP_NODELAY failed");
    } else {
        printf("TCP_NODELAY选项设置成功\n");
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`setsockopt` 函数用于配置套接字选项，合理设置选项可以优化网络性能。

---

#### 函数 `getsockopt`
#### 语法
```c
int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
```

#### 参数说明
- `sockfd`：套接字描述符
- `level`：选项级别
- `optname`：选项名称
- `optval`：用于存储选项值的缓冲区
- `optlen`：选项值长度的指针
- **返回值**：成功返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`getsockopt` 用于获取套接字选项的当前值，用于调试和状态检查。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int sockfd;
    int optval;
    socklen_t optlen = sizeof(optval);
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 获取发送缓冲区大小
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &optval, &optlen) == -1) {
        perror("getsockopt SO_SNDBUF failed");
    } else {
        printf("发送缓冲区大小: %d 字节\n", optval);
    }
    
    // 获取接收缓冲区大小
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen) == -1) {
        perror("getsockopt SO_RCVBUF failed");
    } else {
        printf("接收缓冲区大小: %d 字节\n", optval);
    }
    
    // 获取套接字类型
    if (getsockopt(sockfd, SOL_SOCKET, SO_TYPE, &optval, &optlen) == -1) {
        perror("getsockopt SO_TYPE failed");
    } else {
        printf("套接字类型: %d\n", optval);
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
`getsockopt` 函数用于查询套接字选项，有助于了解套接字的当前配置状态。

---

## 2.7 套接字关闭与资源释放

正确关闭套接字和释放资源是网络编程中的重要环节。

#### 函数 `close`
#### 语法
```c
int close(int fd);
```

#### 参数说明
- `fd`：文件描述符（包括套接字描述符）
- **返回值**：成功返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`close` 用于关闭套接字，释放系统资源，是网络编程中资源清理的标准方法。

#### 示例代码
```c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int sockfd;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    printf("套接字创建成功，描述符: %d\n", sockfd);
    
    // 关闭套接字
    if (close(sockfd) == -1) {
        perror("close failed");
        return 1;
    }
    
    printf("套接字已关闭\n");
    return 0;
}
```

#### 总结
`close` 函数是关闭套接字的标准方法，确保及时释放系统资源。

---

#### 函数 `shutdown`
#### 语法
```c
int shutdown(int sockfd, int how);
```

#### 参数说明
- `sockfd`：套接字描述符
- `how`：关闭方式：
  - `SHUT_RD`：关闭读端
  - `SHUT_WR`：关闭写端
  - `SHUT_RDWR`：关闭读写两端
- **返回值**：成功返回 0，失败返回 -1 并设置 `errno`

#### 使用场景
`shutdown` 用于优雅地关闭套接字连接，可以选择性地关闭读端或写端。

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
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    // 连接到服务器
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
        close(sockfd);
        return 1;
    }
    
    printf("连接建立成功\n");
    
    // 关闭写端（发送FIN）
    if (shutdown(sockfd, SHUT_WR) == -1) {
        perror("shutdown SHUT_WR failed");
    } else {
        printf("写端已关闭\n");
    }
    
    // 关闭读端
    if (shutdown(sockfd, SHUT_RD) == -1) {
        perror("shutdown SHUT_RD failed");
    } else {
        printf("读端已关闭\n");
    }
    
    // 完全关闭套接字
    close(sockfd);
    printf("套接字已完全关闭\n");
    
    return 0;
}
```

#### 总结
`shutdown` 函数提供了更精细的连接控制，可以实现优雅的连接关闭。

---

## 综合实例

### 实现目标
创建一个简单的TCP聊天服务器和客户端，支持多客户端连接和消息广播功能。

### 涉及知识点
- 套接字创建与绑定
- 服务端监听与连接接收
- 客户端连接
- 数据收发接口
- 套接字选项设置
- 多客户端处理
- 错误处理与资源释放

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
#include <pthread.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 8080

typedef struct {
    int sockfd;
    struct sockaddr_in addr;
    char nickname[32];
} client_t;

client_t *clients[MAX_CLIENTS];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_client(client_t *client) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == NULL) {
            clients[i] = client;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->sockfd == sockfd) {
            clients[i] = NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void broadcast_message(char *message, int sender_sockfd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] && clients[i]->sockfd != sender_sockfd) {
            if (send(clients[i]->sockfd, message, strlen(message), 0) < 0) {
                perror("Failed to send message");
            }
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    client_t *client = (client_t *)arg;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE + 64];
    
    // 发送欢迎消息
    sprintf(message, "欢迎 %s 加入聊天室！\n", client->nickname);
    broadcast_message(message, client->sockfd);
    
    while (1) {
        int bytes_received = recv(client->sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("客户端 %s 断开连接\n", client->nickname);
            break;
        }
        
        buffer[bytes_received] = '\0';
        sprintf(message, "%s: %s", client->nickname, buffer);
        printf("%s", message);
        broadcast_message(message, client->sockfd);
    }
    
    // 发送离开消息
    sprintf(message, "%s 离开了聊天室\n", client->nickname);
    broadcast_message(message, client->sockfd);
    
    remove_client(client->sockfd);
    close(client->sockfd);
    free(client);
    
    pthread_detach(pthread_self());
    return NULL;
}

int main() {
    int server_sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t tid;
    
    // 创建套接字
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置地址重用选项
    int opt = 1;
    setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // 设置地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // 绑定套接字
    if (bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind failed");
        close(server_sockfd);
        return 1;
    }
    
    // 开始监听
    if (listen(server_sockfd, 5) == -1) {
        perror("listen failed");
        close(server_sockfd);
        return 1;
    }
    
    printf("聊天服务器启动，监听端口 %d\n", PORT);
    printf("等待客户端连接...\n");
    
    while (1) {
        // 接受连接
        client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            perror("accept failed");
            continue;
        }
        
        // 创建客户端结构
        client_t *client = malloc(sizeof(client_t));
        client->sockfd = client_sockfd;
        client->addr = client_addr;
        
        // 获取客户端昵称
        char nickname[32];
        int bytes_received = recv(client_sockfd, nickname, sizeof(nickname) - 1, 0);
        if (bytes_received > 0) {
            nickname[bytes_received] = '\0';
            strcpy(client->nickname, nickname);
        } else {
            strcpy(client->nickname, "Anonymous");
        }
        
        // 添加客户端到列表
        add_client(client);
        
        // 创建线程处理客户端
        if (pthread_create(&tid, NULL, handle_client, (void*)client) != 0) {
            perror("Failed to create thread");
            close(client_sockfd);
            free(client);
        }
        
        printf("客户端 %s 已连接 (%s:%d)\n", 
               client->nickname, 
               inet_ntoa(client_addr.sin_addr), 
               ntohs(client_addr.sin_port));
    }
    
    close(server_sockfd);
    return 0;
}
```

#### 客户端代码
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define PORT 8080

void *receive_messages(void *sockfd) {
    int client_sockfd = *(int*)sockfd;
    char buffer[BUFFER_SIZE];
    
    while (1) {
        int bytes_received = recv(client_sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("服务器断开连接\n");
            break;
        }
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    
    return NULL;
}

int main() {
    int client_sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char nickname[32];
    pthread_t receive_thread;
    /0
    // 创建套接字
    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("socket failed");
        return 1;
    }
    
    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // 获取服务器IP地址
    printf("请输入服务器IP地址: ");
    char server_ip[16];
    fgets(server_ip, sizeof(server_ip), stdin);
    server_ip[strcspn(server_ip, "\n")] = 0;
    
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(client_sockfd);
        return 1;
    }
    
    // 连接到服务器
    if (connect(client_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect failed");
        close(client_sockfd);
        return 1;
    }
    
    printf("成功连接到服务器\n");
    
    // 输入昵称
    printf("请输入您的昵称: ");
    fgets(nickname, sizeof(nickname), stdin);
    nickname[strcspn(nickname, "\n")] = 0;
    
    // 发送昵称到服务器
    send(client_sockfd, nickname, strlen(nickname), 0);
    
    // 创建接收消息线程
    if (pthread_create(&receive_thread, NULL, receive_messages, &client_sockfd) != 0) {
        perror("Failed to create receive thread");
        close(client_sockfd);
        return 1;
    }
    
    printf("开始聊天（输入 'quit' 退出）:\n");
    
    // 发送消息循环
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
        
        if (send(client_sockfd, buffer, strlen(buffer), 0) < 0) {
            perror("Failed to send message");
            break;
        }
    }
    
    close(client_sockfd);
    return 0;
}
```

### 运行与调试
```bash
# 编译服务器
gcc -o chat_server chat_server.c -lpthread

# 编译客户端
gcc -o chat_client chat_client.c -lpthread

# 运行服务器
./chat_server

# 运行客户端（在另一个终端）
./chat_client
```

### 常见问题与解答
- **Q: 为什么服务器无法绑定端口？**
- A: 可能是端口已被占用，使用 `SO_REUSEADDR` 选项或更换端口。

- **Q: 客户端连接失败怎么办？**
- A: 检查服务器IP地址和端口号是否正确，确保防火墙允许连接。

- **Q: 如何支持更多客户端？**
- A: 增加 `MAX_CLIENTS` 的值，但要注意系统资源限制。

- **Q: 消息发送失败怎么处理？**
- A: 检查网络连接状态，实现重传机制或连接恢复逻辑。

### 参考资料
- 《UNIX网络编程 卷1：套接字联网API》
- Linux socket 编程手册页
- POSIX 线程编程指南 