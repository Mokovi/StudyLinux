# CMake基础

## CMake简介

### 什么是CMake
- 跨平台构建系统
- 生成原生构建文件
- 支持复杂项目结构

### 安装CMake
```bash
# Ubuntu/Debian
sudo apt install cmake

# CentOS/RHEL
sudo yum install cmake
```

## 基本使用

### 简单项目
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(HelloWorld)

add_executable(hello hello.c)
```

### 构建过程
```bash
# 创建构建目录
mkdir build
cd build

# 生成构建文件
cmake ..

# 编译
make
```

## 项目结构

### 基本结构
```
project/
├── CMakeLists.txt
├── src/
│   ├── main.c
│   └── utils.c
├── include/
│   └── utils.h
└── build/
```

### 多文件项目
```cmake
# 主CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyProject)

# 添加头文件目录
include_directories(include)

# 添加源文件
add_executable(program 
    src/main.c
    src/utils.c
)
```

## 常用命令

### 项目设置
```cmake
# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置编译选项
add_compile_options(-Wall -Wextra)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
```

### 库文件
```cmake
# 添加库
add_library(utils STATIC src/utils.c)

# 链接库
target_link_libraries(program utils)

# 安装库
install(TARGETS utils
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
```

### 条件编译
```cmake
# 选项
option(DEBUG "Enable debug" ON)

# 条件编译
if(DEBUG)
    add_definitions(-DDEBUG)
endif()
```

## 高级特性

### 查找包
```cmake
# 查找包
find_package(OpenSSL REQUIRED)

# 使用包
target_link_libraries(program OpenSSL::SSL)
```

### 自定义命令
```cmake
# 生成头文件
add_custom_command(
    OUTPUT generated.h
    COMMAND generator input.txt generated.h
    DEPENDS input.txt
)
```

### 测试
```cmake
# 启用测试
enable_testing()

# 添加测试
add_test(NAME MyTest COMMAND program)
```

## 最佳实践

### 变量命名
- 使用大写字母
- 使用下划线分隔
- 添加项目前缀

### 模块化
```cmake
# 包含子目录
add_subdirectory(src)
add_subdirectory(tests)
```

### 版本控制
```cmake
# 设置版本
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)
```

## 常见问题

### 1. 找不到包
```cmake
# 设置包路径
set(CMAKE_PREFIX_PATH /path/to/lib)
```

### 2. 链接错误
```cmake
# 设置库路径
link_directories(/path/to/lib)
```

### 3. 编译选项
```cmake
# 设置编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
```

## 参考资料
- CMake官方文档：https://cmake.org/documentation/
- CMake教程：https://cmake.org/cmake/help/latest/guide/tutorial/index.html 