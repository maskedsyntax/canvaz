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
- **Persistence**: Restore your wallpaper settings across sessions using the `--restore` flag.
- **Online Fetching**: Download random wallpapers from the web.
- **Native Backend**:
  - **GNOME/Unity/Cinnamon**: Seamless integration via `gsettings`.
  - **X11 (i3, bspwm, etc.)**: **Native implementation** using X11 libraries (no external dependencies like `feh` or `nitrogen` required).

## Installation

### Pre-built Packages
You can download the latest packages from the [Releases](https://github.com/maskedsyntax/canvaz/releases) page.

#### Debian/Ubuntu
```bash
sudo apt install ./canvaz-0.2.0-Linux.deb
```

#### Fedora/RHEL
```bash
sudo dnf install ./canvaz-0.2.0-1.x86_64.rpm
```

### Arch Linux
A `PKGBUILD` is available in the releases. To build and install:
```bash
mkdir canvaz-pkg && cd canvaz-pkg
wget https://github.com/maskedsyntax/canvaz/releases/download/v0.2.0/PKGBUILD
makepkg -si
```

## Usage

### Launching the GUI
```bash
canvaz
```

### Restoring Wallpaper (Session Startup)
To automatically restore your wallpaper when you log in (e.g., in your `.xinitrc` or WM config), run:
```bash
canvaz --restore
```

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

## License

MIT License.
