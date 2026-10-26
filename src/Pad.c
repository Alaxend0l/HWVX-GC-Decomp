#include "common.h"

void OSClearContext(void *);
void OSSetCurrentContext(void *);

s32 __PADSpec;
s32 Spec;
void *MakeStatus;

void SPEC0_MakeStatus(s32, void *, u32 *);
void SPEC1_MakeStatus(s32, void *, u32 *);
void SPEC2_MakeStatus_80076F6C(s32, void *, u32 *);
