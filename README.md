# Landcraft

<!-- TODO: add a screenshot or GIF here, e.g. ![Landcraft](docs/preview.png) -->

**A real-time 3D isometric terrain editor written in modern C++ with SFML.**

Landcraft lets you sculpt, paint and procedurally generate stylized isometric
worlds in real time: raise and dig terrain with weighted brushes, paint
multi-texture surfaces through a GPU splatmap, generate landscapes from noise,
and save/load your maps.

> Built from scratch as a software engineering project, Landcraft features a command-based undo/redo architecture, a custom UI framework,  map serialization, procedural generation tools, and a cross-platform Conan/CMake build system with automated GitHub Actions releases.

<br>

## Table of Contents

- [Key technologies](#key-technologies)
- [Technical Highlights](#technical-highlights)
- [Features](#features)
- [Architecture](#architecture)
- [Project Status](#project-status)
- [Try It](#try-it)
- [Controls](#controls)
- [Building from Source](#building-from-source)

<br>

## Key technologies:
**C++17 • SFML (2.6.2) • GLSL (1.20) • CMake (3.28+) • Conan (2.x) • GitHub Actions**

<br>

## Technical Highlights
The engineering work behind Landcraft, at a glance:

- **Custom isometric renderer** — reversible `world ↔ screen` projection with
  yaw/pitch, depth-correct draw ordering, and pixel-accurate mouse picking
  (`src/view/world/Camera.cpp`, `src/view/world/TileMap.cpp`).
- **Command pattern with undo/redo** — every editing action (elevation,
  painting, procedural generation) is a reversible command (`src/commands/`).
- **In-house UI toolkit** — buttons, text inputs, spatial keyboard navigation
  and focus management (`src/ui/`).
- **Cross-platform build & release automation** — Conan and CMake custom
  build/clean scripts for Windows and Linux, and a tag-driven GitHub Actions
  pipeline that builds and publishes release archives.

<br>

## Features

### Terrain editing
- **Elevation tool**: raise or dig terrain in real time with continuous
  brush-based editing.
- **Two selection modes**: edit either individual *tile corners* or whole *tiles*.
- **Weighted brushes**: 8 brush shapes with adjustable size
- Continous stroke interpolation to ensure seamless terrain modification

### Texture painting
- Real-time **GPU splatmap painting** with grass, sand, rock and snow, and texture erasing support

### Procedural generation
- **Noise-based heightmap generation** using FastNoiseLite as the noise source,
  with configurable **seed, frequency, octaves, exponent and noise type**
- **Height-step / terracing mode** for stylized, stepped landscapes.

### Navigation & UX
- Smooth (lerped) **zoom-to-cursor**, **panning** (drag or keyboard), and
  **orbit rotation** (yaw + pitch) with recenter option.
- Maps **save / load** system via native file dialogs.
- **Undo / redo** with a command history.
- Keyboard-navigable UI (Tab / arrow keys)

<br>

## Architecture

The codebase follows a Model–View–Controller separation:

```text
src/
├── model/          # Core data & world state, edited by the controllers
│                   #   (no rendering, no input handling)
├── view/
│   ├── world/      # Real-time rendering of the 3D world (camera, terrain mesh,
│   │               #   water, environment, brush overlay)
│   └── menu/       # On-screen menus and tool panels
├── controller/     # Input handling & orchestration tying model and view together
├── editionTools/   # The editor's editing tools (how the user mutates the world)
├── commands/       # Reversible actions powering undo/redo
├── ui/             # Reusable UI framework (widgets + management)
└── utils/          # Shared helpers: math, file I/O, resource loading
```

<br>


## Project Status

Landcraft is an actively developed personal project. The terrain editing,
texture painting, procedural generation, rendering and build/release pipeline
are functional. Some areas (e.g. additional edition tools) are still evolving.

<br>

## Try It
No build required — grab a ready-to-run package from the
[**Releases**](../../releases/latest) page:

1. Open the [Releases](../../releases/latest) page and download the archive for
   your OS:
   - **Windows** — `Landcraft-<version>-Windows.zip`
   - **Linux** — `Landcraft-<version>-Linux.tar.gz`
2. Extract the archive (keep the `assets/` folder next to the executable).
3. Launch the editor:
   - **Windows** — double-click `landcraft.exe`
   - **Linux** — `./landcraft` (you may need X11 libraries installed)

> Prefer building it yourself? See [Building from Source](#building-from-source).

<br>

## Controls
> Movement keys follow an AZERTY layout (`Z/Q/S/D`).

| Action | Input |
| --- | --- |
| Pan camera | Middle-mouse drag, or `Z` / `Q` / `S` / `D` |
| Zoom | Mouse wheel (to cursor), or `I` / `P` |
| Orbit (yaw) | Right-mouse drag, or `K` / `M` |
| Tilt (pitch) | Right-mouse drag, or `O` / `L` |
| Recenter view | `R` |
| Switch edition tool | `T` |
| Toggle selection mode (corner/tile) | `Space` |
| Brush size | `+` / `-` |
| Cycle brush shape | `A` / `E` |
| Select paint texture | `0`–`4` (clear, grass, sand, rock, snow) |
| Generate terrain / cycle noise | `G` / `N` |
| Undo / Redo | `Ctrl+Z` / `Ctrl+Y` |
| Save / Load map | `Ctrl+S` / `Ctrl+O` |
| Quit (with confirmation) | `Esc` |

When a UI input is focused, navigation keys (`Tab`, arrow keys, `Space`,
`Enter`, `Esc`) are captured by the UI layer instead of the world.

<br>

## Building from Source
The project ships with **custom build scripts** that bootstrap the whole
toolchain (check Python/CMake, install Conan locally, resolve dependencies,
configure CMake and build). Both scripts accept a build type and a runtime
linkage mode.

### Prerequisites
- A C++17 compiler (MSVC on Windows, GCC on Linux)
- [Python 3.7+](https://www.python.org/downloads/) and [CMake 3.28+](https://cmake.org/download/)
- Conan is installed automatically by the scripts if missing

### Windows

```bat
@REM Usage: build_windows.bat [BUILD_TYPE] [RUNTIME_LINK]
@REM BUILD_TYPE   = Debug | Release      (default: Debug)
@REM RUNTIME_LINK = static | dynamic     (default: dynamic)

scripts\build_windows.bat Release dynamic
```

### Linux

```bash
# Usage: ./scripts/build_linux.sh [BUILD_TYPE] [RUNTIME_LINK]
# BUILD_TYPE   = Debug | Release       (default: Debug)
# RUNTIME_LINK = static | dynamic      (default: dynamic)

chmod +x ./scripts/build_linux.sh
./scripts/build_linux.sh Release static
```

The resulting executable is placed in `bin/` next to its required shared
libraries. Run it with an optional map path:

```bash
./bin/landcraft                                   # loads the default map
./bin/landcraft assets/maps/landcraft-map.legend  # loads a specific map
```

> The build supports both **static** and **dynamic** runtime linkage. CMake
> handles MSVC runtime selection and Linux `RPATH`/static `libgcc`/`libstdc++`
> so the produced builds are portable.

To clean build artifacts: `scripts\clean_windows.bat` / `./scripts/clean_linux.sh`.