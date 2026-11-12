// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "exports/qil_software_download.h"

#include "exports/DeviceDiscovery.h"
#include "exports/SoftwareDownload.h"
#include "exports/SoftwareDownloadUtility.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <map>
#include <memory>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

struct QilSoftwareDownloadHandleOpaque {
   std::unique_ptr<QC::SoftwareDownload> implementation;
   std::string lastError;
};

struct QilDownloadBuildOptionsHandleOpaque {
   std::unique_ptr<QC::DownloadBuildOptions> implementation;
};

namespace {
constexpr int32_t kOk = static_cast<int32_t>(QC::ErrorCode::DEVICE_NO_ERROR);
constexpr int32_t kInvalidParameters = static_cast<int32_t>(QC::ErrorCode::DEVICE_INVALID_PARAMETERS);
constexpr int32_t kUnknownError = static_cast<int32_t>(QC::ErrorCode::DEVICE_UNKNOWN_ERROR);
std::mutex g_discoveryMutex;
std::string g_discoveryLastError;

namespace fs = std::filesystem;

std::vector<fs::path> findBuildFiles(const fs::path& buildPath, const std::regex& pattern)
{
   std::vector<fs::path> result;
   std::error_code error;
   if(!fs::is_directory(buildPath, error))
      return result;
   fs::recursive_directory_iterator entries(
      buildPath, fs::directory_options::skip_permission_denied, error);
   const fs::recursive_directory_iterator end;
   for(auto it = entries; it != end && !error; it.increment(error))
   {
      if(it->is_regular_file(error) && std::regex_search(it->path().filename().string(), pattern))
         result.push_back(it->path());
   }
   std::sort(result.begin(), result.end());
   return result;
}

int extractFileNumber(const fs::path& path)
{
   std::smatch match;
   static const std::regex numberPattern("(\\d+)");
   const std::string filename = path.filename().string();
   if(!std::regex_search(filename, match, numberPattern))
      return -1;
   try { return std::stoi(match[1].str()); } catch(...) { return -1; }
}

void sortPartitionFiles(std::vector<fs::path>& files)
{
   std::sort(files.begin(), files.end(), [](const fs::path& left, const fs::path& right) {
      const int leftNumber = extractFileNumber(left);
      const int rightNumber = extractFileNumber(right);
      if(leftNumber != -1 && rightNumber != -1) return leftNumber < rightNumber;
      if(leftNumber != -1) return true;
      if(rightNumber != -1) return false;
      return left.filename().string() < right.filename().string();
   });
}

std::map<int, std::string> readSaharaImageList(const fs::path& xmlPath, const fs::path& manifestDirectory)
{
   std::map<int, std::string> result;
   xmlDocPtr document = xmlReadFile(xmlPath.string().c_str(), nullptr, XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
   if(document == nullptr) return result;
   xmlNodePtr root = xmlDocGetRootElement(document);
   for(xmlNodePtr images = root ? root->children : nullptr; images; images = images->next)
   {
      if(images->type != XML_ELEMENT_NODE || xmlStrcmp(images->name, BAD_CAST "images") != 0) continue;
      for(xmlNodePtr image = images->children; image; image = image->next)
      {
         if(image->type != XML_ELEMENT_NODE || xmlStrcmp(image->name, BAD_CAST "image") != 0) continue;
         xmlChar* idValue = xmlGetProp(image, BAD_CAST "image_id");
         xmlChar* pathValue = xmlGetProp(image, BAD_CAST "image_path");
         if(idValue != nullptr && pathValue != nullptr)
         {
            try {
               const int imageId = std::stoi(reinterpret_cast<const char*>(idValue));
               result[imageId] =
                  (manifestDirectory / reinterpret_cast<const char*>(pathValue)).lexically_normal().string();
            } catch(...) {
            }
         }
         if(idValue != nullptr) xmlFree(idValue);
         if(pathValue != nullptr) xmlFree(pathValue);
      }
   }
   xmlFreeDoc(document);
   return result;
}

std::map<int, std::string> findDeviceProgrammer(const fs::path& buildPath)
{
   const auto manifests = findBuildFiles(buildPath, std::regex("qsahara_device_programmer\\.xml$", std::regex::icase));
   for(const auto& manifest: manifests)
   {
      auto result = readSaharaImageList(manifest, manifest.parent_path());
      if(!result.empty()) return result;
   }
   const std::vector<std::regex> priority = {
      std::regex("xbl_s_devprg_ns\\.(melf|elf)$", std::regex::icase),
      std::regex("prog_firehose_ddr\\.(mbn|elf)$", std::regex::icase),
      std::regex("prog_firehose_lite\\.(mbn|elf)$", std::regex::icase),
   };
   for(const auto& pattern: priority)
   {
      const auto files = findBuildFiles(buildPath, pattern);
      if(!files.empty()) return {{13, files.front().string()}};
   }
   auto elfFiles = findBuildFiles(buildPath, std::regex("prog_firehose.*\\.elf$", std::regex::icase));
   if(!elfFiles.empty()) return {{13, elfFiles.front().string()}};
   auto mbnFiles = findBuildFiles(buildPath, std::regex("prog_firehose.*\\.mbn$", std::regex::icase));
   if(!mbnFiles.empty()) return {{13, mbnFiles.front().string()}};
   return {};
}

std::vector<std::string> findPartitionXmls(const fs::path& buildPath, const std::regex& pattern)
{
   auto files = findBuildFiles(buildPath, pattern);
   sortPartitionFiles(files);
   std::vector<std::string> result;
   for(const auto& file: files) result.push_back(file.string());
   return result;
}

void autoDetectDownloadOptions(
   const std::string& buildPath, QC::DownloadBuildOptions& options, std::string* diagnostics)
{
   const fs::path path(buildPath);
   std::ostringstream details;
   details << "QIL auto-detection build path: " << buildPath;
   const bool hasFirehoseProgrammer = options.__isset.firehoseProgPath && !options.firehoseProgPath.empty();
   const bool hasSaharaImages = options.__isset.saharaImageList && !options.saharaImageList.empty();
   if(!hasFirehoseProgrammer && !hasSaharaImages)
   {
      const auto saharaImages = findDeviceProgrammer(path);
      if(saharaImages.empty())
      {
         details << "; no programmer files found";
         throw std::runtime_error("Unable to find device programmer in build path: " + buildPath +
            "\\nSearched for: qsahara_device_programmer.xml, xbl_s_devprg_ns.melf/.elf, "
            "prog_firehose_ddr.mbn/.elf, prog_firehose_lite.mbn/.elf");
      }
      for(const auto& image: saharaImages)
      {
         if(!fs::is_regular_file(image.second))
         {
            throw std::runtime_error("Sahara image file not found: " + image.second);
         }
      }
      options.__set_saharaImageList(saharaImages);
      details << "; selected Sahara programmer files: ";
      bool first = true;
      for(const auto& image: saharaImages)
      {
         if(!first) details << ", ";
         details << image.second;
         first = false;
      }
   }
   else if(hasFirehoseProgrammer)
      details << "; explicit Firehose programmer: " << options.firehoseProgPath;
   else
      details << "; explicit Sahara image list supplied";

   if(!options.__isset.patchXmlList && !options.__isset.rawXmlList)
   {
      const auto patchXmls = findPartitionXmls(path, std::regex("patch.*\\.xml$", std::regex::icase));
      if(!patchXmls.empty()) options.__set_patchXmlList(patchXmls);
      details << "; patch XML files found: " << patchXmls.size();
   }
   else
      details << "; patch XML list supplied: " << options.patchXmlList.size();
   if(!options.__isset.rawXmlList)
   {
      const auto rawXmls = findPartitionXmls(path, std::regex("rawprogram.*\\.xml$", std::regex::icase));
      if(!rawXmls.empty()) options.__set_rawXmlList(rawXmls);
      details << "; raw XML files found: " << rawXmls.size();
   }
   else
      details << "; raw XML list supplied: " << options.rawXmlList.size();
   if(diagnostics != nullptr) *diagnostics = details.str();
}

int32_t copyText(const std::string& value, char* buffer, size_t capacity, size_t* required)
{
   const size_t needed = value.size() + 1;
   if(required != nullptr)
      *required = needed;
   if(buffer == nullptr || capacity == 0)
      return capacity >= needed ? kOk : kInvalidParameters;
   const size_t count = std::min(value.size(), capacity - 1);
   std::memcpy(buffer, value.data(), count);
   buffer[count] = '\0';
   return count == value.size() ? kOk : kInvalidParameters;
}

int32_t requireString(const char* value)
{
   return value == nullptr ? kInvalidParameters : kOk;
}

template <typename Fn>
int32_t optionCall(QilDownloadBuildOptionsHandle handle, Fn&& fn)
{
   if(handle == nullptr || !handle->implementation)
      return kInvalidParameters;
   try {
      fn(*handle->implementation);
      return kOk;
   } catch(...) {
      return kUnknownError;
   }
}

template <typename Fn>
int32_t downloadCall(QilSoftwareDownloadHandle handle, Fn&& fn)
{
   if(handle == nullptr || !handle->implementation)
      return kInvalidParameters;
   try {
      QC::ErrorType result = fn(*handle->implementation);
      handle->lastError = result.errorString;
      return static_cast<int32_t>(result.errorCode);
   } catch(const std::exception& error) {
      handle->lastError = error.what();
      return kUnknownError;
   } catch(...) {
      handle->lastError = "Unknown C++ exception";
      return kUnknownError;
   }
}

int32_t copyFlashInfo(const QC::FlashInfo& source, QilFlashInfo& target)
{
   int32_t result = kOk;
   const auto copy = [&result](const std::string& value, char* buffer, size_t capacity) {
      if(copyText(value, buffer, capacity, nullptr) != kOk)
         result = kInvalidParameters;
   };
   copy(source.totalBlocks, target.total_blocks, target.total_blocks_capacity);
   copy(source.blockSize, target.block_size, target.block_size_capacity);
   copy(source.pageSize, target.page_size, target.page_size_capacity);
   copy(source.numPhysicalPartitions, target.num_physical_partitions, target.num_physical_partitions_capacity);
   copy(source.manufacturerId, target.manufacturer_id, target.manufacturer_id_capacity);
   copy(source.serialNum, target.serial_num, target.serial_num_capacity);
   copy(source.fwVersion, target.fw_version, target.fw_version_capacity);
   copy(source.memType, target.mem_type, target.mem_type_capacity);
   copy(source.prodName, target.prod_name, target.prod_name_capacity);
   copy(source.specVersion, target.spec_version, target.spec_version_capacity);
   return result;
}

template <typename T, typename Converter>
int32_t setList(QilDownloadBuildOptionsHandle handle, const T* values, size_t count, Converter&& converter)
{
   if(count != 0 && values == nullptr)
      return kInvalidParameters;
   return optionCall(handle, [&] (QC::DownloadBuildOptions& options) {
      std::vector<typename Converter::value_type> converted;
      converted.reserve(count);
      for(size_t i = 0; i < count; ++i)
         converted.push_back(converter(values[i]));
      converter.set(options, converted);
   });
}

int32_t copyDeviceInfo(const QC::DeviceInfo& source, QilDeviceInfo& target)
{
   int32_t result = kOk;
   target.device_handle = source.deviceHandle;
   const auto copy = [&result](const std::string& value, char* buffer, size_t capacity) {
      if(copyText(value, buffer, capacity, nullptr) != kOk)
         result = kInvalidParameters;
   };
   copy(source.description, target.description, target.description_capacity);
   copy(source.serialNumber, target.serial_number, target.serial_number_capacity);
   copy(source.adbSerialNumber, target.adb_serial_number, target.adb_serial_number_capacity);
   copy(source.location, target.location, target.location_capacity);
   copy(source.vid, target.vid, target.vid_capacity);
   copy(source.pid, target.pid, target.pid_capacity);
   copy(source.edlChipId, target.edl_chip_id, target.edl_chip_id_capacity);
   copy(source.devicePhysicalLocation, target.device_physical_location,
        target.device_physical_location_capacity);
   return result;
}

bool isEdlDevice(const QC::DeviceInfo& device)
{
   static const std::regex edlPattern(".*?EDL|QDLoader.*?");
   const std::list<QC::ProtocolInfo> protocols = QC::DeviceDiscovery::getProtocolList(device.deviceHandle);
   for(const auto& protocol: protocols)
   {
      const bool edlProtocol =
         protocol.protocolType == QC::ProtocolType::PROT_SAHARA ||
         protocol.protocolType == QC::ProtocolType::PROT_FIREHOSE;
      if(edlProtocol && std::regex_search(protocol.description, edlPattern))
         return true;
   }
   return false;
}
}

extern "C" {

QIL_API uint32_t qil_software_download_abi_version(void)
{
   return QIL_SOFTWARE_DOWNLOAD_ABI_VERSION;
}

QIL_API int32_t qil_device_discovery_start_monitoring(void)
{
   std::lock_guard<std::mutex> lock(g_discoveryMutex);
   try {
      QC::DeviceDiscovery::startMonitoring();
      g_discoveryLastError.clear();
      return kOk;
   } catch(const std::exception& error) {
      g_discoveryLastError = error.what();
      return kUnknownError;
   } catch(...) {
      g_discoveryLastError = "Unknown C++ exception";
      return kUnknownError;
   }
}

QIL_API int32_t qil_device_discovery_stop_monitoring(void)
{
   std::lock_guard<std::mutex> lock(g_discoveryMutex);
   try {
      QC::DeviceDiscovery::stopMonitoring();
      g_discoveryLastError.clear();
      return kOk;
   } catch(const std::exception& error) {
      g_discoveryLastError = error.what();
      return kUnknownError;
   } catch(...) {
      g_discoveryLastError = "Unknown C++ exception";
      return kUnknownError;
   }
}

QIL_API int32_t qil_device_discovery_last_error(
   char* buffer, size_t capacity, size_t* required_size)
{
   std::lock_guard<std::mutex> lock(g_discoveryMutex);
   return copyText(g_discoveryLastError, buffer, capacity, required_size);
}

QIL_API int32_t qil_device_discovery_get_edl_devices(
   QilDeviceInfo* records, size_t capacity, size_t* written, size_t* required)
{
   std::lock_guard<std::mutex> lock(g_discoveryMutex);
   if(capacity != 0 && records == nullptr)
      return kInvalidParameters;
   try {
      // The native device manager is stopped after a flash service is
      // destroyed. Re-start it before the next enumeration so the bridge
      // owns the monitor lifecycle instead of leaving a stale monitor active
      // across a USB reset/departure.
      QC::DeviceDiscovery::startMonitoring();
      const std::list<QC::DeviceInfo> allDevices = QC::DeviceDiscovery::getDeviceList();
      std::vector<QC::DeviceInfo> edlDevices;
      for(const auto& device: allDevices)
      {
         if(isEdlDevice(device))
            edlDevices.push_back(device);
      }

      if(required != nullptr)
         *required = edlDevices.size();
      const size_t outputCount = std::min(capacity, edlDevices.size());
      int32_t result = kOk;
      for(size_t i = 0; i < outputCount; ++i)
      {
         if(copyDeviceInfo(edlDevices[i], records[i]) != kOk)
            result = kInvalidParameters;
      }
      if(written != nullptr)
         *written = outputCount;
      g_discoveryLastError.clear();
      return result;
   } catch(const std::exception& error) {
      g_discoveryLastError = error.what();
      return kUnknownError;
   } catch(...) {
      g_discoveryLastError = "Unknown C++ exception";
      return kUnknownError;
   }
}

QIL_API QilSoftwareDownloadHandle qil_software_download_create(uint64_t device_handle)
{
   try {
      auto handle = std::make_unique<QilSoftwareDownloadHandleOpaque>();
      QC::DeviceInfo deviceInfo{};
      deviceInfo.deviceHandle = device_handle;
      handle->implementation = std::make_unique<QC::SoftwareDownload>(deviceInfo);
      return handle.release();
   } catch(...) {
      return nullptr;
   }
}

QIL_API void qil_software_download_destroy(QilSoftwareDownloadHandle handle)
{
   delete handle;
}

QIL_API int32_t qil_software_download_last_error(
   QilSoftwareDownloadHandle handle, char* buffer, size_t capacity, size_t* required_size)
{
   if(handle == nullptr)
      return kInvalidParameters;
   return copyText(handle->lastError, buffer, capacity, required_size);
}

QIL_API int32_t qil_software_download_initialize_service(QilSoftwareDownloadHandle handle)
{
   return downloadCall(handle, [] (QC::SoftwareDownload& value) { return value.initializeService(); });
}

QIL_API int32_t qil_software_download_destroy_service(QilSoftwareDownloadHandle handle)
{
   return downloadCall(handle, [] (QC::SoftwareDownload& value) {
      QC::ErrorType result = value.destroyService();
      {
         std::lock_guard<std::mutex> lock(g_discoveryMutex);
         QC::DeviceDiscovery::stopMonitoring();
      }
      return result;
   });
}

QIL_API int32_t qil_software_download_download_build(
   QilSoftwareDownloadHandle handle, const char* build_path, QilDownloadBuildOptionsHandle options)
{
   if(requireString(build_path) != kOk || options == nullptr || !options->implementation)
      return kInvalidParameters;
   return downloadCall(handle, [build_path, options] (QC::SoftwareDownload& value) {
      QC::DownloadBuildOptions effectiveOptions = *options->implementation;
      std::string diagnostics;
      autoDetectDownloadOptions(build_path, effectiveOptions, &diagnostics);

      QC::ErrorType result = value.downloadBuild(build_path, effectiveOptions);
      if(result.errorCode != QC::ErrorCode::DEVICE_NO_ERROR && !diagnostics.empty())
         result.errorString = diagnostics + "; native service error: " + result.errorString;
      return result;
   });
}

QIL_API int32_t qil_software_download_flatten_meta(
   QilSoftwareDownloadHandle handle,
   const char* build_path,
   const QilFlattenMetaBuildOptions* options)
{
   if(requireString(build_path) != kOk || options == nullptr ||
      requireString(options->memory_type) != kOk ||
      requireString(options->product_flavor) != kOk)
      return kInvalidParameters;

   return downloadCall(handle, [build_path, options] (QC::SoftwareDownload&) {
      QC::FlattenMetaBuildOptions flattenOptions(options->memory_type, options->product_flavor);
      if(options->sku_config != nullptr && options->sku_config[0] != '\0')
         flattenOptions.__set_skuConfig(options->sku_config);
      if(options->output_path != nullptr && options->output_path[0] != '\0')
         flattenOptions.__set_outputPath(options->output_path);
      return QC::SoftwareDownloadUtility::flattenMeta(build_path, flattenOptions);
   });
}

QIL_API QilDownloadBuildOptionsHandle qil_download_build_options_create(int32_t memory_type)
{
   try {
      auto handle = std::make_unique<QilDownloadBuildOptionsHandleOpaque>();
      handle->implementation = std::make_unique<QC::DownloadBuildOptions>(
         static_cast<QC::MemoryType::type>(memory_type));
      return handle.release();
   } catch(...) {
      return nullptr;
   }
}

QIL_API void qil_download_build_options_destroy(QilDownloadBuildOptionsHandle options)
{
   delete options;
}

QIL_API int32_t qil_download_build_options_set_flash_args(
   QilDownloadBuildOptionsHandle handle, const QilFlashArgs* args)
{
   if(args == nullptr)
      return kInvalidParameters;
   if(args->slot_is_set > 1)
      return kInvalidParameters;

   return optionCall(handle, [args] (QC::DownloadBuildOptions& options) {
      options.__set_memoryType(static_cast<QC::MemoryType::type>(args->memory_type));
      options.__set_resetAfterDownload(args->reset != 0);
      options.__set_erase(args->erase != 0);
      if(args->slot_is_set != 0)
         options.__set_slot(args->slot);
      if(args->device_programmer != nullptr && args->device_programmer[0] != '\0')
         options.__set_firehoseProgPath(args->device_programmer);
   });
}

#undef QIL_OPTION_BOOL
#undef QIL_OPTION_INT
#undef QIL_OPTION_ENUM
#undef QIL_OPTION_STRING

}
