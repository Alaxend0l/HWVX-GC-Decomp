#include "common.h"

typedef struct OSFontHeader {
    u16 fontType;
    u16 firstChar;
    u16 lastChar;
    u16 invalChar;
    u16 ascent;
    u16 descent;
    u16 width;
    u16 leading;
    u16 cellWidth;
    u16 cellHeight;
    u32 sheetSize;
    u16 sheetFormat;
    u16 sheetColumn;
    u16 sheetRow;
    u16 sheetWidth;
    u16 sheetHeight;
    u16 widthTable;
    u32 sheetImage;
    u32 sheetFullSize;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;
} OSFontHeader;

static OSFontHeader *FontData;
static u8 *SheetImage;
static u8 *WidthTable;
static int CharsInSheet;

u32 OSLoadFont(OSFontHeader *fontData, void *temp);
void ExpandFontSheet(u8 *src, u8 *dst);


u16 OSGetFontEncode(void) {
    static u16 fontEncode = 0xFFFF;

    if (fontEncode <= 1) {
        return fontEncode;
    }
    switch (*(u32 *)0x800000CC) {
    case 0:
        fontEncode = (*(volatile u16 *)0xCC00206E & 2) ? 1 : 0;
        break;
    case 1:
    case 2:
    default:
        fontEncode = 0;
        break;
    }
    return fontEncode;
}

s32 __OSReadROM(void *buffer, s32 length, s32 offset);
static void ReadROM(void *buf, int length, int offset) {
    int len;

    while (length > 0) {
        len = (length <= 256) ? length : 256;
        length -= len;
        while (!__OSReadROM(buf, len, offset)) {
            ;
        }
        offset += len;
        (u8 *)buf += len;
    }
}

BOOL OSInitFont(OSFontHeader *fontData) {
    void *temp;

    if (OSGetFontEncode() == 1) {
        temp = (void *)((u8 *)fontData + 0xD3F00);
    } else {
        temp = (void *)((u8 *)fontData + 0x1D120);
    }
    temp = (void *)((u32)temp & ~31);
    if (OSLoadFont(fontData, temp) == 0) {
        return FALSE;
    }
    SheetImage = (u8 *)FontData + FontData->sheetImage;
    SheetImage = (u8 *)(((u32)SheetImage + 31) & ~31);
    ExpandFontSheet((u8 *)FontData + FontData->sheetImage, SheetImage);
    return TRUE;
}

