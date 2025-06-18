以下是对 Linux 系统命令的重构与补充，满足以下要求：

1. **总体介绍**：Linux 系统命令及相关理论概念
2. **分类与排序**：按功能分组，分类内按字母或用途排序
3. **快速跳转**：目录锚点
4. **常用功能扩展**：每个命令补充语法与示例
5. **综合实例**：一个 Bash 脚本示例，涵盖所有知识点
6. **补充命令**：增加若干常用系统运维命令

---

## 快速跳转

* [一、总体介绍](#一总体介绍)
* [二、命令分类](#二命令分类)

  * [1. 系统电源管理](#1系统电源管理)
  * [2. 系统信息查询](#2系统信息查询)
  * [3. 存储与文件系统](#3存储与文件系统)
  * [4. 时间与日历](#4时间与日历)
  * [5. 帮助与别名](#5帮助与别名)
* [三、综合示例](#三综合示例)

---

## 一、总体介绍

Linux 提供了丰富的命令行工具，用以完成系统管理、资源监控、文件操作、网络配置等任务。

* **命令类型**

  * **内置命令**（Shell Built-in）：由 Shell 自身提供，执行速度快，例如 `cd`、`alias`、`help`。
  * **外部命令**（External）：存放于 `/bin`、`/usr/bin` 等目录，需要 fork + exec 调用，例如 `ls`、`df`、`shutdown`。
  * **别名**（Alias）：用户或系统定义的命令替换，便于快速输入；可用 `alias`/`unalias` 管理。
  * **函数**（Function）：Shell 脚本或交互式 Shell 中定义的多行命令集合。
* **执行流程**

  1. Shell 识别输入：检查是否为别名 → 内置 → 函数 → 外部程序。
  2. 若为外部：通过系统调用 `fork()` 创建子进程，`exec()` 加载并执行程序。
  3. Shell 等待返回或在后台继续运行，根据命令末尾的 `&` 决定前后台执行。

---

## 二、命令分类

### 1. 系统电源管理

| 命令         | 功能       | 语法                                                     |
| ---------- | -------- | ------------------------------------------------------ |
| `shutdown` | 定时关机/重启  | `shutdown [OPTIONS] TIME [“消息”]`<br>`shutdown +5 "升级"` |
| `reboot`   | 立即重启     | `reboot`                                               |
| `poweroff` | 立即关机     | `poweroff`                                             |
| `halt`     | 停止所有 CPU | `halt`                                                 |

```bash
# 5 分钟后重启并广播消息
shutdown -r +5 "系统维护"
# 立即关机
shutdown -h now
# 立即重启（不提示）
reboot
# 立即关机（无网络广播）
poweroff
```

### 2. 系统信息查询

| 命令           | 功能       | 语法示例                             |
| ------------ | -------- | -------------------------------- |
| `uname`      | 内核/架构信息  | `uname -a`                       |
| `hostname`   | 主机名查看/设置 | `hostnamectl set-hostname web01` |
| `uptime`     | 系统运行时间   | `uptime`                         |
| `whoami`     | 当前用户名    | `whoami`                         |
| `top`/`htop` | 实时资源监控   | `top`, `htop`                    |
| `free`       | 内存使用     | `free -h`                        |

```bash
# 查看完整系统信息
uname -a
# 查看并设置主机名
hostnamectl status
hostnamectl set-hostname db-server
# 查看系统运行多久、负载
uptime
# 当前用户
whoami
# 动态监控内存/CPU/进程
top
```

### 3. 存储与文件系统

| 命令       | 功能         | 语法示例                          |
| -------- | ---------- | ----------------------------- |
| `df`     | 磁盘分区使用     | `df -hT`                      |
| `du`     | 目录/文件大小    | `du -sh /var/log`             |
| `mount`  | 挂载文件系统     | `mount /dev/sdb1 /mnt/backup` |
| `umount` | 卸载文件系统     | `umount /mnt/backup`          |
| `lsblk`  | 块设备信息      | `lsblk -f`                    |
| `blkid`  | 分区 UUID/类型 | `blkid /dev/sdb1`             |

```bash
# 查看所有文件系统使用情况
df -hT
# 查看当前目录每个子目录大小
du -sh ./*
# 挂载并查看设备格式
mount /dev/sdc1 /media/usb
lsblk -f
# 卸载
umount /media/usb
```

### 4. 时间与日历

| 命令            | 功能        | 语法示例                                     |
| ------------- | --------- | ---------------------------------------- |
| `date`        | 显示/设置系统时间 | `date`, `date -s "2025-06-18 12:00"`     |
| `timedatectl` | 管理时区与 NTP | `timedatectl set-timezone Asia/Shanghai` |
| `cal`         | 日历        | `cal`, `cal -y`, `cal 2025`              |

```bash
# 显示当前日期时间
date
# 设置系统时间（需 root）
date -s "2025-06-18 14:30"
# 查看/修改时区
timedatectl status
timedatectl set-timezone America/Los_Angeles
# 显示整年日历
cal 2025
```

### 5. 帮助与别名

| 命令        | 功能              | 语法示例                              |
| --------- | --------------- | --------------------------------- |
| `man`     | 查看外部命令手册        | `man ls`                          |
| `info`    | GNU 工具信息        | `info coreutils 'ls invocation'`  |
| `help`    | 查看 Shell 内置命令帮助 | `help cd`                         |
| `alias`   | 定义/查看命令别名       | `alias ll='ls -alF --color=auto'` |
| `unalias` | 删除别名            | `unalias ll`                      |
| `type`    | 显示命令类别          | `type ls`, `type ll`              |
| `which`   | 显示可执行文件路径       | `which python3`                   |

```bash
# 查看 ls 手册
man ls
# 查看 bash 内置 cd 帮助
help cd
# 定义别名、查看所有别名
alias gs='git status'
alias
# 删除别名
unalias gs
# 查看命令是内置还是外部
type cd
type ls
type gs
# 查找可执行程序实际路径
which bash
```

## 三、综合实例

### 场景说明

每天凌晨 2:00 执行该脚本，完成以下任务：

1. **系统电源管理**：若上次备份失败次数超过限制，立即重启并通知管理员；
2. **系统信息查询**：记录当前内核版本、主机名、运行时长、负载；
3. **存储与文件系统**：

   * 检查根分区和备份分区剩余空间；
   * 如备份盘空间低于 10%，发送告警并退出；
   * 临时挂载网络备份盘 `/dev/sdb1` 到 `/mnt/backup`；
4. **时间与日历**：确保时区为 UTC，并校准 NTP；
5. **帮助与别名**：使用自定义别名简化长命令；
6. **日志与通知**：

   * 将所有输出写入 `/var/log/nightly_backup.log`；
   * 通过 `mail` 给运维群发通知。

---

```bash
#!/bin/bash
# nightly_backup.sh — 夜间自动备份与健康检查
# Crontab: 0 2 * * * /usr/local/bin/nightly_backup.sh

LOGFILE=/var/log/nightly_backup.log
ALERT_EMAIL=ops-team@example.com
BACKUP_SRC="/var/www /etc"
BACKUP_DEST="/mnt/backup/www_$(date +%F)"
DISK_WARN_PCT=10

# 1. 切换到 UTC 并同步时间
timedatectl set-timezone UTC
timedatectl set-ntp true

# 2. 定义别名（帮助 & 别名）
alias dfh='df -hT'
alias du1='du -sh'

# 3. 日志函数
log() {
  echo "$(date +'%F %T') $1" | tee -a "$LOGFILE"
}

# 4. 记录系统信息
log "=== 系统健康检查开始 ==="
log "内核版本: $(uname -r)"
log "主机名: $(hostnamectl --static)"
log "运行时长: $(uptime -p)"
log "当前负载: $(uptime | awk -F 'load average:' '{print $2}')"

# 5. 检查根分区剩余空间
ROOT_PCT=$(df / | tail -1 | awk '{print 100-$5}')
log "根分区可用: ${ROOT_PCT}%"
if [ "$ROOT_PCT" -lt "$DISK_WARN_PCT" ]; then
  log "ERROR: 根分区可用空间低于 ${DISK_WARN_PCT}%"
  echo "根分区空间不足：${ROOT_PCT}%" | mail -s "Backup FAILED: Low Root Space" "$ALERT_EMAIL"
  exit 1
fi

# 6. 挂载备份盘
if ! mountpoint -q /mnt/backup; then
  log "挂载备份盘 /dev/sdb1 -> /mnt/backup"
  mount /dev/sdb1 /mnt/backup || {
    log "ERROR: 挂载失败"
    echo "无法挂载备份盘" | mail -s "Backup FAILED: Mount Error" "$ALERT_EMAIL"
    exit 1
  }
fi

# 7. 检查备份盘剩余空间
BACKUP_PCT=$(dfh | awk '/\/mnt\/backup/ {print 100-$5}' | sed 's/%//')
log "备份盘可用: ${BACKUP_PCT}%"
if [ "$BACKUP_PCT" -lt "$DISK_WARN_PCT" ]; then
  log "ERROR: 备份盘可用空间低于 ${DISK_WARN_PCT}%"
  echo "备份盘空间不足：${BACKUP_PCT}%" | mail -s "Backup FAILED: Low Backup Space" "$ALERT_EMAIL"
  exit 1
fi

# 8. 执行备份（rsync 增量）
log "开始备份：${BACKUP_SRC} -> ${BACKUP_DEST}"
mkdir -p "$BACKUP_DEST"
rsync -aH --delete $BACKUP_SRC "$BACKUP_DEST" 2>&1 | tee -a "$LOGFILE"
RSYNC_STATUS=${PIPESTATUS[0]}
if [ "$RSYNC_STATUS" -ne 0 ]; then
  log "ERROR: rsync 失败 (状态码 $RSYNC_STATUS)"
  echo "rsync 失败，请查看日志" | mail -s "Backup FAILED: rsync Error" "$ALERT_EMAIL"
  FAIL_COUNT=$((FAIL_COUNT+1))
else
  log "备份完成成功"
fi

# 9. 卸载备份盘
log "卸载 /mnt/backup"
umount /mnt/backup

# 10. 失败次数检查 & 电源管理
if [ "$FAIL_COUNT" -ge 3 ]; then
  log "连续 3 次失败，计划重启"
  echo "连续三次备份失败，系统将重启" | mail -s "Backup ERROR: Rebooting" "$ALERT_EMAIL"
  shutdown -r now
fi

log "=== 系统健康检查与备份结束 ==="
```

#### 使用说明

1. **部署**：将脚本保存为 `/usr/local/bin/nightly_backup.sh` 并赋可执行权限：

   ```bash
   sudo chmod +x /usr/local/bin/nightly_backup.sh
   ```
2. **定时执行**：编辑 root 的 crontab：

   ```bash
   sudo crontab -e
   # 在文件末尾添加：
   0 2 * * * /usr/local/bin/nightly_backup.sh
   ```
3. **日志查看**：执行后日志写入 `/var/log/nightly_backup.log`，出现异常时邮件通知到运维组。

该示例将前面介绍的所有命令分类（电源管理、系统信息、存储管理、时间管理、帮助与别名）融入一个真实运维脚本，覆盖了常见的生产环境需求。

