# Qualcomm Image Loader (QIL)
## High Level Design — Architecture & Data Flow

> **Qualcomm Internal · 2026**
> QIL v1.1 · April 2026 · Qualcomm Technologies, Inc.

**Keywords:** Firmware Flashing · Cross-Platform · SAHARA / Firehose · C++17 · CMake · Windows & Linux · UFS · eMMC · NVMe

---

## Slide 1 — Project Overview

### What is QIL? `PURPOSE`

QIL is a professional **firmware image flashing tool** for Qualcomm-based devices. It enables engineers to transfer, validate, and manage firmware images to mobile devices operating in Emergency Download (EDL) mode.

### Who Uses It? `AUDIENCE`

Software engineers and manufacturing teams use QIL to flash custom ROMs, firmware updates, recovery images, and perform storage provisioning on Qualcomm-chipset devices during development and production.

### Distribution Model `DEPLOYMENT`

- Standalone **CLI executable** (`qil`)
- Static **SDK library** for third-party integration
- Pre-built binaries: Windows x64/ARM64 & Linux x86_64
- WSL (Windows Subsystem for Linux) support

### Supported Storage Types `SCOPE`

- UFS (Universal Flash Storage)
- eMMC (embedded MultiMediaCard)
- NAND flash memory
- SPINOR (SPI NOR Flash)
- NVMe (Non-Volatile Memory Express)

---

## Slide 2 — System Architecture

### Layered Architecture

```
┌──────────────────────────────────────────────────────────────┐
│                      CLI INTERFACE                           │
│          Command parser · Progress UI · Handlers             │
└──────────────────────────────────┬───────────────────────────┘
                                   ↓
┌──────────────────────────────────────────────────────────────┐
│                   EXPORTED PUBLIC API                        │
│     DeviceDiscovery · ImageManagement · SoftwareDownload     │
└──────────────────────────────────┬───────────────────────────┘
                                   ↓
┌──────────────────────────────────────────────────────────────┐
│                  CORE FUNCTION LAYER                         │
│             ImageTransfer · Callback Orchestration           │
└──────────────────────────────────┬───────────────────────────┘
                                   ↓
┌──────────────────────────────────────────────────────────────┐
│                    PROTOCOL LAYER                            │
│             SAHARA Protocol  |  Firehose Protocol            │
└──────────────────────────────────┬───────────────────────────┘
                                   ↓
┌──────────────────────────────────────────────────────────────┐
│             COMMUNICATION & DEVICE LAYER                     │
│     USB I/O · QDS (Win/Linux) · libusb-1.0 · DeviceManager  │
└──────────────────────────────────────────────────────────────┘
```

### Layer Descriptions

| Layer | Description |
|---|---|
| **CLI Interface** | Parses user commands, routes to handlers, and renders progress spinners and status output. |
| **Public API Layer** | Stable SDK surface for external tool integration — device discovery, image ops, and firmware transfer. |
| **Protocol Layer** | Dual SAHARA → Firehose pipeline. SAHARA bootstraps the device; Firehose performs actual image transfer. |
| **Communication Layer** | Platform-specific USB drivers (QDS) wrapped by libusb for cross-OS hardware access. |

---

## Slide 3 — Key Modules

### CLI Layer `CLI`

| Module | Role |
|---|---|
| `main.cpp` | Process entry point |
| `CliParser` | Argument parsing |
| `CliCommands` | Command dispatch |
| Progress UI | Spinner and progress bar |

### Public API `EXPORTS`

| Class | Role |
|---|---|
| `SoftwareDownload` | SDK façade |
| `DeviceDiscovery` | Device enumeration |
| `ImageManagement` | Firmware operations |

### ImageTransfer `FUNCTION`

- Main transfer orchestration
- Stage coordination
- Validation logic (modes 0–4)
- Event/progress emission

### SAHARA Protocol `PROTOCOL`

- EDL bootstrap sequence
- Device handshake
- Programmer image upload
- Mode transition to Firehose

### Firehose Protocol `PROTOCOL`

- XML command parser
- Partition write / erase
- UFS provisioning
- Read & digest operations

### Device & Comm `DEVICE`

- USB I/O abstraction
- QDS Windows/Linux implementation
- Connection management
- Buffer / DataPacket management

---

## Slide 4 — Dual-Protocol EDL Communication

> Device firmware transfer operates as a two-stage protocol handoff — SAHARA bootstraps the device, Firehose performs the heavy-lifting image transfer.

### Stage 1 · SAHARA (Bootstrap)

1. Device powers on in EDL mode
2. Host detects device on USB
3. Sends Hello / Hello Response
4. Transfers EDL programmer binary
5. Device executes programmer
6. Transitions to Firehose mode

### Stage 2 · Firehose (Transfer)

1. Parses `rawprogram.xml` & `patch.xml`
2. Configures storage parameters
3. Erases target partitions
4. Transfers firmware image data
5. Applies patches & digests
6. Optionally provisions UFS
7. Issues device reset to boot

### XML-Driven Commands

Firehose commands are described in `rawprogram.xml` and `patch.xml`. This allows firmware teams to ship configuration alongside the image without recompiling QIL.

### Device Reset & Boot

After transfer, QIL sends a Firehose reset command. The device exits EDL and boots the newly flashed firmware image, confirming a successful flash cycle.

---

## Slide 5 — Firmware Flash Data Flow

```
 ┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
 │  1. CLI     │ ──► │  2. Device  │ ──► │  3. USB     │ ──► │  4. SAHARA  │
 │  Parse      │     │  Discover   │     │  Connect    │     │  Boot       │
 │             │     │             │     │             │     │             │
 │ qil         │     │ Enumerate   │     │ QDS opens   │     │ Upload      │
 │ --flash-    │     │ EDL-mode    │     │ USB channel │     │ programmer, │
 │  build ...  │     │ USB devices │     │ via libusb  │     │ → Firehose  │
 └─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘

 ┌─────────────┐     ┌─────────────┐     ┌─────────────┐     ┌─────────────┐
 │  5. XML     │ ──► │  6. Image   │ ──► │  7.         │ ──► │  8. Device  │
 │  Parse      │     │  Transfer   │     │  Validation │     │  Reset      │
 │             │     │             │     │             │     │             │
 │ Load        │     │ Stream      │     │ SHA256 /    │     │ Reset;      │
 │ rawprogram  │     │ firmware    │     │ binary      │     │ normal boot │
 │ & patch XML │     │ via FH      │     │ readback    │     │ begins      │
 └─────────────┘     └─────────────┘     └─────────────┘     └─────────────┘
```

### Additional Notes

**Event-Driven Progress**
Callbacks fire at each stage boundary, driving the CLI progress bar and log output in real time.

**Multi-Device Support**
DeviceManager can track multiple connected devices simultaneously, each with its own transfer state machine.

**Validation Modes 0–4**
From no validation (mode 0) through full binary readback + SHA256 comparison (mode 4), selectable at runtime.

---

## Slide 6 — Technology Stack

| Category | Technology | Role | Notes |
|---|---|---|---|
| **Language** | C++17 | All production source code | Modern C++ standards throughout |
| **Build System** | CMake 3.15+ | Cross-platform build configuration | Out-of-tree builds; multi-platform targets |
| **USB Library** | libusb 1.0.27 | Cross-platform USB I/O | LGPL-2.1; dynamically linked |
| **XML Parsing** | libxml2 + libiconv | Firehose command parsing | `rawprogram.xml` / `patch.xml` |
| **Compression** | liblzma · libz | Image compression support | Transparent decompression during transfer |
| **Logging** | kLogger | Structured log output | Header-only custom library |
| **Windows Build** | MSVC v142 (VS 2019) | Windows x64 & ARM64 compilation | WDK for driver-level USB access |
| **Linux Build** | GCC / G++ | Linux x86_64 compilation | Kernel USB drivers (CDC/ACM) |
| **License** | BSD 3-Clause | QIL source license | LGPL compliance for libusb |

---

## Slide 7 — Cross-Platform Support

### Windows x64 `WINDOWS x64`

- MSVC v142 (VS 2019) compiler
- User-space QDS driver model
- WinUSB / custom driver layer
- `qds_win` platform implementation
- Native ARM64 Windows support

### Linux x86_64 `LINUX x86_64`

- GCC/G++ compiler toolchain
- Kernel USB CDC/ACM drivers
- `qds_lnx` platform implementation
- udev rules for device permissions
- Full parity with Windows feature set

### Windows Subsystem for Linux `WSL`

- Linux binary runs inside WSL
- USB passthrough via `usbipd-win`
- Transparent device forwarding
- Same Linux build artifacts

### Platform Abstraction Layer

OS-specific code is isolated in `src/platform/` and `src/qds/`. The core SAHARA, Firehose, and ImageTransfer modules are fully platform-agnostic, enabling clean port to new OS targets.

### Build Matrix

CMake generates separate build trees per OS/architecture. A single top-level `CMakeLists.txt` configures all targets with conditional platform guards.

---

## Slide 8 — Key Operations & SDK API

### CLI Commands

| Flag | Operation |
|---|---|
| `--flash-build` | Full firmware flash with optional validation |
| `--devices` | List connected EDL-mode devices |
| `--get-flash-info` | Query device storage capabilities |
| `--erase-partitions` | Selective partition erasure |
| `--ufs-provision` | Configure UFS storage parameters |
| `--send-image` | Transfer custom binary to partition |
| `--send-xml` | Send raw Firehose XML commands |
| `--read-images` | Read partition data from device |
| `--create-validation-digest` | Pre-compute validation checksums |

### SDK Public API

**`DeviceDiscovery`**
Enumerate, monitor, and filter connected Qualcomm devices. Supports async callbacks for hot-plug events.

**`ImageManagement`**
High-level firmware operations: flash, erase, read, validate. Returns structured status and progress events.

**`SoftwareDownload`**
Top-level SDK façade. Coordinates discovery and transfer, providing a single integration point for host tools.

---

## Slide 9 — Design Summary

| Metric | Value | Description |
|---|:---:|---|
| Architecture tiers | **5** | Layered architecture from CLI to hardware |
| Protocol stages | **2** | SAHARA bootstrap → Firehose transfer |
| Platform targets | **3** | Windows x64, ARM64, Linux x86_64 |
| Flash storage types | **5** | UFS, eMMC, NAND, SPINOR, NVMe |
| CLI commands | **9** | Covering full device lifecycle |
| Validation modes | **4** | From none to full SHA256 readback |

### Design Pillars

**Modular Design**
Platform-agnostic core with OS-specific adapters — cleanly separates protocol logic from hardware access.

**SDK-First API**
Stable public API surface allows QIL to be embedded in larger toolchains without exposing internal complexity.

**Production Ready**
Validation modes, structured logging, and event callbacks make QIL suitable for both dev-lab and manufacturing environments.

---

*QIL v1.1 · April 2026 · Qualcomm Technologies, Inc. · BSD 3-Clause License*
