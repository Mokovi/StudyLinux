# Makefile基础

## Makefile简介

### 什么是Makefile
- 自动化构建工具
- 管理项目编译过程
- 处理文件依赖关系

### 基本语法
```makefile
target: prerequisites
    command
```

## 基本使用

### 简单示例
```makefile
# 最简单的Makefile
hello: hello.c
    gcc hello.c -o hello

clean:
    rm -f hello
```

### 变量使用
```makefile
# 定义变量
CC = gcc
CFLAGS = -Wall -g

# 使用变量
hello: hello.c
    $(CC) $(CFLAGS) hello.c -o hello
```

### 常用变量
- `$@`：目标文件名
- `$<`：第一个依赖文件
- `$^`：所有依赖文件
- `$?`：比目标新的依赖文件

## 多文件项目

### 基本结构
```makefile
# 定义变量
CC = gcc
CFLAGS = -Wall -g
SRCS = main.c utils.c
OBJS = $(SRCS:.c=.o)
TARGET = program

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
    $(CC) $(OBJS) -o $(TARGET)

# 生成目标文件
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
    rm -f $(OBJS) $(TARGET)
```

### 依赖关系
```makefile
# 显式依赖
main.o: main.c utils.h
    $(CC) $(CFLAGS) -c main.c

# 隐式规则
%.o: %.c
    $(CC) $(CFLAGS) -c $<
```

## 高级特性

### 条件判断
```makefile
ifeq ($(DEBUG),1)
    CFLAGS += -g
else
    CFLAGS += -O2
endif
```

### 函数使用
```makefile
# 获取目录下所有.c文件
SRCS := $(wildcard *.c)

# 替换文件后缀
OBJS := $(SRCS:.c=.o)

# 添加前缀
INCLUDES := $(addprefix -I, $(DIRS))
```

### 包含其他Makefile
```makefile
include config.mk
```

## 最佳实践

### 项目结构
```
project/
├── src/
│   ├── main.c
│   └── utils.c
├── include/
│   └── utils.h
├── lib/
├── Makefile
└── config.mk
```

### 常用目标
```makefile
.PHONY: all clean install uninstall

all: $(TARGET)

clean:
    rm -f $(OBJS) $(TARGET)

install: $(TARGET)
    install -m 755 $(TARGET) /usr/local/bin

uninstall:
    rm -f /usr/local/bin/$(TARGET)
```

### 调试技巧
```makefile
# 显示执行的命令
make V=1

# 显示变量值
make -p

# 不执行命令，只显示
make -n
```

## 常见问题

### 1. 循环依赖
- 检查目标之间的依赖关系
- 使用中间目标

### 2. 并行编译
```makefile
# 启用并行编译
make -j4
```

### 3. 自动依赖
```makefile
# 生成依赖文件
%.d: %.c
    $(CC) -MM $< > $@

# 包含依赖文件
-include $(SRCS:.c=.d)
```

## 参考资料
- GNU Make手册：https://www.gnu.org/software/make/manual/
- Makefile教程：https://makefiletutorial.com/ 