// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD 3-Clause Clear License
#include "qcom-libusb.h"
#ifdef TOOLS_TARGET_WINDOWS
#include "../common/utils.h"
#endif
#include <mutex>
#include <iostream>
#include <unordered_map>
using namespace std;
using namespace Utils;

#ifdef QCOM_LNX_ENV
#include "../../qds_lnx/inc/qdpublic.h"

typedef struct libusb_callback_ctx
{
    QcomLibUSBdevCtx *qcomDev;
    DeviceCtx *deviceCtx;
}LibusbCallbackCtx;

//static LibusbCallbackCtx* gUserCallback = nullptr;
#endif

#ifdef QCOM_WIN_ENV
extern std::atomic<BOOL> _IsQcomLibusbEnable;
#endif

static mutex dev_handle_mutex;
atomic<int> active_transfer(0);
static atomic<bool> processing_device(false);  // Guard against reentrancy in callbacks
static unordered_map<uint64_t, QcomDeviceInfo*> dev_handle_map;
using libusb_handle_map_iter = std::unordered_map<uint64_t, QcomDeviceInfo*>::iterator;

#ifdef QCOM_WIN_ENV
BOOLEAN BuildUserSwKeyPath(LPCSTR SwKeyPath, LPSTR UserKeyPath)
{
    if (!SwKeyPath || !UserKeyPath)
        return FALSE;

    size_t baseLen = strlen(QC_REG_SW_KEY);

    UserKeyPath[0] = '\0';

    if (strncmp(SwKeyPath, QC_REG_SW_KEY, baseLen) == 0) {
        StringCchCopyA(UserKeyPath, QCOM_MAX_STRING_SIZE, QC_REG_SW_KEY_USER);
        StringCchCatA(UserKeyPath, QCOM_MAX_STRING_SIZE, SwKeyPath + baseLen);
    }
    else {
        StringCchCopyA(UserKeyPath, QCOM_MAX_STRING_SIZE, QC_REG_SW_KEY_USER);
        StringCchCatA(UserKeyPath, QCOM_MAX_STRING_SIZE, SwKeyPath);
    }

    return TRUE;
}

BOOLEAN SetProtocolInRegistry(LPCSTR SW_KEY_PATH, DWORD protocol)
{
    HKEY  hSwKey;
    DWORD retCode;

    char userKeyPath[QCOM_MAX_STRING_SIZE];
    BuildUserSwKeyPath(SW_KEY_PATH, userKeyPath);

    retCode = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        SW_KEY_PATH,
        0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_WRITE,
        NULL, &hSwKey, NULL);

    if (retCode != ERROR_SUCCESS) {
        retCode = RegCreateKeyExA(HKEY_CURRENT_USER,
            userKeyPath,
            0, NULL,
            REG_OPTION_NON_VOLATILE, KEY_WRITE,
            NULL, &hSwKey, NULL);

        if (retCode != ERROR_SUCCESS) {
            QCD_Printf(Error, "Failed to open/create registry key: %x\n", retCode);
            return false;
        }
        QCD_Printf(Debug, "New registry path User: %s\n", userKeyPath);
    }

    retCode = RegSetValueExA(hSwKey,
        QC_SPEC_PROTOC,
        0,
        REG_DWORD,
        (const BYTE*)&protocol,
        sizeof(DWORD));

    if (retCode != ERROR_SUCCESS) {
        QCD_Printf(Error, "Failed to set registry value for Protocol: %x\n", retCode);
        RegCloseKey(hSwKey);
        return false;
    }

    RegCloseKey(hSwKey);

    return true;
}

BOOLEAN SetSerialNumInRegistry(LPCSTR SW_KEY_PATH, const char* SerNum)
{
    HKEY  hSwKey;
    DWORD retCode;

    char userKeyPath[QCOM_MAX_STRING_SIZE];
    BuildUserSwKeyPath(SW_KEY_PATH, userKeyPath);

    retCode = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        SW_KEY_PATH,
        0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_WRITE,
        NULL, &hSwKey, NULL);

    if (retCode != ERROR_SUCCESS) {
        retCode = RegCreateKeyExA(HKEY_CURRENT_USER,
            userKeyPath,
            0, NULL,
            REG_OPTION_NON_VOLATILE, KEY_WRITE,
            NULL, &hSwKey, NULL);

        if (retCode != ERROR_SUCCESS) {
            QCD_Printf(Error, "Failed to open/create registry key: %x\n", retCode);
            return false;
        }
        QCD_Printf(Debug, "New registry path User: %s\n", userKeyPath);
    }

    retCode = RegSetValueExA(hSwKey,
        QC_SPEC_SERNUM,
        0,
        REG_SZ,
        (const BYTE*)SerNum,
        strlen(SerNum) + 1);
    
    if (retCode != ERROR_SUCCESS) {
        QCD_Printf(Error, "Failed to set registry value for SerNum: %x\n", retCode);
        RegCloseKey(hSwKey);
        return false;
    }

    RegCloseKey(hSwKey);

    return true;
}

BOOLEAN SetSerialNumMsmInRegistry(LPCSTR SW_KEY_PATH, const char* SerNumMsm)
{
    HKEY  hSwKey;
    DWORD retCode;

    char userKeyPath[QCOM_MAX_STRING_SIZE];
    BuildUserSwKeyPath(SW_KEY_PATH, userKeyPath);

    retCode = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        SW_KEY_PATH,
        0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_WRITE,
        NULL, &hSwKey, NULL);

    if (retCode != ERROR_SUCCESS) {
        retCode = RegCreateKeyExA(HKEY_CURRENT_USER,
            userKeyPath,
            0, NULL,
            REG_OPTION_NON_VOLATILE, KEY_WRITE,
            NULL, &hSwKey, NULL);

        if (retCode != ERROR_SUCCESS) {
            QCD_Printf(Error, "Failed to open/create registry key: %x\n", retCode);
            return false;
        }
        QCD_Printf(Debug, "New registry path User: %s\n", userKeyPath);
    }

    retCode = RegSetValueExA(hSwKey,
        QC_SPEC_SERNUM_MSM,
        0,
        REG_SZ,
        (const BYTE*)SerNumMsm,
        strlen(SerNumMsm) + 1);

    if (retCode != ERROR_SUCCESS) {
        QCD_Printf(Error, "Failed to set registry value for SerNumMsm: %x\n", retCode);
        RegCloseKey(hSwKey);
        return false;
    }

    RegCloseKey(hSwKey);

    return true;
}

BOOLEAN SetDevDetailsInRegistry(LPCSTR SW_KEY_PATH, const char* DevDetails)
{
    HKEY  hSwKey;
    DWORD retCode;

    if (DevDetails == NULL || DevDetails[0] == '\0') {
        QCD_Printf(Error, "Invalid DevDetails parameter\n");
        return false;
    }

    char userKeyPath[QCOM_MAX_STRING_SIZE];
    BuildUserSwKeyPath(SW_KEY_PATH, userKeyPath);

    retCode = RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        SW_KEY_PATH,
        0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_WRITE,
        NULL, &hSwKey, NULL);

    if (retCode != ERROR_SUCCESS) {
        retCode = RegCreateKeyExA(HKEY_CURRENT_USER,
            userKeyPath,
            0, NULL,
            REG_OPTION_NON_VOLATILE, KEY_WRITE,
            NULL, &hSwKey, NULL);

        if (retCode != ERROR_SUCCESS) {
            QCD_Printf(Error, "Failed to open/create registry key: %x\n", retCode);
            return false;
        }
    }

    retCode = RegSetValueExA(hSwKey,
        QC_SPEC_DEV_NAME,
        0,
        REG_SZ,
        (const BYTE*)DevDetails,
        strlen(DevDetails) + 1);

    if (retCode != ERROR_SUCCESS) {
        QCD_Printf(Error, "Failed to set registry value for DevDetails: %x\n", retCode);
        RegCloseKey(hSwKey);
        return false;
    }

    RegCloseKey(hSwKey);

    return true;
}
#endif

VOID DisplayDevices(QcomLibUSBdevCtx* qcom_dev)
{
     QcomDeviceInfo* pDevInfo;
    LIST_ENTRY* head = &qcom_dev->deviceListHead;
    LIST_ENTRY* pos;

    if (!IsListEmpty(head))
    {
        head = &qcom_dev->deviceListHead;
        pos = head->Flink;
        while (pos != head)
        {
            pDevInfo = CONTAINING_RECORD(pos, QcomDeviceInfo, List);
            QCD_Printf(Info, "%s: SerNum: [%s] SerNumMsm: [%s] Protocol: [%ld]\n", __func__, pDevInfo->SerNum, pDevInfo->SerNumMSM, pDevInfo->protocol);
            pos = pos->Flink;
        }
    }
    else {
        QCD_Printf(Info, "%s: deviceListHead is empty\n", __func__);
    }
}

#ifdef QCOM_LNX_ENV
BOOL IsQcomLibusbEnable()
{
    if (access(LIBUSB_CONFIG_FILE, R_OK) != 0) {
        return 0;
    }

    FILE *file = fopen(LIBUSB_CONFIG_FILE, "r");
    if (!file)
        return 0;

    char read_string[QCOM_MAX_STRING_SIZE];
    while(fgets(read_string, sizeof(read_string), file)) {
        if (strstr(read_string, "QCOM_LIBUSB_SUPPORT=1")) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

BOOL create_libusb_devnode(PQcomDeviceInfo newDeviceInf,
                           const char *devName,
                           char *productID,
                           int busNum,
                           int devNum,
                           int config,
                           int interfaceNum)
{
    char devnode_path[QCOM_MAX_STRING_SIZE];
    memset(devnode_path, 0, QCOM_MAX_STRING_SIZE);
    
    snprintf(newDeviceInf->DevDesc, QCOM_MAX_STRING_SIZE, "%s_%s_%d-%d:%d.%d",
            devName, productID, busNum, devNum, config, interfaceNum);
    
    snprintf(devnode_path, QCOM_MAX_STRING_SIZE, "/dev/%s", newDeviceInf->DevDesc);

    strncpy(newDeviceInf->DevDesc, devnode_path, QCOM_MAX_STRING_SIZE - 1);
    newDeviceInf->DevDesc[QCOM_MAX_STRING_SIZE - 1] = '\0';
    QCD_Printf(Info, "%s: checknew devdesc: %s\n",  __func__, newDeviceInf->DevDesc);

    if (mknod(devnode_path, S_IFCHR | 0666, 0) < 0) {
        QCD_Printf(Info, "%s: device nodes already present\n", __func__);
    }

    return EXIT_SUCCESS;
}

const char *fetchFriendlyDeviceName(int protocol, const char *pid)
{
    if (protocol == INTERFACE_DIAG_PROTOCOL)
        return QCOM_DIAG_DEV_NAME;
    else if (protocol == INTERFACE_MODEM_PROTOCOL)
        return QCOM_MODEM_DEV_NAME;
    else if (protocol == INTERFACE_DPL_PROTOCOL)
        return QCOM_DPL_DEV_NAME;
    else if (protocol == INTERFACE_QDSS_PROTOCOL)
        return QCOM_QDSS_DEV_NAME;
    else if (protocol == INTERFACE_EDL_PROTOCOL || (pid && strncmp(pid, QCOM_EDL_PID, 4) == 0))
        return QCOM_EDL_DEV_NAME;
    else if (protocol == INTERFACE_SAHARA_PROTOCOL)
        return QCOM_DIAG_DEV_NAME;
    else
        return nullptr;
}

VOID clearLibusbDeviceCtx(QcomLibUSBdevCtx* qcom_dev)
{
    PLIST_ENTRY pEntry;
    PLIST_ENTRY ItemList = &qcom_dev->deviceListHead;
    PQcomDeviceInfo pItem;

    //EnterCriticalSection(&opLock);
    while (!IsListEmpty(ItemList))
    {
        pEntry = RemoveHeadList(ItemList);
        pItem = CONTAINING_RECORD(pEntry, QcomDeviceInfo, List);
        if (pItem->DevDetailsMP) 
        { 
            delete pItem->DevDetailsMP; 
            pItem->DevDetailsMP = nullptr; 
        }
        free(pItem);
    }
    //LeaveCriticalSection(&opLock);

    return;
}

BOOL remove_device(QcomLibUSBdevCtx* qcom_dev, libusb_device* dev, void* deviceCtx)
{
    PDeviceCtx pdeviceCtx = (PDeviceCtx)deviceCtx;
    PLIST_ENTRY head = &qcom_dev->deviceListHead;
    PLIST_ENTRY pEntry;
    PLIST_ENTRY pos;
    PQcomDeviceInfo pDevInfo;
    int isQcDriver = 1;

    QCD_Printf(Info, "%s happens\n", __func__);

    // Skip removal if we're in the middle of processing a device - prevents
    // reentrancy crash when device disconnects during process_device
    if (processing_device.load())
    {
        QCD_Printf(Info, "%s: skipping removal during device processing\n", __func__);
        return FALSE;
    }

    if (!IsListEmpty(head))
    {
        head = &qcom_dev->deviceListHead;
        pos = head->Flink;
        while (pos != head)
        {
            pEntry = RemoveHeadList(head);
            pDevInfo = CONTAINING_RECORD(pEntry, QcomDeviceInfo, List);
            pDevInfo->Flag = ((ULONG)QC_DEV_TYPE_USB << 8) | QC_DEV_STATE_DEPARTURE << 4 | isQcDriver;
            QCD_Printf(Info, "%s device desc: %s\n", __func__, pDevInfo->DevDesc);
            PushLibUSBToApplication(pDevInfo);
            unlink(pDevInfo->DevDesc);
            pos = pos->Flink;
            free(pDevInfo);
        }
        //system("rm -rf /dev/QCOM_LIBUSB*");  //Added temporarily
    }
    else
    {
        QCD_Printf(Info, "%s: lnx: deviceListHead is empty\n",__func__);
    }
}

BOOL libusb_device_change_callback(libusb_context* ctx,
    libusb_device* dev,
    libusb_hotplug_event event,
    void* user_data)
{
    LibusbCallbackCtx* Callbackdata = static_cast<LibusbCallbackCtx*>(user_data);
    QcomLibUSBdevCtx* qcomDev = Callbackdata->qcomDev;
    DeviceCtx* device_ctx = Callbackdata->deviceCtx;

    if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED) {
        QCD_Printf(Info, "%s: LIBUSB Arrival\n", __func__);
        process_device(qcomDev, dev, device_ctx);
    }
    else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT) {
        QCD_Printf(Info, "%s: LIBUSB Departure\n", __func__);
        remove_device(qcomDev, dev, device_ctx);
        // delete gUserCallback;
        // gUserCallback = nullptr;
        if (qcomDev->dev == dev) {
            libusb_unref_device(qcomDev->dev);
            QCD_Printf(Info, "%s: unref matched libusb_device *dev - qcomDev->dev: 0x%p\n", __func__, qcomDev->dev);
            qcomDev->dev = nullptr;
        }
    }

    return EXIT_SUCCESS;
}

VOID register_libusb_hotplug_callback(QcomLibUSBdevCtx* qcom_dev, void* device_ctx) {
    if (libusb_has_capability(LIBUSB_CAP_HAS_HOTPLUG)) {
        QCD_Printf(Info, "libusb hotplug feature is available\n");

        LibusbCallbackCtx* gUserCallback = new LibusbCallbackCtx;
        gUserCallback->qcomDev = qcom_dev;
        gUserCallback->deviceCtx = (DeviceCtx*)device_ctx;
        QCD_Printf(Info, "%s: UserCallback->qcomDev: 0x%p\n", __func__, qcom_dev);

        libusb_hotplug_register_callback(qcom_dev->ctx,
            (libusb_hotplug_event)
            LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
            LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,
            LIBUSB_HOTPLUG_ENUMERATE,
            QCOM_VENDOR_ID,
            LIBUSB_HOTPLUG_MATCH_ANY,
            LIBUSB_HOTPLUG_MATCH_ANY,
            libusb_device_change_callback,
            gUserCallback,
            NULL);
    }
    else {
        QCD_Printf(Info, "libusb hotplug feature is not available\n");
    }

    return;
}
#endif

BOOL process_device(QcomLibUSBdevCtx* qcom_dev, libusb_device* dev, void* deviceCtx) {
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor* config = nullptr;
    struct libusb_device_handle* handle = nullptr;
    const struct libusb_interface_descriptor* altsetting = nullptr;
    const struct libusb_endpoint_descriptor* endpoint = nullptr;
    int ret = -1;
    char node_path[QCOM_MAX_STRING_SIZE];
    PCHAR value = nullptr;
    CHAR serialNo[QCOM_MAX_STRING_SIZE];
    CHAR serialNoMSM[QCOM_MAX_STRING_SIZE];
    char epm_device[QCOM_MAX_STRING_SIZE];
    int tmp_count = 0;
    int isQcDriver = 1;

    // Get device descriptor
    ret = libusb_get_device_descriptor(dev, &desc);
    if (ret < 0) {
        QCD_Printf(Error, ": %s: Failed to get device descriptor\n", __func__);
        return ret;
    }

    // Match Vendor ID
    if (desc.idVendor != QCOM_VENDOR_ID) {
        return EXIT_FAILURE;
    }

#ifdef QCOM_WIN_ENV
    ZeroMemory(serialNo, QCOM_MAX_STRING_SIZE);
    ZeroMemory(serialNoMSM, QCOM_MAX_STRING_SIZE);
    ZeroMemory(epm_device, QCOM_MAX_STRING_SIZE);
#else
    PDeviceCtx pdeviceCtx = (PDeviceCtx)deviceCtx;
    memset(serialNo, 0, QCOM_MAX_STRING_SIZE);
    memset(serialNoMSM, 0, QCOM_MAX_STRING_SIZE);
    memset(epm_device, 0, QCOM_MAX_STRING_SIZE);
#endif

    char VidString[5];
    char PidString[5];
    snprintf(VidString, sizeof(VidString), "%04X", desc.idVendor);
    snprintf(PidString, sizeof(PidString), "%04X", desc.idProduct);

    if (strstr(PidString, "9301") != nullptr) {
        QCD_Printf(Info, "%s: enumeration of %s is not supported\n", __func__, PidString);
        return LIBUSB_ERROR_NOT_SUPPORTED;
    }

    if (strstr(PidString, "9302") != nullptr) {
        QCD_Printf(Info, "%s: enumeration of %s is not supported\n", __func__, PidString);
        return LIBUSB_ERROR_NOT_SUPPORTED;
    }

	QCD_Printf(Info, "%s: libusb_device *dev context: 0x%p, desc: 0x%p\n", __func__, dev, desc);

    // Set flag to prevent reentrancy - remove_device callback can be triggered
    // during libusb operations below
    processing_device.store(true);

    ret = libusb_open(dev, &handle);
    if (ret < 0) {
        QCD_Printf(Error, "%s: Failed to open device: %s\n", __func__, libusb_error_name(ret));
        processing_device.store(false);
        return ret;
    }

    if (desc.iSerialNumber) {
        ret = libusb_get_string_descriptor_ascii(handle, desc.iSerialNumber, (PUCHAR)serialNo, sizeof(serialNo));
        if (ret < 0) {
            QCD_Printf(Error, "%s: Failed to get iSerialNumber descriptor: %s\n", __func__, libusb_strerror(ret));
            libusb_close(handle);
            processing_device.store(false);
            return ret;
        }
    }
    // QCD_Printf(Info,"serNum (adbID) = %s\n", serialNo);
    // set serNum value in registry for QC_SPEC_SERNUM

    if (desc.iProduct) {
        ret = libusb_get_string_descriptor_ascii(handle, desc.iProduct, (PUCHAR)serialNoMSM, sizeof(serialNoMSM));
        if (ret < 0) {
            QCD_Printf(Error, "%s: Failed to get iProduct descriptor: %s\n", __func__, libusb_strerror(ret));
            libusb_close(handle);
            processing_device.store(false);
            return ret;
        }
    }

    value = strstr(serialNoMSM, "_SN:");
    
    strncpy(epm_device, serialNoMSM, QCOM_MAX_STRING_SIZE - 1);
    epm_device[QCOM_MAX_STRING_SIZE - 1] = '\0';
    if (value) {
        char delimiter[] = "_ ";
        value += strlen("_SN:");
        /* To extract the product serial info */
        if (value = strtok_r(value, delimiter, &value)) {
            //QCD_Printf(Info, "serNumMSM = %s\n", value);
        }
    }
    // set serNumMSM value in registry for QC_SPEC_SERNUM_MSM

    libusb_close(handle);

    if (strstr(epm_device, "Embedded Power Measurement (EPM) device") != nullptr) {
       QCD_Printf(Info, "%s: enumeration of %s is not supported\n", __func__, epm_device);
        processing_device.store(false);
        return LIBUSB_ERROR_NOT_SUPPORTED;
    }

    if (strstr(epm_device, "DEBUG BOARD") != nullptr) {
        QCD_Printf(Info, "%s: enumeration of %s is not supported\n", __func__, epm_device);
        processing_device.store(false);
        return LIBUSB_ERROR_NOT_SUPPORTED;
    }

    // Get active configuration descriptor
    ret = libusb_get_active_config_descriptor(dev, &config);
    if (ret < 0) {
        fprintf(stderr, "Failed to get configuration descriptor: %s\n", libusb_error_name(ret));
        processing_device.store(false);
        return EXIT_FAILURE;
    }

    if (qcom_dev->dev != nullptr) {
        libusb_unref_device(qcom_dev->dev);
        QCD_Printf(Info, "%s: unref previous libusb_device *dev: 0x%p\n", __func__, qcom_dev->dev );
        qcom_dev->dev = nullptr;
    }
    qcom_dev->dev = dev;
    libusb_ref_device(qcom_dev->dev);
    qcom_dev->num_devices++;
    qcom_dev->desc = &desc;
    qcom_dev->config = config;

    // Iterate through interfaces
    for (int i = 0; i < config->bNumInterfaces; i++) {
        // Iterate through alternate settings
        altsetting = config->interface[i].altsetting;

        PQcomDeviceInfo NewDeviceInf;

        NewDeviceInf = (PQcomDeviceInfo)malloc(sizeof(QcomDeviceInfo));
        if (!NewDeviceInf)
        {
            QCD_Printf(Fatal, "Malloc failed\n");
            libusb_free_config_descriptor(config);
            processing_device.store(false);
            return EXIT_FAILURE;
        }
        
        InitializeListHead(&NewDeviceInf->List);

#ifdef QCOM_WIN_ENV
        ZeroMemory(NewDeviceInf, sizeof(QcomDeviceInfo));
#else
        memset(NewDeviceInf, 0, sizeof(QcomDeviceInfo));
#endif

        NewDeviceInf->interfaceNumber = altsetting->bInterfaceNumber;
        NewDeviceInf->devCtx = qcom_dev;
        for (int j = 0; j < altsetting->bNumEndpoints; j++) {
            endpoint = &altsetting->endpoint[j];
            if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_BULK) {
                if (endpoint->bEndpointAddress & LIBUSB_ENDPOINT_IN) {
                    NewDeviceInf->bulk_in_endpointAddr = endpoint->bEndpointAddress;
                    NewDeviceInf->bulk_in_size = endpoint->wMaxPacketSize;
                    QCD_Printf(Debug,"Bulk In Endpoint: %X, Max packet size: %d\n", endpoint->bEndpointAddress, endpoint->wMaxPacketSize);
                } else {
                    NewDeviceInf->bulk_out_endpointAddr = endpoint->bEndpointAddress;
                    NewDeviceInf->bulk_out_size = endpoint->wMaxPacketSize;
                    QCD_Printf(Debug,"Bulk Out Endpoint: %X, Max packet size: %d\n", endpoint->bEndpointAddress, endpoint->wMaxPacketSize);
                }
            } else if ((endpoint->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_INTERRUPT) {
                 //QCD_Printf(Info,"Interrupt Endpoint: %X, Max packet size: %d\n", endpoint->bEndpointAddress, endpoint->wMaxPacketSize);
            }
        }

#ifdef QCOM_WIN_ENV
	    if(serialNo != NULL) {
	    	StringCchCopy((PTSTR)NewDeviceInf->SerNum, 128, (PTSTR)serialNo);
	    }
        if(value != NULL) {
	    	StringCchCopy((PTSTR)NewDeviceInf->SerNumMSM, 128, (PTSTR)value);
	    }
        // Copy full iProduct descriptor string to DevDetails
        if(epm_device[0] != '\0') {
            StringCchCopy((PTSTR)NewDeviceInf->DevDetails, QCOM_MAX_STRING_SIZE, (PTSTR)epm_device);
        }
        NewDeviceInf->protocol = altsetting->bInterfaceProtocol;
        InsertTailList(&qcom_dev->deviceListHead, &NewDeviceInf->List);
#else
    std::unordered_map<std::string, std::string> devDetailsMP;
    devDetailsMP.clear();
    if (serialNo != NULL) {
        strncpy(NewDeviceInf->SerNum, serialNo, QCOM_MAX_STRING_SIZE - 1);
        NewDeviceInf->SerNum[QCOM_MAX_STRING_SIZE - 1] = '\0';
    }
    if (value != NULL) {
        strncpy(NewDeviceInf->SerNumMSM, value, QCOM_MAX_STRING_SIZE - 1);
        NewDeviceInf->SerNumMSM[QCOM_MAX_STRING_SIZE - 1] = '\0';
    }
    // Copy full iProduct descriptor string to DevDetails
    if (epm_device[0] != '\0') {
        strncpy(NewDeviceInf->DevDetails, epm_device, QCOM_MAX_STRING_SIZE - 1);
        NewDeviceInf->DevDetails[QCOM_MAX_STRING_SIZE - 1] = '\0';
    }
    if (pdeviceCtx != NULL) {
        if (pdeviceCtx->mDevParams.DevPath) {
            strncpy(NewDeviceInf->DevPath, pdeviceCtx->mDevParams.DevPath, QCOM_MAX_STRING_SIZE - 1);
            NewDeviceInf->DevPath[QCOM_MAX_STRING_SIZE - 1] = '\0';
        }
        if (pdeviceCtx->mDevParams.Loc) {
            strncpy(NewDeviceInf->Loc, pdeviceCtx->mDevParams.Loc, QCOM_MAX_STRING_SIZE - 1);
            NewDeviceInf->Loc[QCOM_MAX_STRING_SIZE - 1] = '\0';
        }
        if (pdeviceCtx->mDevParams.HwId) {
            strncpy(NewDeviceInf->HwId, pdeviceCtx->mDevParams.HwId, QCOM_MAX_STRING_SIZE - 1);
            NewDeviceInf->HwId[QCOM_MAX_STRING_SIZE - 1] = '\0';
        }
        if (pdeviceCtx->mDevParams.ParentDev) {
            strncpy(NewDeviceInf->ParentDev, pdeviceCtx->mDevParams.ParentDev, QCOM_MAX_STRING_SIZE - 1);
            NewDeviceInf->ParentDev[QCOM_MAX_STRING_SIZE - 1] = '\0';
        }
        if (pdeviceCtx->mDevParams.ParentLocationInfomation) {
            strncpy(NewDeviceInf->ParentLocationInformation, pdeviceCtx->mDevParams.ParentLocationInfomation,   QCOM_MAX_STRING_SIZE - 1);
            NewDeviceInf->ParentLocationInformation[QCOM_MAX_STRING_SIZE - 1] = '\0';
        }
        NewDeviceInf->DevDetailsMP = new std::unordered_map<std::string, std::string>();
        ParseDevDesc(NewDeviceInf->DevDetails, devDetailsMP);
        if(NewDeviceInf->DevDetailsMP)
        {
            *(NewDeviceInf->DevDetailsMP) = devDetailsMP;
        }
    }
    NewDeviceInf->protocol = altsetting->bInterfaceProtocol |
                             altsetting->bInterfaceClass << 8 |
                             altsetting->bAlternateSetting << 16 |
                             altsetting->bInterfaceNumber << 24;
    NewDeviceInf->Flag = ((ULONG)QC_DEV_TYPE_USB << 8) | (QC_DEV_STATE_ARRIVAL << 4) | ((ULONG)QC_DEV_BUS_TYPE_USB << 16) | (isQcDriver);

        const char *devname = fetchFriendlyDeviceName(altsetting->bInterfaceProtocol, PidString);
        if (devname != nullptr &&
            strstr(epm_device, "Embedded Power Measurement (EPM) device") == nullptr) {
                //QCD_Printf(Info, "Check count: %d\n", ++tmp_count);
            create_libusb_devnode(NewDeviceInf, devname, PidString,
                                  libusb_get_bus_number(dev),
                                  libusb_get_device_address(dev),
                                  config->bConfigurationValue,
                                  altsetting->bInterfaceNumber);
            if (NewDeviceInf->DevDesc[0] != NULL) {
                    strncpy(NewDeviceInf->DevName, NewDeviceInf->DevDesc, QCOM_MAX_STRING_SIZE - 1);
                    NewDeviceInf->DevName[QCOM_MAX_STRING_SIZE - 1] = '\0';
            }
        
            PushLibUSBToApplication(NewDeviceInf);
            InsertTailList(&qcom_dev->deviceListHead, &NewDeviceInf->List);
        }
#endif

        //QCD_Printf(Info,"%s: p:%d, b:%d-%d\n", __func__, altsetting->bInterfaceProtocol, libusb_get_bus_number(dev), libusb_get_device_address(dev));

    }

    DisplayDevices(qcom_dev);
    libusb_free_config_descriptor(config);

#ifdef QCOM_WIN_ENV
    UpdateArrivalListLibusb(qcom_dev);
#endif

    // Reset processing flag - reentrancy safe now
    processing_device.store(false);
    return 0;
}

BOOL enumerate_devices(QcomLibUSBdevCtx* qcom_dev)
{
    struct libusb_device** dev_list = nullptr;
    ssize_t cnt;
    int i;

    // Get the list of USB devices
    cnt = libusb_get_device_list(qcom_dev->ctx, &dev_list);
    if (cnt < 0) {
        QCD_Printf(Error, "Error getting USB device list\n");
        libusb_exit(qcom_dev->ctx);
        return EXIT_FAILURE;
    }
    QCD_Printf(Info, "%s: count = %d\n", __func__, cnt);
    qcom_dev->num_devices = -1;
    
    // Iterate through the devices
    for (i = 0; i < cnt; i++) {
        process_device(qcom_dev, dev_list[i], NULL);
    }

    // Cleanup
    libusb_free_device_list(dev_list, 1);

    return 0;
}

/* libusb core device communication API's */

#ifdef QCOM_LNX_ENV
PQcomDeviceInfo find_device(PQcomLibUSBdevCtx devContext, const char* devDesc)
{
    PLIST_ENTRY head = &devContext->deviceListHead;
    PLIST_ENTRY peek = head->Flink;
    while (peek != head)
    {
        QcomDeviceInfo* device = CONTAINING_RECORD(peek, QcomDeviceInfo, List);
        if (strncmp(devDesc, device->DevDesc, QCOM_MAX_STRING_SIZE) == 0) {
            QCD_Printf(Info, "%s: Check2 lnx: device is return: %s\n", __func__, devDesc);
            return device;
        }
        peek = peek->Flink;
    }
    return nullptr;
}
#endif

#ifdef QCOM_WIN_ENV
BOOL fetchDevInfoCtx(const char* DeviceName, ULONG protocol)
{
    if (strstr(DeviceName, "Diagnostics") && protocol == INTERFACE_DIAG_PROTOCOL)
        return true;
    else if (strstr(DeviceName, "Modem") && protocol == INTERFACE_MODEM_PROTOCOL)
        return true;
    else if (strstr(DeviceName, "DPL") && protocol == INTERFACE_DPL_PROTOCOL)
        return true;
    else if (strstr(DeviceName, "QDSS") && protocol == INTERFACE_QDSS_PROTOCOL)
        return true;
    else if (strstr(DeviceName, "WWAN") && protocol == INTERFACE_RMNET_PROTOCOL)
        return true;
    else if (strstr(DeviceName, "QDLoader") && protocol == INTERFACE_EDL_PROTOCOL)
        return true;
    else if (strstr(DeviceName, "Diagnostics") && protocol == INTERFACE_SAHARA_PROTOCOL)
        return true;
    else if (strstr(DeviceName, "QDLoader") && protocol == INTERFACE_SAHARA_PROTOCOL)
        return true;
    else
        return false;
}

VOID UpdateArrivalListLibusb(QcomLibUSBdevCtx* qcomdevCtx)
{
    PLIST_ENTRY head = &qcomdevCtx->deviceListHead;
    PLIST_ENTRY head_arrival = &qcomdevCtx->ArrivalListLibusb;
    PLIST_ENTRY pos_arrival = head_arrival->Flink;
    PLIST_ENTRY pos = head->Flink;
    PQcomDeviceInfo NewDevArrival = nullptr;
    QcomDeviceInfo* pDevInfo = nullptr;
    int count = 0;

    if (IsListEmpty(head)) {
        if (!IsListEmpty(head_arrival)) {
    /* Cleanup arrival list if main list is empty */
    //EnterCriticalSection(&opLock);
            pos_arrival = head_arrival->Flink;
            while (pos_arrival != head_arrival) {
                PLIST_ENTRY next = pos_arrival->Flink;
                QcomDeviceInfo* pDevInfoFree = CONTAINING_RECORD(pos_arrival, QcomDeviceInfo, List);
                RemoveEntryList(pos_arrival);
                free(pDevInfoFree);
                pos_arrival = next;
            }
        QCD_Printf(Info, "%s: Cleared ArrivalListLibusb\n", __func__);
        }
    //LeaveCriticalSection(&opLock);
        return;
    }

    pos = head->Flink;
    pos_arrival = head_arrival->Flink;
    bool isFirstTimeArrival = IsListEmpty(head_arrival);

    while (pos != head)
    {
        pDevInfo = CONTAINING_RECORD(pos, QcomDeviceInfo, List);
        pos = pos->Flink;
        bool modeChangeDetected = false;
        bool skipSameData = false;
        //pos_arrival = head_arrival->Flink;

        if (!isFirstTimeArrival) {
            pos_arrival = head_arrival->Flink;
            while (pos_arrival != head_arrival) {
                QcomDeviceInfo* pDevExist = CONTAINING_RECORD(pos_arrival, QcomDeviceInfo, List);
                PLIST_ENTRY next = pos_arrival->Flink;

                if (strncmp(pDevExist->SerNumMSM, pDevInfo->SerNumMSM, QCOM_MAX_STRING_SIZE) == 0) {

                    bool isOldCrash = (pDevExist->protocol == INTERFACE_SAHARA_PROTOCOL || pDevExist->protocol == INTERFACE_EDL_PROTOCOL);
                    bool isNewCrash = (pDevInfo->protocol == INTERFACE_SAHARA_PROTOCOL || pDevInfo->protocol == INTERFACE_EDL_PROTOCOL);
                    bool isOldNormal = (pDevExist->protocol != INTERFACE_SAHARA_PROTOCOL && pDevExist->protocol != INTERFACE_EDL_PROTOCOL);
                    bool isNewNormal = (pDevInfo->protocol != INTERFACE_SAHARA_PROTOCOL && pDevInfo->protocol != INTERFACE_EDL_PROTOCOL);

                    if ((isOldCrash && isNewNormal) || (isOldNormal && isNewCrash))
                    {
                        QCD_Printf(Info, "%s: Mode change detected!! Updating ArrivalListLibusb\n", __func__);
                        QCD_Printf(Info, "%s: SerNumMSM: %s, new protocol: %d, removing old protocol: %d\n",
                            __func__, pDevExist->SerNumMSM, pDevInfo->protocol, pDevExist->protocol);
                        //PLIST_ENTRY next = pos_arrival->Flink;
                        RemoveEntryList(pos_arrival);
                        free(pDevExist);
                        pos_arrival = next;
                        modeChangeDetected = true;
                        continue;
                    }
                    if (pDevExist->protocol == pDevInfo->protocol || pDevExist->interfaceNumber == pDevInfo->interfaceNumber) {
                        //QCD_Printf(Info, "%s: skip same data\n", __func__);
                        skipSameData = true;
                        break;
                    }
                }
                pos_arrival = next;
            }
            if (!modeChangeDetected && skipSameData)
                continue;
        }

        NewDevArrival = (PQcomDeviceInfo)malloc(sizeof(QcomDeviceInfo));
        if (NewDevArrival) {
            memcpy(NewDevArrival, pDevInfo, sizeof(QcomDeviceInfo));
            InitializeListHead(&NewDevArrival->List);
            InsertTailList(&qcomdevCtx->ArrivalListLibusb, &NewDevArrival->List);
            count++;
            QCD_Printf(Info, "%s: cn:%d pDevInfo: 0x%p, NewDevArrival: 0x%p, SerNumMsm: %s, InfNo: %d, protocol: %d\n",
                __func__, count, pDevInfo, NewDevArrival, NewDevArrival->SerNumMSM, NewDevArrival->interfaceNumber, NewDevArrival->protocol);
        }
    }

}

PQcomDeviceInfo find_device(PQcomLibUSBdevCtx devContext, unsigned int bInterfaceNumber)
{
    PLIST_ENTRY head = &devContext->ArrivalListLibusb;
    PLIST_ENTRY peek = head->Flink;

    while (peek != head)
    {
        QcomDeviceInfo* device = CONTAINING_RECORD(peek, QcomDeviceInfo, List);
        if (bInterfaceNumber == device->interfaceNumber)
        {
            return device;
        }
        peek = peek->Flink;
    }
    return nullptr;
}

PQcomDeviceInfo find_device(PQcomLibUSBdevCtx devContext, const char *SernumMsm, const char* devDesc)
{
    PLIST_ENTRY head = &devContext->ArrivalListLibusb;
    PLIST_ENTRY peek = head->Flink;

    while (peek != head)
    {
        QcomDeviceInfo* device = CONTAINING_RECORD(peek, QcomDeviceInfo, List);

        if (strncmp(SernumMsm, device->SerNumMSM, QCOM_MAX_STRING_SIZE) == 0) {
            QCD_Printf(Info, "%s: Check1 : %s, device->serialNumMSM= %s, Scandev SernumMsm= %s\n",__func__, devDesc, device->SerNumMSM, SernumMsm);
            if (fetchDevInfoCtx(devDesc, device->protocol))
            {
                QCD_Printf(Info, "%s: Check2: device is return: %s\n", __func__, devDesc);
                return device;
            }
        }
        peek = peek->Flink;
    }
    return nullptr;
}
#endif

BOOL QcomLibusbDevice::qcom_libusb_open(QcomLibUSBdevCtx* qcomDev, char* DeviceName, char* SernumMsm, void** devHandle)
{
     QCD_Printf(Info, "%s: DeviceName= %s, qcomDev (qcom_dev): 0x%p\n", __func__, DeviceName, qcomDev);
#ifdef QCOM_WIN_ENV
    QcomDeviceInfo* qcomDevInfo = find_device(qcomDev, SernumMsm, DeviceName);
#else
    QcomDeviceInfo* qcomDevInfo = find_device(qcomDev, DeviceName);
#endif
    return qcom_libusb_open_internal(qcomDevInfo, devHandle);
}

int qcom_libusb_open_internal(QcomDeviceInfo* qcomDevInfo, void** devHandle)
{
    if (qcomDevInfo == nullptr) {
        QCD_Printf(Info, "%s: qcomDevInfo is NULL\n", __func__);
        return LIBUSB_ERROR_NOT_FOUND;
    }
    
    PQcomLibUSBdevCtx qcomDevCtx = qcomDevInfo->devCtx;
    int ret = LIBUSB_ERROR_OTHER;

    if (qcomDevCtx == nullptr || qcomDevInfo->devCtx == nullptr)
    {
       QCD_Printf(Info, "%s: qcomDevCtx = 0x%p, or devCtx = 0x%p is NULL\n", __func__, qcomDevCtx, qcomDevInfo->devCtx);
        return LIBUSB_ERROR_INVALID_PARAM;
    }

    QCD_Printf(Info, "%s libusb_dev: 0x%p, interface number: %d\n", __func__, qcomDevCtx->dev, qcomDevInfo->interfaceNumber);

    if (qcomDevCtx->dev == nullptr) {
        QCD_Printf(Info, "libusb_dev is NULL, wait! device gone\n");
        return LIBUSB_ERROR_NO_DEVICE;
    }

    if (qcomDevCtx->dev_handle == nullptr) {
        QCD_Printf(Info, "%s: Open started..\n", __func__);
        ret = libusb_open(qcomDevCtx->dev, &(qcomDevCtx->dev_handle));
        if (ret != LIBUSB_SUCCESS) {
            QCD_Printf(Error, "failed to open interface. status: %s\n", libusb_error_name(ret));
            return ret;
        }
        else {
            QCD_Printf(Info, "%s ret-status: %s and dev_handle: %p, libusb_dev: 0x%p\n", __func__, libusb_error_name(ret), qcomDevCtx->dev_handle, qcomDevCtx->dev);
        }
    }

    if (libusb_kernel_driver_active(qcomDevCtx->dev_handle, qcomDevInfo->interfaceNumber) == 1) {
        QCD_Printf(Info, "kernel driver is active\n");
        ret = libusb_detach_kernel_driver(qcomDevCtx->dev_handle, qcomDevInfo->interfaceNumber);
        if (ret < 0) {
            QCD_Printf(Error, "failed to detach kernel driver: %s\n", libusb_error_name(ret));
            libusb_close(qcomDevCtx->dev_handle);
            *devHandle = nullptr;
            qcomDevCtx->dev_handle = nullptr;
            return ret;
        } else {
            QCD_Printf(Info, "detached active kernel driver\n");
        }
    }

       // Now, we claim the interface
    ret = libusb_claim_interface(qcomDevCtx->dev_handle, qcomDevInfo->interfaceNumber);
    if (ret < 0) {
        QCD_Printf(Error, "failed to claim interface: %s\n", libusb_error_name(ret));
        //libusb_close(qcomDevCtx->dev_handle);
        *devHandle = nullptr;
        //qcomDevCtx->dev_handle = nullptr;
        return ret;
    }
       
    qcomDevInfo->virtual_handle = static_cast<uint64_t>((uintptr_t)qcomDevCtx->dev_handle + (4 * qcomDevInfo->interfaceNumber) + 4);
    //dev_handle_map[qcomDevInfo->virtual_handle] = qcomDevInfo;
    dev_handle_map.insert(make_pair(qcomDevInfo->virtual_handle, qcomDevInfo));
    *devHandle = reinterpret_cast<void *>(qcomDevInfo->virtual_handle);
    unique_lock<mutex> lock(dev_handle_mutex);
    qcomDevCtx->ref_count++; // this need mutex protection or atomicity
    lock.unlock();



    QCD_Printf(Info, "%s device opened and interface claimed successfully:\n", __func__);
    QCD_Printf(Info, "%s: dev: 0x%p, qcomDevInfo: 0x%p, IntfNo: %d, VirtualHandle : 0x%llx\n",
        __func__, qcomDevCtx->dev, qcomDevInfo, qcomDevInfo->interfaceNumber, qcomDevInfo->virtual_handle);

    return ret;
}

int QcomLibusbDevice::qcom_libusb_open(QcomLibUSBdevCtx* qcomDev, unsigned int bInterfaceNumber, void** devHandle)
{
    QcomDeviceInfo* qcomDevInfo = find_device(qcomDev, bInterfaceNumber);
    return qcom_libusb_open_internal(qcomDevInfo, devHandle);
}

//libusb_handle_map_iter fetchDeviceCtxfromMap(struct libusb_device_handle* Handle)
libusb_handle_map_iter fetchDeviceCtxfromMap(uint64_t Handle)
{

    auto iter = dev_handle_map.find(Handle);
    if (iter == dev_handle_map.end()) {
        QCD_Printf(Info, "Invalid device handle: 0x%llx\n", Handle);
    }

    return iter;
}

VOID QcomLibusbDevice::qcom_libusb_close(void** devHandle)
{
    int ret = 0;

    if (*devHandle == nullptr)
    {
        QCD_Printf(Error, "%s: Error: invalid devHandle\n", __func__);
        return;
    }

    uint64_t virtualHandle = reinterpret_cast<uint64_t>(*devHandle);

    QCD_Printf(Info, "%s: Verify virtual Handle: 0x%llx\n", __func__, virtualHandle);
    if (virtualHandle == 0) {
        QCD_Printf(Error, "Error: invalid virtual Handle: 0x%llx\n", virtualHandle);
        return;
    }

    auto iter = fetchDeviceCtxfromMap(virtualHandle);
    if (iter == dev_handle_map.end()) {
        return;
    }

    QcomDeviceInfo* qcomDevInfo = iter->second;
   
    QCD_Printf(Info, "%s: Close virtual Handle: 0x%llx\n", __func__, virtualHandle);

    unique_lock<mutex> lock(dev_handle_mutex);
    while(active_transfer > 0) {
        lock.unlock();
#ifdef QCOM_WIN_ENV
        Sleep(10);
#else
        usleep(10);
#endif
        lock.lock();
    }

    if (qcomDevInfo != nullptr)
    {
            PQcomLibUSBdevCtx devCtx = qcomDevInfo->devCtx;
            QCD_Printf(Info, "%s dev: 0x%p, qcomDevInfo: 0x%p, InterfaceNo: %d\n", __func__, devCtx->dev, qcomDevInfo, qcomDevInfo->interfaceNumber);
            ret = libusb_release_interface(devCtx->dev_handle, qcomDevInfo->interfaceNumber); // need to verify
            if (ret != LIBUSB_SUCCESS) {
                QCD_Printf(Error, "failed to release interface: %s\n", libusb_error_name(ret));
            }
            
            /* remove virtual handle entry from dev_handle_map*/
            dev_handle_map.erase(iter);
            devCtx->ref_count--;

            if (devCtx->ref_count == 0) {
                QCD_Printf(Info, "%s: closing real libusb dev_handle: %p\n", __func__, devCtx->dev_handle);
                libusb_close(devCtx->dev_handle);
                devCtx->dev_handle = nullptr;
#ifdef QCOM_WIN_ENV
                if (devCtx->dev) {
                    libusb_unref_device(devCtx->dev);
                    QCD_Printf(Info, "%s: unref libusb_device *dev - devCtx->dev: 0x%p\n", __func__, devCtx->dev);
                    devCtx->dev = nullptr;
                }
#endif
            }
            QCD_Printf(Info, "%s: close virtual Handle: 0x%llx, remaining open interfaces: %d\n", __func__, virtualHandle, devCtx->ref_count);
            *devHandle = nullptr;
    }
    else {
        QCD_Printf(Info, "QcomDeviceInfo context not available\n");
    }

    return;
}

int QcomLibusbDevice::qcom_libusb_read
(
    void **devHandle,
    void* buffer,
    int bufferLen,
    int* bytesRead,
    unsigned timeOut
)
{
    int ret = LIBUSB_ERROR_OTHER;

    if (*devHandle == nullptr || buffer == nullptr || bufferLen == 0)
    {
        return LIBUSB_ERROR_INVALID_PARAM;
    }

    uint64_t virtualHandle = reinterpret_cast<uint64_t>(*devHandle);

    auto iter = fetchDeviceCtxfromMap(virtualHandle);
    if (iter == dev_handle_map.end()) {
        return LIBUSB_ERROR_NOT_FOUND;
    }

    QcomDeviceInfo* qcomDevInfo = iter->second;


    ret = libusb_bulk_transfer(qcomDevInfo->devCtx->dev_handle, qcomDevInfo->bulk_in_endpointAddr,reinterpret_cast<unsigned char*>(buffer), bufferLen, bytesRead, timeOut);
    QCD_Printf(Debug, "%s: Received bytes: %d/%d, ret: %s\n", __func__, *bytesRead, bufferLen,libusb_error_name(ret));
    if (ret != LIBUSB_SUCCESS)
    {
       QCD_Printf(Error, "%s: failed to read bulk transfer: %s\n", __func__, libusb_error_name(ret));
       goto cleanup;
    }

cleanup:
    return ret;
}

int QcomLibusbDevice::qcom_libusb_write
(
    void** devHandle,
    void* buffer,
    int bufferLen,
    int* bytesSent,
    unsigned timeOut
)
{
    int ret = LIBUSB_ERROR_OTHER;
    timeOut = 1000;

    if (*devHandle == nullptr || buffer == nullptr || bufferLen == 0)
    {
        return LIBUSB_ERROR_INVALID_PARAM;
    }

    uint64_t virtualHandle = reinterpret_cast<uint64_t>(*devHandle);

    auto iter = fetchDeviceCtxfromMap(virtualHandle);
    if (iter == dev_handle_map.end()) {
        return LIBUSB_ERROR_NOT_FOUND;
    }

    QcomDeviceInfo* qcomDevInfo = iter->second;
    
    unique_lock<mutex> lock(dev_handle_mutex);
    ++active_transfer;
    lock.unlock();

    ret = libusb_bulk_transfer(qcomDevInfo->devCtx->dev_handle, qcomDevInfo->bulk_out_endpointAddr, reinterpret_cast<unsigned char*>(buffer), bufferLen, bytesSent, timeOut);
    QCD_Printf(Debug, "%s: Sent bytes: %d/%d, ret: %s\n",__func__, *bytesSent, bufferLen, libusb_error_name(ret));
    if (ret != LIBUSB_SUCCESS)
    {
        QCD_Printf(Error, "%s: failed to write bulk transfer: %s\n", __func__,libusb_error_name(ret));
        goto cleanup;
    }

    if ((bufferLen % qcomDevInfo->bulk_out_size) == 0) {
        int zlpSent = 0;
        int ret_zlp = libusb_bulk_transfer(qcomDevInfo->devCtx->dev_handle, qcomDevInfo->bulk_out_endpointAddr, nullptr, 0, &zlpSent, timeOut);
        if (ret_zlp != LIBUSB_SUCCESS)
        {
            QCD_Printf(Error, "%s: failed to send ZLP bulk transfer: %s\n", __func__, libusb_error_name(ret_zlp));
            ret = ret_zlp;
            goto cleanup;
        }
        QCD_Printf(Debug, "%s: ZLP sent successfully. bytes: %d/%d, ret: %s\n",__func__, zlpSent, 0, libusb_error_name(ret_zlp));
    }


cleanup:
    lock.lock();
    --active_transfer;
    lock.unlock();
    return ret;
}


BOOL initialize_libusb(QcomLibUSBdevCtx *qcom_dev)
{
    // Initialize libusb
    QCD_Printf(Info, "%s\n", __func__);
    if (libusb_init(&qcom_dev->ctx) < 0) {
        QCD_Printf(Error, "Failed to initialize libusb\n");
        return EXIT_FAILURE;
    }
    InitializeListHead(&qcom_dev->deviceListHead);
    InitializeListHead(&qcom_dev->ArrivalListLibusb);
	qcom_dev->dev = nullptr;
	qcom_dev->dev_handle = nullptr;
	qcom_dev->ref_count = 0;

    //libusb_set_option(qcom_dev->ctx, LIBUSB_OPTION_LOG_LEVEL, 3); /* Enable libusb library logs */

    return LIBUSB_SUCCESS;
}

VOID deinitialize_libusb(QcomLibUSBdevCtx* qcom_dev)
{
#ifdef QCOM_WIN_ENV
    if (_IsQcomLibusbEnable == true) {
        QCD_Printf(Info, "freeing up libusb_device\n");
        if (qcom_dev->dev) {
            libusb_unref_device(qcom_dev->dev);
            QCD_Printf(Info, "%s: unref libusb_device *dev: 0x%p\n", __func__, qcom_dev->dev );
            qcom_dev->dev = nullptr;
        }
    }
#endif
#ifdef QCOM_LNX_ENV
    if (qcom_dev->dev) {
        libusb_unref_device(qcom_dev->dev);
        QCD_Printf(Info, "%s: unref libusb_device *dev: 0x%p\n", __func__, qcom_dev->dev );
        qcom_dev->dev = nullptr;
    }
    system("rm -rf /dev/QCOM_LIBUSB*");
#endif
    libusb_exit(qcom_dev->ctx);
    
    QCD_Printf(Info, "%s\n", __func__);
}
