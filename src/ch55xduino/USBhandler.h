#ifndef __USB_HANDLER_H__
#define __USB_HANDLER_H__

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "USBconstant.h"
// clang-format on

// clang-format off
extern __xdata __at (EP0_ADDR) uint8_t Ep0Buffer[];
extern __xdata __at (EP1_ADDR) uint8_t Ep1Buffer[];
extern __xdata __at (EP2_ADDR) uint8_t Ep2Buffer[];
// clang-format on

extern volatile __data uint8_t UpPoint1_Busy;
extern volatile __data uint8_t UpPoint2_Busy;

void USB_EP1_IN();
void USB_EP1_OUT();
void USB_EP2_IN();
void USB_EP2_OUT();

#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer)

// Out
#define EP0_OUT_Callback USB_EP0_OUT
#define EP1_OUT_Callback USB_EP1_OUT
#define EP2_OUT_Callback USB_EP2_OUT
#define EP3_OUT_Callback NOP_Process
#define EP4_OUT_Callback NOP_Process

// SOF
#define EP0_SOF_Callback NOP_Process
#define EP1_SOF_Callback NOP_Process
#define EP2_SOF_Callback NOP_Process
#define EP3_SOF_Callback NOP_Process
#define EP4_SOF_Callback NOP_Process

// IN
#define EP0_IN_Callback USB_EP0_IN
#define EP1_IN_Callback USB_EP1_IN
#define EP2_IN_Callback USB_EP2_IN
#define EP3_IN_Callback NOP_Process
#define EP4_IN_Callback NOP_Process

// SETUP
#define EP0_SETUP_Callback USB_EP0_SETUP
#define EP1_SETUP_Callback NOP_Process
#define EP2_SETUP_Callback NOP_Process
#define EP3_SETUP_Callback NOP_Process
#define EP4_SETUP_Callback NOP_Process

void USBInterrupt(void);
void USBDeviceCfg();
void USBDeviceIntCfg();
void USBDeviceEndPointCfg();

#endif
