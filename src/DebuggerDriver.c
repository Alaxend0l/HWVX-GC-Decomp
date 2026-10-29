

void DBClose(void) {

}

void DBOpen(void) {

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

