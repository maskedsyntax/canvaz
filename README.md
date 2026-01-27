<p align="center">
  <img src="./docs/images/canvaz.svg" alt="Canvaz Logo" width="150" />
</p>

<h1 align="center">Canvaz</h1>

<p align="center">
  <b> A modern wallpaper companion for Linux desktops </b>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Platform-Linux-blue" />
  <img src="https://img.shields.io/badge/Built_with-Qt6_%26_C++-green" />
  <img src="https://img.shields.io/badge/License-MIT-green" />
</p>

---

## Overview

**Canvaz** is a modern, lightweight wallpaper manager built with C++ and Qt.  
Inspired by `nitrogen`, it provides a clean, dark-themed interface to manage your desktop backgrounds efficiently.

## Features

- **Multi-Monitor Support**: Independently set wallpapers for Screen 1, Screen 2, or both.
- **Scaling Options**: Automatic, Scaled, Centered, Tiled, Zoomed, Zoomed Fill.
- **Color Background**: Option to set a solid color background.
- **Library Management**: Add multiple directory paths to scan for wallpapers.
- **High Performance**: Asynchronous image scanning and thumbnail generation for instant startup times.
- **Online Fetching**: Download random wallpapers from the web.
- **Native Backend**:
  - **GNOME/Unity/Cinnamon**: Seamless integration via `gsettings`.
  - **X11 (i3, bspwm, etc.)**: **Native implementation** using X11 libraries (no external dependencies like `feh` or `nitrogen` required).

## Build & Install

### Requirements
- Qt 6 (Widgets, Gui, Core, Network)
- CMake
- A C++17 compiler
- X11 development libraries (`libx11-dev`)

### Building

```bash
mkdir build
cd build
cmake ..
make
```

### Running

```bash
./build/canvaz
```

## Legacy

The original Python/GTK and C/GTK prototypes have been moved to the `legacy/` directory.

## License

MIT License.
