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

unsigned long __OSMaskInterrupts(unsigned long mask);
unsigned long __OSUnmaskInterrupts(unsigned long mask);
void *__OSSetInterruptHandler(short interrupt, void *handler);
void MWCallback(void);
void DBGHandler(int interrupt, void *context);
void DBInitInterrupts(void) {
    __OSMaskInterrupts(0x18000);
    __OSMaskInterrupts(0x40);
    DBGCallback = (DBGCallbackType)MWCallback;
    __OSSetInterruptHandler(0x19, DBGHandler);
    __OSUnmaskInterrupts(0x40);
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

