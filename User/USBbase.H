#ifndef	__USBBASE_H__
#define __USBBASE_H__

#include "CH554.H"

extern void USBDeviceInit();
extern void Enp2BlukIn();
extern UINT8X UserEp2Buf[64];
extern UINT8 Endp2Busy;
extern UINT8 Endp2OutFlag;
extern UINT8 SetupReq,SetupLen,Ready,Count,FLAG,UsbConfig;
extern UINT8X Ep2Buffer[128>(2*MAX_PACKET_SIZE+4)?128:(2*MAX_PACKET_SIZE+4)];

#endif
