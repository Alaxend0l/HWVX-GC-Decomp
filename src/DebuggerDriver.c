typedef void (*MTRCallbackType)(int);
typedef void (*DBGCallbackType)(short, void *);

static MTRCallbackType MTRCallback;
static DBGCallbackType DBGCallback;
static unsigned long SendMailData;
static long RecvDataLeng;
static unsigned char *pEXIInputFlag;
static unsigned char EXIInputFlag;

void DBClose(void) {

}

void DBOpen(void) {

}

extern void (*DBGCallback)(short, void *);
volatile unsigned long __PIRegs[12] : 0xCC003000;
void DBGHandler(int interrupt, void *context) {
    __PIRegs[0] = 0x1000;
    if (DBGCallback) {
        DBGCallback(interrupt, context);
    }
}

#pragma peephole on
extern unsigned char EXIInputFlag;
extern void (*MTRCallback)(int);
void MWCallback(void) {
    EXIInputFlag = 1;
    if (MTRCallback) {
        MTRCallback(0);
    }
}

