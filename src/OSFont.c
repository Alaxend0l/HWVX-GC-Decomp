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

extern u16 HankakuToCode[];
extern u16 Zenkaku2Code[];

static BOOL IsSjisLeadByte(u8 c) {
    return (0x81 <= c && c <= 0x9F) || (0xE0 <= c && c <= 0xFC);
}

static BOOL IsSjisTrailByte(u8 c) {
    return 0x40 <= c && c <= 0xFC && c != 0x7F;
}


u16 OSGetFontEncode(void);
static int GetFontCode(u16 code) {
    if (OSGetFontEncode() == 1) {
        if (0x20 <= code && code <= 0xDF) {
            return HankakuToCode[code - 0x20];
        } else if (0x889E < code && code <= 0x9872) {
            int i = ((code >> 8) - 0x88) * 188;
            int j = code & 0xFF;

            if (!IsSjisTrailByte(j)) {
                return 0;
            }
            j -= 0x40;
            if (0x40 <= j) {
                j--;
            }
            return i + j + 0x2BE;
        } else if (0x8140 <= code && code < 0x879E) {
            int i = ((code >> 8) - 0x81) * 188;
            int j = code & 0xFF;

            if (!IsSjisTrailByte(j)) {
                return 0;
            }
            j -= 0x40;
            if (0x40 <= j) {
                j--;
            }
            return Zenkaku2Code[i + j];
        }
    } else if (0x20 < code && code <= 0xFF) {
        return code - 0x20;
    }
    return 0;
}

static void Decode(u8 *s, u8 *d) {
    int i;
    int j;
    int k;
    int p;
    int q;
    int r7;
    int r25;
    int cnt;
    int os;
    unsigned int flag;
    unsigned int r22;

    os = *(int *)(s + 0x4);
    r7 = *(int *)(s + 0x8);
    r25 = *(int *)(s + 0xC);
    q = 0;
    flag = 0;
    p = 16;
    do {
        if (flag == 0) {
            r22 = *(unsigned int *)(s + p);
            p += 4;
            flag = 32;
        }
        if (r22 & 0x80000000) {
            d[q++] = s[r25++];
        } else {
            j = s[r7] << 8;
            j |= s[r7 + 1];
            r7 += 2;
            k = q - (j & 0xFFF);
            cnt = j >> 12;
            if (cnt == 0) {
                cnt = s[r25++] + 0x12;
            } else {
                cnt += 2;
            }
            for (i = 0; i < cnt; i++, q++, k++) {
                d[q] = d[k - 1];
            }
        }
        r22 <<= 1;
        flag--;
    } while (q < os);
}

static void ReadROM(void *buf, int length, int offset);

static u32 GetFontSize(u8 *buf) {
    if (buf[0] == 'Y' && buf[1] == 'a' && buf[2] == 'y') {
        return *(u32 *)(buf + 4);
    }
    return 0;
}

static u32 ReadFont(void *img, void *fontData) {
    u32 size;

    if (OSGetFontEncode() == 1) {
        ReadROM(img, 0x4D000, 0x1AFF00);
    } else {
        ReadROM(img, 0x3000, 0x1FCF00);
    }
    return GetFontSize(img);
}

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

u32 OSLoadFont(OSFontHeader *fontData, void *temp) {
    u32 size;

    SheetImage = 0;
    size = ReadFont(temp, fontData);
    if (size) {
        Decode(temp, (u8 *)fontData);
        FontData = fontData;
        WidthTable = (u8 *)FontData + FontData->widthTable;
        CharsInSheet = FontData->sheetColumn * FontData->sheetRow;
        if (OSGetFontEncode() == 1) {
            int i;
            int x;
            u8 *image;
            int row;
            int column;
            int sheet;
            int numChars;
            int code;
            int y;
            u16 fix[4] = { 0x2ABE, 0x003D, 0x003D, 0x003D };

            code = GetFontCode('T');
            sheet = code / CharsInSheet;
            numChars = code - sheet * CharsInSheet;
            row = numChars / FontData->sheetColumn;
            column = numChars - row * FontData->sheetColumn;
            x = column * FontData->cellWidth;
            y = row * FontData->cellHeight;
            image = (u8 *)FontData + FontData->sheetImage;
            image += (sheet * FontData->sheetSize) / 2;
            for (i = 4; i < 8; i++) {
                u8 *p;
                p = image + ((FontData->sheetWidth / 8 * 32) / 2) * ((y + i) / 8);
                p += (x / 8) * 16;
                p += ((y + i) % 8) * 2;
                p += (x % 8) / 4;
                *(u16 *)p = fix[i - 4];
            }
        }
    }
    return size;
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

char *OSGetFontTexture(char *string, void **image, s32 *x, s32 *y, s32 *width) {
    u16 code;
    s32 fontCode;
    s32 sheet;
    s32 numChars;
    s32 row;
    s32 column;

    code = (u8)*string;
    if (code == '\0') {
        *image = 0;
        return string;
    }
    string++;
    if (OSGetFontEncode() == 1 && IsSjisLeadByte(code) && *string) {
        code = (code << 8) | (u8)*string++;
    }
    fontCode = GetFontCode(code);

    sheet = fontCode / CharsInSheet;
    *image = SheetImage + FontData->sheetSize * sheet;

    numChars = fontCode - sheet * CharsInSheet;
    row = numChars / FontData->sheetColumn;
    column = numChars - row * FontData->sheetColumn;
    *x = column * FontData->cellWidth;
    *y = row * FontData->cellHeight;

    if (width) {
        *width = WidthTable[fontCode];
    }
    return string;
}

