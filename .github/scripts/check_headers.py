from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parents[2]
work = root / "build" / "header-checks"
work.mkdir(parents=True, exist_ok=True)
sources = []
for index, header in enumerate(sorted((root / "include" / "SDK").rglob("*.h"))):
    source = work / f"header{index}.cpp"
    source.write_text(f'#include "{header.relative_to(root / "include").as_posix()}"\n')
    sources.append(source.name)
for index, header in enumerate([root / "include/SDK/API.h", *sorted((root / "include/SDK/Native").glob("*.h"))]):
    source = work / f"c_header{index}.c"
    source.write_text(f'#include "{header.relative_to(root / "include").as_posix()}"\n')
    sources.append(source.name)
(work / "CMakeLists.txt").write_text(
    'cmake_minimum_required(VERSION 3.20)\nproject(HeaderChecks LANGUAGES C CXX)\n'
    'set(CMAKE_C_STANDARD 11)\nset(CMAKE_CXX_STANDARD 17)\n'
    f'add_library(headers OBJECT {" ".join(sources)})\n'
    f'target_include_directories(headers PRIVATE "{(root / "include").as_posix()}")\n'
)
configure = ["cmake", "-S", str(work), "-B", str(work / "out")]
if os.name == "nt":
    configure += ["-A", "Win32"]
else:
    configure += ["-DCMAKE_C_FLAGS=-m32", "-DCMAKE_CXX_FLAGS=-m32"]
subprocess.run(configure, check=True)
subprocess.run(["cmake", "--build", str(work / "out"), "--config", "Release", "--parallel", "2"], check=True)
