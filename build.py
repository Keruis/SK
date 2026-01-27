#!/usr/bin/env python3
import os
import subprocess
import sys
from pathlib import Path
import include.config as config

# -----------------------------
# 编译器设置
# -----------------------------
CC = "g++"
CXX_STANDARD = "-std=c++11"
OPT_FLAGS = ["-O2", "-march=native"]

# 输出目录
BUILD_DIR = "build"
os.makedirs(BUILD_DIR, exist_ok=True)

# -----------------------------
# 收集 include 路径
# -----------------------------
INCLUDE_FLAGS = [f"-I{inc}" for inc in config.INCLUDES]

# 模块头文件所在目录也加入 include
for module in config.ENABLED_MODULES:
    header_path = Path(config.MODULES[module])
    INCLUDE_FLAGS.append(str(header_path.parent))

# 去重
INCLUDE_FLAGS = list(dict.fromkeys(INCLUDE_FLAGS))

# -----------------------------
# 编译函数
# -----------------------------
def compile_sources(sources, output_name):
    objects = []
    for src in sources:
        obj = os.path.join(BUILD_DIR, Path(src).stem + ".o")
        cmd = [CC, CXX_STANDARD, *OPT_FLAGS, *INCLUDE_FLAGS, "-c", src, "-o", obj]
        print("Compiling:", " ".join(cmd))
        subprocess.run(cmd, check=True)
        objects.append(obj)
    # 链接生成可执行文件
    output_exe = os.path.join(BUILD_DIR, output_name)
    cmd_link = [CC, *objects, "-o", output_exe]
    print("Linking:", " ".join(cmd_link))
    subprocess.run(cmd_link, check=True)
    print(f"Build finished! Executable: {output_exe}")
    return output_exe

# -----------------------------
# 解析命令行参数
# -----------------------------
run_test = None
if "--run" in sys.argv:
    idx = sys.argv.index("--run")
    if idx + 1 < len(sys.argv):
        run_test = sys.argv[idx + 1]
    else:
        print("Usage: build.py --run <test_name>")
        sys.exit(1)

# -----------------------------
# 收集 src 文件
# -----------------------------
src_files = list(Path("src").glob("*.cpp"))
src_files = [str(f) for f in src_files]

# -----------------------------
# 收集 tests 文件
# -----------------------------
test_files = list(Path("tests").glob("*.cpp"))
test_files = [str(f) for f in test_files]

# 如果指定了 --run，过滤只编译该测试
if run_test:
    test_files = [f for f in test_files if Path(f).stem == run_test]
    if not test_files:
        print(f"Test '{run_test}' not found in tests/")
        sys.exit(1)

# -----------------------------
# 编译主程序（可选）
# -----------------------------
if not run_test:
    if src_files:
        compile_sources(src_files, "myproject")

# -----------------------------
# 编译测试文件
# -----------------------------
for test_file in test_files:
    output_name = f"test_{Path(test_file).stem}"
    exe_path = compile_sources([test_file] + src_files, output_name)

    # 如果指定 --run，直接执行
    if run_test:
        print(f"Running {exe_path} ...")
        subprocess.run([exe_path], check=True)