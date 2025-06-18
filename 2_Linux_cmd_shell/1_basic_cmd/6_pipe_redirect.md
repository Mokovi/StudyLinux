以下内容对 Linux 管道与重定向命令进行重构与扩充，满足以下要求：

1. **总体介绍**：管道与重定向基础及原理
2. **分类与排序**：按“基础重定向”“标准/错误输出合并”“管道扩展”“高级用法”分类
3. **快速跳转**：章节锚点
4. **命令扩展**：每类补充语法与示例
5. **综合实例**：示例脚本涵盖所有知识点

---

## 快速跳转

* [一、管道与重定向基础](#一管道与重定向基础)
* [二、基础重定向](#二基础重定向)
* [三、标准/错误输出合并](#三标准错误输出合并)
* [四、管道与参数传递](#四管道与参数传递)
* [五、高级用法](#五高级用法)
* [六、综合示例](#六综合示例)

---

## 一、管道与重定向基础

* **管道（`|`）**：将前一命令的标准输出（stdout）作为下一命令的标准输入（stdin）。
* **重定向（`>`, `<`）**：将命令的 stdin/stdout/stderr 定向到文件或来自文件。
* **文件描述符**：

  * `0`：标准输入（stdin）
  * `1`：标准输出（stdout）
  * `2`：标准错误（stderr）

原理：Shell 内部打开文件或管道，将命令的文件描述符指向相应目标，实现数据流转。

---

## 二、基础重定向

| 符号   | 功能                     | 语法示例                          |
| ---- | ---------------------- | ----------------------------- |
| `>`  | stdout 覆盖写入            | `ls /etc > files.txt`         |
| `>>` | stdout 追加写入            | `echo hi >> log.txt`          |
| `<`  | stdin 重定向              | `sort < unsorted.txt`         |
| `2>` | stderr 重定向             | `grep foo file 2> errors.log` |
| `&>` | stdout+stderr 合并（Bash） | `make &> build.log`           |

**示例：**

```bash
# 将 find 输出写入文件，覆盖已有内容
find /var/log -type f > logs.txt

# 将错误单独收集
grep ERROR /var/log/syslog 2> errs.txt

# 读取文件进行排序
sort < data.txt > sorted.txt

# 追加标准输出和错误
./run_tests.sh >> tests.log 2>&1
```

---

## 三、标准/错误输出合并

| 符号 / 命令 | 功能                   | 示例                           |        |                   |
| ------- | -------------------- | ---------------------------- | ------ | ----------------- |
| `2>&1`  | 将 stderr 重定向到 stdout | `cmd > out.log 2>&1`         |        |                   |
| \`      | &\`                  | 将 stdout+stderr 一并通过管道（Bash） | \`make | & tee build.log\` |
| `&>`    | 合并输出到文件（Bash）        | `./script.sh &> all.log`     |        |                   |

**示例：**

```bash
# 同时捕获标准输出和错误，并通过管道传递给 tee
./deploy.sh |& tee deploy.log

# 兼容 sh：先合并再管道
./deploy.sh > tmp.out 2>&1 && cat tmp.out | grep SUCCESS
```

---

## 四、管道与参数传递

| 命令      | 功能             | 示例                                    |                                   |
| ------- | -------------- | ------------------------------------- | --------------------------------- |
| \`      | \`             | 管道                                    | `cat access.log \| grep "200 OK"` |
| `tee`   | 分支输出           | `ps aux \| tee processes.txt`         |                                   |
| `xargs` | 将 stdin 转为命令参数 | `find . -name "*.tmp" \| xargs rm -f` |                                   |
| `cut`   | 按列提取           | `ps aux \| cut -d' ' -f1,2`           |                                   |
| `awk`   | 文本处理与过滤        | \`cat file                            | awk '\$3>50 {print \$1,\$3}'\`    |
| `sed`   | 流编辑            | \`grep ERROR log                      | sed 's/ERROR/ERR/g'\`             |

**示例：**

```bash
# 查找并删除所有 .log 3 天前的文件
find /var/log -type f -mtime +3 | xargs -r rm -f

# 查看内存使用前 5 的进程
ps aux | sort -k4 -nr | head -n 6 | tee top5.txt

# 用 awk 提取第 3 列大于 100 的行
cat data.csv | awk -F, '$3>100 {print $1,$3}'
```

---

## 五、高级用法

| 功能                | 符号/命令              | 说明及示例          |                           |
| ----------------- | ------------------ | -------------- | ------------------------- |
| **Here Document** | `<<EOF … EOF`      | 多行 stdin 重定向   |                           |
| **Here String**   | `<<< "text"`       | 将单行字符串作为 stdin |                           |
| **Process Subst** | `<(...)`, `>(...)` | 在管道中实现并行读写文件   |                           |
| **stderr 管道**     | \`2>               | \`             | 将 stderr 管道化（部分 Shell 支持） |

**示例：**

```bash
# Here Document：批量传送邮件内容
mail -s "Report" ops@example.com <<EOF
Hello Team,
请查收今日报告。
谢谢！
EOF

# Here String：将固定字符串传给命令
grep "pattern" <<< "$MYVAR"

# 并行读写：比较两个文件差异
diff <(sort file1.txt) <(sort file2.txt)

# 将 stderr 也通过管道
bash -c 'echo out; echo err >&2' 2>| grep err
```

## 六、综合示例：Nginx 日志自动分析与报告

下面的脚本 `nginx_log_report.sh` 展示了一个真实运维场景：每天凌晨自动整理前一天的 Nginx 访问日志，生成流量与异常报告，并通过邮件发送给运维团队。此示例涵盖了基础重定向、stdout/stderr 合并、管道链、`tee`、`xargs`、Here Document、Process Substitution 等各项知识点。

```bash
#!/bin/bash
# nginx_log_report.sh
# 用途：每日自动分析 /var/log/nginx/access.log，生成流量统计、异常列表，并发送邮件报告。

########################################
# 配置项
LOG_DIR="/var/log/nginx"
ARCHIVE_DIR="/var/log/nginx/archive"
REPORT_DIR="/var/log/nginx/report"
DATE_YESTERDAY=$(date -d "yesterday" +%F)
EMAIL_TO="ops-team@example.com"
########################################

mkdir -p "$ARCHIVE_DIR" "$REPORT_DIR"

# 1. 日志切割与归档
#    将 access.log 重命名为 access-YYYY-MM-DD.log，nginx 进程自动重开新日志
mv "$LOG_DIR/access.log" "$ARCHIVE_DIR/access-$DATE_YESTERDAY.log" \
  && nginx -s reopen \
  || { echo "[$(date)] 日志切割或重启 nginx 失败" >&2; exit 1; }

# 2. 基础重定向：初始化报告文件
echo "Nginx 日志分析报告：$DATE_YESTERDAY" > "$REPORT_DIR/report-$DATE_YESTERDAY.txt"
echo "===================================" >> "$REPORT_DIR/report-$DATE_YESTERDAY.txt"

# 3. 流量概况（状态码分布、Top 10 客户端 IP）
#    stderr 合并到 run.log，以便排查脚本出错
{
  echo -e "\n[状态码分布]"
  awk '{print $9}' "$ARCHIVE_DIR/access-$DATE_YESTERDAY.log" \
    | sort | uniq -c | sort -nr | head -n 10

  echo -e "\n[Top 10 客户端 IP]"
  awk '{print $1}' "$ARCHIVE_DIR/access-$DATE_YESTERDAY.log" \
    | sort | uniq -c | sort -nr | head -n 10
} >> "$REPORT_DIR/report-$DATE_YESTERDAY.txt" 2>> "$REPORT_DIR/run-$DATE_YESTERDAY.log"

# 4. 异常请求（非 2xx/3xx 状态）详情
echo -e "\n[错误与重定向请求]" >> "$REPORT_DIR/report-$DATE_YESTERDAY.txt"
grep -E 'HTTP/[0-9\.]+" (4|5)[0-9]' "$ARCHIVE_DIR/access-$DATE_YESTERDAY.log" \
  | tee "$REPORT_DIR/errors-$DATE_YESTERDAY.log" \
  | awk '{print $1, $4, $9, $7}' \
  >> "$REPORT_DIR/report-$DATE_YESTERDAY.txt"

# 5. 日志清理：删除 30 天前的归档
find "$ARCHIVE_DIR" -type f -name "access-*.log" -mtime +30 \
  | tee -a "$REPORT_DIR/report-$DATE_YESTERDAY.txt" \
  | xargs -r rm -f

# 6. 差异对比：比较最近两天归档数量是否异常
PREV_LOG="$ARCHIVE_DIR/access-$(date -d "2 days ago" +%F).log"
if [[ -f "$PREV_LOG" ]]; then
  echo -e "\n[两日日志行数对比]" >> "$REPORT_DIR/report-$DATE_YESTERDAY.txt"
  diff <(wc -l "$PREV_LOG") <(wc -l "$ARCHIVE_DIR/access-$DATE_YESTERDAY.log") \
    >> "$REPORT_DIR/report-$DATE_YESTERDAY.txt" 2>&1
fi

# 7. 发送邮件报告（Here Document）
mail -s "Nginx 日志报告 $DATE_YESTERDAY" "$EMAIL_TO" <<EOF
运维团队，您好：

以下是 Nginx 在 $DATE_YESTERDAY 的访问日志分析报告，请查收。

报告文件位置：$REPORT_DIR/report-$DATE_YESTERDAY.txt  
错误日志：$REPORT_DIR/errors-$DATE_YESTERDAY.log  

如有疑问，请回复此邮件。

祝好，
自动运维脚本
EOF

echo "[$(date)] 报告已发送至 $EMAIL_TO"
```

### 步骤解析

1. **日志切割与重启**

   * `mv … && nginx -s reopen`：将旧日志重命名后通知 nginx 重开新文件；错误时输出到 `stderr` 并退出。
2. **初始化报告**

   * `>` / `>>`：覆盖写入标题，追加写入分隔线。
3. **流量分析**

   * `awk | sort | uniq -c | sort -nr | head`：管道链提取状态码与 IP；将 stdout 追加到报告，将 stderr (`2>>`) 写入独立的运行日志。
4. **捕获异常请求**

   * `grep | tee | awk >> report`：`tee` 分支保存原始错误行，`awk` 格式化后追加到报告。
5. **过期清理**

   * `find | tee | xargs rm -f`：`find` 输出通过 `tee` 写入报告，再用 `xargs` 批量删除。
6. **差异对比**

   * `diff <(…) <(…)`：Process Substitution 同时对比前两天日志行数；合并输出到报告。
7. **邮件发送**

   * Here Document (`<<EOF`) 将报告路径与说明自动填充到邮件正文。

该脚本覆盖了所有重定向、管道与高级用法，贴近真实运维场景，可每日定时（`cron`）执行以实现自动化监控与报警。

