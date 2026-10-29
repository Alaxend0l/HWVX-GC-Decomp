#include "common.h"

typedef struct THPAudioRecordHeader {
    u32 offsetNextChannel;
    u32 sampleSize;
    s16 lCoef[8][2];
    s16 rCoef[8][2];
    s16 lYn1;
    s16 lYn2;
    s16 rYn1;
    s16 rYn2;
} THPAudioRecordHeader;

typedef struct THPAudioDecodeInfo {
    u8 *encodeData;
    u32 offsetNibbles;
    u8 predictor;
    u8 scale;
    s16 yn1;
    s16 yn2;
} THPAudioDecodeInfo;

static void __THPAudioInitialize(THPAudioDecodeInfo *info, u8 *ptr);
static s32 __THPAudioGetNewSample(THPAudioDecodeInfo *info);

#pragma use_lmw_stmw on

u32 THPAudioDecode(s16 *buffer, u8 *audioFrame, s32 flag) {
    THPAudioRecordHeader *header;
    THPAudioDecodeInfo decInfo;
    u8 *left;
    u8 *right;
    s16 *decLeftPtr;
    s16 *decRightPtr;
    s16 yn1;
    s16 yn2;
    s32 increment;
    s32 sample;
    s64 yn;
    u32 i;

    if (buffer == NULL || audioFrame == NULL) {
        return 0;
    }
    header = (THPAudioRecordHeader *)audioFrame;
    left = audioFrame + sizeof(THPAudioRecordHeader);
    right = left + header->offsetNextChannel;
    if (flag == 1) {
        decRightPtr = buffer;
        decLeftPtr = buffer + header->sampleSize;
        increment = 1;
    } else {
        decRightPtr = buffer;
        decLeftPtr = buffer + 1;
        increment = 2;
    }
    if (header->offsetNextChannel == 0) {
        __THPAudioInitialize(&decInfo, left);
        yn1 = header->lYn1;
        yn2 = header->lYn2;
        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfo);
            yn = header->lCoef[decInfo.predictor][1] * yn2;
            yn += header->lCoef[decInfo.predictor][0] * yn1;
            yn += (sample << decInfo.scale) << 11;
            yn <<= 5;
            if ((u16)(yn & 0xFFFF) > 0x8000) {
                yn += 0x10000;
            } else if ((u16)(yn & 0xFFFF) == 0x8000 && (yn & 0x10000)) {
                yn += 0x10000;
            }
            if (yn > 0x7FFFFFFF) {
                yn = 0x7FFFFFFF;
            }
            if (yn < -0x80000000LL) {
                yn = -0x80000000LL;
            }
            *decLeftPtr = (s16)(yn >> 16);
            decLeftPtr += increment;
            *decRightPtr = (s16)(yn >> 16);
            decRightPtr += increment;
            yn2 = yn1;
            yn1 = (s16)(yn >> 16);
        }
    } else {
        __THPAudioInitialize(&decInfo, left);
        yn1 = header->lYn1;
        yn2 = header->lYn2;
        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfo);
            yn = header->lCoef[decInfo.predictor][1] * yn2;
            yn += header->lCoef[decInfo.predictor][0] * yn1;
            yn += (sample << decInfo.scale) << 11;
            yn <<= 5;
            if ((u16)(yn & 0xFFFF) > 0x8000) {
                yn += 0x10000;
            } else if ((u16)(yn & 0xFFFF) == 0x8000 && (yn & 0x10000)) {
                yn += 0x10000;
            }
            if (yn > 0x7FFFFFFF) {
                yn = 0x7FFFFFFF;
            }
            if (yn < -0x80000000LL) {
                yn = -0x80000000LL;
            }
            *decLeftPtr = (s16)(yn >> 16);
            decLeftPtr += increment;
            yn2 = yn1;
            yn1 = (s16)(yn >> 16);
        }
        __THPAudioInitialize(&decInfo, right);
        yn1 = header->rYn1;
        yn2 = header->rYn2;
        for (i = 0; i < header->sampleSize; i++) {
            sample = __THPAudioGetNewSample(&decInfo);
            yn = header->rCoef[decInfo.predictor][1] * yn2;
            yn += header->rCoef[decInfo.predictor][0] * yn1;
            yn += (sample << decInfo.scale) << 11;
            yn <<= 5;
            if ((u16)(yn & 0xFFFF) > 0x8000) {
                yn += 0x10000;
            } else if ((u16)(yn & 0xFFFF) == 0x8000 && (yn & 0x10000)) {
                yn += 0x10000;
            }
            if (yn > 0x7FFFFFFF) {
                yn = 0x7FFFFFFF;
            }
            if (yn < -0x80000000LL) {
                yn = -0x80000000LL;
            }
            *decRightPtr = (s16)(yn >> 16);
            decRightPtr += increment;
            yn2 = yn1;
            yn1 = (s16)(yn >> 16);
        }
    }
    return header->sampleSize;
}

static s32 __THPAudioGetNewSample(THPAudioDecodeInfo *info) {
    s32 sample;

    if (!(info->offsetNibbles & 0xF)) {
        info->predictor = (u8)((*info->encodeData >> 4) & 7);
        info->scale = (u8)(*info->encodeData & 0xF);
        info->encodeData++;
        info->offsetNibbles += 2;
    }
    if (info->offsetNibbles & 1) {
        sample = (s32)((*info->encodeData & 0xF) << 28) >> 28;
        info->encodeData++;
    } else {
        sample = (s32)((*info->encodeData & 0xF0) << 24) >> 28;
    }
    info->offsetNibbles++;
    return sample;
}

static void __THPAudioInitialize(THPAudioDecodeInfo *info, u8 *ptr) {
    info->encodeData = ptr;
    info->offsetNibbles = 2;
    info->predictor = (u8)((*info->encodeData >> 4) & 7);
    info->scale = (u8)(*info->encodeData & 0xF);
    info->encodeData++;
}

