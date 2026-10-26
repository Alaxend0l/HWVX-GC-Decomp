#include "common.h"

extern VehicleInterfaceData g_vehicleInterfaceData[4];

void GSI_VehicleItemInit(void);

void GSI_Init(void)
{
    GSI_VehicleItemInit();
}

void GSI_VehicleItemInit(void)
{
    VehicleInterfaceData *data;
    void PlayerManager_Init(void);
    void *memset(void *dst, s32 c, u32 n);
    u32 i;

    data = g_vehicleInterfaceData;
    for (i = 0; i < 4; i++, data++) {
        memset(data, 0, sizeof(VehicleInterfaceData));
        data->iface = -1;
        data->packIndex = -1;
        data->packID = -1;
        data->itemID = -1;
        data->selection = -1;
        memset(data->objects, 0xFF, 8);
    }
    PlayerManager_Init();
}

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
