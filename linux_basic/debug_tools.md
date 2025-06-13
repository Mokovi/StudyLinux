# Linux调试工具

## GDB调试器

### 基本使用
```bash
# 编译时添加调试信息
gcc -g program.c -o program

# 启动GDB
gdb program

# 常用命令
break main    # 设置断点
run          # 运行程序
next         # 单步执行
step         # 步入函数
continue     # 继续执行
print var    # 打印变量
backtrace    # 显示调用栈
quit         # 退出GDB
```

### 高级特性
```bash
# 条件断点
break 10 if i == 100

# 观察点
watch var

# 命令列表
commands 1
print i
continue
end
```

## Strace

### 基本使用
```bash
# 跟踪系统调用
strace ./program

# 跟踪特定系统调用
strace -e trace=open,read ./program

# 统计系统调用
strace -c ./program
```

### 常用选项
```bash
# 显示时间戳
strace -t ./program

# 显示调用时间
strace -T ./program

# 跟踪子进程
strace -f ./program
```

## Lsof

### 基本使用
```bash
# 列出所有打开的文件
lsof

# 列出特定进程的文件
lsof -p PID

# 列出特定用户打开的文件
lsof -u username
```

### 常用选项
```bash
# 列出网络连接
lsof -i

# 列出特定端口的连接
lsof -i :80

# 列出特定目录的文件
lsof +D /path/to/dir
```

## Perf

### 基本使用
```bash
# 性能分析
perf record ./program
perf report

# 实时分析
perf top

# 统计事件
perf stat ./program
```

### 高级特性
```bash
# 跟踪特定事件
perf record -e cpu-clock ./program

# 分析调用图
perf record -g ./program
perf report --call-graph

# 系统范围分析
perf record -a
```

## 最佳实践

### 调试策略
1. 使用GDB进行代码级调试
2. 使用Strace分析系统调用
3. 使用Lsof检查文件操作
4. 使用Perf进行性能分析

### 性能分析
1. 使用Perf top查看热点
2. 使用Perf record进行详细分析
3. 使用Perf report查看结果

### 问题诊断
1. 使用Strace跟踪系统调用
2. 使用Lsof检查文件描述符
3. 使用GDB分析崩溃

## 常见问题

### 1. 程序崩溃
```bash
# 生成core文件
ulimit -c unlimited

# 使用GDB分析
gdb program core
```

### 2. 性能问题
```bash
# 使用Perf分析
perf record ./program
perf report

# 使用Strace分析系统调用
strace -c ./program
```

### 3. 内存问题
```bash
# 使用Valgrind
valgrind --leak-check=full ./program

# 使用GDB检查内存
(gdb) x/nfu addr
```

## 参考资料
- GDB手册：https://sourceware.org/gdb/current/onlinedocs/gdb/
- Strace手册：https://man7.org/linux/man-pages/man1/strace.1.html
- Perf手册：https://perf.wiki.kernel.org/index.php/Main_Page 