int GetTRKConnected(void);
void SetTRKConnected(int);
void OSReport(const char *);

static inline int usr_putchar_serial(char c) {
    char buf[2];
    int connect = GetTRKConnected();

    buf[0] = c;
    buf[1] = '\0';
    SetTRKConnected(0);
    OSReport(buf);
    SetTRKConnected(connect);
    return 0;
}

void usr_put_initialize(void) {

}
