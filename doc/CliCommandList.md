# QFS Command Reference

This document provides a concise reference for the **QFS** command‑line utilities used for software download, memory dump collection, and flat‑build creation.

---
## 0. Set a Device to EDL mode
A device can be set into EDL mode via the following methods:<br>
1. ADB<br>
    Installation:<br>
    a. Download ADB here: https://developer.android.com/tools/releases/platform-tools <br>
    Setting Device to EDL mode:<br>
    a. run the following command to set the device into EDL mode:<br>
      ```bash
         adb reboot edl
      ```
      or 
      ```bash
         adb shell reboot edl
      ```
2. Hardware device switch<br>
   Some devices feature a hardware switch that can put the device into EDL mode. Consult your device's documentation for details. <br>
   Some common hardware EDL mode switch procedures are as follows:<br>
    Option A. Hold both volume downs button while powering on the device.<br>
    Option B. Hold down the F_DL button while powering on the device<br>
   





## 1. Software Download
| Command | Description | Required Argument | Optional Argument |
| --------| ------------| ----------------- | ----------------- | 
| `--create-flash-build-vip-digest` | Command used to create flash build VIP digest. Offline process, no device needed | --build<br>--memory-type<br>--out<br>--reset | --slot<br>--erase<br>--cdt<br>--validation-mode<br>--raw-program<br>--patch-program<br>--digest-header-type<br>--verbose |
| `--create-ufs-provision-vip-digest` | Command used to create UFS provision VIP digest. Offline process, no device needed | --out<br>--ufs-provision-xml | --slot<br>--digest-header-type<br>--verbose |
| `--create-validation-digest` | Command used to create build validation digest. Offline process, no device needed | --build<br>--memory-type<br>--out | --raw-program<br>--verbose |
| `--devices` | Command used to list all available device identifiers. |  | --verbose |
| `--erase-partitions` | Command used to erase specified partitions in device. | --device<br>--device-programmer<br>--memory-type | --slot<br>--partition-index<br>--skip-sahara<br>--firehose-init-time<br>--firehose-rx-timeout<br>--verbose |
| `--flash-build` | Command used to flash firmware build to device. | --build<br>--device<br>--memory-type<br>--reset | --read-image-path<br>--slot<br>--erase<br>--device-programmer<br>--cdt<br>--active-partition<br>--chained-digest<br>--signed-digest<br>--validation-mode<br>--raw-program<br>--partition-index<br>--patch-program<br>--skip-sahara<br>--firehose-init-time<br>--firehose-rx-timeout<br>--validate-image-size<br>--verbose |
| `--get-flash-info` | Command used to get device flash information only. | --device<br>--device-programmer<br>--memory-type<br>--reset | --slot<br>--partition-index<br>--skip-lun-info<br>--skip-sahara<br>--firehose-init-time<br>--firehose-rx-timeout<br>--verbose |
| `--help, -h` | Command used to display help information |  |  |
| `--read-images` | Command used to read partition images from device. | --build<br>--device<br>--memory-type<br>--read-image-path<br>--reset | --slot<br>--erase<br>--device-programmer<br>--raw-program<br>--skip-sahara<br>--firehose-init-time<br>--firehose-rx-timeout<br>--verbose |
| `--reset-device` | Command used to reset device from firehose mode<br>Normally used in a subsequent command when the previous command was executed with '--reset=false' option. | --device | --verbose |
| `--send-xml` | Command used to send a firehose command sequence in an XML file. Can be used to send peek command. | --device<br>--device-programmer<br>--memory-type<br>--reset<br>--xml-path | --slot<br>--skip-sahara<br>--firehose-init-time<br>--firehose-rx-timeout<br>--verbose |
| `--send-image` | Command used to send a binary image to a user defined region in device (With **partition index and start index**). | --device<br>--device-programmer<br>--memory-type<br>--image-path<br>--lun<br>--reset<br>--start-sector | --slot<br>--skip-sahara<br>--firehose-init-time<br>--firehose-rx-timeout<br>--verbose |
| `--ufs-provision` | Command used to execute a UFS provision. | --device<br>--device-programmer<br>--ufs-provision-xml | --slot<br>--chained-digest<br>--signed-digest<br>--skip-sahara<br>--firehose-init-time<br>--firehose-rx-timeout<br>--verbose |
| `--version` | Display QIL Application version. |  | --verbose |
|
## <br><br>
| Options | Value | Description |
| ------- | ----- | ----------- |
| `--active-partition` | `<0\|1\|...>` | Set the bootable partition index during flat build download. (e.g. **1 for UFS and 0 for eMMC**). |
| `--build` | `<FLAT_BUILD_PATH>` | Given absolute path to flat build directory. Directory shall contain both images and XML config files:<br>•**Flash images**: Serach path for programmer, download binary or XML<br>•**Create vip digest**: Serach path for download binary or XML<br>•**Create validation digest**: Serach path for download binary or XML |
| `--cdt` | `<CDT_BINARY_FILE_PATH>` | Given absolute path to CDT (Configuration Data Table) binary file. used to replace CDT binary during --flash-build |
| `--chained-digest` | `<CHAINED_DIGEST_FILE_PATH>` | Given absolute path or relative path to --build path for chained digest file, used for vip download. Must be used together with signed digest file. Chained digest file AND signed digest file needs to be created before performing a VIP download. See Digest file creation section. |
| `--device` | `<MSM SN>` | Specify target device identifier for device operation. Use SERIAL NUMBER from --devices command |
| `--device-programmer` | `<PROGRAMMER_FILE_PATH>` | Given absolute path to a device programmer file or sahara XML used during Sahara transfer. Can be relative path if --build path is also given in some process. |
| `--digest-header-type` | `<ELF\|MBN>`| Configure digest header type for VIP digest only used in create VIP digest. If --digest-header-type is not set, will create MBN type digest |
| `--erase` || Specify enabling erasure of a specific partition before download. If --partition-index is not specified, it will erase default configuration (**0,1,2,4,5 for UFS and 0 for eMMC**) |
| `--firehose-init-time` | `<TIME_IN_MS>` | Configure firehose initialization time in ms. Increase for slower devices or connections. |
| `--firehose-rx-timeout` | `<TIME_IN_MS>` | Configure firehose data reception timeout in ms. Increase for slower devices or connections. |
| `--image-path` | `<BINARY_IMAGE_FILE_PATH>` | Specify binary image path for --send-image. |
| `--partition-index` | `<COMMA_SPERATED_PARTITION_INDEXES_LIST>` | Comma-separated list of partition indexes to erase during download, erase partition only or get flash info.<br>•**Flash Images**: Only erase specified partition, but this does not impact the download partition which is configured by rawprogram<br>•**Get flash info**: Specify the partition index to get information on<br>•**Erase flash only**: Specify the flash partition to be erased. |
| `--patch-program` | `<SEMICOLON_SPERATED_PATCH_XML_FILE_NAME_LIST>` | Semicolon-separated list of patch XML files. Specify dedicated XML files to override auto-detection of patch files, only used in download build. |
| `--lun` | `<LUN_NUMBER>` | Configure partition index for --send-image. |
| `--memory-type` | `<UFS\|EMMC\|NAND\|SPINOR>` | Specify type of flash memory on the target device. |
| `--raw-program` | `<SEMICOLON_SPERATED_RAW_XML_FILE_NAME_LIST>` | Semicolon-separated list of rawprogram XML files. Specify dedicated XML file if you don't want to use default list, only used in download build & read images. |
| `--read-image-path` | `<DIR_PATH_FOR_READ>` | Directory path to store read-out binary images:<br>•**Flash build**: Stores binaries read back from device during validation (validation-mode 1 or 3 only)<br>•**Read images**: Output directory for partition images read from device |
| `--reset` | <true\|false>| Enable or disable reset device after firehose process completion. Device will remain in firehose mode if set to false. |
| `--signed-digest` | `<SIGNED_DIGEST_FILE_PATH>` | Given absolute path or relative path to --build path for signed digest file, used for vip download. Signed digest file needs to be created before performing a VIP download. See Digest file creation section. |
| `--skip-lun-info` || Enable skipping getting detailed LUN info for --get-flash-info. |
| `--skip-sahara` || While device already in firehose mode. Enable skipping the transfer of device programmer using Sahara. Normally used in a subsequent command when the previous command was executed with '--reset=false' or when the previous command failed and the device remains in firehose mode. |
| `--slot` | `<SLOT_INDEX>` | Configure Memory slot number. Default to slot 0 if not specified |
| `--start-sector` | `<START_SECTOR_NUMBER>` | Configure start sector for --send-image. |
| `--out` | `<DIR_PATH_FOR_READ>` | Output path to save generated digest file. Must be used inside create vip digest or build validation digest process. |
| `--ufs-provision-xml` | `<PROVISION_XML>` | Given absolute Path to UFS provision XML file. Only used together with --ufs-provision & --create-ufs-provision-vip-digest. |
| `--validate-image-size` || Validate image sizes against rawprogram.xml during download. Compares image file sizes with sizes specified in rawprogram.xml and fails if any image exceeds the defined size. |
| `--validation-mode` | `<0\|1\|2\|3\|4>` | Validate firmware images during download. No validation if not specified:<br> 0 - No validation<br> 1 - **Binary Readback**: Read back binary data from mobile -> create digest 1 from those flash data -> read original download binary file -> create digest 2 for download file in runtime -> compare digest 2 with digest 1<br> 2 - **SHA256 Readback**: Read digest 1 from mobile directly -> read download binary file -> create digest 2 for original download file in runtime -> compare digest 2 with digest 1<br> 3 - **Binary readback with digest file (Requires Build Validation File)**: Read back binary data from mobile -> create digest 1 from those flash data -> load pre-created digest 2 from file -> compare digest 2 with digest 1<br> 4 - **SHA256 Readback with digest file (Requires Build Validation File)**: Read digest 1 from mobile directly -> load pre-created digest 2 from file -> compare digest 2 with digest 1 |
| `--verbose` || Enable verbose logging output. Shows detailed operation logs and debug information. |
| `--xml-path` | `<PEEK_XML\|CMD_FILE_PATH>` | Configure command XML file path used by --send-xml. Can be used to send peek command. |
|
### Examples

- **List available Qualcomm devices**

  ```bash
  qil --devices
  ```

- **Download a flat build**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS
  ```

- **UFS provision (with device programmer)**

  ```bash
  qil --device=<MSM SN> --ufs-provision --device-programmer="<DEVICEPROG_FILE>" --ufs-provision-xml="<FLAT_BUILD>/provision.xml"
  ```

- **Custom raw and patch XMLs**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS \
       --raw-program="<FLAT_BUILD>/rawprogram0.xml;<FLAT_BUILD>/rawprogram1.xml" \
       --patch-program="<FLAT_BUILD>/patch0.xml;<FLAT_BUILD>/patch1.xml"
  ```

- **Set active bootable partition**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --active-partition="1"
  ```

- **Erase specific partitions before download**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --erase --build="<FLAT_BUILD>" --memory-type=UFS --partition-index="1,2,3"
  ```

- **Erase entire flash before download**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --erase --build="<FLAT_BUILD>" --memory-type=UFS
  ```

- **Download to a specific memory slot**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --slot=1
  ```

- **Adjust Firehose initialization time**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --firehose-init-time=2000
  ```

- **Adjust Firehose reception timeout**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --firehose-rx-timeout=2000
  ```

- **Read partition images from the device**

  ```bash
  qil --device=<MSM SN> --read-images --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --read-image-path="<PATH_FOR_READ_IMAGE>"
  ```

- **Dump device PBL information**

  ```bash
  qil --device=<MSM SN> --send-xml --device-programmer="<FLAT_BUILD>/prog_firehose_ddr.elf" --reset=true --xml-path="<FLAT_BUILD>/Peek_PBL.xml" --memory-type=UFS --read-image-path="<PATH_FOR_READ_IMAGE>"
  ```

- **Retrieving flash storage info using flat build**

  ```bash
  qil --device=<MSM SN> --get-flash-info --device-programmer="<FLAT_BUILD>/prog_firehose_ddr.elf" --reset=true --memory-type=UFS
  ```

- **Download meta build on the device with validation**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --validation-mode=1
  ```

- **Download with validation and save read-back binaries**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --validation-mode=1 --read-image-path="<VALIDATION_OUTPUT>"
  ```

- **Download flat build with CDT image**

  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --cdt="<CDT_IMAGE>"
  ```
  
- **Skip getting individual LUN info from the device flash**

  ```bash
  qil --device=<MSM SN> --get-flash-info --device-programmer="<FLAT_BUILD>/prog_firehose_ddr.elf" --reset=true --memory-type=UFS --skip-lun-info
  ```

- **Validate image size during download**
  ```bash
  qil --device=<MSM SN> --flash-build --reset=true --build="<FLAT_BUILD>" --memory-type=UFS --validate-image-size
  ```

- **Erase flash memory only with presetting partition index**

  ```bash
  qil --device=<MSM SN> --erase-partitions --device-programmer="<FLAT_BUILD>/prog_firehose_ddr.elf" --memory-type=UFS --slot=0
  ```

- **Create digest file for VIP download with slot 0**

  ```bash
  qil --create-flash-build-vip-digest --build="<FLAT_BUILD>" --memory-type=UFS --erase --reset=true --slot=0 --out="<OUT_PATH>"
  ```

- **Create digest file for VIP download with elf header**

  ```bash
  qil --create-flash-build-vip-digest --build="<FLAT_BUILD>" --memory-type=UFS --erase --reset=true --slot=0 --digestheadertype=ELF --out="<OUT_PATH>"
  ```

- **Create digest file for VIP provision with slot 0**

  ```bash
  qil --create-ufs-provision-vip-digest --ufs-provision-xml="<FLAT_BUILD>/provision.xml" --slot=0 --out="<OUT_PATH>"
  ```

- **Create build validation digest file**

  ```bash
  qil --create-validation-digest --build="<FLAT_BUILD>" --memory-type=UFS --slot=0 --out="<OUT_PATH>"
  ```

- **Create build validation digest file with custom raw program**

  ```bash
  qil --create-validation-digest --build="<FLAT_BUILD>" --memory-type=UFS --out="<OUT_PATH>" --raw-program="rawprogram0.xml;rawprogram1.xml"
  ```

- **A complex sample for UFS provision and download to multiple flash using skip sahara**

  ```bash
  qil --device=<MSM SN> --ufs-provision --device-programmer="<DEVICEPROG_FILE>" --ufs-provision-xml="<FLAT_BUILD>/provision.xml"
    qil --device=<MSM SN> --flash-build --reset=false --skip-sahara --build="<FLAT_BUILD_UFS>" --memory-type=UFS
      qil --device=<MSM SN> --flash-build --reset=true --skip-sahara --build="<FLAT_BUILD_SPI_NOR>" --memory-type=SPINOR
  ```
---

## 2. Collect Memory Dumps

| Option | Argument | Description |
|--------|----------|-------------|
| `--device` | `<MSM SN>` | Target device identifier. |
| `--dumpdir` | `<PATH>` | Destination directory for the dump. |
| `--reset` || Enable reset device after dump. |
| `--uniquets` || Enable creation of a unique timestamped sub‑folder. |

### Examples

- **List available Qualcomm devices**

  ```bash
  QMDC --devices
  ```

- **Basic memory dump**

  ```bash
  QMDC --device=<MSM SN> --dumpdir="<PATH>" --reset=true
  ```

- **Dump with unique timestamp folder**

  ```bash
  QMDC --device=<MSM SN> --dumpdir="<PATH>" --reset=true --uniquets
  ```

---

## 3. Create Flatbuild (Flatten Meta Build)

| Option | Argument | Description |
|--------|----------|-------------|
| `--flatten` | (no argument) | Trigger flattening mode. |
| `--build` | `"<META_BUILD>/contents.xml"` | Path to the meta‑build contents XML. |
| `--memory-type` | `UFS\|EMMC\|NAND\|SPINOR` | Target memory type. |
| `--flavor` | `asic` | Build flavor (e.g., **asic**). |
| `--out` | `<OUT_PATH>` | Output directory for the flat build. |

### Example

```bash
  QFC --flatten --build="<META_BUILD>/contents.xml" --memory-type=UFS --flavor=asic --out=<OUT_PATH>
```

---

### Notes

- For complex builds, combine options as needed (e.g., `--raw-program` together with `--patch-program`).
- Multiple raw\|patch XML files are separated by a semicolon (`;`).
- Boolean options accept `TRUE`, `FALSE` (**case-insensitive**).

--- 

*This reference is intended for developers and QA engineers working with Qualcomm’s QFS tooling.*
