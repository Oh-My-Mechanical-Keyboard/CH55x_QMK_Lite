
#pragma once

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "usbCommonDescriptors/StdDescriptors.h"
#include "usbCommonDescriptors/HIDClassCommon.h"
// clang-format on

#define EP0_ADDR 0
#define EP1_ADDR 10
#define EP2_ADDR 138

#define EP2_SIZE 32

/** Type define for the device configuration descriptor structure. This must be
 * defined in the application code, as the configuration descriptor contains
 * several sub-descriptors which vary between devices, and which describe the
 * device's usage to the host.
 */
typedef struct {
  USB_Descriptor_Configuration_Header_t Config;

  // Keyboard HID Interface
  USB_Descriptor_Interface_t HID_Interface;
  USB_HID_Descriptor_HID_t HID_KeyboardHID;
  USB_Descriptor_Endpoint_t HID_ReportINEndpoint;
  USB_Descriptor_Endpoint_t HID_ReportOUTEndpoint;
  USB_Descriptor_Interface_t HID_RawInterface;
  USB_HID_Descriptor_HID_t HID_RawDescriptor;
  USB_Descriptor_Endpoint_t HID_RawReportINEndpoint;
  USB_Descriptor_Endpoint_t HID_RawReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

extern __code USB_Descriptor_Device_t DeviceDescriptor;
extern __code USB_Descriptor_Configuration_t ConfigurationDescriptor;
extern __code uint8_t ReportDescriptor[];
extern __code uint8_t RawHIDReportDescriptor[];
extern __code uint8_t LanguageDescriptor[];
extern __code uint16_t SerialDescriptor[];
extern __code USB_Descriptor_String_t ProductDescriptor;
extern __code USB_Descriptor_String_t ManufacturerDescriptor;

