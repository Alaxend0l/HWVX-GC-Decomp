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

int DBGRead(unsigned long addr, void *data, unsigned long size);
int DBRead(void *data, unsigned long size) {
    int interrupts;
    unsigned long base;

    interrupts = OSDisableInterrupts();
    if (SendMailData & 0x10000) {
        base = 0x1000;
    } else {
        base = 0;
    }
    DBGRead(base + 0x1e000, data, (size + 3) & ~3);
    RecvDataLeng = 0;
    EXIInputFlag = 0;
    OSRestoreInterrupts(interrupts);
    return 0;
}

int OSDisableInterrupts(void);
int OSRestoreInterrupts(int level);
int DBGReadStatus(unsigned long *status);
int DBGReadMailbox(unsigned long *mailbox);
unsigned long DBQueryData(void) {
    int interrupts;
    unsigned long v;

    EXIInputFlag = 0;
    if (!RecvDataLeng) {
        interrupts = OSDisableInterrupts();
        DBGReadStatus(&v);
        if (v & 1) {
            DBGReadMailbox(&v);
            v &= 0x1fffffff;
            if ((v & 0x1f000000) == 0x1f000000) {
                SendMailData = v;
                RecvDataLeng = v & 0x7fff;
                EXIInputFlag = 1;
            }
        }
        OSRestoreInterrupts(interrupts);
    }
    return RecvDataLeng;
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

volatile unsigned long __EXIRegs[16] : 0xCC006800;

static inline int DBGEXISelect(unsigned long v) {
    unsigned long regs = __EXIRegs[10];
    regs &= 0x405;
    regs |= 0x80 | (v << 4);
    __EXIRegs[10] = regs;
    return 1;
}

static inline int DBGEXIDeselect(void) {
    __EXIRegs[10] &= 0x405;
    return 1;
}

static inline int DBGEXISync(void) {
    while (__EXIRegs[13] & 1) {
    }
    return 1;
}

void DBInitComm(unsigned char **inputFlagPtr, MTRCallbackType callback) {
    int interrupts;

    interrupts = OSDisableInterrupts();
    pEXIInputFlag = &EXIInputFlag;
    *inputFlagPtr = pEXIInputFlag;
    MTRCallback = callback;
    __OSMaskInterrupts(0x18000);
    __EXIRegs[10] = 0;
    OSRestoreInterrupts(interrupts);
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

int DBGEXIImm(void *buffer, long bytecounter, unsigned long write);
#pragma push
#pragma use_lmw_stmw on
int DBGReadStatus(unsigned long *status) {
    int err;
    unsigned long cmd;

    __EXIRegs[10] = (__EXIRegs[10] & 0x405) | 0xC0;
    cmd = 0x40000000;
    err = !DBGEXIImm(&cmd, 2, 1);
    while (__EXIRegs[13] & 1) {
    }
    err |= !DBGEXIImm(status, 4, 0);
    while (__EXIRegs[13] & 1) {
    }
    __EXIRegs[10] &= 0x405;
    return !err;
}
#pragma pop

