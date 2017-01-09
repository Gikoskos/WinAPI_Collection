#include <Common.h>

#include <stdio.h> //fprintf
#include <setupapi.h>
#include <initguid.h>
#include <usbiodef.h> //for GUID_DEVINTERFACE_USB_DEVICE

#define P_ERR(func) \
    if (GetLastError()) { \
        fprintf(stderr, "%s failed with GetLastError == %ld", func, GetLastError()); \
    }

typedef struct _USBDEVICE_NODE {
    SLIST_ENTRY ItemEntry;
    PWSTR *szDescription;
    PWSTR *sz;
    ULONG pid; //product ID
    ULONG vid; //vendor ID
} USBDEV_DATA;


/**
 * @brief Fill a PROCESSENTRY32W array with data from all running processes
 *
 * Get all PROCESSENTRY32W data for every process that's currently running
 * on your Windows system.
 *
 * @param pe32List
 * @return The number of running processes on the system, or 0 on error.
 */
BOOL GetDevIDs(USBDEV_DATA *dev, TCHAR *devpath)
{
    if (devpath == NULL)  return FALSE;
    /* precaution to check if devicepath actually has vid and pid stored */
    if (devpath[8] == 'v' && devpath[9] == 'i' && devpath[10] == 'd') {
        TCHAR temp[5];

        temp[4] = '\0';
        temp[0] = devpath[12];
        temp[1] = devpath[13];
        temp[2] = devpath[14];
        temp[3] = devpath[15];
        dev->vid = _tcstoul(temp, NULL, 16);
        temp[0] = devpath[21];
        temp[1] = devpath[22];
        temp[2] = devpath[23];
        temp[3] = devpath[24];
        dev->pid = _tcstoul(temp, NULL, 16);
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Fill a USBDEV_DATA array with data from all connected USB devices
 *
 * Get all USBDEV_DATA data for every USB device that's currently connected
 * on the PC.
 *
 * @param pe32List
 * @return The number of running processes on the system, or 0 on error.
 */
BOOL GetConnectedUSBDevs(_Out_ USBDEV_DATA **dev)
{
    HDEVINFO hUSBDevInfo;
    SP_DEVICE_INTERFACE_DATA USBDevIntf, USBHubIntf;
    SP_DEVICE_INTERFACE_DETAIL_DATA *USBDevIntfDetail, *USBHubIntfDetail;
    SP_DEVINFO_DATA DevData;
    USBDEV_DATA usbDev;
    DWORD dwSize, dwUSBDevIdx = 0, dwUSBHubIdx = 0;
    PWSTR szUSBDevName;
    wchar_t tmp;
    UINT idx = 0;

    hUSBDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE,
                                      NULL,
                                      NULL,
                                      DIGCF_PRESENT | 
                                      DIGCF_ALLCLASSES | 
                                      DIGCF_DEVICEINTERFACE);
    if (hUSBDevInfo == INVALID_HANDLE_VALUE) {
        P_ERR("SetupDiGetClassDevsW");
        return FALSE;
    }

    USBDevIntf.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    if (!SetupDiEnumDeviceInterfaces(hUSBDevInfo,
                                     NULL,
                                     &GUID_DEVINTERFACE_USB_DEVICE,
                                     dwUSBDevIdx,
                                     &USBDevIntf)
        ) {
        P_ERR("SetupDiEnumDeviceInterfaces");
        SetupDiDestroyDeviceInfoList(hUSBDevInfo);
        return FALSE;
    }

    while (GetLastError() != ERROR_NO_MORE_ITEMS) {
        DevData.cbSize = sizeof(DevData);
        SetupDiGetDeviceInterfaceDetail(hUSBDevInfo, &USBDevIntf, NULL, 0, &dwSize, NULL);

        USBDevIntfDetail = win_malloc(dwSize);
        USBDevIntfDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(hUSBDevInfo,
                                            &USBDevIntf,
                                            USBDevIntfDetail,
                                            dwSize,
                                            &dwSize,
                                            &DevData)
            ) {
            P_ERR("SetupDiGetDeviceInterfaceDetail");
            goto SKIP_DEVICE;
        }

        if (!GetDevIDs(&usbDev, USBDevIntfDetail->DevicePath))
            goto SKIP_DEVICE;

        wprintf(L"%04x:%04x\n", usbDev.vid, usbDev.pid);

        dwSize = 0;
        if (!SetupDiGetDeviceRegistryProperty(hUSBDevInfo, 
                                              &DevData,
                                              SPDRP_DEVICEDESC,
                                              NULL, (PBYTE)&tmp, sizeof(wchar_t),
                                              &dwSize)
            &&
            GetLastError() == ERROR_INSUFFICIENT_BUFFER
           ) {
            szUSBDevName = win_malloc(dwSize);
            SetupDiGetDeviceRegistryProperty(hUSBDevInfo, 
                                             &DevData,
                                             SPDRP_DEVICEDESC,
                                             NULL,
                                             (PBYTE)szUSBDevName,
                                             dwSize,
                                             NULL);
            wprintf(L"len = %ld\tstr = %s\n", dwSize, szUSBDevName);
            win_free(szUSBDevName);
        } else
            P_ERR("SetupDiGetDeviceRegistryProperty");

SKIP_DEVICE:
        idx++;
        win_free(USBDevIntfDetail);

        SetupDiEnumDeviceInterfaces(hUSBDevInfo,
                                    NULL,
                                    &GUID_DEVINTERFACE_USB_DEVICE,
                                    ++dwUSBDevIdx,
                                    &USBDevIntf);
    }

    SetupDiDestroyDeviceInfoList(hUSBDevInfo);

    return FALSE;
}

int wmain(int argc, wchar_t **argv)
{
    USBDEV_DATA *usb_dev_array;
 
    GetConnectedUSBDevs(&usb_dev_array);
    return 0;
}
