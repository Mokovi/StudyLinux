# 进程基础与管理

## 快速跳转目录
* [一、进程基础理论](#一进程基础理论)
* [二、进程查看与监控](#二进程查看与监控)
  - [ps](#ps)
  - [top/htop](#tophtop)
  - [pstree](#pstree)
  - [pgrep](#pgrep)
* [三、进程控制与管理](#三进程控制与管理)
  - [kill](#kill)
  - [renice](#renice)
  - [nohup/daemonize](#nohupdaemonize)
* [四、作业控制](#四作业控制)
  - [jobs](#jobs)
  - [bg](#bg)
  - [fg](#fg)
  - [kill %n](#kill-n)
* [五、服务进程管理](#五服务进程管理)
  - [systemctl](#systemctl)
* [六、综合实例：守护、孤儿与僵尸进程](#六综合实例守护孤儿与僵尸进程)

---

## 一、进程基础理论

- **进程（Process）**：程序的一次运行实例，包括唯一的 PID、父子关系、运行状态、资源限额等。
- **守护进程（Daemon）**：通常在后台运行，无控制终端，常用于提供系统服务（如 `sshd`, `cron`）。
- **孤儿进程（Orphan）**：父进程退出后，子进程被 `init`（PID 1）收养，仍能继续运行。
- **僵尸进程（Zombie）**：子进程结束但父进程未调用 `wait` 或 `waitpid` 收集其退出状态，保留在进程表中占用少量资源。

---

## 二、进程查看与监控

> **命令分类**：静态快照、动态监控、树状结构、按条件查找

### 2.1 命令 `ps`
#### 基本用法
```bash
ps aux
ps -ef
ps -u username
ps -p 1234
ps axo pid,user,%mem,command --sort=-%mem
```
#### 参数说明
- `a`：显示所有用户的进程
- `u`：以用户为主显示
- `x`：显示无控制终端的进程
- `-e`：等同于 -A，显示所有进程
- `-f`：全格式
- `-p`：指定PID
- `--sort`：排序

#### 使用场景
查看系统进程快照、筛选特定用户/进程、按资源排序。

#### 总结
`ps` 是最常用的进程快照工具，适合静态分析。

---

### 2.2 命令 `top` / `htop`
#### 基本用法
```bash
top
top -o %CPU
sudo apt install htop
htop
```
#### 参数说明
- `-o`：指定排序字段

#### 使用场景
动态监控进程资源消耗，交互式管理。

#### 总结
`top`/`htop` 适合实时监控和交互操作。

---

### 2.3 命令 `pstree`
#### 基本用法
```bash
pstree
pstree -p
pstree -u root
```
#### 参数说明
- `-p`：显示PID
- `-u`：显示用户

#### 使用场景
树状查看进程父子关系。

#### 总结
`pstree` 适合分析进程层级结构。

---

### 2.4 命令 `pgrep`
#### 基本用法
```bash
pgrep nginx
pgrep -f "python3 server.py"
pgrep -u nobody
```
#### 参数说明
- `-f`：匹配完整命令行
- `-u`：指定用户

#### 使用场景
按条件查找进程PID。

#### 总结
`pgrep` 用于批量查找和脚本自动化。

---

## 三、进程控制与管理

> **命令分类**：信号发送、后台/前台切换、优先级调整

### 3.1 命令 `kill`
#### 基本用法
```bash
kill 1234
kill -9 1234
kill -HUP $(pgrep nginx)
kill -l
```
#### 参数说明
- 默认发送 SIGTERM (15)
- `-9`：SIGKILL，强制终止
- `-HUP`：重载配置
- `-l`：列出所有信号

#### 使用场景
终止、重载、信号管理进程。

#### 总结
`kill` 是进程信号管理的基础命令。

---

### 3.2 命令 `renice`
#### 基本用法
```bash
renice -n +5 -p 1234
renice -n -10 -u www-data
```
#### 参数说明
- `-n`：nice值，越小优先级越高
- `-p`：指定PID
- `-u`：指定用户

#### 使用场景
调整进程优先级，优化资源分配。

#### 总结
`renice` 用于动态调整进程调度优先级。

---

### 3.3 命令 `nohup` / `daemonize`
#### 基本用法
```bash
nohup ./backup.sh > backup.log 2>&1 &
sudo apt install daemonize
daemonize --name=mydaemon --output=/var/log/mydaemon.log /usr/local/bin/myapp
```
#### 参数说明
- `nohup`：忽略挂断信号
- `daemonize`：轻量级守护进程工具

#### 使用场景
让进程后台常驻、脱离终端。

#### 总结
`nohup`/`daemonize` 适合脚本/服务后台运行。

---

## 四、作业控制

> **命令分类**：查看、挂起、后台、恢复

### 4.1 命令 `jobs`
#### 基本用法
```bash
jobs -l
```
#### 参数说明
- `-l`：显示PID

#### 使用场景
查看当前Shell作业及状态。

#### 总结
`jobs` 用于管理Shell作业。

---

### 4.2 命令 `bg`
#### 基本用法
```bash
bg %1
```
#### 参数说明
- `%1`：作业编号

#### 使用场景
将挂起作业放后台运行。

#### 总结
`bg` 用于恢复挂起作业到后台。

---

### 4.3 命令 `fg`
#### 基本用法
```bash
fg %2
```
#### 参数说明
- `%2`：作业编号

#### 使用场景
将后台作业带到前台。

#### 总结
`fg` 用于前台恢复作业。

---

### 4.4 命令 `kill %n`
#### 基本用法
```bash
kill %3
```
#### 参数说明
- `%3`：作业编号

#### 使用场景
终止指定作业。

#### 总结
`kill %n` 用于终止Shell作业。

---

## 五、服务进程管理

> **命令分类**：systemd 常用子命令

### 5.1 命令 `systemctl`
#### 基本用法
```bash
systemctl status nginx
systemctl start nginx
systemctl enable nginx
systemctl stop nginx
systemctl disable nginx
systemctl reload nginx
systemctl list-units --type=service
```
#### 参数说明
- `status`：查看服务状态
- `start/stop/restart`：启动/停止/重启
- `enable/disable`：设置/取消开机自启
- `reload`：重载配置
- `list-units`：列出所有服务

#### 使用场景
管理 systemd 服务进程。

#### 总结
`systemctl` 是现代Linux服务管理核心命令。

---

## 六、综合实例：守护、孤儿与僵尸进程

### 实现目标
部署并管理一个“实时日志收集”脚本，涵盖守护进程启动、作业控制、进程监控与管理，以及 systemd 服务管理。

### 涉及命令
- nohup
- jobs
- bg
- fg
- ps
- pgrep
- top
- renice
- kill
- systemctl

### 命令实现
```bash
# 1. 后台常驻（守护进程）
chmod +x log_collector.sh
nohup ./log_collector.sh > /dev/null 2>&1 &
COLLECTOR_PID=$!
disown $COLLECTOR_PID
echo "日志收集脚本已以 PID=$COLLECTOR_PID 后台运行"

# 2. 作业控制
./log_collector.sh &
jobs -l
bg %1
fg %1

# 3. 进程监控
ps -p $COLLECTOR_PID -o pid,ppid,%cpu,%mem,cmd
pgrep -fl log_collector.sh
top -p $COLLECTOR_PID

# 4. 调优与管理
renice -n +10 -p $COLLECTOR_PID
kill -HUP $COLLECTOR_PID
kill $COLLECTOR_PID
kill -9 $COLLECTOR_PID

# 5. 使用 systemd 管理
systemctl daemon-reload
systemctl start log-collector
systemctl enable log-collector
systemctl status log-collector
journalctl -u log-collector -f
systemctl restart log-collector
systemctl stop log-collector
```

### 运行与调试
- 逐步执行上述命令，观察每步输出。
- 检查脚本是否后台常驻、作业控制是否生效、systemd 服务是否可控。

### 常见问题与解答
- Q: nohup 后台进程如何彻底脱离终端？
- A: 配合 disown 命令。
- Q: systemd 服务日志如何查看？
- A: 使用 journalctl -u 服务名。

### 参考资料
- man 手册
- Linux 命令大全

---

