# 一、系统信息与诊断完全指南（System Information & Diagnostics）

## 目录
- [一、基础概念与用途](#一基础概念与用途)
- [二、系统调用接口](#二系统调用接口)
  - [2.1 uname](#21-uname)
  - [2.2 sysconf](#22-sysconf)
  - [2.3 getrusage](#23-getrusage)
- [三、虚拟文件系统](#三虚拟文件系统)
  - [3.1 /proc 文件系统](#31-proc-文件系统)
  - [3.2 /sys 文件系统](#32-sys-文件系统)
  - [3.3 编程访问示例](#33-编程访问示例)
- [四、性能分析工具](#四性能分析工具)
  - [4.1 perf](#41-perf)
  - [4.2 gprof](#42-gprof)
  - [4.3 valgrind](#43-valgrind)
- [五、最佳实践与注意事项](#五最佳实践与注意事项)
- [六、综合实例](#六综合实例)
- [七、常见问题与解答](#七常见问题与解答)
- [八、参考资料](#八参考资料)

---

# 一、基础概念与用途

系统信息与诊断模块用于获取操作系统、硬件和进程的运行状况，以及对程序进行性能剖析和调优，包括：
- 系统调用接口：通过标准库或系统调用查询内核和硬件信息
- 虚拟文件系统：使用 `/proc`、`/sys` 等文件系统获取实时运行数据
- 性能分析工具：`perf`、`gprof`、`valgrind` 等对程序进行采样、分析和检测

**总结**：系统信息与诊断是开发、运维和性能优化的基础，掌握多种手段有助于定位问题和提升效率。

---

# 二、系统调用接口

## 2.1 函数 `uname`
#### 语法
```c
#include <sys/utsname.h>
int uname(struct utsname *buf);
```
#### 参数说明
- `buf`：指向 `struct utsname` 的缓冲区，用于存放返回的系统信息
- 返回值：成功返回 0，失败返回 -1 并设置 `errno`

#### 结构体说明
```c
struct utsname {
    char sysname[];    // 内核名称
    char nodename[];   // 节点名称（主机名）
    char release[];    // 内核版本
    char version[];    // 内核发布版本
    char machine[];    // 硬件架构
#ifdef _GNU_SOURCE
    char domainname[]; // NIS 或 DNS 域名
#endif
};
```

#### 使用场景
获取当前内核名称、版本、主机名、操作系统等信息。

#### 示例代码
```c
#include <stdio.h>
#include <sys/utsname.h>
int main() {
    struct utsname info;
    if (uname(&info) == 0) {
        printf("系统名称：%s\n", info.sysname);
        printf("节点名称：%s\n", info.nodename);
        printf("内核版本：%s\n", info.release);
        printf("发布版本：%s\n", info.version);
        printf("硬件架构：%s\n", info.machine);
    } else {
        perror("uname failed");
    }
    return 0;
}
```
#### 总结
`uname` 用于快速获取主机和内核的基本信息，常用于系统适配和诊断脚本。

---

## 2.2 函数 `sysconf`
#### 语法
```c
#include <unistd.h>
long sysconf(int name);
```
#### 参数说明
- `name`：查询项（如 `_SC_PAGESIZE`、`_SC_NPROCESSORS_ONLN` 等）
- 返回值：成功返回对应配置值，失败返回 -1 并设置 `errno`（部分项不支持时也返回 -1 但不一定设置 errno）

#### 常用 `name` 值
| 名称                     | 含义              |
| ---------------------- | --------------- |
| `_SC_PAGESIZE`         | 内存页大小（字节）   |
| `_SC_PHYS_PAGES`       | 物理页面总数        |
| `_SC_OPEN_MAX`         | 最大打开文件数      |
| `_SC_NPROCESSORS_ONLN` | 在线 CPU 核心数      |
| `_SC_CLK_TCK`          | 每秒时钟滴答数      |

#### 使用场景
查询系统运行时配置，如页面大小、CPU 数目、最大打开文件数等。

#### 示例代码
```c
#include <stdio.h>
#include <unistd.h>
int main() {
    long pagesize = sysconf(_SC_PAGESIZE);
    long cpus     = sysconf(_SC_NPROCESSORS_ONLN);
    if (pagesize == -1 || cpus == -1) {
        perror("sysconf failed");
    } else {
        printf("页面大小：%ld 字节\n", pagesize);
        printf("CPU 核心数：%ld\n", cpus);
    }
    return 0;
}
```
#### 总结
`sysconf` 可用于获取系统资源限制和硬件参数，适合跨平台代码和资源自适应。

---

## 2.3 函数 `getrusage`
#### 语法
```c
#include <sys/resource.h>
#include <sys/time.h>
int getrusage(int who, struct rusage *usage);
```
#### 参数说明
- `who`：`RUSAGE_SELF`（当前进程）或 `RUSAGE_CHILDREN`（已终止的子进程）
- `usage`：指向 `struct rusage`，存放返回资源数据
- 返回值：成功返回 0，失败返回 -1 并设置 `errno`

#### 结构体说明
```c
struct rusage {
    struct timeval ru_utime;  // 用户 CPU 时间
    struct timeval ru_stime;  // 系统 CPU 时间
    long   ru_maxrss;         // 最大驻留集大小（KB）
    long   ru_nvcsw;          // 自愿上下文切换次数
    long   ru_nivcsw;         // 强制上下文切换次数
    // ... 其他字段略
};
```

#### 使用场景
获取当前进程或其子进程的资源使用情况，如 CPU 时间、内存使用、上下文切换次数等。

#### 示例代码
```c
#include <stdio.h>
#include <sys/resource.h>
#include <sys/time.h>
int main() {
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        printf("用户CPU时间：%ld.%06lds\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec);
        printf("系统CPU时间：%ld.%06lds\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        printf("最大驻留集大小：%ld KB\n", usage.ru_maxrss);
        printf("自愿上下文切换：%ld\n", usage.ru_nvcsw);
        printf("强制上下文切换：%ld\n", usage.ru_nivcsw);
    } else {
        perror("getrusage failed");
    }
    return 0;
}
```
#### 总结
`getrusage` 适合性能分析、资源监控和进程行为统计。

---

# 三、虚拟文件系统

## 3.1 `/proc` 文件系统
- **作用**：暴露内核和进程信息的伪文件系统
- **常用路径**：
  - `/proc/cpuinfo`：CPU 信息
  - `/proc/meminfo`：内存信息
  - `/proc/uptime`：系统运行时间
  - `/proc/[pid]/status`：某进程状态
- **示例：读取 CPU 型号**
```bash
grep '^model name' /proc/cpuinfo | uniq
```

**总结**：`/proc` 提供了丰富的内核和进程信息，是系统监控和调试的重要入口。

---

## 3.2 `/sys` 文件系统
- **作用**：提供内核对象（设备、驱动、总线等）属性与状态
- **常用路径**：
  - `/sys/class/net/`：网络接口
  - `/sys/block/`：块设备
  - `/sys/devices/`：设备拓扑
- **示例：查看网卡速率**
```bash
cat /sys/class/net/eth0/speed
```

**总结**：`/sys` 适合自动化采集硬件和驱动信息，部分路径需 root 权限。

---

## 3.3 编程访问示例
#### 示例代码
```c
#include <stdio.h>
#include <stdlib.h>
int main() {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) { perror("fopen"); return 1; }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
    }
    fclose(fp);
    return 0;
}
```
#### 总结
`/proc` 和 `/sys` 提供了丰富的系统和硬件信息，适合脚本和程序自动化采集。

---

# 四、性能分析工具

| 工具         | 功能                    | 安装                                                            | 使用示例                                                             |
| ---------- | --------------------- | ------------------------------------------------------------- | ---------------------------------------------------------------- |
| `perf`     | 基于硬件事件的采样分析           | `sudo apt install linux-tools-common linux-tools-$(uname -r)` | `perf record -g ./app; perf report`                              |
| `gprof`    | 基于编译时插桩的函数调用图         | `sudo apt install binutils gprof`                             | `gcc -pg app.c -o app; ./app; gprof app gmon.out > analysis.txt` |
| `valgrind` | 内存泄漏、未初始化内存使用、线程错误检测等 | `sudo apt install valgrind`                                   | `valgrind --leak-check=full ./app`                               |

## 4.1 工具 `perf`
#### 说明
- 基于硬件事件的采样分析工具，适合热点定位、性能瓶颈分析。
#### 常用命令
```bash
perf record -F 99 -g -- ./your_program arg1 arg2
perf report
```
#### 总结
`perf` 适合生产环境下的低开销性能采样。

## 4.2 工具 `gprof`
#### 说明
- 基于编译时插桩的函数调用图分析工具，适合开发阶段函数级性能分析。
#### 常用命令
```bash
gcc -pg main.c -o main
./main
gprof main gmon.out > callgraph.txt
```
#### 总结
`gprof` 适合函数调用关系和热点分析，注意插桩带来的运行时开销。

## 4.3 工具 `valgrind`
#### 说明
- 内存泄漏、未初始化内存、线程错误检测等，适合开发和测试阶段。
#### 常用命令
```bash
valgrind --tool=memcheck --leak-check=full ./your_program
valgrind --tool=helgrind ./your_program
```
#### 总结
`valgrind` 是内存和线程问题定位的利器，适合开发调试。

---

# 五、最佳实践与注意事项

1. 组合多种方式：对关键信息可同时使用 `sysconf`、`/proc` 和 `uname` 交叉验证
2. 取样频率：`perf` 默认 99Hz，可根据程序执行时间适当调整
3. 插桩开销：`gprof` 会显著增加运行时开销，仅在开发阶段使用
4. 自动化脚本：将常用命令封装进脚本，定期收集系统指标
5. 安全权限： `/sys` 某些路径可能需要 `root` 权限访问

**总结**：结合多种手段和工具，能更全面、准确地掌握系统和程序运行状况。

---

# 六、综合实例

## 实现目标
开发一个 batch_image_resizer 守护进程，每隔一段时间扫描指定目录，将新图片统一缩放到固定大小，并记录运行指标。
- 获取系统与运行时信息（`uname`、`sysconf`、`getrusage`）
- 通过 `/proc` 与 `/sys` 查看 CPU、内存、I/O 状态
- 编程方式访问部分虚拟文件系统接口
- 用性能分析工具 (`perf`、`gprof`、`valgrind`) 找出瓶颈

## 涉及知识点
- 系统调用接口（uname、sysconf、getrusage）
- 虚拟文件系统（/proc、/sys）
- 性能分析工具（perf、gprof、valgrind）
- 资源采集与自动化脚本

## 代码实现（`batch_image_resizer.c`）
```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>    // uname
#include <unistd.h>         // sysconf
#include <sys/resource.h>   // getrusage
#include <dirent.h>
#include <sys/stat.h>
#include <jpeglib.h>        // 假设使用 libjpeg

// 扫描目录并缩放 JPEG 的简化示例
void process_images(const char* dir) {
    struct dirent *ent;
    DIR *dp = opendir(dir);
    if (!dp) return;
    while ((ent = readdir(dp))) {
        if (strstr(ent->d_name, ".jpg")) {
            // 这里只打印文件名，真实场景中进行解码->缩放->编码
            printf("[Resizer] Found image: %s/%s\n", dir, ent->d_name);
            // ... 使用 libjpeg 执行缩放操作 ...
        }
    }
    closedir(dp);
}

// 打印基本系统调用接口信息
void print_sysinfo() {
    struct utsname u;
    uname(&u);
    printf("uname: %s %s %s %s %s\n",
           u.sysname, u.nodename, u.release, u.version, u.machine);

    long ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    long pagesz = sysconf(_SC_PAGESIZE);
    printf("CPUs: %ld, PageSize: %ld\n", ncpu, pagesz);

    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("ru_utime: %ld.%06lds, ru_maxrss: %ld KB\n",
           usage.ru_utime.tv_sec, usage.ru_utime.tv_usec,
           usage.ru_maxrss);
}

// 编程方式读取 /proc/self/status 的 VmRSS
void read_proc_status() {
    FILE *f = fopen("/proc/self/status", "r");
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            printf("%s", line);
            break;
        }
    }
    fclose(f);
}

// 编程方式读取 /sys/class/thermal/thermal_zone0/temp（若存在）
void read_sys_temp() {
    FILE *f = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!f) return;
    int temp;
    if (fscanf(f, "%d", &temp)==1) {
        printf("CPU temp: %.1f°C\n", temp/1000.0);
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc<2) {
        fprintf(stderr, "Usage: %s <image_dir>\n", argv[0]);
        return 1;
    }
    printf("=== Batch Image Resizer Starting ===\n");
    print_sysinfo();         // 系统调用接口
    read_proc_status();      // /proc 编程访问
    read_sys_temp();         // /sys 编程访问

    process_images(argv[1]); // 核心功能
    return 0;
}
```

## 运行与调试
- 编译：
  ```bash
  gcc -std=gnu11 -O2 -pg batch_image_resizer.c -o batch_image_resizer -ljpeg
  ```
- 运行：
  ```bash
  ./batch_image_resizer /data/images
  ```
- perf 采样：
  ```bash
  perf record -F 99 -p $(pidof batch_image_resizer) -g -- sleep 10
  perf report
  ```
- gprof 分析：
  ```bash
  ./batch_image_resizer /data/images
  gprof batch_image_resizer gmon.out > gprof_report.txt
  ```
- valgrind 检查：
  ```bash
  valgrind --leak-check=full --show-leak-kinds=all \
      --log-file=valgrind.log ./batch_image_resizer /data/images
  ```

## 常见问题与解答
| 问题现象         | 可能原因               | 解决方案                             |
| ------------ | ------------------ | -------------------------------- |
| 采集信息不全       | 权限不足/路径错误           | 检查文件路径和权限，部分 /sys 需 root |
| perf 无法采样      | 内核未启用 perf_event      | 检查内核配置，或使用 sudo            |
| gprof 无输出      | 未加 -pg 编译或未运行程序   | 确认编译参数和程序执行               |
| valgrind 误报      | 依赖库未符号化/多线程复杂   | 关注主程序内存泄漏，忽略系统库警告     |

---

# 八、参考资料
- 《UNIX环境高级编程》
- Linux man pages：uname, sysconf, getrusage, perf, gprof, valgrind
- Linux 内核文档
- perf/gprof/valgrind 官方文档
