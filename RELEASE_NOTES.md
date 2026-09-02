# QIL Release Notes

## v1.3.1

### New Features

- **Skip flash if data matched** (`--skip-flash-if-data-matched`): Skip flashing a partition/image before writing it if the on-device data already matches a pre-created build validation digest file, using the on-device `getsha256digest` query
- **Auto-detect EDL device** (`--devices` optional): Made `--devices` parameter optional for all commands; QIL will automatically detect a connected device in EDL mode. If multiple devices are present, an error is thrown asking the user to specify the device
- **JSON device output** (`--json`, `--out=<path>`): Added support for writing device list to JSON file with equivalent `--json` and `--out=<path>` options to `qil --devices`
- **UFS 16-LUN support**: Enhanced UFS support for 16-LUN configurations
- **Ubuntu 26.04 support**: Added support for Ubuntu 26.04 with static linking for libxml2
- **Linux ARM64 support**: Added Linux ARM64 support for QIL/QMDC with proper struct alignment enforcement

### Bug Fixes

- **Skip-flash-if-data-matched disabled during simulate/VIP runs**: Fixed the pre-write skip check so it only runs during a real flash to hardware; it is now correctly disabled for `--simulate`, `--createvipdigests`/`--createdigests`, `--flattenbuildto`, and VIP download runs, none of which have real on-device data to compare against
- **ARM64 SIGBUS error**: Fixed SIGBUS error on ARM64 by enforcing 8-byte struct alignment in utils.h while maintaining compatibility with x86_64

## v1.2.3

### Bug Fixes
- Fix ARM64 SIGBUS by enforcing 8-byte struct alignment
  
---
## v1.2.2

> **Note:** Since this version, CLI only works with user space driver 1.0.2.2 (Windows) or 1.0.1.8 (Linux).

### New Features

- **Windows ARM64 port detection**: Implemented new port number parser for ARM64 using ACPI path check, while preserving existing x86 port detection logic
- **Open-source userspace driver support**: Updated libusb dynamic loader to support open-source userspace installer v1.0.2.2 and later

### Bug Fixes

- **Non-EDL device filtering**: Fixed devices with non-Qualcomm VID/PID (e.g., 90dB port) being incorrectly added during device enumeration; now filters by VID/PID at both `processAddDevice` and `extractDevInfo` levels
- **ARM64 port mismatch**: Fixed port mismatch issue on ARM64 by skipping USB interface nodes and using `CM_Get_DevNode_Registry_Property` instead of `CM_Get_DevNode_PropertyW`
- **ARM64 ACPI port chain resolution**: Fixed port chain resolution for ARM64 platforms using ACPI path check
- **Incorrect boolean return value**: Fixed incorrect boolean return value in QDS
- **Logger permission errors**: Fixed logger permission errors in multi-user scenarios
- **ARM64 processor identification**: Added ACPI condition for correctly identifying ARM64 processors

### Documentation

- Updated User Guide (removed legacy PDF, updated markdown version)
- Updated license information

---

## v1.2.1

> **Note:** Since this version, CLI only works with user space driver 1.00.1.6 (Windows) or 1.00.1.7 (Linux).

### New Features

- **Preserve Partitions CLI argument** (`--preserve-partitions`): Added support for partition backup and restore during flash operations
- **Port Trace (PTRACE) logging**: Added PTRACE log support in QIL; port-trace files are only written when the `--port-trace` flag is set
- **QDS dynamic library loading**: Ported QDS dynamic library loading into QIL for improved multi-device support

### Bug Fixes

- **Ubuntu 24.04 compatibility**: Fixed QUD not functional on Ubuntu 24.04
- **Erase partitions index**: Fixed default `partition-index` for `--erase-partitions` when not specified
- **Read timeout**: Fixed read timeout issue in dynamic library loading

### Refactoring

- Renamed `src/rpc` to `src/service` and updated namespace
- Removed unused dead code across codebase (Round 2 cleanup)
- Removed old non-Qualcomm license headers and fixed unused/missing license headers

---

## v1.1.8

### New Features

- **Win32 support**: Added Win32 build support for QIL

---

## v1.1.7

### New Features

- **Docker support**: Added Docker containerization for QIL with CI integration
- **ZLP (Zero Length Packet) for WSL**: Enabled ZLP option with auto-disable for WSL environments
- **Configurable output path**: Enabled config output path for validation results
- **RB3 flashing support**: Applied fixes for RB3 flashing and enumeration
- **LGPL compliance artifacts**: Added compliance documentation for bundled libusb static link
- **Log file improvements**:
  - Added line numbers to log output
  - Added PID to log filenames
  - Added file sequencing to log filenames
  - Added version information to logs
  - Allowed user to specify maximum log file size

### Bug Fixes

- **FirehoseLoader crash**: Fixed crash in FirehoseLoader
- **Long delays on open failure**: Fixed long delays caused by device open failure
- **Libusb segfault prevention**: Extended reentrancy guard to cover all libusb calls in `process_device`; prevented segfault from libusb hotplug callback reentrancy on Linux
- **Log buffer flush on exit**: Wait for log buffer to be flushed before exit
- **VIP digest file generation**: Fixed digest file generation by clearing stream state in all loops
- **ADB device filter**: Enabled PID filter to prevent ADB devices from being displayed on Linux
- **Port detection**: Only allow 9008 (EDL) port; updated detection method to exclude non-relevant USB devices
- **Thread timing crash**: Fixed thread timing issue that leads to crash
- **Error logging**: Fixed corrupted error logging by removing non-ASCII characters and setting `SetConsoleOutputCP(CP_UTF8)`
- **ADB server check**: Error out if ADB server is running and cannot be killed

### Removed Features

- **Alpaca protocol**: Removed alpaca feature, detection, and related code
- **ADB/Diag protocol**: Removed ADB, diag, and unknown protocol support
- **QUTS references**: Removed all QUTS code, comments, file paths, and license checks
- **Unused code**: Cleaned up unused code across the codebase

### Improvements

- **Console log format**: Updated log format to use spaced bracket style
- **Detection log size**: Reduced detection log size
- **Log level alignment**: Aligned with QDS log level
- **Copyright headers**: Added new Qualcomm copyright headers; removed old copyright file headers
- **Device detection optimization**: Optimized device detection for faster enumeration
- **Verbose logging**: Ensure debug logs print only when verbose mode is enabled
- **Firehose logging**: Improved firehose protocol logging

### Documentation

- Updated User Guide and CLI command list
- Updated Docker README
- Added open-source compliance markdown files
- Removed duplicate Docker user guide

### Removed

- Removed test directory and unneeded files
- Removed unused code
