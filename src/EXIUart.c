#include "common.h"


u32 OSGetConsoleType(void);
s32 EXILock(s32 chan, u32 dev, void *unlockedCallback);
s32 EXISelect(s32 chan, u32 dev, u32 freq);
s32 EXIImm(s32 chan, void *buf, s32 len, u32 type, void *callback);
s32 EXISync(s32 chan);
s32 EXIDeselect(s32 chan);
s32 EXIUnlock(s32 chan);

static s32 Chan;
static u32 Dev;
static u32 Enabled;
static u32 BarnacleEnabled;

s32 InitializeUART(u32 baudRate) {
    if (BarnacleEnabled == 0xA5FF005A) {
        return 0;
    }
    if (!(OSGetConsoleType() & 0x10000000)) {
        Enabled = 0;
        return 2;
    }
    Chan = 0;
    Dev = 1;
    Enabled = 0xA5FF005A;
    return 0;
}

s32 EXILock(s32 chan, u32 dev, void *unlockedCallback);
s32 EXISelect(s32 chan, u32 dev, u32 freq);
s32 EXIImm(s32 chan, void *buf, s32 len, u32 type, void *callback);
s32 EXISync(s32 chan);
s32 EXIDeselect(s32 chan);
s32 EXIUnlock(s32 chan);
s32 WriteUARTN(const void *buf, u32 len) {
    u32 cmd;
    s32 qLen;
    s32 xLen;
    char *ptr;
    BOOL locked;
    s32 error;

    if (Enabled != 0xA5FF005A) {
        return 2;
    }
    locked = EXILock(Chan, Dev, 0);
    if (!locked) {
        return 0;
    }
    for (ptr = (char *)buf; ptr - (char *)buf < len; ptr++) {
        if (*ptr == '\n') {
            *ptr = '\r';
        }
    }
    error = 0;
    cmd = 0xA0010000;
    while (len) {
        {
            u32 qcmd;
            if (!EXISelect(Chan, Dev, 3)) {
                qLen = -1;
            } else {
                qcmd = 0x20010000;
                EXIImm(Chan, &qcmd, 4, 1, 0);
                EXISync(Chan);
                EXIImm(Chan, &qcmd, 1, 0, 0);
                EXISync(Chan);
                EXIDeselect(Chan);
                qLen = 16 - (qcmd >> 24);
            }
        }
        if (qLen < 0) {
            error = 3;
            break;
        }
        if (qLen < 12 && qLen < len) {
            continue;
        }
        if (!EXISelect(Chan, Dev, 3)) {
            error = 3;
            break;
        }
        EXIImm(Chan, &cmd, 4, 1, 0);
        EXISync(Chan);
        while (qLen && len) {
            if (qLen < 4 && qLen < len) {
                break;
            }
            xLen = (len < 4) ? (s32)len : 4;
            EXIImm(Chan, (void *)buf, xLen, 1, 0);
            buf = (char *)buf + xLen;
            len -= xLen;
            qLen -= xLen;
            EXISync(Chan);
        }
        EXIDeselect(Chan);
    }
    EXIUnlock(Chan);
    return error;
}

