

void GXSetZMode(int, int, int);
void GXSetBlendMode(int, int, int, int);
void GXSetNumTexGens(int);
void GXSetNumChans(int);
void GXSetNumTevStages(int);
void GXSetTevOrder(int, int, int, int);
void GXSetTevOp(int, int);
void GXSetTevSwapMode(int, int, int);
void GXSetTevSwapModeTable(int, int, int, int, int);
void THPGXRestore(void) {
    GXSetZMode(1, 7, 0);
    GXSetBlendMode(0, 1, 0, 0xF);
    GXSetNumTexGens(1);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetTevSwapMode(0, 0, 0);
    GXSetTevSwapMode(1, 0, 0);
    GXSetTevSwapMode(2, 0, 0);
    GXSetTevSwapMode(3, 0, 0);
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
}
