# 包管理

## 快速跳转

* [一、包管理基础简介](#一包管理基础简介)
* [二、分类与命令一览](#二分类与命令一览)

  * [2.1 Debian系（apt / dpkg）](#21-debian系apt--dpkg)
  * [2.2 RPM系（yum / dnf / rpm / zypper）](#22-rpm系yum--dnf--rpm--zypper)
  * [2.3 Arch系（pacman）](#23-arch系pacman)
  * [2.4 通用与沙箱（snap / flatpak / brew）](#24-通用与沙箱snap--flatpak--brew)
* [三、常用操作详解](#三常用操作详解)

  * [3.1 更新与升级](#31-更新与升级)
  * [3.2 安装与删除](#32-安装与删除)
  * [3.3 源管理（添加/移除 PPA / Repository）](#33-源管理添加移除-ppa--repository)
  * [3.4 查询与锁版本](#34-查询与锁版本)
* [四、综合示例：多发行版包管理脚本](#四综合示例多发行版包管理脚本)

---

## 一、包管理基础简介

* **包（Package）**：打包好的二进制或源码文件集合，包含可执行文件、库、元数据（名称、版本、依赖、描述等）。
* **仓库（Repository）**：集中存放软件包的服务器或镜像，客户端通过索引文件（`Packages.gz`、`repodata` 等）获取可用软件列表。
* **依赖管理（Dependency Management）**：自动检测并安装包所需的其他包，避免“依赖地狱”。
* **源码 vs 二进制包（Source vs Binary）**：源码包需编译、可定制；二进制包即装即用。
* **签名与校验（Signing & Verification）**：保证包来源可信、防篡改。

---

## 二、分类与命令一览

### 2.1 Debian系（apt / dpkg）

| 命令          | 功能      | 示例                                |
| ----------- | ------- | --------------------------------- |
| `apt`       | 高级包管理工具 | `apt update && apt install nginx` |
| `apt-get`   | 经典命令行工具 | `apt-get remove --purge apache2`  |
| `dpkg`      | 低级包安装管理 | `dpkg -i package.deb`             |
| `apt-cache` | 本地包信息查询 | `apt-cache show curl`             |

### 2.2 RPM系（yum / dnf / rpm / zypper）

| 命令       | 适用系统          | 功能     | 示例                      |
| -------- | ------------- | ------ | ----------------------- |
| `yum`    | RHEL/CentOS   | 经典包管理  | `yum install httpd`     |
| `dnf`    | Fedora/RHEL8+ | 新一代包管理 | `dnf upgrade --refresh` |
| `rpm`    | RPM 系         | 低级包安装  | `rpm -Uvh package.rpm`  |
| `zypper` | openSUSE      | 包管理    | `zypper install vim`    |

### 2.3 Arch系（pacman）

| 命令           | 功能        | 示例                                           |
| ------------ | --------- | -------------------------------------------- |
| `pacman`     | 包管理（滚动发行） | `pacman -Syu`, `pacman -Rns firefox`         |
| `pacman-key` | 密钥管理      | `pacman-key --init && pacman-key --populate` |

### 2.4 通用与沙箱（snap / flatpak / brew）

| 命令        | 功能                    | 示例                                      |
| --------- | --------------------- | --------------------------------------- |
| `snap`    | 容器化包管理                | `snap install spotify`                  |
| `flatpak` | 跨发行版沙箱包               | `flatpak install flathub org.gimp.GIMP` |
| `brew`    | macOS / LinuxHomebrew | `brew install git`                      |

---

## 三、常用操作详解

### 3.1 更新与升级

```bash
# Debian系
apt update                     # 刷新仓库索引
apt upgrade                    # 升级所有可升级包
apt full-upgrade               # 自动处理依赖冲突，可能移除旧包

# RPM系
yum makecache                  # 刷新缓存
yum update                     # 升级所有包
dnf upgrade --refresh          # 强制刷新后升级

# Arch系
pacman -Sy                     # 刷新索引
pacman -Syu                    # 全面同步升级

# snap / flatpak
snap refresh                   # 更新所有 snap 包
flatpak update                # 更新所有 flatpak 应用
```

### 3.2 安装与删除

```bash
# 安装
apt install package1 package2
yum install httpd mariadb-server
dnf install nodejs npm
pacman -S package_name
snap install vlc
flatpak install flathub com.spotify.Client
brew install htop

# 删除
apt remove package --purge
yum remove httpd
dnf remove sqlite
pacman -Rns unneeded-pkg
snap remove spotify
flatpak uninstall --delete-data com.gimp.GIMP
brew uninstall wget
```

### 3.3 源管理（添加/移除 PPA / Repository）

```bash
# Debian/Ubuntu 添加 PPA
add-apt-repository ppa:deadsnakes/ppa
apt update

# RPM 系添加第三方 repo
# CentOS7
cat > /etc/yum.repos.d/nginx.repo << EOF
[nginx]
name=nginx repo
baseurl=http://nginx.org/packages/centos/7/$basearch/
gpgcheck=1
gpgkey=https://nginx.org/keys/nginx_signing.key
EOF
yum makecache

# Fedora DNF Copr
dnf copr enable username/repo

# openSUSE zypper
zypper addrepo https://download.opensuse.org/repositories/home:/user/openSUSE_Leap_15.3/home:user.repo
zypper refresh

# Arch AUR （使用 yay 等辅助工具）
yay -S some-aur-package
```

### 3.4 查询与锁版本

```bash
# 查询
apt-cache policy nginx
yum list installed | grep httpd
dnf info postgresql
pacman -Qi firefox
snap list | grep spotify
flatpak info org.gimp.GIMP
brew info python

# 锁版本（防止自动升级）
apt-mark hold package-name
yum versionlock add package-name*
dnf mark install package-1.2.3-4.el8.x86_64  # 需安装插件
pacman -S --lock pkg.tar.zst                # 需提前下载包
```

---

## 四、综合示例：多发行版包管理脚本

下面脚本兼顾 Debian、RPM、Arch、snap/flatpak 平台，演示：

1. 刷新索引、升级
2. 添加第三方源
3. 安装/删除软件
4. 查询、锁定版本

```bash
#!/usr/bin/env bash
set -euo pipefail

echo "1. 刷新并升级"
if command -v apt &>/dev/null; then
  apt update && apt full-upgrade -y
elif command -v yum &>/dev/null; then
  yum makecache && yum update -y
elif command -v dnf &>/dev/null; then
  dnf upgrade --refresh -y
elif command -v pacman &>/dev/null; then
  pacman -Syu --noconfirm
fi

echo "2. 添加示例源"
if command -v add-apt-repository &>/dev/null; then
  add-apt-repository -y ppa:deadsnakes/ppa && apt update
elif command -v dnf &>/dev/null; then
  dnf copr enable -y username/repo
elif command -v zypper &>/dev/null; then
  zypper addrepo -f https://download.opensuse.org/repositories/home:user/openSUSE_Leap_15.3/home:user.repo && zypper refresh
fi

echo "3. 安装常用工具"
PKGS=(git curl htop)
if command -v apt &>/dev/null; then
  apt install -y "${PKGS[@]}"
elif command -v yum &>/dev/null; then
  yum install -y "${PKGS[@]}"
elif command -v dnf &>/dev/null; then
  dnf install -y "${PKGS[@]}"
elif command -v pacman &>/dev/null; then
  pacman -S --noconfirm "${PKGS[@]}"
fi

echo "4. snap/flatpak 示例"
if command -v snap &>/dev/null; then
  snap install core && snap install spotify
fi
if command -v flatpak &>/dev/null; then
  flatpak install -y flathub org.gimp.GIMP
fi

echo "5. 查询并锁定版本"
if command -v apt &>/dev/null; then
  apt-cache policy git
  apt-mark hold git
elif command -v yum &>/dev/null; then
  yum list installed git
  yum install -y yum-plugin-versionlock
  yum versionlock add git*
elif command -v pacman &>/dev/null; then
  pacman -Qi git
  pacman -S --noconfirm --downloadonly git
  pacman -S --noconfirm --lock /var/cache/pacman/pkg/git-*.pkg.tar.zst
fi

echo "完成：多发行版包管理流程演示。"
```

**使用说明**：

1. 将脚本保存为 `manage_pkgs.sh`，`chmod +x manage_pkgs.sh`。
2. 在支持的系统上执行：

   ```bash
   sudo ./manage_pkgs.sh
   ```
3. 脚本将自动检测发行版类型并执行相应的包管理操作，涵盖索引刷新、升级、源管理、安装删除、版本查询与锁定。

---

以上内容涵盖了包管理的理论背景、分类命令、常用操作、以及多平台综合示例，便于快速查阅与实战。
