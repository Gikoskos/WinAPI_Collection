#include <Common.h>

#include <stdio.h>
#include <stdlib.h>
#include <setupapi.h>
#include <initguid.h>
#include <usbiodef.h> //for GUID_DEVINTERFACE_USB_DEVICE

#define P_ERR(func) \
    if (GetLastError()) { \
        fprintf(stderr, "%s failed with GetLastError == %ld", func, GetLastError()); \
    }

typedef struct _USBDEV_DATA {
    PWSTR *szDesc;
    ULONG pid; //product ID
    ULONG vid; //vendor ID
} USBDEV_DATA;


/*****************************************************************************************
 *****************************************************************************************
 ******************singly linked list with sentinel implementation from ******************
 *************https://github.com/Gikoskos/Data_Structures_and_Algorithms_in_C*************
 *****************************************************************************************
 *****************************************************************************************/

typedef void (*CustomDataCallback)(void*);


typedef struct _SLListNode {
    void *pData;
    struct _SLListNode *nxt;
} SLListNode;

typedef struct _SSLList {
    SLListNode *head, *sentinel;
} SSLList;


SSLList *newSSLList(void)
{
    SSLList *newList = calloc(1, sizeof(SSLList));

    newList->head = calloc(1, sizeof(SLListNode));
    newList->sentinel = newList->head;

    return newList;
}

SLListNode *insertNodeSSLList(SSLList *ssllList, void *pData)
{
    SLListNode *new_node = NULL;

    if (ssllList) {

        new_node = malloc(sizeof(SLListNode));

        new_node->pData = pData;

        new_node->nxt = ssllList->head;
        ssllList->head = new_node;

    }

    return new_node;
}

SLListNode *appendNodeSSLList(SSLList *ssllList, void *pData)
{
    SLListNode *new_node = NULL;

    if (ssllList) {
        if (ssllList->sentinel == ssllList->head) {

            new_node = insertNodeSSLList(ssllList, pData);
            
        } else {
            SLListNode *curr;

            new_node = malloc(sizeof(SLListNode));

            new_node->pData = pData;
            new_node->nxt = ssllList->sentinel;

            for (curr = ssllList->head; curr->nxt != ssllList->sentinel; curr = curr->nxt);

            curr->nxt = new_node;
        }
    }

    return new_node;
}

void *deleteNodeSSLList(SSLList *ssllList, void *pData)
{
    void *pRet = NULL;

    if (ssllList) {
        SLListNode *curr, *prev = NULL;

        ssllList->sentinel->pData = pData;
        for (curr = ssllList->head; curr->pData != pData; curr = curr->nxt)
            prev = curr;

        if (curr != ssllList->sentinel) {
            pRet = pData;

            if (prev)
                prev->nxt = curr->nxt;
            else
                ssllList->head = curr->nxt;

            free(curr);
        }
    }

    return pRet;
}

SLListNode *findNodeSSLList(SSLList *ssllList, void *pToFind)
{
    SLListNode *curr = NULL;

    if (ssllList) {
        ssllList->sentinel->pData = pToFind;
        for (curr = ssllList->head; curr->pData != ssllList->sentinel; curr = curr->nxt);

        if (curr == ssllList->sentinel)
            curr = NULL;
    }

    return curr;
}

void traverseSSLList(SSLList *ssllList, CustomDataCallback handleData)
{
    if (ssllList && handleData)
        for (SLListNode *curr = ssllList->head; curr != ssllList->sentinel; curr = curr->nxt)
            handleData(curr->pData);
}

void deleteSSLList(SSLList **ssllList, CustomDataCallback freeData)
{
    if (ssllList) {
        SLListNode *curr, *tmp;

        for (curr = (*ssllList)->head; curr != (*ssllList)->sentinel;) {
            if (freeData)
                freeData(curr->pData);

            tmp = curr;
            curr = curr->nxt;
            free(tmp);
        }

        if (*ssllList) {
            free((*ssllList)->sentinel);
            free(*ssllList);
            *ssllList = NULL;
        }
    }
}

/*****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************
 *****************************************************************************************/


/**
 * @brief Extract USB IDs from a given devpath
 *
 * Extracts USB IDs from a devpath string, converts them to
 * unsigned long and stores them in a USBDEV_DATA structure.
 *
 * @param dev The USB IDs are stored in this structure.
 * @param devpath Wide char string that contains the device system path. 
 * @return TRUE on success, FALSE if the arguments are invalid.
 */
BOOL GetDevIDs(USBDEV_DATA *dev, wchar_t *devpath)
{
    if (!devpath || !dev)  return FALSE;
    /* precaution to check if devicepath actually has vid and pid stored */
    if (devpath[8] == 'v' && devpath[9] == 'i' && devpath[10] == 'd') {
        wchar_t temp[5];

        temp[4] = '\0';
        temp[0] = devpath[12];
        temp[1] = devpath[13];
        temp[2] = devpath[14];
        temp[3] = devpath[15];
        dev->vid = wcstoul(temp, NULL, 16);
        temp[0] = devpath[21];
        temp[1] = devpath[22];
        temp[2] = devpath[23];
        temp[3] = devpath[24];
        dev->pid = wcstoul(temp, NULL, 16);
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief Fills a singly linked list with data from all connected USB devices
 *
 * This function creates a new SSLList and scans the PC for all connected
 * USB devices (and hubs in the future). For every USB device found, a new USBDEV_DATA structure
 * is dynamically allocated and filled with that USB device's vendor and product IDs,
 * and a short description string in WinAPI utf-16 (if you call WinAPI's encoding UTF-16) format.
 *
 * @return An SSLList with all the connected USB devices, if everything ran successfully. NULL on failure
 */
SSLList *GetConnectedUSBDevList(void)
{
    HDEVINFO hUSBDevInfo;
    SP_DEVICE_INTERFACE_DATA USBDevIntf;
    SP_DEVICE_INTERFACE_DETAIL_DATA *USBDevIntfDetail;
    SP_DEVINFO_DATA DevData;
    DWORD dwSize, dwUSBDevIdx = 0;
    wchar_t tmp;
    UINT idx = 0;
    SSLList *usbList = newSSLList();

    hUSBDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0,
                                      DIGCF_PRESENT | DIGCF_ALLCLASSES | DIGCF_DEVICEINTERFACE);
    if (hUSBDevInfo == INVALID_HANDLE_VALUE) {
        P_ERR("SetupDiGetClassDevsW");
        return 0;
    }

    USBDevIntf.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    if (!SetupDiEnumDeviceInterfaces(hUSBDevInfo, 0, &GUID_DEVINTERFACE_USB_DEVICE, dwUSBDevIdx, &USBDevIntf)) {
        P_ERR("SetupDiEnumDeviceInterfaces");
        SetupDiDestroyDeviceInfoList(hUSBDevInfo);
        return 0;
    }

    while (GetLastError() != ERROR_NO_MORE_ITEMS) {
        USBDEV_DATA *usbDev = win_calloc(1, sizeof(USBDEV_DATA));

        DevData.cbSize = sizeof(DevData);
        SetupDiGetDeviceInterfaceDetail(hUSBDevInfo, &USBDevIntf, 0, 0, &dwSize, 0);

        USBDevIntfDetail = win_malloc(dwSize);
        USBDevIntfDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(hUSBDevInfo, &USBDevIntf, USBDevIntfDetail, dwSize, &dwSize, &DevData)) {
            P_ERR("SetupDiGetDeviceInterfaceDetail");
            win_free(usbDev);
            goto SKIP_DEVICE;
        }

        if (!GetDevIDs(usbDev, USBDevIntfDetail->DevicePath)) {
            win_free(usbDev);
            goto SKIP_DEVICE;
        }

        dwSize = 0;
        if (!SetupDiGetDeviceRegistryProperty(hUSBDevInfo, &DevData, SPDRP_DEVICEDESC, 0, (PBYTE)&tmp, sizeof(wchar_t), &dwSize)) {

            if (dwSize) {

                dwSize++;
                usbDev->szDesc = win_malloc(dwSize);
                if (!SetupDiGetDeviceRegistryProperty(hUSBDevInfo, &DevData, SPDRP_DEVICEDESC, 0, (PBYTE)usbDev->szDesc, dwSize, 0)) {
                    P_ERR("SetupDiGetDeviceRegistryProperty");
                    win_free(usbDev->szDesc);
                    usbDev->szDesc = 0;
                }

            }

        } else {
            P_ERR("SetupDiGetDeviceRegistryProperty");
        }

        appendNodeSSLList(usbList, (void*)usbDev);

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

    return usbList;
}

void PrintUSBData(void *param)
{
    USBDEV_DATA *dev = (USBDEV_DATA*)param;

    wprintf(L"USB ID %04x:%04x\t Description = %s\n", dev->pid, dev->vid, dev->szDesc);
}

void DeleteUSBData(void *param)
{
    USBDEV_DATA *dev = (USBDEV_DATA*)param;

    win_free(dev->szDesc);
    win_free(dev);
}

int main(void)
{
    SSLList *usbList = GetConnectedUSBDevList();
 
    traverseSSLList(usbList, PrintUSBData);

    deleteSSLList(&usbList, DeleteUSBData);

    return 0;
}
