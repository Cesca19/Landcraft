# 🌠 Landcraft

[![CMake](https://img.shields.io/badge/CMake-064F8C?logo=cmake&logoColor=fff)](https://cmake.org/)
[![Conan](https://img.shields.io/badge/-Conan-6699CB?style=flat&logo=conan&logoColor=white)](https://conan.io/)
[![Platforms](https://img.shields.io/badge/Platforms-Windows%20%F0%9F%AA%9F%20%7C%20%F0%9F%90%A7%20Linux-F7C59F?labelColor=0D1117)](#-build--run-instructions)
[![Programming languages](https://img.shields.io/badge/C++-027FDE?style=flat-square&logo=C%2B%2B&logoColor=white)](https://isocpp.org/)

**Landcraft**  
<br>

## ⚙️ Features & Goals
- Portable setup using **CMake**  
- Dependency management with **Conan**  
- Ready-to-use build scripts for **Windows** and **Linux**  
- Continuous Integration with **GitHub Actions**
<br>

## 📁 Project Structure
| Path | Description |
|------|--------------|
| `CMakeLists.txt` | Main CMake configuration file |
| `conanfile.py` | Conan dependency definitions (includes SFML) |
| `.gitignore` | Excludes build and temporary files |
| `src/` | Project source files |
| `include/` | Public header files - In case you're building a library or a package |
| `lib/` | Third party librairies |
| `docs/` | Documentation files |
| `.github/` | Configuration files for GitHub Actions and other GitHub features |
| `scripts/` | Helper scripts for setup, builds or other tasks |

<br>

## 🧰 Prerequisites

#### 🪟 Windows
- [Python 3](https://www.python.org/downloads/) — required for Conan
- [CMake](https://cmake.org/download/) ≥ 3.27
- [Visual Studio](https://visualstudio.microsoft.com/) — with C++ desktop development tools   
 

#### 🐧 Linux
- **GCC** (added with build essentials)
- **CMake** ≥ 3.27
- **Python 3**  
- **Make**
<br>


## 💻 Build an run Instructions
#### 1. Clone the repository
```bash
git clone git@github.com:Cesca19/Landcraft.git
cd Landcraft
```
<br>

#### 2. Launch the build scripts
Open your terminal at the root of the repository and launch the following commands

##### On Linux
```
./scripts/build_linux.sh
```

##### On Windows
```
.\scripts\build_windows.bat
```
<br>

#### 3. Execute the binaries
Ater a successful build, execute the binaries by launching the following commands

##### On Linux
```
./bin/landcraft
```

##### On Windows
```
.\bin\landcraft.exe
```
<br>

## 🛠️ Build Options
The build scripts support configurable options:
* Build Type: Debug (default) or Release
* Runtime Link: dynamic (default) or static
Example:
```
./scripts/build_linux.sh Release static
```
This will produce a Release build with static libgcc/libstdc++, leaving system libraries dynamic. Only the direct dependencies listed in conanfile.py will follow this shared/static setting. 

<br>



