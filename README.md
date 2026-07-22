<p align="center">
  <img src="src/icons/Calcx.svg" alt="CalcX Logo" width="128">
</p>

# CalcX

A modern, fully customizable calculator for Linux. Built with Qt6 and C++.

![CalcX Dark Theme](https://img.shields.io/badge/theme-dark-brightgreen)
![Qt6](https://img.shields.io/badge/Qt-6.4+-41cd52)
![License](https://img.shields.io/badge/license-MIT-blue)

## Features

- **4 Theme Modes**: Dark, White, System (auto-follows desktop), and CUSTOM
- **Full Customization**: Background color/image, button colors, text colors, corner radius, and window opacity
- **Keyboard Support**: Full keyboard and numpad input
- **Smooth UI**: Hover effects, focus management, error handling
- **Persistent Settings**: Themes and preferences saved via QSettings

## Installation

### Via .deb package

```bash
sudo dpkg -i CalcX-1.0.0-Linux.deb
```

### Build from source

```bash
# Prerequisites
sudo apt install qt6-base-dev g++ make

# Build
cd CalcX
mkdir build && cd build
qmake6 ../CalcX.pro
make -j$(nproc)

# Run
./CalcX
```

## Usage

| Key | Function |
|-----|----------|
| `0-9` | Digits |
| `+` `-` `*` `/` | Operators |
| `Enter` / `Return` | Calculate (=) |
| `Backspace` | Delete last digit |
| `Escape` / `Delete` | Clear (C) |
| `.` | Decimal point |
| `%` | Percent |

## Customization

Click the **⚙** button to open theme settings:

- **CalcX - Dark**: Default dark theme
- **CalcX - White**: Light theme
- **CalcX - System**: Auto-detects your system dark/light mode
- **CUSTOM**: Unlock full control over every color, shape, and opacity

## License

MIT
