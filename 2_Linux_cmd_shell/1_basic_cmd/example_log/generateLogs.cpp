#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

const std::string LOG_DIR = "/tmp/myapp_logs/";
const std::vector<std::string> LEVELS = {"INFO", "WARNING", "ERROR"};
const std::vector<std::string> MESSAGES = {
    "Initialization complete.",
    "Connection lost.",
    "User login successful.",
    "File not found.",
    "Memory usage high.",
    "Unknown error occurred.",
    "Disk write failed.",
    "Reconnected to server.",
    "Permission denied.",
    "Update completed."
};

// 获取当前时间戳字符串
std::string current_timestamp() {
    std::time_t t = std::time(nullptr);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%F %T", std::localtime(&t));
    return std::string(buf);
}

// 生成一个日志文件
void generate_log_file(const std::string& filename, int lines_per_file) {
    std::ofstream ofs(filename);
    for (int i = 0; i < lines_per_file; ++i) {
        const std::string& level = LEVELS[rand() % LEVELS.size()];
        const std::string& msg   = MESSAGES[rand() % MESSAGES.size()];
        ofs << current_timestamp()
            << " [" << level << "] "
            << msg
            << std::endl;
    }
}

// 主函数
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int file_count     = 5;    // 生成文件数量
    int lines_per_file = 100;  // 每个文件的日志行数

    // 创建目录（完全限定）
    std::filesystem::create_directories(LOG_DIR);

    for (int i = 1; i <= file_count; ++i) {
        std::string filepath = LOG_DIR + "log" + std::to_string(i) + ".log";
        generate_log_file(filepath, lines_per_file);
        std::cout << "Generated: " << filepath << std::endl;
    }

    std::cout << "All logs written to: " << LOG_DIR << std::endl;
    return 0;
}
