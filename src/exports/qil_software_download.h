// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#pragma once

#include "qil_export.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define QIL_SOFTWARE_DOWNLOAD_ABI_VERSION 1u

typedef struct QilSoftwareDownloadHandleOpaque* QilSoftwareDownloadHandle;
typedef struct QilDownloadBuildOptionsHandleOpaque* QilDownloadBuildOptionsHandle;

// Options for flattening a meta-build. memory_type and product_flavor are
// required; sku_config and output_path are optional and may be NULL.
typedef struct QilFlattenMetaBuildOptions {
   const char* memory_type;
   const char* product_flavor;
   const char* sku_config;
   const char* output_path;
} QilFlattenMetaBuildOptions;

typedef struct QilPreservedPartition {
   const char* partition_index;
   const char* name;
} QilPreservedPartition;

typedef struct QilPartitionInfo {
   int32_t lun;
   const char* name;
   const char* partition_type_guid;
   const char* unique_partition_guid;
   int64_t starting_lba;
   int64_t ending_lba;
   int64_t attributes;
} QilPartitionInfo;

// Structured flashing arguments. This intentionally excludes CLI-only
// arguments; use the individual option setters for advanced native options.
typedef struct QilFlashArgs {
   int32_t memory_type;
   uint8_t reset;
   uint8_t erase;
   uint8_t slot_is_set;
   int32_t slot;
   const char* device_programmer;
} QilFlashArgs;

typedef struct QilDeviceInfo {
   uint64_t device_handle;
   char* description; size_t description_capacity;
   char* serial_number; size_t serial_number_capacity;
   char* adb_serial_number; size_t adb_serial_number_capacity;
   char* location; size_t location_capacity;
   char* vid; size_t vid_capacity;
   char* pid; size_t pid_capacity;
   char* edl_chip_id; size_t edl_chip_id_capacity;
   char* device_physical_location; size_t device_physical_location_capacity;
} QilDeviceInfo;

typedef struct QilFlashInfo {
   char* total_blocks; size_t total_blocks_capacity;
   char* block_size; size_t block_size_capacity;
   char* page_size; size_t page_size_capacity;
   char* num_physical_partitions; size_t num_physical_partitions_capacity;
   char* manufacturer_id; size_t manufacturer_id_capacity;
   char* serial_num; size_t serial_num_capacity;
   char* fw_version; size_t fw_version_capacity;
   char* mem_type; size_t mem_type_capacity;
   char* prod_name; size_t prod_name_capacity;
   char* spec_version; size_t spec_version_capacity;
} QilFlashInfo;

QIL_API uint32_t qil_software_download_abi_version(void);

QIL_API int32_t qil_device_discovery_start_monitoring(void);
QIL_API int32_t qil_device_discovery_stop_monitoring(void);
QIL_API int32_t qil_device_discovery_last_error(
   char* buffer, size_t capacity, size_t* required_size);
QIL_API int32_t qil_device_discovery_get_edl_devices(
   QilDeviceInfo* records, size_t capacity, size_t* written, size_t* required);

QIL_API QilSoftwareDownloadHandle qil_software_download_create(uint64_t device_handle);
QIL_API void qil_software_download_destroy(QilSoftwareDownloadHandle handle);
QIL_API int32_t qil_software_download_last_error(
   QilSoftwareDownloadHandle handle, char* buffer, size_t capacity, size_t* required_size);

QIL_API int32_t qil_software_download_initialize_service(QilSoftwareDownloadHandle handle);
QIL_API int32_t qil_software_download_destroy_service(QilSoftwareDownloadHandle handle);
QIL_API int32_t qil_software_download_download_build(
   QilSoftwareDownloadHandle handle, const char* build_path, QilDownloadBuildOptionsHandle options);
QIL_API int32_t qil_software_download_flatten_meta(
   QilSoftwareDownloadHandle handle,
   const char* build_path,
   const QilFlattenMetaBuildOptions* options);

QIL_API QilDownloadBuildOptionsHandle qil_download_build_options_create(int32_t memory_type);
QIL_API void qil_download_build_options_destroy(QilDownloadBuildOptionsHandle options);
QIL_API int32_t qil_download_build_options_set_flash_args(
   QilDownloadBuildOptionsHandle options, const QilFlashArgs* args);

#ifdef __cplusplus
}
#endif
