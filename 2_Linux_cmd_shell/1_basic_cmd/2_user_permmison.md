# Linux用户、组与权限管理

在Linux中，每一个文件和目录都有自己的访问权限，通过文件列表可以查看到。

 ![](../resource/1.png)

## RWX权限详解

 **0-9位说明**:
1)	第0位确定文件的类型(d,-,c,b)
2)	第1-3位确定所有者(该文件的所有者)拥有该文件的权限 --User
3)	第4-6位确定所属组(同用户组的)拥有该文件的权限--group
4)	第7-9位确定其它用户拥有该文件的权限==other

**rwx作用到文件**

- [ r ]代表可读(read): 可以读取,查看

- [ w ]代表可写(write): 可以修改,但是不代表可以删除该文件,删除一个文件的前提条件是对该文件所在的目录有写权限，才能删除该文件.

- [ x ]代表可执行(execute):可以被执行

**rwx作用到目录**

- [ r ]代表可读(read): 可以读取，ls查看目录内容

- [ w ]代表可写(write): 可以修改,目录内创建+删除+重命名目录

- [ x ]代表可执行(execute):可以进入该目录

**rwx用数字表示**

* r=4(即2²),w=2(即2¹),x=1(即2º)



## 快速跳转

* [一、用户/组管理](#一用户组管理)
* [二、密码与账号策略](#二密码与账号策略)
* [三、权限与所有者管理](#三权限与所有者管理)
* [四、提升/切换身份](#四提升切换身份)

---

## 一、用户/组管理

* `useradd`：创建新用户
* `groupadd`：创建新组
* `usermod`：修改用户属性
* `id`：显示用户身份信息
* `userdel`：删除用户
* `groupdel`：删除用户组

### useradd

```bash
useradd -m username              # -m：创建用户主目录 (/home/username)
useradd -r serviceuser           # -r：创建系统账户（UID < 1000）
useradd -s /usr/bin/zsh username # -s：指定登录 Shell
useradd -G wheel,developers username  # -G：附加用户到附加组列表
useradd -c "描述信息" username        # -c：添加备注/描述
```

#### 🐚 什么是“登录 Shell”？

Linux 中每个用户都有一个默认的命令行解释器（shell），用于执行命令、编写脚本等。常见的 shell 有：

| Shell | 路径                            | 特点                            |
| ----- | ----------------------------- | ----------------------------- |
| Bash  | `/bin/bash` 或 `/usr/bin/bash` | 默认、最常见                        |
| Zsh   | `/usr/bin/zsh`                | 更强的自动补全和提示，适合高级用户             |
| Sh    | `/bin/sh`                     | 最基本 shell，常用于脚本               |
| Fish  | `/usr/bin/fish`               | 现代、用户友好                       |
| Dash  | `/bin/dash`                   | 更快更小，Ubuntu 的 `/bin/sh` 链接指向它 |
| rbash | `/usr/bin/rbash`              | 受限版本，用于限制用户的某些操作，常用于提升系统安全性、创建受限用户环境|

用户登录时，系统会调用 `/etc/passwd` 文件中该用户对应的 shell 启动一个交互会话。

**✅ 查看已安装的 Shell**

```bash
cat /etc/shells
```

#### 显示所有用户：

```bash
cut -d: -f1 /etc/passwd | sort
```

`cut -d: -f1` 表示用冒号 `:` 分隔，取第一列（用户名）。

---

### groupadd

```bash
groupadd developers             # 创建组 developers
groupadd -g 2000 projectgrp     # -g：指定 GID
groupadd -r systemgrp           # -r：创建系统组 (GID < 1000)
```

显示所有用户组：

```bash
cut -d: -f1 /etc/group | sort
```

---

### usermod

```bash
usermod -aG sudo username        # -aG：附加用户到 sudo 组（保留原组）
usermod -G audio,video username  # -G：设置用户组列表（覆盖原组）
usermod -l newname oldname       # -l：修改登录名
usermod -d /new/home -m username # -d：更改并移动主目录; -m：移动
usermod -s /bin/bash username    # -s：设置登录 Shell
```

---

### id

```bash
id                              # 显示当前用户的 UID、GID 及所属组
id username                     # 显示指定用户的信息
id -u username                  # -u：仅显示 UID
id -g username                  # -g：仅显示 GID
id -G username                  # -G：仅显示所有组的 GID 列表
id -nG username                 # -nG：以名称而非数字显示所有组
```

---

### userdel

```bash
userdel username        # 删除用户（保留家目录和邮件）
userdel -r username     # 删除用户并移除其主目录 (/home/username) 及邮件
```

> **注意**：
>
> * `userdel` 不能删除当前登录的用户；
> * 建议先使用 `id username` 或 `getent passwd username` 确认用户信息；
> * 删除操作不可恢复，必要时先备份用户数据。

---

### groupdel

```bash
groupdel groupname      # 删除用户组
```

> **注意**：
>
> * 不能删除仍被用户作为主组或附属组的组；
> * 建议先使用 `getent group groupname` 确认组内成员；
> * 一般先删除相关用户，再删除该组。


---

## 二、密码与账号策略

* `passwd`：修改或设置用户密码
* `chage`：查看/修改密码过期策略
* `login.defs`：系统默认账号策略配置（编辑文件说明）

### passwd

```bash
passwd                         # 修改自身密码
passwd username                # 以 root 身份设置指定用户密码
passwd -l username             # -l：锁定账号（在 /etc/shadow 密码字段前加 !）
passwd -u username             # -u：解锁账号
passwd -e username             # -e：强制用户下次登录时更改密码
passwd --stdin username < passfile  # 从标准输入读取新密码（部分发行版）
```

### chage

```bash
chage -l username              # -l：列出用户的密码生效、过期信息
chage -m 1 username            # -m：密码最短使用天数 (MIN_DAYS)
chage -M 90 username           # -M：密码最大使用天数 (MAX_DAYS)
chage -W 7 username            # -W：过期前警告天数 (WARN_DAYS)
chage -I 30 username           # -I：过期后账号不可用天数 (INACTIVE)
```

### login.defs (编辑说明)

```text
# /etc/login.defs 定义系统级账号策略
PASS_MAX_DAYS   90   # 密码最长使用天数
PASS_MIN_DAYS   1    # 密码最短使用天数
PASS_WARN_AGE   7    # 密码即将过期警告天数
UID_MIN         1000 # 普通用户 UID 下限
GID_MIN         1000 # 普通组 GID 下限
```

---

## 三、权限与所有者管理

* `chmod`：修改文件/目录权限
* `chown`：修改文件/目录所有者和组
* `chgrp`：修改文件/目录所属组

### chmod

```bash
chmod +x script.sh            # +x：添加执行权限
chmod 755 script.sh          # 数字模式：rwxr-xr-x
chmod -R 700 dir/            # -R：递归应用到目录及子项
chmod u+rw,go-w file.txt       # 用户 u 添加读写权限，组 g 和其他 o 移除写权限
chmod a=r file.txt            # a=r：所有人只读
```

### chown

```bash
chown user:group file.txt      # 设置文件所有者 user，所属组 group
chown user file.txt            # 仅改变所有者
chown :group file.txt         # 仅改变组
chown -R user:group dir/       # -R：递归应用到目录及子项
```

### chgrp

```bash
chgrp group file.txt           # 设置文件所属组
chgrp -R group dir/          # -R：递归设置目录组
```

---

## 四、提升/切换身份

* `sudo`：以特权身份执行命令
* `visudo`：安全地编辑 /etc/sudoers
* `su`：切换用户身份

### sudo

```bash
sudo command                   # 以 root 权限执行 command
sudo -i                        # -i：以登录 shell 切换到 root（保留环境）
sudo -u username command       # -u：以指定用户身份运行
sudo -l                        # 列出当前用户可执行的 sudo 权限
```

### visudo

```bash
# 安全地编辑 sudoers 文件，自动语法检查
sudo visudo                   
# 若使用特定编辑器：
EDITOR=vim sudo visudo         # 指定 vim 编辑
```

### su

```bash
su -                          # 切换到 root 并读取 root 环境
su - username                 # 切换到 username 并读取其环境
su username                   # 切换到 username，不加载登录 Shell 环境
```
## 综合实例：开发者环境快速配置

**场景**：为新加入的开发者 Alice 创建一个项目账户，并配置其密码策略、目录结构、权限以及 sudo 权限。

```bash
# 1. 创建用户组 developers 组
sudo groupadd -g 1500 developers     # -g: 指定 GID

# 2. 创建用户 Alice，指定 zsh 作为登录 Shell，并加入 developers 组
sudo useradd -m -s /usr/bin/zsh -G developers alice  # -m: 建主目录；-s: 指 Shell；-G: 附加组

# 3. 设置初始密码并强制下次登录修改
sudo passwd alice                   # 交互式设置密码
sudo passwd -e alice                # -e: 过期，下次登录修改

# 4. 配置密码过期策略
sudo chage -m 1 alice               # -m: 密码最短使用间隔 1 天
sudo chage -M 60 alice              # -M: 密码最长使用 60 天
sudo chage -W 7 alice               # -W: 到期前 7 天警告

# 5. 创建项目目录并设置权限
sudo mkdir -p /opt/projects/myapp
sudo chown alice:developers /opt/projects/myapp  # 所有者:组
sudo chmod 750 /opt/projects/myapp              # owner=rwx, group=rx, others=

# 6. 配置 sudo 权限：编辑 /etc/sudoers
sudo visudo
# 添加：
# %developers ALL=(ALL) NOPASSWD: ALL

# 7. 验证配置
id alice                                # 查看 UID/GID/所属组
sudo -u alice -i                         # 切换到 alice 的登录环境
sudo ls /root                            # 以 sudo 执行，验证无需密码
```

**说明**：

| 环节      | 命令                                               | 参数说明                                        |
| ------- | ------------------------------------------------ | ------------------------------------------- |
| 用户组管理   | `groupadd -g 1500 developers`                    | `-g <GID>`: 指定新组的组 ID                       |
| 创建用户    | `useradd -m -s /usr/bin/zsh -G developers alice` | `-m`: 建主目录；`-s`: 指定登录 shell；`-G`: 附加组       |
| 设置密码    | `passwd alice`                                   | 交互式设置密码                                     |
| 强制修改密码  | `passwd -e alice`                                | `-e`: 密码过期，下次登录必须修改                         |
| 密码策略管理  | `chage -m 1 -M 60 -W 7 alice`                    | `-m`: 最短使用间隔；`-M`: 最长使用时间；`-W`: 警告天数        |
| 目录与权限管理 | `mkdir -p/chown/chmod /opt/projects/myapp`       | `-p`: 递归创建；`chown owner:group`; `chmod 750` |
| 提权/切换   | `visudo`, `sudo`, `su`                           | `visudo` 编辑 sudoers；`sudo`: 执行特权；`su`: 切换用户 |


