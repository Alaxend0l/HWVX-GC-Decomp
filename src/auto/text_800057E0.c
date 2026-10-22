/* Functions from 0x800057E0 to 0x80095C8C, found by decomp-toolkit. */
#include "common.h"

s32 fn_800057E0(void) {
    M2C_UNK fn_80012F7C();
    fn_80012F7C();
    return 1;
}

s32 fn_80005804(s32 *arg0) {
    M2C_UNK fn_80067284(s32);
    fn_80067284(*arg0);
    return 1;
}

s32 fn_8000589C(void) {
    return 1;
}

s32 fn_800058A4(void) {
    return 1;
}

s32 fn_800058AC(void) {
    return 1;
}

s32 fn_800058B4(void) {
    return 1;
}

s32 fn_800058BC(void) {
    return 1;
}

void fn_800058C4(void) {
    M2C_UNK fn_800058E8();
    fn_800058E8();
}

void fn_800058E4(void) {

}

void fn_80006C4C(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 1;
    M2C_FIELD(arg0, s8 *, 0xCC) = 0;
}

void fn_80006C60(void) {
    M2C_UNK fn_80047290();
    fn_80047290();
}

void fn_80006C80(void) {
    M2C_UNK fn_8000704C();
    M2C_UNK fn_80060110();
    fn_8000704C();
    fn_80060110();
}

void fn_80007040(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0xC8) = 0x1F4;
}

s32 fn_8000DE50(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB4);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8000F224(u32 arg0) {
    extern s32 lbl_803C6F3C;
    if (arg0 != 0U) {
        lbl_803C6F3C = arg0 - 1;
    }
}

void fn_8000F3F4(void) {

}

void fn_8000F568(void) {
    M2C_UNK fn_800C2684();
    extern s8 lbl_803C6F2C;
    fn_800C2684();
    lbl_803C6F2C = 1;
}

s32 fn_8000F8CC(void) {
    return 1;
}

s32 fn_800101C0(void) {
    return 1;
}

void fn_800101C8(void) {
    M2C_UNK fn_80010228();
    fn_80010228();
}

void fn_8001059C(s8 arg0) {
    extern s8 lbl_803C6F40;
    lbl_803C6F40 = arg0;
}

void fn_80014258(void *arg0, void *arg1) {
    M2C_FIELD(arg1, f32 *, 0) = (f32) M2C_FIELD(arg0, f32 *, 0x3C0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) M2C_FIELD(arg0, f32 *, 0x3D4);
    M2C_FIELD(arg1, f32 *, 8) = (f32) M2C_FIELD(arg0, f32 *, 0x3E8);
}

f32 fn_80016954(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), f32 *, 0x20);
}

s32 fn_80018D88(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0x3BC), void **, 0xA0), s32 *, 0x30);
}

s32 fn_80019594(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x3BC);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8002084C(void *arg0) {
    if (arg0 != NULL) {
        M2C_FIELD(arg0, s32 *, 0x300) = 0;
    }
}

void fn_800209A8(void *arg0) {
    if (arg0 != NULL) {
        M2C_FIELD(arg0, s8 *, 0x9D) = 0;
    }
}

void fn_800236FC(void) {
    extern s32 lbl_803C6F48;
    extern s32 lbl_803C6F4C;
    lbl_803C6F4C = 0;
    lbl_803C6F48 = 0;
}

f32 fn_80023B4C(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0x1BC);
}

s32 fn_8002528C(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB8);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_800252B0(s8 arg0) {
    extern s8 lbl_803C6C78;
    lbl_803C6C78 = arg0;
}

f32 fn_80026CA4(void *arg0) {
    return M2C_FIELD(arg0, f32 *, 0xCC);
}

f32 fn_8002793C(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), f32 *, 0x20);
}

s32 fn_80027E58(void *arg0) {
    return M2C_FIELD(M2C_FIELD(M2C_FIELD(arg0, void **, 0xC0), void **, 0x10), s32 *, 0x30);
}

s32 fn_80027EF8(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 fn_800288F0(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_800290FC(void) {
    M2C_UNK fn_80051164();
    fn_80051164();
}

s32 fn_8002BA44(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xD0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

u8 fn_8002D260(void) {
    extern u8 lbl_803C6F50;
    return lbl_803C6F50;
}

s32 fn_8002D268(void) {
    return 0xB;
}

void fn_800338DC(void) {

}

void fn_80034C20(u16 arg0) {
    extern s32 lbl_803C6FB4;
    lbl_803C6FB4 = (s32) arg0;
}

void fn_8003C4A0(void) {

}

s32 fn_80040640(void) {
    return 1;
}

s32 fn_800409E0(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0xD4);
}

s32 fn_800409E8(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xC0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 fn_80041E00(void) {
    return 0;
}

void fn_80041E08(void) {

}

void fn_80042378(void) {

}

void fn_800428E4(void) {

}

void fn_800428E8(void) {
    M2C_UNK fn_800A2838(s32);
    extern s32 lbl_803C6FF8;
    fn_800A2838(lbl_803C6FF8);
    lbl_803C6FF8 = 0;
}

void fn_800435B0(void) {
    extern s8 lbl_803C7000;
    lbl_803C7000 = 1;
}

s32 fn_80043A94(void) {
    extern void *lbl_803C6CD8;
    return M2C_FIELD(lbl_803C6CD8, s32 *, 4);
}

void fn_80045848(void) {
    M2C_UNK fn_80045498();
    fn_80045498();
}

void fn_80047750(void *arg0) {
    void *temp_r5;

    temp_r5 = M2C_FIELD(arg0, void **, 0x2C);
    M2C_FIELD(arg0, void **, 0x2C) = NULL;
    M2C_FIELD(arg0, s8 *, 0x30) = 1;
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, s32 *, 0x94) = 0;
    }
}

s32 fn_80048638(void) {
    return 0;
}

void fn_8004906C(void) {

}

f32 fn_8004B134(void *arg0) {
    return M2C_FIELD(M2C_FIELD(arg0, void **, 0x120), f32 *, 0x2C);
}

s32 fn_8004B140(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x120);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8004B1E8(void) {
    extern void *lbl_803C7030;
    M2C_FIELD(lbl_803C7030, s32 *, 0x2000) = 0;
    M2C_FIELD(lbl_803C7030, s32 *, 0x2004) = 0;
}

void fn_8004B6C8(void) {
    M2C_UNK fn_8004B6E8();
    fn_8004B6E8();
}

void fn_8004B798(void) {
    M2C_UNK fn_800A3D08(s32);
    extern s32 lbl_803C703C;
    fn_800A3D08(lbl_803C703C);
}

void fn_8004B8CC(void) {

}

s32 fn_8004CC74(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_8004D198(s32 arg0) {
    extern s8 lbl_803C70B0;
    extern s32 lbl_803C70B4;
    lbl_803C70B4 = arg0;
    lbl_803C70B0 = 1;
}

void fn_8004F984(s32 arg0, s32 arg1, f32 farg0) {
    extern s32 lbl_803C707C;
    extern s32 lbl_803C7080;
    extern f32 lbl_803C7084;
    extern f32 lbl_803C7088;
    switch (arg0) {                                 /* irregular */
    case 0:
        lbl_803C7088 = farg0;
        lbl_803C7080 = arg1;
        return;
    case 1:
        lbl_803C7084 = farg0;
        lbl_803C707C = arg1;
        return;
    }
}

f32 fn_8004F9B0(void) {
    extern f32 lbl_803C7090;
    return lbl_803C7090;
}

f32 fn_8004F9B8(void) {
    extern f32 lbl_803C708C;
    return lbl_803C708C;
}

void fn_8004FAD0(s32 arg0) {
    extern s32 lbl_803C70BC;
    lbl_803C70BC = arg0;
}

s32 fn_8004FC40(void) {
    M2C_UNK fn_80063048();
    fn_80063048();
    return 1;
}

s32 fn_80050514(void) {
    return 1;
}

s32 fn_8005051C(void) {
    return 1;
}

s32 fn_80050524(void) {
    return 0;
}

s32 fn_8005052C(void) {
    return 1;
}

void fn_80050534(s32 arg0, s32 arg1) {
    extern s32 lbl_803C70E0;
    extern s32 lbl_803C70F8;
    lbl_803C70E0 = arg0;
    lbl_803C70F8 = arg1;
}

s32 fn_80050F8C(void) {
    extern void *lbl_803C7100;
    return M2C_FIELD(lbl_803C7100, s32 *, 0xD0);
}

s32 fn_800512DC(void) {
    return 1;
}

s32 fn_800512E4(void) {
    return 1;
}

void fn_800512EC(void) {

}

f32 fn_80051D80(void) {
    extern f32 lbl_803C8554;
    return lbl_803C8554;
}

void fn_80051D88(void) {

}

void fn_80051D8C(void) {

}

s32 fn_80051D90(void *arg0) {
    void *temp_r3;

    temp_r3 = M2C_FIELD(arg0, void **, 0x94);
    if (temp_r3 != NULL) {
        return M2C_FIELD(temp_r3, s32 *, 0x2C);
    }
    return 0;
}

void fn_800532C4(s32 arg0) {
    M2C_UNK fn_80055014(s32);
    fn_80055014(arg0 + 0x88);
}

s32 fn_80053448(void) {
    return -1;
}

void fn_80053E6C(void) {

}

s32 fn_80053E94(void) {
    return 0;
}

void fn_80053F58(void) {

}

u32 fn_80053F5C(void *arg0, u32 arg1) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x20);
    if (temp_r0 == arg1) {
        return M2C_FIELD(arg0, u32 *, 0x24);
    }
    return temp_r0;
}

void fn_80053F78(void **arg0, void *arg1) {
    void *temp_r5;

    M2C_FIELD(arg1, void **, 0xA4) = (void *) *arg0;
    M2C_FIELD(arg1, s32 *, 0xA0) = 0;
    temp_r5 = M2C_FIELD(arg1, void **, 0xA4);
    if (temp_r5 != NULL) {
        M2C_FIELD(temp_r5, void **, 0xA0) = arg1;
    }
    *arg0 = arg1;
}

void fn_800561D0(void) {
    M2C_UNK fn_800561F4();
    M2C_UNK fn_800428E4();
    fn_800561F4();
    fn_800428E4();
}

void fn_8005946C(void) {
    M2C_UNK fn_8005948C();
    fn_8005948C();
}

s8 fn_80059A8C(void) {
    s32 fn_8004D05C();
    return (s8) (fn_8004D05C() == 0);
}

s32 fn_80059AB8(void) {
    return 1;
}

s32 fn_80059AC0(void) {
    return 1;
}

s32 fn_80059AC8(void) {
    return 1;
}

s32 fn_80059AD0(void) {
    return 1;
}

s32 fn_80059AD8(void) {
    return 1;
}

s32 fn_80059AE0(void) {
    return 1;
}

void fn_80059AE8(void) {
    M2C_UNK fn_80059B34();
    fn_80059B34();
}

void fn_80059B08(void) {
    extern s32 lbl_803C7120;
    fn_800A2838(lbl_803C7120);
    lbl_803C7120 = 0;
}

void fn_80059F84(s8 arg0) {
    extern s8 lbl_803C7124;
    lbl_803C7124 = arg0;
}

u8 fn_80059F8C(void) {
    extern u8 lbl_803C7124;
    return lbl_803C7124;
}

s32 fn_8005A4E0(void) {
    return 1;
}

s32 fn_8005B314(void) {
    return 1;
}

s32 fn_8005B734(void) {
    return 0;
}

s32 fn_8005B73C(void) {
    return 0;
}

void fn_8005BBF8(void) {

}

void fn_8005BBFC(void) {
    extern s32 lbl_803C7130;
    extern s32 lbl_803C7134;
    lbl_803C7134 = 0;
    lbl_803C7130 = 0;
}

void fn_8005BE54(void) {
    extern s32 lbl_803C7138;
    lbl_803C7138 = 0;
}

void fn_8005DD64(void) {
    M2C_UNK fn_8005E49C();
    fn_8005E49C();
}

void fn_8005F124(void) {

}

void fn_8005F128(void) {

}

void fn_8005F900(void) {

}

void fn_80060574(void) {

}

void fn_80060FE8(s32 arg0, s8 arg1) {
    extern u8 lbl_803C7684;
    s32 temp_r3;
    u8 var_r0;

    temp_r3 = 1 << arg0;
    var_r0 = lbl_803C7684 & ~temp_r3;
    if (arg1 != 0) {
        var_r0 = lbl_803C7684 | temp_r3;
    }
    lbl_803C7684 = var_r0;
}

s32 fn_80061014(void) {
    extern s32 lbl_803C7168;
    return lbl_803C7168;
}

s32 fn_80061FC8(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_80063538(void) {
    M2C_UNK fn_8002E730(M2C_UNK, M2C_UNK);
    fn_8002E730(3, 1);
}

s32 fn_800652CC(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0x110);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

void fn_80067284(u32 arg0) {
    extern u32 lbl_803C71C8;
    extern s8 lbl_803C71CC;
    lbl_803C71CC = 1;
    lbl_803C71C8 = arg0;
    if ((arg0 < 1U) && (arg0 > 4U)) {
        lbl_803C71C8 = -1U;
    }
}

void fn_800676EC(void) {
    M2C_UNK fn_8006770C();
    fn_8006770C();
}

s32 fn_80068C98(void *arg0, u32 arg1, u32 arg2) {
    u32 temp_r0;

    temp_r0 = M2C_FIELD(arg0, u32 *, 0xB0);
    if ((temp_r0 >= arg1) && (temp_r0 < arg2)) {
        return 1;
    }
    return 0;
}

s32 ActionSpeakerChangeMode(void) {
    return 1;
}

s32 ActionVehicleAISetNamedPath(void) {
    return 1;
}

s32 ActionSetPlayerVehicle(void) {
    return 1;
}

s32 ActionVehicleRemoveCreature(void) {
    return 1;
}

s32 ActionScriptLightChangeMode(void) {
    return 1;
}

void Action_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Action_Destroy(void) {

}

void AI_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void SongManager_StopBKSong(void) {

}

s32 ActionMusicSetSong(void) {
    return 1;
}

s32 CameraManager_Create(void) {
    return 1;
}

void CameraManager_Destroy(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void EventSeq_Release(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void Grid_Release(void) {

}

void InterfaceManager_SetControllerConfigInputString(void) {

}

s32 Item_PreMove(void) {
    return 1;
}

s32 LogHandler_Create(void) {
    return 0;
}

void LogHandler_Destroy(void) {

}

void Looker_Draw(void) {

}

void MatrixCache_Init(void) {

}

void Mover_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 Player_NULL(void) {
    return 0;
}

void ProcessManager_Create(void) {

}

void ShowStats_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void ShowStats(void) {

}

void StateMovie_SetMovieToPlay(s32 arg0) {
    extern s32 g_moviePlaybackName;
    g_moviePlaybackName = arg0;
}

s32 StateRenderTest_Shutdown(void) {
    return 1;
}

s32 StateShutdown_Init(void) {
    return 1;
}

s32 StateShutdown_Heartbeat(void) {
    return 0;
}

s32 StateShutdown_Shutdown(void) {
    return 1;
}

s32 Thing_PreMove(void) {
    return 1;
}

s32 Thing_Move(void) {
    return 1;
}

void Thing_Draw(void) {

}

void Thing_DynamicCollideImpact(void) {

}

void Thing_DynamicRotation(void) {

}

void Thing_CollideWithRay(void) {

}

s32 Thing_GetOrientedCollision(void) {
    return 0;
}

void Thing_PassiveTargeting(void) {

}

void Trail_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 TriggerMoveableEnd(void) {
    return 1;
}

s32 TriggerCreaturePickedUp(void) {
    return 1;
}

s32 TriggerTrue(void) {
    return 1;
}

s32 TriggerGameRoundsComplete(void) {
    return 1;
}

s32 TriggerGameRoundXComplete(void) {
    return 1;
}

s32 TriggerItemDropped(void) {
    return 1;
}

void Trigger_Create(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 TriggerLevelStart(void) {
    return 1;
}

s32 TriggerInterfaceEnd(void) {
    return 1;
}

s32 TriggerCreatureKilled(void) {
    return 0;
}

s32 TriggerCreatureDroppedOff(void) {
    return 0;
}

void VertexColorSeqManager_Create(void) {

}

void World_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void PlayerManager_Create(void) {

}

void PlayerManager_Destroy(void) {

}

void PlayerManager_Render_DEBUG(void) {

}

void AI_Creature_Heartbeat(void) {

}

s32 ViewManager_GetViewFormat(void) {
    extern s32 g_viewFormat;
    return g_viewFormat;
}

void GSI_Init(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

void OSCreateAlarm(s32 *arg0) {
    *arg0 = 0;
}

s32 fn_80069F84(s32 arg0) {
    extern s32 lbl_803C6D70;
    s32 temp_r0;

    temp_r0 = lbl_803C6D70;
    lbl_803C6D70 = arg0;
    return temp_r0;
}

s32 OSGetArenaHi(void) {
    extern s32 __OSArenaHi_803C7228;
    return __OSArenaHi_803C7228;
}

s32 OSGetArenaLo(void) {
    extern s32 __OSArenaLo_803C6D78;
    return __OSArenaLo_803C6D78;
}

void OSSetArenaHi(s32 arg0) {
    extern s32 __OSArenaHi_803C7228;
    __OSArenaHi_803C7228 = arg0;
}

void OSSetArenaLo(s32 arg0) {
    extern s32 __OSArenaLo_803C6D78;
    __OSArenaLo_803C6D78 = arg0;
}

s32 fn_8006AC34(void) {
    return *(s32 *)0x800000D4;
}

void fn_8006D134(void) {
    extern s32 lbl_803C7260;
    lbl_803C7260 = 1;
}

s32 __OSSyncSram(void) {
    extern M2C_UNK Scb_802EEE20;
    return M2C_FIELD(&Scb_802EEE20, s32 *, 0x4C);
}

void OSInitThreadQueue(void *arg0) {
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 0) = 0;
}

s32 fn_8006E668(void) {
    return *(s32 *)0x800000E4;
}

s32 __DBIsExceptionMarked(u8 arg0) {
    extern void *__DBInterface;
    return M2C_FIELD(__DBInterface, s32 *, 4) & (1 << arg0);
}

s32 ARGetSize(void) {
    extern s32 __AR_Size_803C7424;
    return __AR_Size_803C7424;
}

void __ARQCallbackHack(void) {

}

s32 __AXGetCommandListCycles(void) {
    extern s32 __AXCommandListCycles_803C74B0;
    return __AXCommandListCycles_803C74B0;
}

void __AXClQuit(void) {

}

void AXRegisterCallback(s32 arg0) {
    extern s32 __AXUserFrameCallback_803C74D0;
    __AXUserFrameCallback_803C74D0 = arg0;
}

void __AXSPBQuit(void) {

}

s32 __AXGetNumVoices(void) {
    extern s32 __AXNumVoices_803C7510;
    return __AXNumVoices_803C7510;
}

s32 DSPCheckInit(void) {
    extern s32 __DSP_init_flag_803C7528;
    return __DSP_init_flag_803C7528;
}

s32 __CARDIsOpened(void) {
    return 0;
}

f32 fn_8006FB1C(void) {
    M2C_ERROR(/* unknown instruction: ps_merge01 $f2, $f0, $f1 */);
    return M2C_ERROR(/* unknown instruction: ps_merge10 $f1, $f1, $f0 */);
}

s32 fn_8007350C(s32 arg0) {
    extern s32 autoInvalidation_803C6DA8;
    s32 temp_r0;

    temp_r0 = autoInvalidation_803C6DA8;
    autoInvalidation_803C6DA8 = arg0;
    return temp_r0;
}

s32 fn_8007385C(void) {
    return 0x80000000;
}

s32 fn_800777FC(void) {
    return *(s32 *)0xCC006C00 & 1;
}

s32 fn_800778EC(void) {
    return (((u32) *(u32 *)0xCC006C00 >> 6U) & 1) ^ 1;
}

s32 fn_800779D4(void) {
    return ((u32) *(u32 *)0xCC006C00 >> 1U) & 1;
}

u8 fn_80077A00(void) {
    return (u8) *(s32 *)0xCC006C04;
}

u8 fn_80077A2C(void) {
    return (u8) ((u32) *(u32 *)0xCC006C04 >> 8U);
}

void fn_80077BA0(void) {
    extern s32 lbl_803C73F0;
    lbl_803C73F0 = 0;
}

void fn_800780E4(void) {
    extern s32 lbl_803C743C;
    lbl_803C743C = 0;
}

s32 fn_800780F0(void) {
    return 0x4000;
}

s32 fn_800780F8(void) {
    extern s32 lbl_803C7424;
    return lbl_803C7424;
}

void fn_80078B8C(void) {

}

void fn_80078CC4(void) {
    extern s32 lbl_803C7464;
    lbl_803C7464 = 0;
}

void fn_800790F0(void *arg0) {
    extern void *lbl_803C7468;
    M2C_FIELD(arg0, void **, 8) = (void *) lbl_803C7468;
    lbl_803C7468 = arg0;
}

void fn_80079530(void) {
    extern s32 lbl_803C7474;
    extern s32 lbl_803C7470;
    lbl_803C7470 = 0;
    lbl_803C7474 = 0;
}

void fn_800798D4(s32 arg0, s32 arg1) {
    extern s32 lbl_803C7470;
    extern s32 lbl_803C7478;
    lbl_803C7470 = arg0;
    lbl_803C7478 = arg1;
}

void fn_800798E0(s32 arg0, s32 arg1) {
    extern s32 lbl_803C7474;
    extern s32 lbl_803C747C;
    lbl_803C7474 = arg0;
    lbl_803C747C = arg1;
}

s32 fn_800798EC(void) {
    extern s32 lbl_803C74B0;
    return lbl_803C74B0;
}

void fn_8007A004(void) {

}

void fn_8007A1F8(void) {
    extern s32 lbl_803C74D4;
    lbl_803C74D4 = 1;
}

void fn_8007A25C(void) {
    extern s32 lbl_803C74D8;
    lbl_803C74D8 = 1;
}

void fn_8007A6C0(s32 arg0) {
    extern s32 lbl_803C74D0;
    lbl_803C74D0 = arg0;
}

void fn_8007AAF8(void) {

}

s32 fn_8007AB90(void) {
    extern s32 lbl_803C7510;
    return lbl_803C7510;
}

void fn_8007B740(void) {

}

s32 fn_8007BC88(void) {
    return ((u16) *(u16 *)0xCC005000 >> 0xFU) & 1;
}

s32 fn_8007BC98(void) {
    return ((u16) *(u16 *)0xCC005004 >> 0xFU) & 1;
}

s32 fn_8007BD8C(void) {
    extern s32 lbl_803C7528;
    return lbl_803C7528;
}

s32 fn_8007BE1C(void) {
    return *(u16 *)0xCC00500A & 0x200;
}

void fn_8007C838(void) {

}

s32 fn_8007F0C0(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x88);
}

s32 fn_8007F4C4(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x84);
}

s32 fn_80081D8C(void) {
    return 0;
}

void fn_80084304(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s32 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10) = arg2;
}

void fn_80085DEC(void) {
    *(s8 *)0xCC008000 = 0x48;
}

void fn_800874EC(void *arg0, f32 farg0, f32 farg1, f32 farg2) {
    M2C_FIELD(arg0, f32 *, 0x1C) = farg0;
    M2C_FIELD(arg0, f32 *, 0x20) = farg1;
    M2C_FIELD(arg0, f32 *, 0x24) = farg2;
}

void fn_800875CC(void *arg0, f32 farg0, f32 farg1, f32 farg2) {
    M2C_FIELD(arg0, f32 *, 0x28) = farg0;
    M2C_FIELD(arg0, f32 *, 0x2C) = farg1;
    M2C_FIELD(arg0, f32 *, 0x30) = farg2;
}

s32 fn_8008811C(void *arg0) {
    return (M2C_FIELD(arg0, s32 *, 0xC) << 5) & 0x03FFFFE0;
}

s32 fn_80088128(void *arg0) {
    return M2C_FIELD(arg0, s32 *, 0x14);
}

void fn_8008A25C(void) {

}

void fn_8008B31C(void) {
    extern void *__cpReg;
    M2C_FIELD(__cpReg, s16 *, 4) = 4;
}

void fn_8008CC84(void) {

}

void TRKDestructEvent(void *arg0) {
    M2C_UNK fn_8008FD50(s32);
    fn_8008FD50(M2C_FIELD(arg0, s32 *, 8));
}

s32 TRKTerminateSerialHandler(void) {
    return 0;
}

void fn_8008EF90(void *arg0, s8 arg1) {
    M2C_FIELD(arg0, s8 *, 0) = arg1;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg0, s32 *, 8) = -1;
}

s32 fn_8008FF1C(void) {
    return 0;
}

void usr_put_initialize(void) {

}

s32 TRKReleaseMutex(void) {
    return 0;
}

s32 TRKAcquireMutex(void) {
    return 0;
}

s32 TRKInitializeMutex(void) {
    return 0;
}

s32 fn_80093530(void) {
    return 0;
}

s32 fn_80093538(void) {
    return 0;
}

s32 fn_80093540(void) {
    return 0;
}

s32 fn_80094538(void *arg0) {
    M2C_FIELD(arg0, s8 *, 0) = 0;
    M2C_FIELD(arg0, s8 *, 1) = 0xA;
    M2C_FIELD(arg0, s8 *, 2) = 1;
    M2C_FIELD(arg0, s8 *, 3) = 0xA;
    return 0;
}

void EnableMetroTRKInterrupts(void) {
    M2C_UNK fn_800957D4();
    fn_800957D4();
}

s32 fn_80095480(void) {
    return 0x54;
}

void TRKUARTInterruptHandler(void) {

}

