# **Linux文件操作命令整体介绍**  
Linux文件操作是系统管理的核心，遵循"一切皆文件"的哲学。
---

## 快速跳转

* [一、文件/目录查看与导航](#一文件目录查看与导航)
  - [ls](#ls) [tree](#tree) [stat](#stat) [find](#find) [locate](#locate) [cd](#cd)
* [二、文件/目录创建与删除](#二文件目录创建与删除)
  - [mkdir](#mkdir) [touch](#touch) [cp](#cp) [mv](#mv) [rm](#rm) [ln](#ln)
* [三、压缩与归档](#三压缩与归档)
  - [tar](#tar) [gzip](#gzip) [zip / unzip](#zip--unzip)
* [四、文本查看与拼接](#四文本查看与拼接)
  - [cat](#cat) [more](#more) [less](#less) [head](#head) [tail](#tail)
* [五、文本搜索与处理](#五文本搜索与处理)
  - [grep  文本搜索](#grep--文本搜索) [sed   流编辑器(stream editor)](#sed---流编辑器stream-editor) [diff](#diff) [sort](#sort) [uniq](#uniq) [wc](#wc) [echo](#echo)
* [六、综合实例：日志备份与关键字提取分析](#六日志备份与关键字提取分析)

---

## 一、文件/目录查看与导航

### ls

```bash
ls               # 列出目录内容
ls -l            # -l：长格式显示，显示权限、链接数、所有者、所属组、大小、修改时间、文件名
ls -a            # -a：显示所有文件，包括以 . 开头的隐藏文件
ls -h            # -h：与 -l 搭配，文件大小以人类可读格式 (KB, MB) 显示
ls -R            # -R：递归显示子目录内容
ls -t            # -t：按修改时间排序，最新修改的文件排在最前
ls -ltr          # -l：长格式； -t：按时间排序； -r：逆序，最旧排前

# 注意 ll 是 ls -alF 的别名
```

### tree

```bash
# 安装: sudo apt install tree

tree              # 以树状图显示当前目录结构,注意会直接递归显示到最底层
tree -L 2         # -L <N>: 限制显示深度为 N 层
tree -a           # -a：显示所有文件，包括隐藏文件和目录
tree -d           # -d：仅显示目录，不显示文件
tree -h           # -h：显示文件/目录大小，并以人类可读格式显示
```

### stat

```bash
stat file.txt     # 显示文件的详细状态，包括大小、访问/修改/改变时间、权限等
stat -c "%n: %s bytes" *    # -c：使用指定格式输出，%n 文件名，%s 文件大小（字节）
```

### find

```bash
find / -name "*.log"          # -name: 按文件名模式查找 /表示系统根目录
find . -type f -mtime -3      # -type f: 仅查找普通文件； -mtime -3: 最近 3 天内修改的文件
find / -type f -size +100M     # -size +100M: 文件大小大于 100MB
find . -perm 0777             # -perm 0777: 权限精确匹配 777
find . -name '*.tmp' -exec rm {} +   # -exec ... {} +: 对查到的文件批量执行 rm
find . -path "./.git" -prune -o -type f -print 
# -path "./.git"：匹配路径为 .git 的目录。
# -prune：告诉 find 不要进入这个目录。
# -o：逻辑 “或者”（or），用于连接前面的“排除”条件和后面的“正常查找”条件。
# -type f -print：正常打印所有文件。
```

### locate

```bash
# 安装: sudo apt install mlocate && sudo updatedb
locate myfile.txt               # 根据数据库快速查找文件
locate -b '\\myfile.txt'      # -b: 基名匹配，不匹配路径
sudo updatedb                   # 更新 locate 数据库
```

### cd

```bash
cd /path/to/dir     # 切换到指定目录
cd ~                # 切换到用户主目录
cd ..               # 切换到上一级目录
cd -                # 切换到上次所在目录
```

---

## 二、文件/目录创建与删除

### mkdir

```bash
mkdir new_dir                    # 创建 new_dir 目录
mkdir -p parent/child/grandchild # -p: 递归创建父目录，若已存在则忽略
mkdir -m 755 mydir               # -m: 指定新目录权限模式 755
```

### touch

```bash
touch newfile.txt                # 创建空文件或更新已有文件的访问和修改时间
touch -t 202506150830.55 file.txt # -t: 指定时间戳 (格式 YYYYMMDDhhmm.ss)
```

### cp

```bash
cp src.txt dest.txt             # 复制文件
cp -r dir1/ dir2/               # -r: 递归复制目录
cp -a src/ dest/                # -a: 归档模式，等同于 -dR --preserve=all，保留属性
cp -i file1 file2               # -i: 交互模式，若目标存在则提示确认
cp -f file1 file2               # -f: 强制复制，覆盖时不提示
```
cp -a 所保留的属性如下：
| 属性             | 含义说明                       |
| -------------- | -------------------------- |
| **mode**       | 文件权限（如 `rwxr-xr--`）        |
| **ownership**  | 所有者（user）和所属组（group）       |
| **timestamps** | 修改时间（mtime）、访问时间（atime）    |
| **context**    | SELinux 安全上下文（如启用 SELinux） |
| **links**      | 硬链接数（会尝试保留硬链接结构）           |
| **xattr**      | 扩展属性（Extended Attributes）  |


### mv

```bash
mv old.txt new.txt              # 重命名文件
mv file.txt /path/to/dir/       # 将文件移动到指定目录
mv -i file1 file2               # -i: 交互模式，覆盖前提示。 默认不提示
mv -f file1 file2               # -f: 强制覆盖，不提示
```

### rm

```bash
rm file.txt                     # 删除文件
rm -r dir/                      # -r: 递归删除目录及其内容
rm -f file.txt                  # -f: 强制删除，不提示错误
rm -rf dir/                     # -r -f: 递归且强制删除
```

### ln

```bash
ln target hardlink.txt          # 创建硬链接
ln -s /path/to/source link      # -s: 创建符号（软）链接
ln -sf target link              # -s: 软链接； -f: 强制覆盖已有链接
```

---

## 三、压缩与归档

### tar

```bash
# 打包

tar -cvf archive.tar dir/       # -c: 创建归档； -v: 显示过程； -f: 指定归档文件名
tar -czvf archive.tar.gz dir/   # -z: 使用 gzip 压缩
tar -cjvf archive.tar.bz2 dir/  # -j: 使用 bzip2 压缩

# 解压

tar -xvf archive.tar            # -x: 解包
tar -xzvf archive.tar.gz        # -z: gzip 解压
tar -xjvf archive.tar.bz2       # -j: bzip2 解压
tar -tvf archive.tar.gz         # -t: 列出归档内容
```

### gzip

```bash
gzip file.txt                     # 压缩为 file.txt.gz，并删除原文件
gzip -k file.txt                  # -k: 保留原文件
gzip -9 largefile                 # -9: 压缩级别 9，最慢但压缩率最高
gzip -d file.txt.gz               # -d: 解压
gunzip file.txt.gz                # 同 -d
```

### zip / unzip

```bash
# 安装: sudo apt install zip unzip
zip archive.zip file1 file2        # 创建 zip 文件
zip -r archive.zip dir/            # -r: 递归包含目录
zip -r archive.zip dir/ -x '*.tmp' # -x: 排除匹配的文件
unzip archive.zip -d target_dir    # -d: 指定解压目录
unzip -l archive.zip               # 列出 zip 内容
zip -u archive.zip newfile.txt     # -u: 更新已有压缩包里的文件
```

### 🧩 综合对比

| 工具      | 功能      | 是否压缩 | 压缩率    | 压缩速度 | 常用扩展名  |
| ------- | ------- | ---- | ------ | ---- | ------ |
| `tar`   | 打包      | 否    | 无      | 快    | `.tar` |
| `gzip`  | 压缩单个文件  | 是    | 一般     | 快    | `.gz`  |
| `bzip2` | 压缩单个文件  | 是    | 高      | 慢    | `.bz2` |
| `zip`   | 打包 + 压缩 | 是    | 一般\~较高 | 较快   | `.zip` |

---

## 四、文本查看与拼接

### cat

```bash
cat file.txt                       # 查看文件内容
cat file1 file2 > merged.txt       # 拼接多个文件并重定向输出
cat -n file.txt                    # -n: 显示行号
cat -T file.txt                    # -T: 将 TAB 显示为 ^I
cat -A file.txt                    # -A: 显示所有不可见字符
```

### more

```bash
more file.txt                      # 分页查看，只能向下翻页
more +5 file.txt                   # 从第 5 行开始显示
command | more                      # 管道分页
```

### less

```bash
less file.txt                      # 分页查看，可上下翻页
less +F file.txt                   # +F: 实时追踪文件新增内容（类似 tail -f）
/grok                              # 在 less 中输入 /pattern 搜索向下
?pattern                           # 向上搜索
g                                  # 跳到文件开头
G                                  # 跳到文件结尾
q                                  # 退出
```

### head

```bash
head file.txt                      # 默认显示前 10 行
head -n 20 file.txt                # -n: 指定行数
head -c 100 file.bin               # -c: 指定字节数
```

### tail

```bash
tail file.txt                      # 默认显示最后 10 行
tail -n 50 file.txt                # -n: 指定行数
tail -f file.txt                   # -f: 实时追踪文件新增内容
tail -n +20 file.txt               # +：从第 20 行开始显示
```

---

## 五、文本搜索与处理

### grep  文本搜索

```bash
grep 'pattern' file.txt            # 基本搜索
grep -r 'pattern' dir/              # -r: 递归搜索目录
grep -i 'pattern' file.txt          # -i: 忽略大小写
grep -n 'pattern' file.txt          # -n: 显示行号
grep -o 'pattern' file.txt          # -o: 只输出匹配部分
grep --exclude-dir={.git,node_modules,} -r 'pattern' .  # 排除指定目录

# 'pattern' 是指 查询内容
```
关于命令顺序：
| 命令                                            | 解释                      | 是否推荐     |
| --------------------------------------------- | ----------------------- | -------- |
| `grep -r 'pattern' .`                         | OK，递归搜索                 | ✅ 推荐     |
| `grep 'pattern' -r .`                         | 通常也能工作，但不稳定             | ⚠️ 不推荐   |
| `grep 'pattern' . --exclude-dir=node_modules` | **错误：--exclude-dir 无效** | ❌ 报警告或忽略 |
一般来说长选项必须放在前面

### sed   流编辑器(stream editor) 

```bash
sed 's/foo/bar/g' file.txt          # s/old/new/g: s：表示替换（substitute）; foo：要查找的旧字符串（可以是正则表达式）; bar：替换成的新字符串; g：表示 全局替换，即一行中所有匹配的都替换,若无则仅替换第一个
#注意仅输出到终端，不会修改文件
sed -i.bak 's/foo/bar/g' file.txt   # -i.bak: 就地编辑并创建 .bak 备份
sed '2d' file.txt                   # 2d: 删除第 2 行
sed '3a\插入内容' file.txt         # 3a\: 在第 3 行后插入内容
sed '/^$/d' file.txt                # 删除空行 /^$/ 正则表达式：匹配空行
```

### diff

```bash
diff file1 file2                  # 基本比较
diff -u old new > changes.patch   # -u: 统一(diff unified)格式，更易读
diff -w file1 file2               # -w: 忽略空白差异
diff --ignore-blank-lines a b     # 忽略空行差异
```

### sort

```bash
sort file.txt                     # 排序
sort -n numbers.txt               # -n: 按数值排序
sort -r names.txt                 # -r: 逆序排序
sort -t: -k3 /etc/passwd          # -t: 指定分隔符；-k3: 按第 3 列排序
```

### uniq

```bash
sort file.txt \| uniq             # uniq 需先排序
uniq -c file.txt                  # -c: 显示重复次数
uniq -d file.txt                  # -d: 仅显示重复行
uniq -i file.txt                  # -i: 忽略大小写
```

### wc

```bash
tc -l file.txt                    # -l: 仅显示行数
tc -w file.txt                    # -w: 仅显示单词数
tc -c file.txt                    # -c: 仅显示字节数
```

### echo

```bash
echo 'text'                      # 输出文本
echo $VAR                        # 输出变量值
echo -n 'text'                  # -n: 不输出结尾换行符
echo -e "Line1\nLine2"          # -e: 解析转义字符
```

---

## 六、日志备份与关键字提取分析

### 场景：

我们有一个目录 `/tmp/myapp_logs/`，每天生成大量 `.log` 文件。我们要：
0. 生成日志。编写程序生存示例日志
1. 备份最近 3 天修改的日志；
2. 打包压缩这些日志；
3. 提取其中包含 "ERROR" 的行并汇总；
4. 将最终结果保存在 `~/backup/summary.log`。

---

### 解决方案：

```bash
# 1. 创建备份目录
mkdir -p ~/backup/logs  

# 2. 查找最近 3 天修改的日志文件并复制到备份目录
find /tmp/myapp_logs/ -type f -name "*.log" -mtime -3 -exec cp {} ~/backup/logs/ \;

# 3. 进入备份目录并打包压缩
cd ~/backup
tar -czvf logs_backup_$(date +%F).tar.gz logs/

# 4. 提取含 ERROR 的行，忽略大小写，保存到 summary.log
grep -i 'error' logs/*.log > summary.log

# 5. 显示提取出的错误数量
wc -l summary.log
```

---

### 涉及命令与参数说明：

* `mkdir -p`：递归创建目录
* `~/backup/log`: `~` 表示`home` 等价于`/home/user/backup/logs`
* `find -mtime -3`：查找最近 3 天修改的文件
* `cp {} target_dir`：复制每个查找到的文件;`{}`就是一个占位符，代表当前`find`找到的那一个文件的完整路径.
* `tar -czvf`：

  * `-c`：创建归档
  * `-z`：gzip 压缩
  * `-v`：显示详细过程
  * `-f`：指定文件名
* `grep -i`：忽略大小写搜索关键词
* `wc -l`：统计行数
* `logs_backup_$(date +%F).tar.gz` : 
    - $( ... ) 是 命令替换（command substitution），Shell 会先执行里面的命令，再把输出结果"插入"到外层命令中去
    - date +%F 则是调用 date 命令，并让它按照 + 后面的格式字符串来输出日期：%F 等同于 %Y-%m-%d，也就是 年-月-日，例如 2025-06-17。

---