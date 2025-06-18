# Linux 基础命令及 Shell

## 1. 常用命令详解

### 文件、文本操作
| 命令       | 功能                          | 示例                          |
|------------|-------------------------------|-------------------------------|
| `ls`       | 列出目录内容                  | `ls -l`                       |
| `cp`       | 复制文件/目录                 | `cp -r dir1 dir2`             |
| `mv`       | 移动/重命名文件               | `mv old.txt new.txt`          |
| `rm`       | 删除文件                      | `rm -rf dir`                  |
| `find`     | 搜索文件                      | `find / -name "*.log"`        |
| `tree`     | 树状显示目录结构              | `tree -L 2`                   |
| `cat`      | 查看/拼接文件                 | `cat file1 file2 > merged`    |
| `tar`      | 打包/解压文件                 | `tar -xvf backup.tar`         |
| `gzip`     | 压缩文件(.gz)                 | `gzip -9 file`                |
| `zip`      | 创建zip压缩包                 | `zip -r archive.zip dir/`     |
| `mkdir`    | 创建目录                      | `mkdir -p dir/subdir`         |
| `stat`     | 显示文件元数据                | `stat file.txt`               |
| `diff`     | 比较文件差异                  | `diff file1 file2`            |
| `ln`       | 创建文件链接                  | `ln -s target link_name`      |
| `cd`       | 切换工作目录                  | `cd ~/projects`               |
| `touch`    | 创建空文件/更新时间戳         | `touch newfile.txt`           |
| `more`     | 分页查看文件(只进)            | `more longfile.txt`           |
| `less`     | 分页查看文件(可退)            | `less /var/log/syslog`        |
| `locate`   | 快速文件搜索(需updatedb)      | `locate myfile.txt`           |
| `grep`     | 文本搜索                      | `grep -rin "error" /var/log`  |
| `sed`      | 流编辑器                      | `sed 's/foo/bar/g' file.txt`  |
| `head`     | 显示文件头部                  | `head -n 20 file.log`         |
| `tail`     | 显示文件尾部                  | `tail -f /var/log/syslog`     |
| `sort`     | 排序                          | `sort -u names.txt`           |
| `uniq`     | 去重                          | `sort file.txt \| uniq`        |
| `wc`       | 统计行数/字数                 | `wc -l file.txt`              |
| `echo`     | 输出文本/变量                 | `echo $PATH`                  |

### 用户/组/权限管理
| 命令         | 功能                          | 示例                          |
|--------------|-------------------------------|-------------------------------|
| `useradd`    | 创建用户                      | `useradd -m newuser`          |
| `groupadd`   | 创建组                        | `groupadd developers`         |
| `usermod`    | 修改用户属性                  | `usermod -aG sudo username`   |
| `passwd`     | 修改密码                      | `passwd`                      |
| `id`         | 显示用户身份信息              | `id`                          |
| `sudo`       | 特权执行                      | `sudo apt update`             |
| `visudo`     | 安全编辑sudo配置              | `visudo`                      |
| `chage`      | 修改密码策略                  | `chage -l user`               |
| `chmod`      | 修改权限                      | `chmod +x script.sh`          |
| `chown`      | 修改所有者                    | `chown user:group file`       |
| `su`         | 切换用户身份                  | `su - username`               |

### 进程管理
| 命令       | 功能                          | 示例                          |
|------------|-------------------------------|-------------------------------|
| `ps`       | 查看进程快照                  | `ps aux`                      |
| `top`      | 动态进程监控                  | `top -o %MEM`                 |
| `htop`     | 增强版top                     | `htop`                        |
| `kill`     | 终止进程                      | `kill -9 1234`                |
| `jobs`     | 查看后台任务                  | `jobs -l`                     |
| `bg`       | 后台继续任务                  | `bg %1`                       |
| `fg`       | 前台恢复任务                  | `fg %2`                       |
| `pstree`   | 树状显示进程                  | `pstree -p`                   |
| `systemctl`| 管理系统服务                  | `systemctl restart nginx`     |
| `pgrep`    | 按名查找进程ID                | `pgrep -f "python"`           |

### 网络命令
| 命令        | 功能                          | 示例                          |
|-------------|-------------------------------|-------------------------------|
| `ping`      | 测试网络连通性                | `ping -c 4 google.com`        |
| `ss`        | 查看网络连接                  | `ss -tunlp`                   |
| `netstat`   | 网络统计(旧版)                | `netstat -tuln`               |
| `curl`      | 数据传输工具                  | `curl -O https://example.com/file` |
| `wget`      | 文件下载                      | `wget -c https://example.com/large.iso` |
| `ip`        | 网络配置                      | `ip addr show`                |
| `traceroute`| 路由追踪                      | `traceroute 8.8.8.8`          |
| `ssh`       | 安全远程登录                  | `ssh user@host -p 2222`       |
| `scp`       | 安全文件传输                  | `scp file.txt user@remote:/path` |
| `ifconfig`  | 网络接口配置(传统)            | `ifconfig eth0`               |
| `route`     | 查看/修改路由表               | `route -n`                    |
| `nslookup`  | DNS查询                       | `nslookup example.com`        |
| `tcpdump`   | 网络抓包工具                  | `tcpdump port 80`             |
| `iptables`  | 防火墙规则管理                | `iptables -L`                 |

### 包管理
| 命令       | 适用系统          | 功能                | 示例                    |
|------------|-------------------|---------------------|-------------------------|
| `apt`      | Debian/Ubuntu     | 包管理工具          | `apt install nginx`     |
| `yum`      | RHEL/CentOS       | 包管理工具          | `yum remove httpd`      |
| `dnf`      | Fedora/RHEL8+     | 新一代yum           | `dnf update`            |
| `rpm`      | RPM系             | 包安装              | `rpm -ivh package.rpm`  |
| `dpkg`     | Debian系          | 包安装              | `dpkg -i package.deb`   |
| `pacman`   | Arch/Manjaro      | 包管理              | `pacman -Syu`           |
| `snap`     | 跨发行版          | 容器化包            | `snap install chromium` |

### 管道与重定向
| 符号/命令  | 功能                          | 示例                          |
|------------|-------------------------------|-------------------------------|
| `\|`        | 管道                          | `cat log.txt \| grep "ERROR"`  |
| `>`        | 覆盖输出重定向                | `ls > files.txt`              |
| `>>`       | 追加输出重定向                | `echo "new" >> log.txt`       |
| `<`        | 输入重定向                    | `sort < input.txt`            |
| `2>`       | 错误输出重定向                | `cmd 2> errors.log`           |
| `2>&1`     | 错误输出合并到标准输出        | `cmd > output.log 2>&1`       |
| `&>`       | 合并所有输出(Bash)            | `cmd &> all_output.log`       |
| `tee`      | 双重输出                      | `ls \| tee files.txt`           |
| `xargs`    | 参数传递                      | `find . -name "*.tmp" \| xargs rm` |

### 系统命令
| 命令         | 功能                               | 示例                                                     |
| ---------- | -------------------------------- | ------------------------------------------------------ |
| `shutdown` | 关机/重启                            | `shutdown +5 "系统升级"`                                   |
| `reboot`   | 立即重启                             | `reboot`                                               |
| `clear`    | 清屏                               | `clear`                                                |
| `date`     | 显示/设置时间                          | `date -s "2024-01-01 08:00"`                           |
| `df`       | 磁盘空间                             | `df -hT`                                               |
| `du`       | 目录大小                             | `du -sh *`                                             |
| `free`     | 内存使用                             | `free -m`                                              |
| `uname`    | 系统信息                             | `uname -r`                                             |
| `mount`    | 挂载文件系统                           | `mount /dev/sdb1 /mnt/data`                            |
| `man`      | 查看命令手册                           | `man ls`                                               |
| `help`     | 显示内置命令帮助                         | `help cd`                                              |
| `cal`      | 显示日历                             | `cal -y`                                               |
| `alias`    | 定义（或查看）命令别名                      | `alias ll='ls -alF --color=auto'`                      |
| `unalias`  | 删除已定义的别名                         | `unalias ll`                                           |
| `type`     | 查看命令的类型（外部命令 / 内置命令 / 别名 / 函数 等） | `type ll`  → `ll is aliased to 'ls -alF --color=auto'` |


> **常用组合技巧**：  
> `find . -type f -name "*.log" | xargs grep -i "error"`  
> `tar -czvf backup-$(date +%F).tar.gz /data 2>&1 | tee backup.log`  
> `ps aux | grep '[n]ginx' | awk '{print $2}' | xargs kill -9`

## 2. Shell 脚本

   * 基础：变量、条件判断、循环、函数
   * 进阶：数组、正则匹配、环境变量
   * 调试：`set -x`, `set -e`
   * 定时任务：`crontab`, `at`

## 3. Shell 常用工具

   * 文本处理：`awk`, `sed`, `grep`, `cut`, `sort`, `uniq`
   * 日志分析：结合 `grep`/`awk`
   * 批量操作：`xargs`, `parallel`

## 4. 实战案例与练习

   * 日志自动归档：按日期切割、压缩
   * 性能监控脚本：CPU/内存/磁盘报警
   * 自动化部署：批量拉取代码、编译、发布





