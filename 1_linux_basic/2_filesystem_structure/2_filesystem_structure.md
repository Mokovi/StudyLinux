# Linux文件系统结构详解

## 文件系统层次结构标准（FHS）

### 根目录（/）
- 所有其他目录的起点
- 包含系统启动所需的基本文件

### 重要系统目录

![](../../resource/Linux1.jpg)

#### /etc
- **用途**：系统配置文件目录
- **重要文件**：
  - `/etc/passwd`：用户账户信息
  - `/etc/shadow`：用户密码信息
  - `/etc/group`：用户组信息
  - `/etc/fstab`：文件系统挂载信息

#### /var
- **用途**：可变数据目录
- **重要子目录**：
  - `/var/log`：系统日志
  - `/var/www`：Web服务器文件
  - `/var/mail`：邮件文件
  - `/var/cache`：应用程序缓存

#### /usr
- **用途**：用户程序目录
- **重要子目录**：
  - `/usr/bin`：用户命令
  - `/usr/sbin`：系统管理命令
  - `/usr/lib`：程序库文件
  - `/usr/local`：本地安装的软件

#### /proc
- **用途**：进程信息文件系统
- **重要文件**：
  - `/proc/cpuinfo`：CPU信息
  - `/proc/meminfo`：内存信息
  - `/proc/net`：网络信息
  - `/proc/[pid]`：进程信息

#### /sys
- **用途**：系统设备信息
- **重要内容**：
  - 设备驱动信息
  - 内核参数
  - 系统状态

#### /dev
- **用途**：设备文件目录
- **重要设备**：
  - `/dev/sda`：硬盘设备
  - `/dev/tty`：终端设备
  - `/dev/null`：空设备
  - `/dev/zero`：零设备

#### /home
- **用途**：用户主目录
- **特点**：
  - 每个用户独立目录
  - 存储用户个人文件
  - 用户配置文件

## 文件系统操作

### 基本命令
```bash
# 查看目录结构
ls -l /etc

# 查看文件内容
cat /etc/passwd

# 查看目录大小
du -sh /var

# 查找文件
find / -name "filename"
```

### 权限管理
```bash
# 修改文件权限
chmod 755 file

# 修改文件所有者
chown user:group file
```

## 最佳实践

### 文件组织
1. 遵循FHS标准
2. 保持目录结构清晰
3. 定期清理临时文件

### 安全建议
1. 定期备份重要数据
2. 合理设置文件权限
3. 监控系统日志

## 参考资料
- Filesystem Hierarchy Standard: https://refspecs.linuxfoundation.org/FHS_3.0/fhs/index.html
- Linux Documentation Project: https://tldp.org/ 
- 菜鸟教程: https://www.runoob.com/linux/linux-system-contents.html