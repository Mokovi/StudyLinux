# CMake
以下是全面覆盖CMake基础到高级核心技术的系统化总结，结合最佳实践与项目级应用场景：

---

### 一、CMake核心概念与工作原理
1. **跨平台构建流程**  
   - **核心作用**：通过`CMakeLists.txt`配置文件生成平台特定的构建文件
   - **工作流程**：  
     ```bash
     mkdir build && cd build  # 推荐Out-of-Source构建
     cmake ..                # 生成构建文件
     make                    # 编译项目
     ```

2. **核心组件**  
   - **变量系统**：  
     ```cmake
     set(CMAKE_CXX_STANDARD 17)          # 设置C++标准
     set(SRC_LIST main.cpp utils.cpp)    # 定义源文件列表
     ```
   - **预定义变量**：  
     - `PROJECT_SOURCE_DIR`：项目根目录  
     - `PROJECT_BINARY_DIR`：构建目录（默认为`build/`）  
     - `CMAKE_BUILD_TYPE`：控制构建类型（Debug/Release）  

---

### 二、基础语法与项目配置
1. **最小化配置**  
   ```cmake
   cmake_minimum_required(VERSION 3.10)  # 指定CMake最低版本
   project(MyProject)                    # 定义项目名称
   add_executable(app main.cpp)          # 生成可执行文件
   ```

2. **条件编译与用户选项**  
   ```cmake
   option(USE_MYMATH "使用自定义数学库" ON)  # 定义开关选项
   if(USE_MYMATH)
     add_subdirectory(MathFunctions)     # 条件包含子目录
     target_link_libraries(app MathFunctions)
   endif()
   ```

3. **头文件与库路径管理**  
   ```cmake
   include_directories(include)          # 添加头文件搜索路径
   link_directories(libs)                # 添加库搜索路径
   target_link_libraries(app pthread)    # 链接系统库
   ```

---

### 三、多目录项目管理
1. **模块化项目结构**  
   ```bash
   project/
   ├── CMakeLists.txt         # 根配置
   ├── src/                   # 主程序
   ├── libs/                  # 子库
   │   └── CMakeLists.txt
   └── tests/                 # 测试代码
   ```
   **根配置示例**：  
   ```cmake
   add_subdirectory(src)      # 递归配置子目录
   add_subdirectory(libs)
   add_subdirectory(tests)
   ```

2. **静态库与动态库构建**  
   ```cmake
   # libs/CMakeLists.txt
   add_library(utils STATIC utils.cpp)   # 静态库
   add_library(network SHARED net.cpp)   # 动态库
   set_target_properties(network PROPERTIES POSITION_INDEPENDENT_CODE ON)  # -fPIC
   ```

---

### 四、高级构建技术
1. **自动依赖处理**  
   ```cmake
   # 生成.d依赖文件
   set(CMAKE_DEPENDS_INCLUDE_ONLY ON)
   set(CMAKE_AUTOMOC ON)                 # Qt项目自动处理moc
   set(CMAKE_AUTOUIC ON)                 # 自动处理UI文件
   ```

2. **安装与打包**  
   ```cmake
   install(TARGETS app DESTINATION bin)  # 安装可执行文件
   install(DIRECTORY include/ DESTINATION include) # 安装头文件
   include(CPack)                        # 生成打包配置（DEB/RPM）
   ```

3. **测试集成**  
   ```cmake
   enable_testing()
   add_test(NAME basic_test COMMAND app --test)
   add_custom_target(check ALL DEPENDS basic_test)  # 定义测试目标
   ```

---

### 五、跨平台开发支持
1. **平台检测与条件编译**  
   ```cmake
   if(UNIX AND NOT APPLE)
     set(LINUX TRUE)
   elseif(WIN32)
     add_definitions(-DWINDOWS_OS)
   endif()
   ```

2. **交叉编译工具链配置**  
   **工具链文件`arm-toolchain.cmake`**：  
   ```cmake
   set(CMAKE_SYSTEM_NAME Linux)
   set(CMAKE_SYSTEM_PROCESSOR arm)
   set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
   ```
   使用方式：`cmake -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake ..`

---

### 六、现代CMake最佳实践（V3.0+）
1. **Target-Centric模式**  
   ```cmake
   add_library(utils OBJECT utils.cpp)    # 生成对象库
   add_executable(app main.cpp)
   target_link_libraries(app PRIVATE utils)  # 精确链接依赖
   ```
   - **作用域控制**：  
     - `PRIVATE`：仅当前目标使用  
     - `INTERFACE`：依赖者使用  
     - `PUBLIC`：当前目标与依赖者共用  

2. **导出配置与Find模块**  
   ```cmake
   include(GNUInstallDirs)
   install(TARGETS utils EXPORT UtilsConfig
     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
   install(EXPORT UtilsConfig DESTINATION share/Utils/cmake)
   ```

---

### 七、调试与性能优化
1. **调试技巧**  
   - **日志输出**：`message(STATUS "Variable: ${VAR}")`  
   - **详细构建日志**：`make VERBOSE=1`  
   - **依赖图生成**：`cmake --graphviz=deps.dot`  

2. **构建加速**  
   ```cmake
   set(CMAKE_CXX_FLAGS "-flto")          # 链接时优化
   set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld") # 使用LLD链接器
   include(CCache)                      # 集成编译缓存
   ```

---

### 八、综合项目实战模板
```cmake
cmake_minimum_required(VERSION 3.20)
project(EnterpriseApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" OFF)

# 模块化子目录
add_subdirectory(src)
add_subdirectory(libs)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# 安装规则
install(TARGETS app DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)

# 包支持
find_package(Boost 1.70 REQUIRED COMPONENTS filesystem)
target_link_libraries(app PRIVATE Boost::filesystem)
```

> **最佳实践建议**：  
> - 始终使用 **Out-of-Source构建**（分离源码与构建目录）  
> - 优先采用 **Target-Based依赖管理** 替代全局`include_directories`  
> - 复杂项目使用 **CPack生成安装包** 或 **CTest集成测试**  

通过以上技术组合，可构建从简单工具到百万行代码级项目的全场景跨平台解决方案。进一步学习可参考：
- **官方文档**：[CMake Manual](https://cmake.org/cmake/help/latest/)  
- **进阶书籍**：《Professional CMake: A Practical Guide》