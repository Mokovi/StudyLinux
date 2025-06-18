# 进程基础

## 快速跳转

* [一、进程基础简介](#一进程基础简介)
* [二、进程查看与监控](#二进程查看与监控)
* [三、进程控制与管理](#三进程控制与管理)
* [四、作业控制](#四作业控制)
* [五、服务进程管理](#五服务进程管理)
* [六、综合示例：守护、孤儿与僵尸进程](#六综合示例守护孤儿与僵尸进程)

---

## 一、进程基础简介

* **进程（Process）**：程序的一次运行实例，包括唯一的 PID、父子关系、运行状态、资源限额等。
* **守护进程（Daemon）**：通常在后台运行，无控制终端，常用于提供系统服务（如 `sshd`, `cron`）。
* **孤儿进程（Orphan）**：父进程退出后，子进程被 `init`（PID 1）收养，仍能继续运行。
* **僵尸进程（Zombie）**：子进程结束但父进程未调用 `wait` 或 `waitpid` 收集其退出状态，保留在进程表中占用少量资源。

---

## 二、进程查看与监控

> **命令分类**：静态快照、动态监控、树状结构、按条件查找

| 命令       | 功能             | 语法示例                                        |
| -------- | -------------- | ------------------------------------------- |
| `ps`     | 静态快照           | `ps aux`, `ps -ef --sort=-%cpu`             |
| `top`    | 动态实时监控         | `top`, `top -o %MEM`, `top -p 1234`         |
| `htop`   | 增强版 `top`（需安装） | `sudo apt install htop`<br>`htop -u bob`    |
| `pstree` | 以树状结构显示进程及子进程  | `pstree -p`, `pstree -u alice`              |
| `pgrep`  | 按名称或属性查找 PID   | `pgrep -u root`, `pgrep -f "nginx: worker"` |

### ps

```bash
# 所有进程详细信息
ps aux
# 标准格式显示
ps -ef
# 查看某用户进程
ps -u username
# 查看指定 PID
ps -p 1234
# 按内存降序排序
ps axo pid,user,%mem,command --sort=-%mem
```

### top / htop

```bash
# top: 默认界面
top
# top: 按 CPU 占用排序
top -o %CPU
# htop: 安装并运行
sudo apt install htop
htop
```

### pstree

```bash
# 默认树状展示
pstree
# 显示 PID
pstree -p
# 某用户进程树
pstree -u root
```

### pgrep

```bash
# 查找所有 nginx 进程 PID
pgrep nginx
# 精确匹配命令行
pgrep -f "python3 server.py"
# 查找某用户的所有进程
pgrep -u nobody
```

---

## 三、进程控制与管理

> **命令分类**：信号发送、后台/前台切换、优先级调整

| 命令                          | 功能        | 示例                              |
| --------------------------- | --------- | ------------------------------- |
| `kill`                      | 发送信号      | `kill -15 1234`, `kill -9 1234` |
| `renice`                    | 调整进程优先级   | `renice +5 -p 1234`             |
| `nohup`                     | 忽略挂断信号，后台 | `nohup myapp &`                 |
| `daemonize`<sup>（需安装）</sup> | 轻量级守护进程工具 | `daemonize -- myapp`            |

### kill

```bash
# 发送 SIGTERM（15）
kill 1234
# 强制 SIGKILL（9）
kill -9 1234
# 重载配置（SIGHUP）
kill -HUP $(pgrep nginx)
# 列出所有信号
kill -l
```

### renice

```bash
# 将 PID 1234 的优先级调高 5（值越小优先级越高）
renice -n +5 -p 1234
# 批量调整
renice -n -10 -u www-data
```

### 后台常驻

```bash
# 忽略挂断，标准输出重定向到 nohup.out
nohup ./backup.sh > backup.log 2>&1 &
# 使用 daemonize
sudo apt install daemonize
daemonize --name=mydaemon --output=/var/log/mydaemon.log /usr/local/bin/myapp
```

---

## 四、作业控制

> **命令分类**：查看、挂起、后台、恢复

| 命令        | 功能            | 示例        |
| --------- | ------------- | --------- |
| `jobs`    | 列出当前 Shell 作业 | `jobs -l` |
| `bg`      | 后台继续挂起作业      | `bg %1`   |
| `fg`      | 前台恢复后台作业      | `fg %2`   |
| `kill %n` | 终止指定作业        | `kill %3` |

```bash
# 查看所有作业及 PID
jobs -l

# 将编号为 1 的挂起作业放后台运行
bg %1

# 将编号为 2 的后台作业带到前台
fg %2

# 直接终止作业（相当于 kill -TERM PID）
kill %3
```

---

## 五、服务进程管理

> **命令分类**：systemd 常用子命令

| 命令                         | 功能         | 示例                                    |
| -------------------------- | ---------- | ------------------------------------- |
| `systemctl status`         | 查看服务状态     | `systemctl status sshd`               |
| `systemctl start/stop`     | 启动 / 停止 服务 | `systemctl start apache2`             |
| `systemctl restart`        | 重启服务       | `systemctl restart mysql`             |
| `systemctl enable/disable` | 设置/取消开机启动  | `systemctl enable docker`             |
| `systemctl list-units`     | 列出所有服务     | `systemctl list-units --type=service` |

```bash
# 查看 nginx 服务详情
systemctl status nginx

# 启动并设置开机自启
systemctl start nginx
systemctl enable nginx

# 停止并取消自启
systemctl stop nginx
systemctl disable nginx

# 重载配置（对于支持的服务）
systemctl reload nginx
```


## 六、综合示例：部署“实时日志收集”脚本

### 场景：

在服务器上部署并管理一个自定义的“实时日志收集脚本”，涵盖守护进程启动、作业控制、进程监控与管理，以及作为 systemd 服务管理。

假设你有一个脚本 `log_collector.sh`，内容如下：
```bash
#!/bin/bash
# log_collector.sh
LOG_DIR="/var/log/myapp"
mkdir -p "$LOG_DIR"
while true; do
  # 收集最新的应用日志到归档
  cp /var/log/myapp/app.log "$LOG_DIR/app-$(date +%Y%m%d-%H%M%S).log"
  sleep 60
done
```

### 1. 后台常驻（守护进程）

使用 `nohup` + `&` + `disown`：

```bash
chmod +x log_collector.sh
nohup ./log_collector.sh > /dev/null 2>&1 &
COLLECTOR_PID=$!
disown $COLLECTOR_PID
echo "日志收集脚本已以 PID=$COLLECTOR_PID 后台运行"
```

此时脚本成为守护进程，重登出也不会被挂断。

### 2. 作业控制：查看、挂起、恢复

如果你不想一开始就 `disown`，直接用 `&` 启动，Shell 会把它当作作业：

```bash
./log_collector.sh &
# 查看作业
jobs -l
# 暂停（Ctrl+Z），再放后台
bg %1
# 恢复到前台查看输出
fg %1
```

### 3. 进程监控

* **查看单个进程**

  ```bash
  ps -p $COLLECTOR_PID -o pid,ppid,%cpu,%mem,cmd
  ```
* **按名称查找**

  ```bash
  pgrep -fl log_collector.sh
  ```
* **动态监控**

  ```bash
  top -p $COLLECTOR_PID
  ```

### 4. 调优与管理

* **调整优先级**

  ```bash
  renice -n +10 -p $COLLECTOR_PID
  ```
* **重载脚本**（使用 SIGHUP）

  ```bash
  kill -HUP $COLLECTOR_PID
  ```
* **停止脚本**

  ```bash
  kill $COLLECTOR_PID        # 发送 SIGTERM
  kill -9 $COLLECTOR_PID     # 强制 SIGKILL
  ```

### 5. 使用 systemd 管理

将脚本封装成 systemd 服务，持久可控：

1. 创建单元文件 `/etc/systemd/system/log-collector.service`：

   ```ini
   [Unit]
   Description=MyApp Log Collector
   After=network.target

   [Service]
   Type=simple
   ExecStart=/usr/local/bin/log_collector.sh
   Restart=on-failure
   User=root
   StandardOutput=null
   StandardError=/var/log/log-collector.err

   [Install]
   WantedBy=multi-user.target
   ```

2. 重新加载并启动：

   ```bash
   systemctl daemon-reload
   systemctl start log-collector
   systemctl enable log-collector
   ```

3. 管理与监控：

   ```bash
   systemctl status log-collector
   journalctl -u log-collector -f
   systemctl restart log-collector
   systemctl stop log-collector
   ```

---

通过上述示例，你既体验了守护方式（`nohup`/`disown`），也掌握了作业控制（`jobs`/`bg`/`fg`），进程监控（`ps`/`top`/`pgrep`），信号管理与优先级调优（`kill`/`renice`），以及更规范的 systemd 服务管理，全面涵盖了进程管理的常用操作。

