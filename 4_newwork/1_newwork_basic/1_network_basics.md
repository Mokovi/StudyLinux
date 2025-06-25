# 一、网络基础知识

## 目录
- [1.1 网络通信概述](#11-网络通信概述)
- [1.2 OSI 七层模型](#12-osi-七层模型)
- [1.3 TCP/IP 四层模型](#13-tcpip-四层模型)
- [1.4 IP 地址与子网掩码](#14-ip-地址与子网掩码)
- [1.5 DNS 域名解析系统](#15-dns-域名解析系统)
- [1.6 ARP 地址解析协议](#16-arp-地址解析协议)
- [1.7 MAC 地址与数据链路层](#17-mac-地址与数据链路层)

---

## 1.1 网络通信概述

网络通信是计算机之间进行数据交换的基础，理解网络通信原理是进行网络编程的前提。

#### 基本概念
- **网络**：由多个计算机设备通过通信链路连接而成的系统
- **协议**：网络通信中双方必须遵循的规则和约定
- **数据包**：网络传输的基本单位，包含头部信息和数据内容

#### 通信模式
- **客户端-服务器模式**：最常见的网络通信模式
- **对等网络模式**：各节点地位平等，既是客户端又是服务器
- **广播模式**：一对多的通信方式

#### 网络分类
- **局域网（LAN）**：覆盖范围较小的网络
- **广域网（WAN）**：覆盖范围较大的网络
- **互联网**：全球性的网络互联

---

## 1.2 OSI 七层模型

OSI（Open Systems Interconnection）模型是国际标准化组织制定的网络通信标准，将网络通信分为七个层次。

#### 模型结构
```
应用层（Application Layer）
表示层（Presentation Layer）
会话层（Session Layer）
传输层（Transport Layer）
网络层（Network Layer）
数据链路层（Data Link Layer）
物理层（Physical Layer）
```

#### 各层功能说明
- **物理层**：负责比特流的传输，定义物理接口标准
- **数据链路层**：负责帧的传输，提供错误检测和纠正
- **网络层**：负责数据包的路由和转发
- **传输层**：负责端到端的可靠传输
- **会话层**：负责建立、管理和终止会话
- **表示层**：负责数据格式转换和加密解密
- **应用层**：为用户提供网络服务接口

#### 数据传输过程
```c
// 发送方：数据从上到下逐层封装
应用层数据 → 添加传输层头部 → 添加网络层头部 → 添加数据链路层头部 → 物理层传输

// 接收方：数据从下到上逐层解封装
物理层接收 → 移除数据链路层头部 → 移除网络层头部 → 移除传输层头部 → 应用层数据
```

#### 总结
OSI 七层模型为网络通信提供了标准化的分层架构，每层都有明确的职责分工，便于理解和实现网络协议。

---

## 1.3 TCP/IP 四层模型

TCP/IP 模型是互联网实际使用的网络协议栈，相比 OSI 模型更加实用。

#### 模型结构
```
应用层（Application Layer）
传输层（Transport Layer）
网络层（Internet Layer）
网络接口层（Network Interface Layer）
```

#### 各层协议
- **应用层**：HTTP、FTP、SMTP、DNS、SSH 等
- **传输层**：TCP、UDP 协议
- **网络层**：IP、ICMP、IGMP 协议
- **网络接口层**：以太网、WiFi、PPP 等

#### 与 OSI 模型对比
```
OSI 七层模型          TCP/IP 四层模型
应用层    ──────────┐
表示层    ──────────┤ 应用层
会话层    ──────────┘
传输层    ──────────  传输层
网络层    ──────────  网络层
数据链路层 ──────────┐
物理层    ──────────┤ 网络接口层
```

#### 数据封装示例
```c
// TCP/IP 数据包结构
┌─────────────────────────────────────────┐
│ 应用层数据 (HTTP 请求)                   │
├─────────────────────────────────────────┤
│ TCP 头部 (源端口、目标端口、序列号等)     │
├─────────────────────────────────────────┤
│ IP 头部 (源IP、目标IP、TTL等)            │
├─────────────────────────────────────────┤
│ 以太网头部 (源MAC、目标MAC、类型等)       │
└─────────────────────────────────────────┘
```

#### 总结
TCP/IP 模型是互联网的基础，理解其分层结构有助于网络编程中协议的选择和实现。

---

## 1.4 IP 地址与子网掩码

IP 地址是网络层用于标识主机的逻辑地址，子网掩码用于划分网络和主机部分。

#### IPv4 地址格式
```c
// IPv4 地址：32位，分为4个8位段
192.168.1.100

// 二进制表示
11000000.10101000.00000001.01100100
```

#### 地址分类
- **A 类**：1.0.0.0 - 126.255.255.255（默认掩码：255.0.0.0）
- **B 类**：128.0.0.0 - 191.255.255.255（默认掩码：255.255.0.0）
- **C 类**：192.0.0.0 - 223.255.255.255（默认掩码：255.255.255.0）
- **D 类**：224.0.0.0 - 239.255.255.255（多播地址）
- **E 类**：240.0.0.0 - 255.255.255.255（保留地址）

#### 子网掩码计算
```c
// 示例：IP 地址 192.168.1.100，子网掩码 255.255.255.0
IP 地址：     192.168.1.100
子网掩码：    255.255.255.0
网络地址：    192.168.1.0    (IP & 掩码)
主机地址：    0.0.0.100      (IP & ~掩码)
广播地址：    192.168.1.255  (网络地址 | ~掩码)
```

#### 特殊地址
- **127.0.0.1**：本地回环地址
- **0.0.0.0**：表示本网络
- **255.255.255.255**：广播地址
- **私有地址**：
  - 10.0.0.0/8
  - 172.16.0.0/12
  - 192.168.0.0/16

#### 网络地址计算示例
```c
// 计算网络地址和主机地址
#include <stdio.h>
#include <arpa/inet.h>

int main() {
    struct in_addr ip, mask, network, broadcast;
    
    // 设置 IP 地址和子网掩码
    inet_pton(AF_INET, "192.168.1.100", &ip);
    inet_pton(AF_INET, "255.255.255.0", &mask);
    
    // 计算网络地址
    network.s_addr = ip.s_addr & mask.s_addr;
    
    // 计算广播地址
    broadcast.s_addr = network.s_addr | ~mask.s_addr;
    
    char network_str[INET_ADDRSTRLEN];
    char broadcast_str[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, &network, network_str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &broadcast, broadcast_str, INET_ADDRSTRLEN);
    
    printf("网络地址: %s\n", network_str);
    printf("广播地址: %s\n", broadcast_str);
    
    return 0;
}
```

#### 总结
IP 地址和子网掩码是网络编程的基础，掌握地址分类和计算方法有助于网络配置和路由理解。

---

## 1.5 DNS 域名解析系统

DNS（Domain Name System）将人类可读的域名转换为机器可识别的 IP 地址。

#### DNS 工作原理
```
用户输入域名 → 本地 DNS 缓存查询 → 递归查询 → 权威 DNS 服务器 → 返回 IP 地址
```

#### DNS 记录类型
- **A 记录**：域名指向 IPv4 地址
- **AAAA 记录**：域名指向 IPv6 地址
- **CNAME 记录**：域名别名
- **MX 记录**：邮件服务器记录
- **NS 记录**：域名服务器记录
- **PTR 记录**：反向解析记录

#### DNS 查询过程
```c
// DNS 查询示例
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    struct hostent *host;
    char *domain = "www.example.com";
    
    // 域名解析
    host = gethostbyname(domain);
    if (host == NULL) {
        herror("gethostbyname failed");
        return 1;
    }
    
    printf("域名: %s\n", host->h_name);
    printf("IP 地址: %s\n", 
           inet_ntoa(*(struct in_addr*)host->h_addr_list[0]));
    
    return 0;
}
```

#### 常用 DNS 服务器
- **8.8.8.8**：Google DNS
- **1.1.1.1**：Cloudflare DNS
- **114.114.114.114**：国内 DNS

#### 总结
DNS 是互联网的基础服务，理解其工作原理有助于网络故障排查和性能优化。

---

## 1.6 ARP 地址解析协议

ARP（Address Resolution Protocol）用于将 IP 地址解析为 MAC 地址。

#### ARP 工作原理
```
1. 主机 A 要发送数据给主机 B（IP: 192.168.1.100）
2. 检查本地 ARP 缓存
3. 如果缓存中没有，发送 ARP 请求广播
4. 主机 B 收到请求后回复自己的 MAC 地址
5. 主机 A 将 MAC 地址存入缓存并发送数据
```

#### ARP 数据包结构
```c
// ARP 头部结构
struct arp_header {
    uint16_t hw_type;      // 硬件类型（以太网为1）
    uint16_t proto_type;   // 协议类型（IP为0x0800）
    uint8_t hw_len;        // 硬件地址长度（MAC为6）
    uint8_t proto_len;     // 协议地址长度（IP为4）
    uint16_t op;           // 操作码（1=请求，2=回复）
    uint8_t sender_mac[6]; // 发送方 MAC 地址
    uint32_t sender_ip;    // 发送方 IP 地址
    uint8_t target_mac[6]; // 目标 MAC 地址
    uint32_t target_ip;    // 目标 IP 地址
};
```

#### ARP 缓存管理
```c
// 查看 ARP 缓存
$ arp -a

// 添加静态 ARP 条目
$ arp -s 192.168.1.100 00:11:22:33:44:55

// 删除 ARP 条目
$ arp -d 192.168.1.100
```

#### ARP 编程示例
```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <net/if_arp.h>

int main() {
    int sockfd;
    struct sockaddr_in addr;
    struct arpreq arp_req;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        return 1;
    }
    
    // 设置 ARP 请求
    memset(&arp_req, 0, sizeof(arp_req));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("192.168.1.100");
    memcpy(&arp_req.arp_pa, &addr, sizeof(addr));
    strcpy(arp_req.arp_dev, "eth0");
    
    // 查询 ARP 条目
    if (ioctl(sockfd, SIOCGARP, &arp_req) == 0) {
        unsigned char *mac = (unsigned char *)arp_req.arp_ha.sa_data;
        printf("MAC 地址: %02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    } else {
        printf("ARP 查询失败\n");
    }
    
    close(sockfd);
    return 0;
}
```

#### 总结
ARP 协议是局域网通信的基础，理解其工作原理有助于网络故障排查和网络安全分析。

---

## 1.7 MAC 地址与数据链路层

MAC（Media Access Control）地址是数据链路层的物理地址，用于局域网内的设备识别。

#### MAC 地址格式
```c
// MAC 地址：48位，分为6个字节
00:11:22:33:44:55

// 二进制表示
00000000:00010001:00100010:00110011:01000100:01010101
```

#### MAC 地址分类
- **单播地址**：第一个字节的最低位为0
- **多播地址**：第一个字节的最低位为1
- **广播地址**：FF:FF:FF:FF:FF:FF

#### 厂商标识
```c
// 前3个字节为厂商标识（OUI）
00:11:22 - 某厂商A
AA:BB:CC - 某厂商B
```

#### MAC 地址操作
```c
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    struct ifreq ifr;
    int sockfd;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        return 1;
    }
    
    // 获取接口信息
    strcpy(ifr.ifr_name, "eth0");
    
    // 获取 MAC 地址
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == 0) {
        unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        printf("MAC 地址: %02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    
    // 获取 IP 地址
    if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0) {
        struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
        printf("IP 地址: %s\n", inet_ntoa(addr->sin_addr));
    }
    
    close(sockfd);
    return 0;
}
```

#### 数据链路层帧格式
```c
// 以太网帧结构
┌─────────────────────────────────────────┐
│ 前导码 (7字节)                          │
├─────────────────────────────────────────┤
│ 帧起始定界符 (1字节)                     │
├─────────────────────────────────────────┤
│ 目标 MAC 地址 (6字节)                    │
├─────────────────────────────────────────┤
│ 源 MAC 地址 (6字节)                      │
├─────────────────────────────────────────┤
│ 类型/长度 (2字节)                        │
├─────────────────────────────────────────┤
│ 数据 (46-1500字节)                       │
├─────────────────────────────────────────┤
│ 帧校验序列 (4字节)                       │
└─────────────────────────────────────────┘
```

#### 总结
MAC 地址是数据链路层的核心概念，理解其格式和用途有助于网络编程中的数据包处理。

---

## 综合实例

### 实现目标
创建一个网络信息查询工具，能够显示本机的网络配置信息，包括 IP 地址、MAC 地址、子网掩码等。

### 涉及知识点
- IP 地址与子网掩码计算
- MAC 地址获取
- 网络接口信息查询
- 系统调用 ioctl
- 网络地址转换函数

### 代码实现
```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void print_network_info(const char *interface) {
    struct ifreq ifr;
    int sockfd;
    
    // 创建套接字
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        return;
    }
    
    printf("=== 网络接口: %s ===\n", interface);
    
    // 获取 MAC 地址
    strcpy(ifr.ifr_name, interface);
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == 0) {
        unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
        printf("MAC 地址: %02x:%02x:%02x:%02x:%02x:%02x\n",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    
    // 获取 IP 地址
    if (ioctl(sockfd, SIOCGIFADDR, &ifr) == 0) {
        struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
        printf("IP 地址: %s\n", inet_ntoa(addr->sin_addr));
    }
    
    // 获取子网掩码
    if (ioctl(sockfd, SIOCGIFNETMASK, &ifr) == 0) {
        struct sockaddr_in *mask = (struct sockaddr_in *)&ifr.ifr_netmask;
        printf("子网掩码: %s\n", inet_ntoa(mask->sin_addr));
    }
    
    // 获取广播地址
    if (ioctl(sockfd, SIOCGIFBRDADDR, &ifr) == 0) {
        struct sockaddr_in *broadcast = (struct sockaddr_in *)&ifr.ifr_broadaddr;
        printf("广播地址: %s\n", inet_ntoa(broadcast->sin_addr));
    }
    
    // 获取接口状态
    if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) == 0) {
        printf("接口状态: %s\n", 
               (ifr.ifr_flags & IFF_UP) ? "启用" : "禁用");
    }
    
    printf("\n");
    close(sockfd);
}

int main() {
    printf("网络信息查询工具\n");
    printf("================\n\n");
    
    // 查询常见网络接口
    print_network_info("lo");      // 回环接口
    print_network_info("eth0");    // 以太网接口
    print_network_info("wlan0");   // 无线网卡接口
    
    return 0;
}
```

### 运行与调试
```bash
# 编译
gcc -o network_info network_info.c

# 运行
./network_info

# 调试
gdb ./network_info
```

### 常见问题与解答
- **Q: 为什么某些接口信息获取失败？**
- A: 可能是因为接口不存在或权限不足，需要以 root 权限运行。

- **Q: 如何获取所有网络接口？**
- A: 可以使用 `getifaddrs()` 函数遍历所有网络接口。

- **Q: MAC 地址显示为全零怎么办？**
- A: 可能是接口未启用或驱动问题，检查网络接口状态。

### 参考资料
- 《TCP/IP 详解 卷1：协议》
- Linux 网络编程相关手册页
- RFC 826 (ARP 协议规范) 