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

static THPHuffmanTab *Ydchuff __attribute__((aligned(32)));
static THPHuffmanTab *Udchuff __attribute__((aligned(32)));
static THPHuffmanTab *Vdchuff __attribute__((aligned(32)));
static THPHuffmanTab *Yachuff __attribute__((aligned(32)));
static THPHuffmanTab *Uachuff __attribute__((aligned(32)));
static THPHuffmanTab *Vachuff __attribute__((aligned(32)));
static THPFileInfo *__THPInfo;
static u8 *__THPHuffmanBits;
static u8 *__THPHuffmanSizeTab;
static u16 *__THPHuffmanCodeTab;
static s16 *__THPMCUBuffer[6];
static void *__THPWorkArea;
static BOOL __THPInitFlag;

u32 PPCMfhid2(void);
void DCZeroRange(void *addr, u32 nBytes);
u8 __THPReadQuantizationTable(void);
void __THPDecompressYUV(void *tileY, void *tileU, void *tileV);
static void __THPSetupBuffers(void);
static void __THPRestartDefinition(void);
u8 __THPReadFrameHeader(void);
u8 __THPReadScaneHeader(void);
u8 __THPReadHuffmanTableSpecification(void);
#pragma push
#pragma opt_propagation off
s32 THPVideoDecode(void *file, void *tileY, void *tileU, void *tileV, void *work) {
    u8 all_done, status;
    u8 marker;

    if (!file) {
        goto _err_no_input;
    }
    if (!tileY || !tileU || !tileV) {
        goto _err_no_output;
    }
    if (!work) {
        goto _err_no_work;
    }
    if (!(PPCMfhid2() & 0x10000000)) {
        goto _err_lc_not_enabled;
    }
    if (__THPInitFlag == FALSE) {
        goto _err_not_initialized;
    }

    __THPWorkArea = work;
    __THPWorkArea = (void *)((u32)(__THPInfo = (THPFileInfo *)(((u32)__THPWorkArea + 31) & ~31)) + sizeof(THPFileInfo));
    DCZeroRange(__THPInfo, sizeof(THPFileInfo));
    __THPInfo->cnt = 33;
    __THPInfo->decompressedY = 0;
    __THPInfo->c = file;
    all_done = FALSE;

    for (;;) {
        if ((*(__THPInfo->c)++) != 255) {
            goto _err_bad_syntax;
        }
        while (*__THPInfo->c == 255) {
            (__THPInfo->c)++;
        }
        marker = (*(__THPInfo->c)++);
        if (marker <= 0xD7) {
            if (marker == 196) {
                status = __THPReadHuffmanTableSpecification();
                if (status != 0) {
                    goto _err_bad_status;
                }
            } else if (marker == 192) {
                status = __THPReadFrameHeader();
                if (status != 0) {
                    goto _err_bad_status;
                }
            } else {
                goto _err_unsupported_marker;
            }
        } else if (marker >= 0xD8 && marker <= 0xDF) {
            if (marker == 221) {
                __THPRestartDefinition();
            } else if (marker == 219) {
                status = __THPReadQuantizationTable();
                if (status != 0) {
                    goto _err_bad_status;
                }
            } else if (marker == 218) {
                status = __THPReadScaneHeader();
                if (status != 0) {
                    goto _err_bad_status;
                }
                all_done = TRUE;
            } else if (marker == 216) {
            } else {
                goto _err_unsupported_marker;
            }
        } else if (marker >= 0xE0) {
            if ((marker >= 224 && marker <= 239) || marker == 254) {
                __THPInfo->c += (__THPInfo->c)[0] << 8 | (__THPInfo->c)[1];
            } else {
                goto _err_unsupported_marker;
            }
        }
        if (all_done) {
            break;
        }
    }

    __THPSetupBuffers();
    __THPDecompressYUV(tileY, tileU, tileV);
    return 0;

_err_no_input:
    return 25;
_err_no_output:
    return 27;
_err_no_work:
    return 26;
_err_unsupported_marker:
    return 11;
_err_bad_syntax:
    return 3;
_err_bad_status:
    return status;
_err_lc_not_enabled:
    return 28;
_err_not_initialized:
    return 29;
}
#pragma pop
static void __THPSetupBuffers(void) {
    u8 i;
    s16 *buffer;

    buffer = (s16 *)(((u32)__THPWorkArea + 31) & ~31);
    for (i = 0; i < 6; i++) {
        __THPMCUBuffer[i] = &buffer[i * 64];
    }
}

u8 __THPReadFrameHeader(void) {
    s32 i;
    u8 utmp8;

    __THPInfo->c += 2;
    if (*(__THPInfo->c)++ != 8) {
        return 10;
    }
    __THPInfo->yPixelSize = (u16)((__THPInfo->c[0] << 8) | __THPInfo->c[1]);
    __THPInfo->c += 2;
    __THPInfo->xPixelSize = (u16)((__THPInfo->c[0] << 8) | __THPInfo->c[1]);
    __THPInfo->c += 2;
    if (*(__THPInfo->c)++ != 3) {
        return 12;
    }
    for (i = 0; (u8)i < 3; i++) {
        __THPInfo->c++;
        utmp8 = *(__THPInfo->c)++;
        if (((u8)i == 0 && utmp8 != 0x22) || ((u8)i > 0 && utmp8 != 0x11)) {
            return 19;
        }
        __THPInfo->components[i].quantizationTableSelector = *(__THPInfo->c)++;
    }
    return 0;
}

u8 __THPReadScaneHeader(void) {
    s32 i;
    s32 compId;

    __THPInfo->c += 2;
    if (*(__THPInfo->c)++ != 3) {
        return 12;
    }
    for (i = 0; (u8)i < 3; i++) {
        __THPInfo->c++;
        compId = *(__THPInfo->c)++;
        __THPInfo->components[i].DCTableSelector = (u8)(compId >> 4);
        __THPInfo->components[i].ACTableSelector = (u8)(compId & 15);
        if ((__THPInfo->validHuffmanTabs & (1 << (compId >> 4))) == 0) {
            return 15;
        }
        if ((__THPInfo->validHuffmanTabs & (1 << ((compId & 15) + 1))) == 0) {
            return 15;
        }
    }
    __THPInfo->c += 3;
    __THPInfo->MCUsPerRow = (u16)((__THPInfo->xPixelSize + 15) / 16);
    __THPInfo->components[0].predDC = 0;
    __THPInfo->components[1].predDC = 0;
    __THPInfo->components[2].predDC = 0;
    return 0;
}

static void __THPHuffGenerateSizeTable(void);
static void __THPHuffGenerateCodeTable(void);
static void __THPHuffGenerateDecoderTables(u8 tabIndex);
u8 __THPReadHuffmanTableSpecification(void) {
    u8 t_class, id, i, tab_index;
    u16 length, num_Vij;

    __THPHuffmanSizeTab = __THPWorkArea;
    __THPHuffmanCodeTab = (u16 *)((u32)__THPWorkArea + 256 + 1);
    length = (u16)((__THPInfo->c[0] << 8) | __THPInfo->c[1]);
    __THPInfo->c += 2;
    length -= 2;

    for (;;) {
        i = *(__THPInfo->c)++;
        id = (u8)(i & 15);
        t_class = (u8)(i >> 4);
        __THPHuffmanBits = __THPInfo->c;
        tab_index = (u8)((id << 1) + t_class);
        num_Vij = 0;

        for (i = 0; i < 16; i++) {
            num_Vij += *(__THPInfo->c)++;
        }

        __THPInfo->huffmanTabs[tab_index].Vij = __THPInfo->c;
        __THPInfo->c += num_Vij;
        __THPHuffGenerateSizeTable();
        __THPHuffGenerateCodeTable();
        __THPHuffGenerateDecoderTables(tab_index);
        __THPInfo->validHuffmanTabs |= 1 << tab_index;
        length -= 17 + num_Vij;

        if (length == 0) {
            break;
        }
    }
    return 0;
}

static void __THPHuffGenerateSizeTable(void) {
    s32 p, l, i;

    p = 0;
    for (l = 1; l <= 16; l++) {
        i = (s32)__THPHuffmanBits[l - 1];
        while (i--) {
            __THPHuffmanSizeTab[p++] = (u8)l;
        }
    }
    __THPHuffmanSizeTab[p] = 0;
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

static void __THPHuffGenerateDecoderTables(u8 tabIndex) {
    s32 p, l;
    THPHuffmanTab *h;

    p = 0;
    h = &__THPInfo->huffmanTabs[tabIndex];
    for (l = 1; l <= 16; l++) {
        if (__THPHuffmanBits[l - 1]) {
            h->valPtr[l] = p - __THPHuffmanCodeTab[p];
            p += __THPHuffmanBits[l - 1];
            h->maxCode[l] = __THPHuffmanCodeTab[p - 1];
        } else {
            h->maxCode[l] = -1;
            h->valPtr[l] = -1;
        }
    }
    h->maxCode[17] = 0xfffff;
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

