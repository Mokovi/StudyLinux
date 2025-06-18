# Linux网络基础命令

## 快速跳转

* [一、网络基础简介](#一网络基础简介)
* [二、连接诊断命令](#二连接诊断命令)
* [三、网络配置与管理](#三网络配置与管理)
* [四、远程访问与传输](#四远程访问与传输)
* [五、下载与数据传输工具](#五下载与数据传输工具)
* [六、安全与监控命令](#六安全与监控命令)
* [七、综合实例：从连接测试到远程传输](#七综合实例从连接测试到远程传输)

---

## 一、网络基础简介

在 Linux 中，网络是操作系统与外部世界通信的核心部分。网络命令用于测试连通性、配置接口、抓包排障、传输数据、管理防火墙等。Linux 提供一整套功能强大的命令行工具来满足网络运维与开发需求。

* **IP 地址、子网掩码、网关、DNS** 是网络的基础配置参数
* 网络接口常见为 `eth0`、`wlan0`、`lo`（本地回环）
* 网络状态主要包括：连接建立、端口监听、数据收发、路由策略、安全防护

---

## 二、连接诊断命令

| 命令           | 功能        | 示例                        |
| ------------ | --------- | ------------------------- |
| `ping`       | 测试目标主机连通性 | `ping -c 4 baidu.com`     |
| `traceroute` | 路由路径追踪    | `traceroute 8.8.8.8`      |
| `nslookup`   | DNS 查询    | `nslookup www.google.com` |

### ping

```bash
ping -c 4 192.168.1.1          # ping 指定 IP，发送 4 个包
ping -i 0.5 www.baidu.com      # 每 0.5 秒发送一个包
```

### traceroute

```bash
traceroute www.example.com     # 显示从本机到目标主机经过的每一跳
```

### nslookup

```bash
nslookup www.qq.com            # 查看域名解析的 IP 地址
nslookup                      # 进入交互模式
> server 8.8.8.8              # 切换 DNS 服务器
```

---

## 三、网络配置与管理

| 命令         | 功能              | 示例                             |
| ---------- | --------------- | ------------------------------ |
| `ip`       | 显示或配置网络参数       | `ip addr`, `ip route`          |
| `ifconfig` | 传统网络配置（已过时）     | `ifconfig eth0`, `ifconfig -a` |
| `route`    | 路由表查看与设置        | `route -n`                     |
| `netstat`  | 查看连接、路由等信息      | `netstat -tuln`, `netstat -rn` |
| `ss`       | 替代 netstat，查看连接 | `ss -tunlp`, `ss -s`           |

### ip

```bash
ip addr show                    # 查看所有接口的 IP 地址
ip link set eth0 down/up        # 启用/禁用接口
ip route                        # 查看路由表
```

### ifconfig（兼容旧系统）

```bash
ifconfig                        # 查看所有启用接口
ifconfig eth0 down/up           # 关闭/启用接口
```

### route

```bash
route -n                        # 显示当前路由表
route add default gw 192.168.1.1  # 添加默认网关
```

### ss

```bash
ss -tunlp                       # 显示所有监听端口及 PID
ss -s                           # 网络摘要统计
```

---

## 四、远程访问与传输

| 命令     | 功能      | 示例                              |
| ------ | ------- | ------------------------------- |
| `ssh`  | 安全远程登录  | `ssh user@192.168.1.10 -p 22`   |
| `scp`  | 安全文件传输  | `scp file.txt user@host:/path/` |
| `sftp` | 交互式文件传输 | `sftp user@host`                |

### ssh

```bash
ssh user@hostname              # 登录远程主机
ssh -p 2222 user@host          # 指定端口
ssh -i ~/.ssh/id_rsa user@host  # 指定密钥文件
```

### scp

```bash
scp local.txt user@remote:/tmp/  # 上传文件
scp -r dir/ user@host:/tmp/      # 上传目录
```

### sftp

```bash
sftp user@remote
> ls
> put localfile
> get remotefile
```

---

## 五、下载与数据传输工具

| 命令     | 功能       | 示例                                      |
| ------ | -------- | --------------------------------------- |
| `curl` | 下载网页/API | `curl https://httpbin.org/get`          |
| `wget` | 文件下载     | `wget -c https://example.com/large.iso` |

### curl

```bash
curl https://api.ipify.org               # 获取公网 IP
curl -O https://example.com/file.zip     # 使用原始文件名下载
curl -d "key=value" https://post.site    # POST 数据
```

### wget

```bash
wget https://example.com/file.tar.gz     # 普通下载
wget -c file.iso                         # 断点续传
wget -r https://example.com/docs/        # 递归下载
```

---

## 六、安全与监控命令

| 命令         | 功能      | 示例                           |
| ---------- | ------- | ---------------------------- |
| `tcpdump`  | 网络抓包    | `tcpdump port 80 -nn`        |
| `iptables` | 防火墙规则管理 | `iptables -L`, `iptables -A` |

### tcpdump

```bash
tcpdump -i eth0                      # 抓取指定接口的数据包
tcpdump port 22                     # 抓取 SSH 端口数据
tcpdump -nn -X -s 0 tcp port 80     # 显示详细数据内容
```

### iptables

```bash
iptables -L                         # 查看所有规则
iptables -A INPUT -p tcp --dport 80 -j ACCEPT  # 允许端口 80
iptables -D INPUT 1                 # 删除第 1 条规则
```

> ⚠️ iptables 在部分系统已由 `nftables` 替代

---

## 七、综合实例：从连接测试到远程传输

### 场景说明：

模拟以下网络运维任务：

1. 检查外网是否连通
2. 查看本机监听端口
3. 追踪访问路由并解析目标 IP
4. 设置本机默认网关
5. 远程登录服务器传输文件
6. 使用 `curl` 获取远程接口数据
7. 抓包验证传输内容

### 操作步骤：

```bash
# 1. 检查连通性
ping -c 4 www.baidu.com

# 2. 查看监听端口和服务
ss -tunlp

# 3. 路由追踪及域名解析
traceroute www.baidu.com
nslookup www.baidu.com

# 4. 查看并设置路由
ip route
sudo ip route add default via 192.168.1.1

# 5. 使用 SSH 登录远程服务器并传文件
ssh user@192.168.1.100
scp result.log user@192.168.1.100:/home/user/

# 6. 使用 curl 请求接口
curl https://api.github.com/users/octocat

# 7. 抓取 HTTP 请求包
sudo tcpdump -i eth0 port 443 -w https_traffic.pcap
```

> 此综合流程涵盖：连接测试、端口查看、路由与配置、远程传输、HTTP 通信与抓包分析等全套常用命令

---

这套网络命令结构清晰、功能分类明确，并辅以详尽示例与综合实战流程，适合网络学习、运维入门、排障实战等场景。若需要，我还可提供图解版本或交互式流程图。
