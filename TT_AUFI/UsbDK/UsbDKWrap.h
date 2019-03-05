/**********************************************************************
* Copyright (c) 2013-2014  Red Hat, Inc.
*
* Developed by Daynix Computing LTD.
*
* Authors:
*     Dmitry Fleytman <dmitry@daynix.com>
*     Pavel Gurvich <pavel@daynix.com>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
**********************************************************************/

#pragma once

typedef struct tag_USB_DK_DEVICE_ID
{
  WCHAR DeviceID[MAX_DEVICE_ID_LEN];
  WCHAR InstanceID[MAX_DEVICE_ID_LEN];
} USB_DK_DEVICE_ID, *PUSB_DK_DEVICE_ID;

static inline
void UsbDkFillIDStruct(USB_DK_DEVICE_ID *ID, PCWCHAR DeviceID, PCWCHAR InstanceID)
{
  wcsncpy_s(ID->DeviceID, DeviceID, MAX_DEVICE_ID_LEN);
  wcsncpy_s(ID->InstanceID, InstanceID, MAX_DEVICE_ID_LEN);
}

typedef struct tag_USB_DK_DEVICE_INFO
{
  USB_DK_DEVICE_ID ID;
  ULONG64 FilterID;
  ULONG64 Port;
  ULONG64 Speed;
  USB_DEVICE_DESCRIPTOR DeviceDescriptor;
} USB_DK_DEVICE_INFO, *PUSB_DK_DEVICE_INFO;

typedef struct tag_USB_DK_CONFIG_DESCRIPTOR_REQUEST
{
  USB_DK_DEVICE_ID ID;
  ULONG64 Index;
} USB_DK_CONFIG_DESCRIPTOR_REQUEST, *PUSB_DK_CONFIG_DESCRIPTOR_REQUEST;

typedef struct tag_USB_DK_ISO_TRANSFER_RESULT
{
  ULONG64 ActualLength;
  ULONG64 TransferResult;
} USB_DK_ISO_TRANSFER_RESULT, *PUSB_DK_ISO_TRANSFER_RESULT;

typedef struct tag_USB_DK_GEN_TRANSFER_RESULT
{
  ULONG64 BytesTransferred;
  ULONG64 UsbdStatus; // USBD_STATUS code
} USB_DK_GEN_TRANSFER_RESULT, *PUSB_DK_GEN_TRANSFER_RESULT;

typedef struct tag_USB_DK_TRANSFER_RESULT
{
  USB_DK_GEN_TRANSFER_RESULT GenResult;
  PVOID64 IsochronousResultsArray; // array of USB_DK_ISO_TRANSFER_RESULT
} USB_DK_TRANSFER_RESULT, *PUSB_DK_TRANSFER_RESULT;

typedef struct tag_USB_DK_TRANSFER_REQUEST
{
  ULONG64 EndpointAddress;
  PVOID64 Buffer;
  ULONG64 BufferLength;
  ULONG64 TransferType;
  ULONG64 IsochronousPacketsArraySize;
  PVOID64 IsochronousPacketsArray;

  USB_DK_TRANSFER_RESULT Result;
} USB_DK_TRANSFER_REQUEST, *PUSB_DK_TRANSFER_REQUEST;

typedef enum
{
  TransferFailure = 0,
  TransferSuccess,
  TransferSuccessAsync
} TransferResult;

typedef enum
{
  NoSpeed = 0,
  LowSpeed,
  FullSpeed,
  HighSpeed,
  SuperSpeed
} USB_DK_DEVICE_SPEED;

typedef enum
{
  ControlTransferType,
  BulkTransferType,
  InterruptTransferType,
  IsochronousTransferType
} USB_DK_TRANSFER_TYPE;

#include "windows.h"
#include <stdint.h>
#include "usb.h"

typedef BOOL(__cdecl *USBDK_GET_DEVICES_LIST)(
  PUSB_DK_DEVICE_INFO *DeviceInfo,
  PULONG DeviceNumber
  );
typedef void(__cdecl *USBDK_RELEASE_DEVICES_LIST)(
  PUSB_DK_DEVICE_INFO DeviceInfo
  );
typedef HANDLE(__cdecl *USBDK_START_REDIRECT)(
  PUSB_DK_DEVICE_ID DeviceId
  );
typedef BOOL(__cdecl *USBDK_STOP_REDIRECT)(
  HANDLE DeviceHandle
  );
typedef BOOL(__cdecl *USBDK_GET_CONFIGURATION_DESCRIPTOR)(
  PUSB_DK_CONFIG_DESCRIPTOR_REQUEST Request,
  PUSB_CONFIGURATION_DESCRIPTOR *Descriptor,
  PULONG Length
  );
typedef void(__cdecl *USBDK_RELEASE_CONFIGURATION_DESCRIPTOR)(
  PUSB_CONFIGURATION_DESCRIPTOR Descriptor
  );
typedef TransferResult(__cdecl *USBDK_WRITE_PIPE)(
  HANDLE DeviceHandle,
  PUSB_DK_TRANSFER_REQUEST Request,
  LPOVERLAPPED lpOverlapped
  );
typedef TransferResult(__cdecl *USBDK_READ_PIPE)(
  HANDLE DeviceHandle,
  PUSB_DK_TRANSFER_REQUEST Request,
  LPOVERLAPPED lpOverlapped
  );
typedef BOOL(__cdecl *USBDK_ABORT_PIPE)(
  HANDLE DeviceHandle,
  ULONG64 PipeAddress
  );
typedef BOOL(__cdecl *USBDK_RESET_PIPE)(
  HANDLE DeviceHandle,
  ULONG64 PipeAddress
  );
typedef BOOL(__cdecl *USBDK_SET_ALTSETTING)(
  HANDLE DeviceHandle,
  ULONG64 InterfaceIdx,
  ULONG64 AltSettingIdx
  );
typedef BOOL(__cdecl *USBDK_RESET_DEVICE)(
  HANDLE DeviceHandle
  );
typedef HANDLE(__cdecl *USBDK_GET_REDIRECTOR_SYSTEM_HANDLE)(
  HANDLE DeviceHandle
  );

struct usbdk_lib {
  HMODULE module;

  USBDK_GET_DEVICES_LIST			GetDevicesList;
  USBDK_RELEASE_DEVICES_LIST		ReleaseDevicesList;
  USBDK_START_REDIRECT			StartRedirect;
  USBDK_STOP_REDIRECT			StopRedirect;
  USBDK_GET_CONFIGURATION_DESCRIPTOR	GetConfigurationDescriptor;
  USBDK_RELEASE_CONFIGURATION_DESCRIPTOR	ReleaseConfigurationDescriptor;
  USBDK_READ_PIPE				ReadPipe;
  USBDK_WRITE_PIPE			WritePipe;
  USBDK_ABORT_PIPE			AbortPipe;
  USBDK_RESET_PIPE			ResetPipe;
  USBDK_SET_ALTSETTING			SetAltsetting;
  USBDK_RESET_DEVICE			ResetDevice;
  USBDK_GET_REDIRECTOR_SYSTEM_HANDLE	GetRedirectorSystemHandle;
};

extern struct usbdk_lib UsbDk;

bool load_usbdk();
void unload_usbdk();

int control_transfer(HANDLE handle, uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
  unsigned char *data, uint16_t wLength, unsigned int timeout);

int bulk_transfer(HANDLE handle,
  uint8_t endpoint, uint8_t *data, int length,
  int *actual_length, OVERLAPPED * ovlp, unsigned int timeout);