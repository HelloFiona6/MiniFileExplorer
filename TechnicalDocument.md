# Technical Document

## 1. 文件架构

Project: MiniFileExplorer

```text
MiniFileExplorer/
├─ Makefile
├─ README.md
├─ TechnicalDocument.md
├─ include/
│  ├─ FileSystem.h
│  ├─ MiniFileExplorer.h
│  ├─ Utils.h
│  └─ commands/
│     └─ Commands.h
├─ src/
│  ├─ main.cpp
│  ├─ MiniFileExplorer.cpp
│  ├─ FileSystem.cpp
│  ├─ Utils.cpp
│  └─ commands/
│     └─ Commands.cpp
└─ build/
	└─ MiniFileExplorer
```

## 2. 代码调用关系与文件系统构建思路

- 程序入口: `src/main.cpp` -> `MiniFileExplorer app; app.run()`。
- `MiniFileExplorer`（声明在 `include/MiniFileExplorer.h`，实现于 `src/MiniFileExplorer.cpp`）负责启动时读取当前工作目录（getcwd）、显示提示、读取用户输入并调用 `execute()`。
- 命令解析：`src/Utils.cpp` 提供 `split()` 将输入拆分为 token 列表；`MiniFileExplorer::execute()` 调用 `handleCommand(app, args)`（在 `include/commands/Commands.h` / `src/commands/Commands.cpp` 中实现）。
- 命令实现使用静态辅助的 `FileSystem` 类（声明在 `include/FileSystem.h`，实现于 `src/FileSystem.cpp`）提供文件/目录的原子操作：存在性检查、列出目录（返回 `FileInfo`）、创建文件/目录、删除文件/目录、判断空目录等。
- 高级功能（如目录递归遍历、复制、移动、目录大小计算、搜索）在 `Commands.cpp` 中组合使用 C++17 `std::filesystem`、`FileSystem::listDir` 以及系统调用（`stat`/`realpath`）实现。

设计要点：
- 将文件系统操作聚合到 `FileSystem` 静态接口，便于跨命令复用与单元测试。
- 命令处理器 `Commands.cpp` 以每个命令为独立静态函数（例如 `cmd_ls`, `cmd_cd` 等），並在 `handleCommand` 中分派。
- 使用 `split()` 做最简单的空格切分，未实现引号或转义规则（可扩展）。

## 3. 功能列表（命令表）

| 命令 | 描述 |
| ---- | ---- |
| `ls [options]` | 列出当前目录内容；options: `-s`（按大小降序），`-t`（按修改时间降序） |
| `cd [path]` | 切换当前目录（支持相对/绝对路径） |
| `touch [filename]` | 创建空文件（若存在则报错） |
| `mkdir [dirname]` | 创建目录（若存在则报错） |
| `rm [filename]` | 删除文件，执行前需确认输入 `y` |
| `rmdir [dirname]` | 删除空目录（非空报错） |
| `stat [name]` | 显示文件/目录详细信息（类型、路径、大小、创建/修改/访问时间） |
| `search [keyword]` | 在当前目录及子目录中递归搜索名称包含关键字的文件/目录（不区分大小写） |
| `cp [src] [dst]` | 复制文件（若目标存在则提示是否覆盖） |
| `mv [src] [dst]` | 移动或重命名文件/目录 |
| `du [dirname]` | 计算目录总大小（自动用 KB/MB 单位显示） |
| `help` | 显示帮助信息 |
| `exit` | 退出程序 |

## 4. 测试指令集与错误处理

下列为每个命令的主要成功案例及对应的错误拦截/提示（可用于手动测试或写自动化脚本）。

- `ls`:
	- 正常：在非空目录打印对齐的 `Name | Type | Size(B) | Modify Time` 列表。
	- `ls -s`：按大小降序（目录按子文件大小计算，空目录显示在末尾）。
	- `ls -t`：按修改时间降序。

- `cd [path]`:
	- 成功：更新当前目录（内部调用 `chdir` 并刷新 `currentDir`）。
	- 错误：若路径不存在 -> `Invalid directory: [path]`；若路径是文件 -> `Not a directory: [path]`；若 `chdir` 失败 -> 使用 `perror("cd")` 打印系统错误。

- `touch [filename]`:
	- 成功：在给定路径创建空文件（使用 `open(O_CREAT|O_EXCL)`)。 
	- 错误：若已存在 -> `File already exists: [name]`；若创建失败 -> `Failed to create file: [name]`。

- `mkdir [dirname]`:
	- 成功：创建目录。
	- 错误：若已存在 -> `Directory already exists: [name]`；若创建失败 -> `Failed to create directory: [name]`。

- `rm [filename]`:
	- 交互：提示 `Are you sure to delete [name]? (y/n): `，仅 `y` 确认删除。
	- 错误：不存在 -> `File not found: [name]`；删除失败 -> `Failed to delete file: [name]`。

- `rmdir [dirname]`:
	- 成功：删除空目录。
	- 错误：不存在 -> `Directory not found: [name]`；不是目录 -> `Not a directory: [name]`；非空 -> `Directory not empty: [name]`；删除失败 -> `Failed to remove directory: [name]`。

- `stat [name]`:
	- 成功：打印 `Type, Path, Size(B), Creation Time, Modification Time, Access Time`。
	- 错误：未提供目标 -> 输出 `Missing target: Please enter 'stat [name]'`；目标不存在 -> `Target not found: [name]`。

- `search [keyword]`:
	- 成功：输出匹配项绝对路径与类型（目录显示 `/` 结尾）。
	- 错误：无关键字 -> `Usage: search [keyword]`；无结果 -> `No results found for '[keyword]'`（需在实现中添加此提示）。

- `cp [src] [dst]`:
	- 成功：复制文件到目标位置。
	- 错误：源不存在 -> `Source not found`；目标已存在 -> `File exists in target: Overwrite? (y/n)`（用户交互实现）；复制失败 -> 输出错误信息。

- `mv [src] [dst]`:
	- 成功：移动或重命名（支持重命名为 `mv a b`）。
	- 错误：源不存在 -> `Source not found`；目标路径非法 -> `Invalid target path`；移动失败 -> 输出错误。

- `du [dirname]`:
	- 成功：计算并返回目录总大小，使用自动单位（B/KB/MB），示例：`Total size of [dirname]: 5 MB`。
	- 错误：目录不存在或不是目录 -> 相应提示（需在实现中严格校验）。

手动命令测试套件（交互式） — 包含正常与错误用例

在交互式 shell 中运行 `./build/MiniFileExplorer`，然后按下面顺序逐条输入命令（`#` 后为期望行为或检查点）。

```text
# 基础交互
help                              # 显示帮助
ls                                # 列出当前目录
ls -s                             # 按大小排序（需能运行）
ls -t                             # 按时间排序（需能运行）

# 创建/查询/删除文件和目录
touch test.txt                    # 创建文件 test.txt
touch test.txt                    # 错误：File already exists: test.txt
stat test.txt                     # 显示文件信息（Type/Path/Size/Times）
stat                              # 错误：Missing target message

mkdir demo                         # 创建目录 demo
mkdir demo                         # 错误：Directory already exists: demo
cd demo                            # 进入 demo
touch a.txt                        # 在 demo 中创建 a.txt
ls                                 # demo 下应看到 a.txt
cd ..                              # 返回上级目录

# 删除文件与目录（包含错误分支）
rm test.txt                        # 提示确认，输入 n -> 不删除
rm test.txt                        # 提示确认，输入 y -> 删除成功
rm demo                            # 错误：Is a directory -> 建议用 rmdir
rmdir demo                         # 错误（若 demo 非空）：Directory not empty
cd demo
rm a.txt                           # 在 demo 下删除 a.txt（确认 y）
cd ..
rmdir demo                         # 成功删除空目录 demo

# stat 的错误处理
stat nonexistent.txt               # 错误：Target not found

# search（搜索）
search main.cpp                    # 在当前目录及子目录搜索包含 'a' 的名称
search abc                         # 错误/空结果：No results found for 'qwerty'

# 复制/覆盖/移动
cp readme.md r1.md                 # 复制到同目录
cp readme.md r1.md                 # 已存在时提示覆盖，输入 n -> 不覆盖
cp readme.md r1.md                 # 先删除r1.md文件中内容。已存在时提示覆盖，输入 y -> 覆盖
mv r1.md src/r2.md                 # 移动并重命名文件
mv r1.md src/r2.md                 # 错误：Source not found
rm src/r2.md       

# 移动目录（包含跨设备或重命名场景）
mkdir mv_test
touch mv_test/f1.txt
mv mv_test mv_test_moved           # 移动/重命名目录（如果当前目录包含，验证 currentDir 更新逻辑）
rm mv_test_moved/f1.txt
rmdir mv_test_moved

# du（目录大小）
du .                               # 计算当前目录总大小，显示 KB/MB 单位
du nonexistent_dir                 # 错误：Invalid target path

# 边界和错误检查
cd /nonexistent/path                # 错误：Invalid directory
rm src                              # 假设 src 是目录 -> 错误信息提示使用 rmdir

exit                               # 退出程序
```

说明：在每个带交互提示（如 `rm`/`cp`/`mv` 覆盖确认）处，测试者需要根据注释输入 `y` 或 `n`，以验证程序对确认和取消的响应。此套件覆盖所有实现命令与典型错误路径。

## 5. 各命令实现思路与逻辑说明

- `ls`:
	- 调用 `FileSystem::listDir(currentDir)` 获取 `FileInfo` 列表。
	- 若无选项，逐行按 `Name | Type | Size(B) | Modify Time` 格式输出（目录名后加 `/`）。
	- `-s`：为每个目录调用 `calcDirSize(path)`（基于 `std::filesystem::recursive_directory_iterator`）计算实际大小，再按大小降序排序；空目录判为 0 并排至末尾。
	- `-t`：使用 `stat` 读取 `st_mtime` 并按时间降序排序。

- `cd`:
	- 参数校验：需要 1 个参数。
	- 使用 `FileSystem::exists` 和 `FileSystem::isDir` 校验，然后 `chdir(path)` 切换并用 `getcwd` 更新 `MiniFileExplorer::currentDir`。

- `touch`:
	- 若 `FileSystem::exists(path)` 返回 false，则用 `open(path, O_CREAT|O_EXCL)` 创建空文件，失败则报错。

- `mkdir`:
	- 用 `FileSystem::createDir(path)` 创建，若已存在提示错误。

- `rm`:
	- 将输入路径规范为绝对路径（若参数为相对路径，基于 `app.getCurrentDir()` 拼接）。
	- 存在性检查，通过交互确认（读取一行输入），确认后用 `FileSystem::removeFile(absolute)` 删除。

- `rmdir`:
	- 检查存在性与是否为目录，调用 `FileSystem::isEmptyDir` 判断是否为空；若为空则 `FileSystem::removeDir` 删除。

- `stat`:
	- 使用 `stat()` 获取类型与时间（ctime、mtime、atime），`realpath` 解析绝对路径并格式化时间输出。

- `search`:
	- 递归 DFS：基于 `FileSystem::listDir` 遍历子目录，进行不区分大小写的子串匹配，匹配则将 `realpath`（或相对路径）与类型收集到结果列表并最终打印。

- `cp`:
	- 使用 `std::filesystem::copy_file`（或手动以缓冲区读取/写入）完成复制；若目标存在则提示覆盖确认。

- `mv`:
	- 使用 `std::filesystem::rename`（或 `std::filesystem::copy_file` + 删除源）实现移动/重命名；校验源与目标路径有效性并处理错误。

- `du`:
	- 基于 `calcDirSize(path)` 递归累计字节数，然后按单位转换为 KB/MB 显示。