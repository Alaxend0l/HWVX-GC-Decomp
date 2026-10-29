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
s32 TRKAppendBuffer(TRKBuffer *b, const void *data, u32 length);
s32 TRKAppendBuffer1_ui16(TRKBuffer *b, u16 data);
s32 TRKReadBuffer(TRKBuffer *b, void *data, u32 length);
s32 TRKTargetAccessMemory(void *data, u32 start, u32 *length, s32 memorySpace, s32 read);

typedef struct DSVersions {
    u8 kernelMajor;
    u8 kernelMinor;
    u8 protocolMajor;
    u8 protocolMinor;
} DSVersions;

s32 TRKTargetVersions(DSVersions *versions);

typedef struct DSCPUType {
    u8 cpuMajor;
    u8 cpuMinor;
    u8 bigEndian;
    u8 defaultTypeSize;
    u8 fpTypeSize;
    u8 extended1TypeSize;
    u8 extended2TypeSize;
} DSCPUType;

s32 TRKTargetCPUType(DSCPUType *cpuType);

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

s32 TRKReadBuffer1_ui32(TRKBuffer *b, u32 *data);
s32 TRKTargetFlushCache(u8 option, u32 start, u32 end);
s32 TRKDoFlushCache(TRKBuffer *b) {
    u32 start;
    u32 end;
    u8 option;
    u8 cacheType;
    s32 err;
    s32 ackTries;
    u8 replyError;

    if (b->length != 10) {
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
    TRKSetBufferPosition(b, 0);
    err = TRKReadBuffer1_ui8(b, &option);
    if (err == 0) {
        err = TRKReadBuffer1_ui8(b, &cacheType);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui32(b, &start);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui32(b, &end);
    }
    if (start > end) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x13);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    if (err == 0) {
        err = TRKTargetFlushCache(cacheType, start, end);
    }
    if (err == 0) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0);
    }
    if (err != 0) {
        switch (err) {
        case 0x703:
            replyError = 0x12;
            break;
        default:
            replyError = 3;
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
    } else {
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
    }
    return err;
}

s32 TRKReadBuffer1_ui16(TRKBuffer *b, u16 *data);
s32 TRKTargetAccessDefault(u32 firstRegister, u32 lastRegister, TRKBuffer *b, u32 *registersLengthPtr, s32 read);
s32 TRKTargetAccessFP(u32 firstRegister, u32 lastRegister, TRKBuffer *b, u32 *registersLengthPtr, s32 read);
s32 TRKTargetAccessExtended1(u32 firstRegister, u32 lastRegister, TRKBuffer *b, u32 *registersLengthPtr, s32 read);
s32 TRKTargetAccessExtended2(u32 firstRegister, u32 lastRegister, TRKBuffer *b, u32 *registersLengthPtr, s32 read);
s32 TRKDoWriteRegisters(TRKBuffer *b) {
    u32 registersLength;
    u16 firstRegister;
    u16 lastRegister;
    u8 c;
    u8 options;
    s32 err;
    s32 ackTries;
    u8 replyError;

    if (b->length <= 6) {
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
    TRKSetBufferPosition(b, 0);
    err = TRKReadBuffer1_ui8(b, &c);
    if (err == 0) {
        err = TRKReadBuffer1_ui8(b, &options);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui16(b, &firstRegister);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui16(b, &lastRegister);
    }
    if (firstRegister > lastRegister) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x14);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    switch (options) {
    case 0:
        err = TRKTargetAccessDefault(firstRegister, lastRegister, b, &registersLength, 0);
        break;
    case 1:
        err = TRKTargetAccessFP(firstRegister, lastRegister, b, &registersLength, 0);
        break;
    case 2:
        err = TRKTargetAccessExtended1(firstRegister, lastRegister, b, &registersLength, 0);
        break;
    case 3:
        err = TRKTargetAccessExtended2(firstRegister, lastRegister, b, &registersLength, 0);
        break;
    default:
        err = 0x703;
        break;
    }
    if (err == 0) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0);
    }
    if (err != 0) {
        switch (err) {
        case 0x703:
            replyError = 0x12;
            break;
        case 0x701:
            replyError = 0x14;
            break;
        case 0x302:
            replyError = 2;
            break;
        case 0x702:
            replyError = 0x15;
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
            replyError = 3;
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
    } else {
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
    }
    return err;
}

s32 TRKDoReadRegisters(TRKBuffer *b) {
    u32 registersLength;
    u16 firstRegister;
    u16 lastRegister;
    u8 c;
    u8 options;
    s32 err;
    s32 ackTries;
    u8 replyError;

    if (b->length != 6) {
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
    TRKSetBufferPosition(b, 0);
    err = TRKReadBuffer1_ui8(b, &c);
    if (err == 0) {
        err = TRKReadBuffer1_ui8(b, &options);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui16(b, &firstRegister);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui16(b, &lastRegister);
    }
    if (firstRegister > lastRegister) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x14);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    if (err == 0) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0);
    }
    switch (options & 7) {
    case 0:
        err = TRKTargetAccessDefault(firstRegister, lastRegister, b, &registersLength, 1);
        break;
    case 1:
        err = TRKTargetAccessFP(firstRegister, lastRegister, b, &registersLength, 1);
        break;
    case 2:
        err = TRKTargetAccessExtended1(firstRegister, lastRegister, b, &registersLength, 1);
        break;
    case 3:
        err = TRKTargetAccessExtended2(firstRegister, lastRegister, b, &registersLength, 1);
        break;
    default:
        err = 0x703;
        break;
    }
    if (err != 0) {
        switch (err) {
        case 0x703:
            replyError = 0x12;
            break;
        case 0x701:
            replyError = 0x14;
            break;
        case 0x702:
            replyError = 0x15;
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
            replyError = 3;
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
    } else {
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
    }
    return err;
}

s32 TRKDoWriteMemory(TRKBuffer *b) {
    u8 buf[0x800];
    u32 start;
    u32 msg_length;
    u16 length;
    u8 c;
    u8 options;
    s32 err;
    s32 ackTries;
    u8 replyError;

    if (b->length <= 8) {
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
    TRKSetBufferPosition(b, 0);
    err = TRKReadBuffer1_ui8(b, &c);
    if (err == 0) {
        err = TRKReadBuffer1_ui8(b, &options);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui16(b, &length);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui32(b, &start);
    }
    if (options & 2) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x12);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    if (b->length != length + 8 || length > 0x800) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x11);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    if (err == 0) {
        msg_length = length;
        err = TRKReadBuffer(b, buf, msg_length);
        if (err == 0) {
            err = TRKTargetAccessMemory(buf, start, &msg_length, (options & 8) == 0, 0);
        }
        length = msg_length;
    }
    if (err == 0) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui16(b, length);
    }
    if (err != 0) {
        switch (err) {
        case 0x702:
            replyError = 0x15;
            break;
        case 0x700:
            replyError = 0x13;
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
            replyError = 3;
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
    } else {
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
    }
    return err;
}

s32 TRKDoReadMemory(TRKBuffer *b) {
    u8 buf[0x800];
    u32 start;
    u32 msg_length;
    u16 length;
    u8 c;
    u8 options;
    s32 err;
    s32 ackTries;
    u8 replyError;

    if (b->length != 8) {
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
    TRKSetBufferPosition(b, 0);
    err = TRKReadBuffer1_ui8(b, &c);
    if (err == 0) {
        err = TRKReadBuffer1_ui8(b, &options);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui16(b, &length);
    }
    if (err == 0) {
        err = TRKReadBuffer1_ui32(b, &start);
    }
    if (options & 2) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x12);
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
        return err;
    }
    if (length > 0x800) {
        TRKResetBuffer(b, 1);
        TRKAppendBuffer1_ui8(b, 0x80);
        TRKAppendBuffer1_ui8(b, 0x11);
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
    if (err == 0) {
        msg_length = length;
        err = TRKTargetAccessMemory(buf, start, &msg_length, (options & 8) == 0, 1);
        length = msg_length;
        if (err == 0) {
            err = TRKAppendBuffer1_ui16(b, length);
        }
        if (err == 0) {
            err = TRKAppendBuffer(b, buf, msg_length);
        }
    }
    if (err != 0) {
        switch (err) {
        case 0x702:
            replyError = 0x15;
            break;
        case 0x700:
            replyError = 0x13;
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
            replyError = 3;
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
    } else {
        ackTries = 3;
        do {
            err = TRKMessageSend(b);
            ackTries--;
        } while (err != 0 && ackTries > 0);
    }
    return err;
}

s32 TRKDoCPUType(TRKBuffer *b) {
    s32 err;
    s32 ackTries;
    DSCPUType cpuType;

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
    err = TRKTargetCPUType(&cpuType);
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, cpuType.cpuMajor);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, cpuType.cpuMinor);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, cpuType.bigEndian);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, cpuType.defaultTypeSize);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, cpuType.fpTypeSize);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, cpuType.extended1TypeSize);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, cpuType.extended2TypeSize);
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

s32 TRKTargetSupportMask(u8 *mask);
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

s32 TRKDoVersions(TRKBuffer *b) {
    s32 err;
    s32 ackTries;
    DSVersions versions;

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
    err = TRKTargetVersions(&versions);
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, versions.kernelMajor);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, versions.kernelMinor);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, versions.protocolMajor);
    }
    if (err == 0) {
        err = TRKAppendBuffer1_ui8(b, versions.protocolMinor);
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
