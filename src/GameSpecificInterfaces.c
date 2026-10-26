#include "common.h"

extern VehicleInterfaceData g_vehicleInterfaceData[4];

void GSI_VehicleSelectionInfo(u8 *info)
{
    u32 i;
    u8 *vehicle;

    for (i = 0; i < 4; i++) {
        if (*(u32 *)info == g_vehicleInterfaceData[i].iface && g_vehicleInterfaceData[i].cars[0] != NULL) {
            vehicle = *(u8 **)(g_vehicleInterfaceData[i].cars[0] + 0x3BC);
            info[0xC9] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x34);
            info[0xCA] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x35);
            info[0xCB] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x36);
            info[0xCC] = *(u8 *)(*(u8 **)(vehicle + 0x84) + 0x37);
            return;
        }
    }
}
