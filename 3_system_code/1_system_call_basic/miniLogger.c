#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define LOG_PATH "/var/log/mini_logger.log"

// 错误输出
void report_error(const char *msg) {
    dprintf(STDERR_FILENO, "%s: %s\n", msg, strerror(errno));
}

// 获取格式化时间字符串
void get_time_str(char *buffer, size_t size) {
    time_t t;
    syscall(SYS_time, &t);
    struct tm tm;
    localtime_r(&t, &tm);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", &tm);
}

// 拼接日志内容：[LEVEL] TIMESTAMP MESSAGE\n
void build_log_entry(const char *level, const char *message, char *out, size_t size) {
    char time_str[64];
    get_time_str(time_str, sizeof(time_str));
    snprintf(out, size, "[%s] %s %s\n", level, time_str, message);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        dprintf(STDERR_FILENO, "Usage: %s <LEVEL> <MESSAGE>\n", argv[0]);
        return 1;
    }

    const char *level = argv[1];
    const char *msg   = argv[2];
    char log_entry[512];

    build_log_entry(level, msg, log_entry, sizeof(log_entry));

    // 打开日志文件（追加写入）
    int fd = syscall(SYS_open, LOG_PATH, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        report_error("open log file");
        return 2;
    }

    // 写入日志
    ssize_t len = strlen(log_entry);
    if (syscall(SYS_write, fd, log_entry, len) != len) {
        report_error("write log");
        syscall(SYS_close, fd);
        return 3;
    }

    syscall(SYS_close, fd);
    return 0;
}