# GCC编译工具链

## GCC简介

### 什么是GCC
- GNU Compiler Collection的缩写
- 支持多种编程语言的编译器集合
- 最常用的C/C++编译器

### 安装GCC
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
```

## 基本使用

### 编译过程
1. **预处理（Preprocessing）**
   ```bash
   gcc -E hello.c -o hello.i
   ```
   - 处理头文件
   - 宏展开
   - 条件编译

2. **编译（Compilation）**
   ```bash
   gcc -S hello.i -o hello.s
   ```
   - 生成汇编代码
   - 语法检查
   - 优化

3. **汇编（Assembly）**
   ```bash
   gcc -c hello.s -o hello.o
   ```
   - 生成目标文件
   - 机器码

4. **链接（Linking）**
   ```bash
   gcc hello.o -o hello
   ```
   - 链接库文件
   - 生成可执行文件

### 常用编译选项
```bash
# 基本编译
gcc hello.c -o hello

# 优化级别
gcc -O0 hello.c -o hello  # 无优化
gcc -O1 hello.c -o hello  # 基本优化
gcc -O2 hello.c -o hello  # 更多优化
gcc -O3 hello.c -o hello  # 最高优化

# 调试信息
gcc -g hello.c -o hello

# 警告信息
gcc -Wall hello.c -o hello
gcc -Wextra hello.c -o hello

# 指定C标准
gcc -std=c11 hello.c -o hello
```

## 多文件编译

### 直接编译
```bash
gcc main.c utils.c -o program
```

### 分步编译
```bash
# 编译各个源文件
gcc -c main.c
gcc -c utils.c

# 链接目标文件
gcc main.o utils.o -o program
```

## 库文件使用

### 静态库
```bash
# 创建静态库
ar rcs libutils.a utils.o

# 使用静态库
gcc main.c -L. -lutils -o program
```

### 动态库
```bash
# 创建动态库
gcc -shared -fPIC utils.c -o libutils.so

# 使用动态库
gcc main.c -L. -lutils -o program
```

## 最佳实践

### 编译选项建议
1. 开发时使用 `-g -Wall -Wextra`
2. 发布时使用 `-O2` 或 `-O3`
3. 指定C标准版本

### 调试技巧
1. 使用 `-g` 选项保留调试信息
2. 使用 `-v` 查看详细的编译过程
3. 使用 `-save-temps` 保留中间文件

## 常见问题

### 1. 找不到头文件
```bash
gcc -I/path/to/include hello.c -o hello
```

### 2. 找不到库文件
```bash
gcc -L/path/to/lib -lname hello.c -o hello
```

### 3. 运行时找不到动态库
```bash
# 设置LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/path/to/lib:$LD_LIBRARY_PATH
```

## 参考资料
- GCC官方文档：https://gcc.gnu.org/onlinedocs/
- GNU Binutils：https://www.gnu.org/software/binutils/ 