# NEMA/ATC SDLC Analyzer

A Saleae Logic 2 protocol analyzer for decoding HDLC/SDLC frames, with support for external clock synchronization used in NEMA TS2 and ATC traffic signal communications.

Forked from [saleae/hdlc-analyzer](https://github.com/saleae/hdlc-analyzer).

## Fork Maintainer

This fork is maintained by [Wuping Xin](https://github.com/wxinix).

## What's New in This Fork

### External Clock Mode

Added a new **"Bit Synchronous (External Clock)"** transmission mode. The original analyzer only supports an internal sampling clock derived from a user-specified baud rate, which causes sampling drift on true synchronous protocols like SDLC. The new mode samples the data line on edges of a user-specified external clock channel.

### Modernized to C++23

The codebase has been updated from C++11 to C++23, including:
- `std::unique_ptr` replacing `std::auto_ptr`
- `override` on all virtual function overrides
- `nullptr`, `static_cast`, `std::make_unique`, designated initializers
- Removed `using namespace std`; all standard library types explicitly qualified
- `constexpr` lookup functions and C++ templates to reduce code redundancy

## Installation

1. Build the analyzer (see below)
2. Open Saleae Logic 2
3. Go to **Preferences** (Ctrl+,) > **Custom Low Level Analyzers**
4. Add the directory containing the built DLL (e.g. `build/Analyzers/Release`)
5. Restart Logic 2
6. The analyzer appears as **"NEMA/ATC SDLC"**

## Analyzer Settings

| Setting | Options | Description |
|---------|---------|-------------|
| **HDLC** | Any digital channel | The data channel carrying the HDLC/SDLC serial bit stream. |
| **Clock** | Any digital channel / None | External clock channel. Required when using "Bit Synchronous (External Clock)" mode; leave as "None" for other modes. |
| **Baud Rate** | 1 - 6,000,000 | Bit rate in symbols per second. Used only by the internal clock modes ("Bit Synchronous" and "Byte Asynchronous") to derive sampling timing. Ignored in External Clock mode. |
| **Transmission Mode** | Bit Synchronous / Bit Synchronous (External Clock) / Byte Asynchronous | See [Transmission Modes](#transmission-modes) below. |
| **Clock Edge** | Rising Edge / Falling Edge | Which clock edge to sample data on. Only applies to External Clock mode. For NEMA TS2, use **Rising Edge** (see below). |
| **Address Field Type** | Basic / Extended | **Basic** = single 8-bit address. **Extended** = variable-length address where the LSB of each byte indicates whether more address bytes follow (LSB = 1 continues, LSB = 0 terminates). NEMA TS2 uses Basic. |
| **Control Field Format** | Basic - Modulo 8 / Extended - Modulo 128 / Extended - Modulo 32768 / Extended - Modulo 2147483648 | Size of the control field: Basic = 1 byte (8-bit sequence numbers), Extended = 2, 4, or 8 bytes for larger sequence number spaces. NEMA TS2 uses Basic (Modulo 8). |
| **FCS Type** | CRC-8 / CRC-16-CCITT / CRC-32 | Frame Check Sequence algorithm for error detection. NEMA TS2 uses **CRC-16-CCITT**. |

### Transmission Modes

- **Bit Synchronous** - Standard HDLC bit-oriented framing with NRZI encoding and zero-bit stuffing after 5 consecutive 1-bits. Uses an internal sampling clock derived from the specified baud rate. Best for captures where no external clock signal is available.
- **Bit Synchronous (External Clock)** - Same NRZI + bit-stuffing framing, but samples the data line on edges of a physical clock signal instead of using a baud rate. Eliminates sampling drift on true synchronous links such as NEMA TS2 / ATC SDLC.
- **Byte Asynchronous** - Byte-oriented framing with start/stop bits and byte-stuffing (escape sequences). Used by protocols that run HDLC over asynchronous serial links (e.g., PPP).

## Usage for NEMA TS2 / ATC SDLC

### Wiring

Connect the Saleae logic analyzer to the TS2 Type AB (EIA-485) port:

| TS2 Port Signal | Pin | Logic Analyzer |
|-----------------|-----|----------------|
| RxData+ | 5 | Digital Channel 0 |
| RxClock+ | 7 | Digital Channel 1 |
| GND | 2, 4, 6, 8 | GND |

### Recommended Configuration

| Setting | Value |
|---------|-------|
| Transmission Mode | Bit Synchronous (External Clock) |
| HDLC | Channel 0 (data) |
| Clock | Channel 1 (clock) |
| Clock Edge | **Rising Edge** |
| Address Field Type | Basic |
| Control Field Format | Basic - Modulo 8 |
| FCS Type | CRC-16-CCITT |

### Baud Rates

- **NEMA TS2** operates at **153,600 bps** (153.6 kbps)
- **ATC** operates at **614,400 bps** (614.4 kbps)

Note: When using External Clock mode, the baud rate setting is ignored — timing is derived entirely from the clock signal. The baud rate only matters for the internal clock modes.

### Clock Edge for NEMA TS2

NEMA TS2 SDLC uses **Rising Edge** sampling. Per the TS2 specification, "the receiving station shall read the data ... when the voltage potential of the Tx Clock+ line makes a positive going transition with respect to the Tx Clock- line." The clock's positive-going (rising) transition occurs at the midpoint of each bit cell.

If decoding produces garbled output, try switching to **Falling Edge** — some signal conditioners or level translators may invert the clock polarity.

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
