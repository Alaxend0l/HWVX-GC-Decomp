#include "common.h"


typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

void TRKResetBuffer(TRKBuffer *b, u8 keepData);
s32 TRKMessageSend(TRKBuffer *b);

typedef struct DSVersions {
    u8 kernelMajor;
    u8 kernelMinor;
    u8 protocolMajor;
    u8 protocolMinor;
} DSVersions;

s32 TRKTargetVersions(DSVersions *versions);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
}

__declspec(section ".data") extern s32 IsTRKConnected;
void __TRK_reset(void);
s32 TRKTargetStop(void);
s32 TRKDoSetOption(TRKBuffer *b) {
    s32 err;
    s32 ackTries;
    u8 spA;
    u8 sp9;
    u8 sp8;

    spA = 0;
    sp9 = 0;
    sp8 = 0;
    TRKSetBufferPosition(b, 0);
    err = TRKReadBuffer1_ui8(b, &spA);
    if (err == 0) {
        err = TRKReadBuffer1_ui8(b, &sp9);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui8(b, &sp8);
    }
    if (err != 0) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 1);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
    } else if (sp9 == 1) {
        SetUseSerialIO(sp8);
    }
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

s32 TRKDoStop(TRKBuffer *b) {
    u8 replyError;
    s32 err;
    s32 ackTries;

    switch (TRKTargetStop()) {
    case 0:
        replyError = 0;
        break;
    case 0x704:
        replyError = 0x21;
        break;
    case 0x705:
        replyError = 0x22;
        break;
    case 0x706:
        replyError = 0x20;
        break;
    default:
        replyError = 1;
        break;
    }
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, replyError);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

s32 TRKDoContinue(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    if (!TRKTargetStopped()) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x16);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    if (err == 0) {
        err = TRKTargetContinue();
    }
    return err;
}

s32 TRKTargetSupportMask(u8 *mask);
s32 TRKAppendBuffer(TRKBuffer *b, const void *data, u32 length);
s32 TRKDoSupportMask(TRKBuffer *b) {
    s32 err;
    s32 ackTries;
    u8 mask[32];

    if (b->length != 1) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 2);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    err = TRKTargetSupportMask(mask);
    if (err == 0) {
        err = TRKAppendBuffer(b, mask, 32);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, 2);
    }
    if (err != 0) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 3);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

s32 TRKDoReset(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    __TRK_reset();
    return 0;
}

void TRKConstructEvent(TRKEvent *event, s32 eventType);
s32 TRKPostEvent(TRKEvent *event);
s32 TRKDoDisconnect(TRKBuffer *b) {
    TRKEvent event;
    s32 ackTries;
    s32 err;

    IsTRKConnected = 0;
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    if (err == 0) {
        TRKConstructEvent(&event, 1);
        TRKPostEvent(&event);
    }
    return err;
}

s32 TRKDoConnect(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    IsTRKConnected = 1;
    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

s32 TRKDoUnsupported(TRKBuffer *b) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, 0x80);
    TRKAppendBuffer1_ui8(b, 0x10);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

s32 TRKStandardACK(TRKBuffer *b, u8 commandID, u8 replyError) {
    s32 err;
    s32 ackTries;

    TRKResetBuffer(b, 1);
    TRKAppendBuffer1_ui8(b, commandID);
    TRKAppendBuffer1_ui8(b, replyError);
    ackTries = 3;
    do {
        err = TRKMessageSend(b);
        ackTries--;
    } while (err != 0 && ackTries > 0);
    return err;
}

void SetTRKConnected(s32 connected) {
    IsTRKConnected = connected;
}

s32 GetTRKConnected(void) {
    return IsTRKConnected;
}
