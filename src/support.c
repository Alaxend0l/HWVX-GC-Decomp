#include "common.h"

typedef struct TRKBuffer {
    u32 mutex;
    s32 isInUse;
    u32 length;
    u32 position;
    u8 data[0x880];
} TRKBuffer;

typedef enum {
    kDSWriteFile = 0xD0,
    kDSReadFile = 0xD1
} MessageCommandID;

s32 TRKGetFreeBuffer(s32 *msgID, TRKBuffer **outMsg);
void TRKReleaseBuffer(s32 idx);
TRKBuffer *TRKGetBuffer(s32 idx);
s32 TRKSetBufferPosition(TRKBuffer *b, u32 pos);
s32 TRKRequestSend(TRKBuffer *msgBuf, s32 *bufferId, u32 p1, u32 p2, s32 p3);
s32 TRKAppendBuffer1_ui32(TRKBuffer *b, u32 data);
s32 TRKReadBuffer1_ui8(TRKBuffer *b, u8 *data);
s32 TRKReadBuffer1_ui32(TRKBuffer *b, u32 *data);
s32 TRKAppendBuffer1_ui16(TRKBuffer *b, u16 data);
s32 TRKAppendBuffer_ui8(TRKBuffer *b, const u8 *data, s32 count);
unsigned long strlen(const char *s);

static inline s32 TRKAppendBuffer1_ui8(TRKBuffer *b, u8 data) {
    if (b->position >= 0x880) {
        return 0x301;
    }
    b->data[b->position++] = data;
    b->length++;
    return 0;
}

#pragma use_lmw_stmw on
s32 HandlePositionFileSupportRequest(u32 fileHandle, u32 *position, u8 mode, u8 *ioResult) {
    s32 error;
    s32 replyBufferId;
    TRKBuffer *replyBuffer;
    s32 bufferId;
    TRKBuffer *buffer;

    error = TRKGetFreeBuffer(&bufferId, &buffer);
    if (error == 0) {
        error = TRKAppendBuffer1_ui8(buffer, 0xD4);
    }
    if (error == 0) {
        error = TRKAppendBuffer1_ui32(buffer, fileHandle);
    }
    if (error == 0) {
        error = TRKAppendBuffer1_ui32(buffer, *position);
    }
    if (error == 0) {
        error = TRKAppendBuffer1_ui8(buffer, mode);
    }
    if (error == 0) {
        *ioResult = 0;
        error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
        if (error == 0) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            TRKSetBufferPosition(replyBuffer, 2);
        }
        if (error == 0) {
            error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
        }
        if (error == 0) {
            error = TRKReadBuffer1_ui32(replyBuffer, position);
        } else {
            *position = -1;
        }
        TRKReleaseBuffer(replyBufferId);
    }
    TRKReleaseBuffer(bufferId);
    return error;
}
s32 HandleCloseFileSupportRequest(u32 fileHandle, u8 *ioResult) {
    s32 error;
    s32 replyBufferId;
    TRKBuffer *replyBuffer;
    s32 bufferId;
    TRKBuffer *buffer;

    error = TRKGetFreeBuffer(&bufferId, &buffer);
    if (error == 0) {
        error = TRKAppendBuffer1_ui8(buffer, 0xD3);
    }
    if (error == 0) {
        error = TRKAppendBuffer1_ui32(buffer, fileHandle);
    }
    if (error == 0) {
        *ioResult = 0;
        error = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
        if (error == 0) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            TRKSetBufferPosition(replyBuffer, 2);
        }
        if (error == 0) {
            error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
        }
        TRKReleaseBuffer(replyBufferId);
    }
    TRKReleaseBuffer(bufferId);
    return error;
}

s32 HandleOpenFileSupportRequest(const char *path, u8 replyError, u32 *param, u8 *ioResult) {
    s32 error;
    s32 replyBufferId;
    TRKBuffer *replyBuffer;
    s32 bufferId;
    TRKBuffer *buffer;

    *param = 0;
    error = TRKGetFreeBuffer(&bufferId, &buffer);
    if (error == 0) {
        error = TRKAppendBuffer1_ui8(buffer, 0xD2);
    }
    if (error == 0) {
        error = TRKAppendBuffer1_ui8(buffer, replyError);
    }
    if (error == 0) {
        error = TRKAppendBuffer1_ui16(buffer, strlen(path) + 1);
    }
    if (error == 0) {
        error = TRKAppendBuffer_ui8(buffer, (const u8 *)path, strlen(path) + 1);
    }
    if (error == 0) {
        *ioResult = 0;
        error = TRKRequestSend(buffer, &replyBufferId, 7, 3, 0);
        if (error == 0) {
            replyBuffer = TRKGetBuffer(replyBufferId);
            TRKSetBufferPosition(replyBuffer, 2);
        }
        if (error == 0) {
            error = TRKReadBuffer1_ui8(replyBuffer, ioResult);
        }
        if (error == 0) {
            error = TRKReadBuffer1_ui32(replyBuffer, param);
        }
        TRKReleaseBuffer(replyBufferId);
    }
    TRKReleaseBuffer(bufferId);
    return error;
}

void TRKProcessInput(s32 bufferIdx);
#pragma push
#pragma use_lmw_stmw on
s32 TRKRequestSend(TRKBuffer *msgBuf, s32 *bufferId, u32 p1, u32 p2, s32 p3) {
    void TRKProcessInput(s32 bufferIdx);
    s32 error;
    TRKBuffer *buffer;
    u32 timer;
    s32 tries;
    s32 badReply;
    u8 msgCmd;
    u8 msgReplyError;

    error = 0;
    badReply = 1;
    *bufferId = -1;
    tries = p2 + 1;
    while (tries != 0 && *bufferId == -1 && error == 0) {
        error = TRKMessageSend(msgBuf);
        if (error == 0) {
            if (p3) {
                timer = 0;
            }
            while (1) {
                do {
                    *bufferId = TRKTestForPacket();
                    if (*bufferId != -1) {
                        break;
                    }
                } while (!p3 || ++timer < 79999980);

                if (*bufferId == -1) {
                    break;
                }

                badReply = 0;
                buffer = TRKGetBuffer(*bufferId);
                TRKSetBufferPosition(buffer, 0);
                if ((error = TRKReadBuffer1_ui8(buffer, &msgCmd)) != 0) {
                    break;
                }
                if (msgCmd >= 0x80) {
                    break;
                }
                TRKProcessInput(*bufferId);
                *bufferId = -1;
            }

            if (*bufferId != -1) {
                if (buffer->length < p1) {
                    badReply = 1;
                }
                if (error == 0 && !badReply) {
                    error = TRKReadBuffer1_ui8(buffer, &msgReplyError);
                }
                if (error == 0 && !badReply) {
                    if (msgCmd != 0x80 || msgReplyError != 0) {
                        badReply = 1;
                    }
                }
                if (error != 0 || badReply) {
                    TRKReleaseBuffer(*bufferId);
                    *bufferId = -1;
                }
            }
        }
        tries--;
    }

    if (*bufferId == -1) {
        error = 0x800;
    }
    return error;
}

s32 TRKReadBuffer1_ui16(TRKBuffer *b, u16 *data);
s32 TRKReadBuffer_ui8(TRKBuffer *b, u8 *data, s32 count);
s32 TRKSuppAccessFile(u32 file, u8 *data, u32 *count, u8 *io_result, s32 need_reply, s32 read) {
    s32 error;
    s32 exit;
    u32 done;
    u32 length;
    s32 replyBufferId;
    s32 bufferId;
    TRKBuffer *buffer;
    TRKBuffer *replyBuffer;
    u16 replyLength;
    u8 replyIOResult;

    if (data == NULL || *count == 0) {
        return 2;
    }

    exit = 0;
    *io_result = 0;
    done = 0;
    error = 0;
    while (!exit && done < *count && error == 0 && *io_result == 0) {
        if (*count - done > 0x800) {
            length = 0x800;
        } else {
            length = *count - done;
        }

        error = TRKGetFreeBuffer(&bufferId, &buffer);
        if (error == 0) {
            error = TRKAppendBuffer1_ui8(buffer, (u8)(read ? 0xD1 : 0xD0));
        }
        if (error == 0) {
            error = TRKAppendBuffer1_ui32(buffer, file);
        }
        if (error == 0) {
            error = TRKAppendBuffer1_ui16(buffer, length);
        }
        if (!read && error == 0) {
            error = TRKAppendBuffer_ui8(buffer, data + done, length);
        }

        if (error == 0) {
            if (need_reply) {
                replyLength = 0;
                replyIOResult = 0;
                error = TRKRequestSend(buffer, &replyBufferId, read ? 5 : 5, 3, !(read && file == 0));
                if (error == 0) {
                    replyBuffer = TRKGetBuffer(replyBufferId);
                    TRKSetBufferPosition(replyBuffer, 2);
                }
                if (error == 0) {
                    error = TRKReadBuffer1_ui8(replyBuffer, &replyIOResult);
                }
                if (error == 0) {
                    error = TRKReadBuffer1_ui16(replyBuffer, &replyLength);
                }
                if (read && error == 0) {
                    if (replyBuffer->length != replyLength + 5) {
                        replyLength = replyBuffer->length - 5;
                        if (replyIOResult == 0) {
                            replyIOResult = 1;
                        }
                    }
                    if (replyLength <= length) {
                        error = TRKReadBuffer_ui8(replyBuffer, data + done, replyLength);
                    }
                }
                if (replyLength != length) {
                    if ((!read || replyLength >= length) && replyIOResult == 0) {
                        replyIOResult = 1;
                    }
                    length = replyLength;
                    exit = 1;
                }
                *io_result = replyIOResult;
                TRKReleaseBuffer(replyBufferId);
            } else {
                error = TRKMessageSend(buffer);
            }
        }
        TRKReleaseBuffer(bufferId);
        done += length;
    }
    *count = done;
    return error;
}

#pragma pop

