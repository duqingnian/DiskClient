#ifndef PDU_H
#define PDU_H

typedef enum {
    TEXT,           // 普通文字消息
    AUDIO,          // 语音消息
    PICTURE,        // 图片消息
    //FILE,          // 文件传输
    NOTICE          // 通知
} MSGTYPE;

#endif // PDU_H
