# NEMA/ATC SDLC Analyzer

A Saleae Logic 2 protocol analyzer for decoding HDLC/SDLC frames, with support for external clock synchronization used in NEMA TS2 and ATC traffic signal communications.

Forked from [saleae/hdlc-analyzer](https://github.com/saleae/hdlc-analyzer).

## Fork Maintainer

This fork is maintained by [Wuping Xin](https://github.com/wxinix).

## What's New in This Fork

### External Clock Mode

Added a new **"Bit Synchronous (External Clock)"** transmission mode. The original analyzer only supports an internal sampling clock derived from a user-specified baud rate, which causes sampling drift on true synchronous protocols like SDLC. The new mode samples the data line on edges of a user-specified external clock channel.

**New analyzer settings:**
- **Clock** - Select the external clock channel
- **Clock Edge** - Rising or falling edge sampling

### Modernized to C++23

The codebase has been updated from C++11 to C++23, including:
- `std::unique_ptr` replacing `std::auto_ptr`
- `override` on all virtual function overrides
- `nullptr`, `static_cast`, `std::make_unique`, designated initializers
- Removed `using namespace std`; all standard library types explicitly qualified

## Installation

1. Build the analyzer (see below)
2. Open Saleae Logic 2
3. Go to **Preferences** (Ctrl+,) > **Custom Low Level Analyzers**
4. Add the directory containing the built DLL (e.g. `build/Analyzers/Release`)
5. Restart Logic 2
6. The analyzer appears as **"NEMA/ATC SDLC"**

## Usage for NEMA TS2 / ATC SDLC

### Wiring

| Signal | Logic Channel |
|--------|---------------|
| RxData+ (Pin 5) | Digital Channel 0 |
| RxClock+ (Pin 7) | Digital Channel 1 |
| GND (Pin 2,4,6,8) | GND |

### Analyzer Configuration

- **Transmission Mode** - Bit Synchronous (External Clock)
- **HDLC** - Channel 0 (data)
- **Clock** - Channel 1 (clock)
- **Clock Edge** - Rising Edge (try Falling if decoding looks incorrect)

## Building

### Prerequisites

- A C++23-compatible compiler (Visual Studio 2022, GCC 13+, or Clang 16+)
- CMake 3.13+
- git

### MacOS

```
mkdir build && cd build
cmake ..
cmake --build .
```

### Linux

```
mkdir build && cd build
cmake ..
cmake --build .
```

### Windows

```
mkdir build && cd build
cmake .. -A x64
cmake --build . --config Release
```

The built analyzer DLL will be at `build/Analyzers/Release/hdlc_analyzer.dll`.

## References

- [Saleae Analyzer SDK](https://github.com/saleae/SampleAnalyzer)
- ISO/IEC 13239:2002 - HDLC specification
- NEMA TS2 specification - SDLC timing requirements
