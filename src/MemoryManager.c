#pragma readonly_strings on

void *memset(void *dst, int val, unsigned long n);
void *OSAllocFromHeap(int heap, unsigned long size);
void OSFreeToHeap(int heap, void *ptr);
void Dprintf(const char *fmt, ...);
extern int __OSCurrHeap;

void *Allocate(unsigned long size)
{
    void *ptr;

    if (size == 0) {
        return 0;
    }
    ptr = OSAllocFromHeap(__OSCurrHeap, size);
    if (ptr == 0) {
        Dprintf("ALLOCATE ERROR Could not allocate the requested memory, going to crash!!\n");
    }
    return ptr;
}

void Deallocate(void *ptr)
{
    if (ptr != 0) {
        OSFreeToHeap(__OSCurrHeap, ptr);
    }
}

void MemCpy32(void)
{
    memcpy();
}

void MemClr32(void *dst, unsigned long size)
{
    memset(dst, 0, size);
}

void HeapDump(void) {

}
