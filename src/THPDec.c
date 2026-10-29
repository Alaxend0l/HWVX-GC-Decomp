#include "common.h"

typedef struct THPComponent {
    u8 quantizationTableSelector;
    u8 DCTableSelector;
    u8 ACTableSelector;
    s16 predDC;
} THPComponent;

typedef struct THPHuffmanTab {
    u8 quick[32];
    u8 increment[32];
    u8 *Vij;
    s32 maxCode[18];
    s32 valPtr[18];
    u8 Vij1;
    u8 unkD5[0xE0 - 0xD5];
} THPHuffmanTab;

typedef struct THPFileInfo {
    f32 quantTabs[3][64];
    THPHuffmanTab huffmanTabs[4];
    THPComponent components[3];
    u16 xPixelSize;
    u16 yPixelSize;
    u16 MCUsPerRow;
    u16 decompressedY;
    u8 unk69A[0x69C - 0x69A];
    u8 *c;
    u32 currByte;
    u32 cnt;
    u8 validHuffmanTabs;
    u8 RST;
    u16 nMCU;
    u16 currMCU;
    u8 unk6AE[0x6BC - 0x6AE];
} THPFileInfo;

static THPFileInfo *__THPInfo;
static u8 *__THPHuffmanBits;
static u8 *__THPHuffmanSizeTab;
static u16 *__THPHuffmanCodeTab;
static s16 *__THPMCUBuffer[6];
static u8 *__THPWorkArea;

static void __THPSetupBuffers(void) {
    u8 i;
    s16 *buffer;

    buffer = (s16 *)(((u32)__THPWorkArea + 31) & ~31);
    for (i = 0; i < 6; i++) {
        __THPMCUBuffer[i] = &buffer[i * 64];
    }
}

static void __THPHuffGenerateCodeTable(void) {
    u8 *sizeTab;
    u16 *codeTab;
    u8 si;
    u16 p;
    u16 code;

    sizeTab = __THPHuffmanSizeTab;
    codeTab = __THPHuffmanCodeTab;
    p = 0;
    code = 0;
    si = sizeTab[0];
    while (sizeTab[p]) {
        while (sizeTab[p] == si) {
            codeTab[p++] = code++;
        }
        code = (u16)(code << 1);
        si++;
    }
}

static void __THPRestartDefinition(void) {
    __THPInfo->RST = TRUE;
    __THPInfo->c += 2;
    __THPInfo->nMCU = (u16)((__THPInfo->c[0] << 8) | __THPInfo->c[1]);
    __THPInfo->c += 2;
    __THPInfo->currMCU = __THPInfo->nMCU;
}

void LCQueueWait(u32 len);
void __THPHuffDecodeDCTCompY(THPFileInfo *info, s16 *block);
void __THPDecompressiMCURow512x448(u8 *lumY, u8 *chromaU, u8 *chromaV, THPFileInfo *info) {
    u8 cl_num;
    LCQueueWait(3);
    for (cl_num = 0; cl_num < info->MCUsPerRow; cl_num++) {
        __THPHuffDecodeDCTCompY(info, (s16 *)lumY);
    }
}

