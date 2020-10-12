#include "serial_cmd.h"
#include "stdio.h"
#include "flowctl.h"

void moveCamera(int dX, int dY) {
    printf("moveCamera: Stub\n");
}

bool setLaserPowerOn(bool on) {
    printf("setLaserPowerOn: %d - ", on);
    bool ret = sendPacket5(0x08, on ? 1 : 2, 0x00, 3);
    printf(ret ? "success\n" : "failed\n");
}

bool measureDistanceAndWaitForReply(int &distanceMM) {
    printf("measureDistanceAndWaitForReply: enter\n");
    dropPendingRxPacket();
    int d = 0;
    if (!sendPacket5(0x09, 0x01, 0, 3)) {
        printf("unable to enable TF mini\n");
        return false;
    }
    std::vector<int> distArr;
    BasicPacket pk = {};
    for (int i = 20; i > 0; --i) {
        msleep(250);
        if (nextBasicPacketAsync(pk)) {
            if (pk[1] == 0x0A) {
                uchar hi = pk[2];
                uchar lo = pk[3];
                int record = 0xffffu & (hi << 8 | lo);
                distArr.push_back(record);
                if (distArr.size() > 5)break;
            }
        }
    }
    if (distArr.empty()) {
        printf("unable to recv from TF mini, no records;\n");
    } else {
        for (int distV:distArr) {
            d += distV;
        }
        d /= distArr.size();
    }
    sendPacket5(0x09, 0x02, 0, 3);
    if (d != 0) {
        distanceMM = d * 10;
    }
    return false;
}

bool sendPacket5(uchar cmd, uchar arg1, uchar arg2, int maxTry) {
    uchar tmp5[5] = {0x75, cmd, arg1, arg2, (uchar) (~cmd)};
    if (maxTry <= 0) {
        write(serialFd, tmp5, 5);
        return true;
    } else {
        do {
            write(serialFd, tmp5, 5);
            msleep(500);
            BasicPacket pk = {};
            if (nextBasicPacketAsync(pk)) {
                if (pk[1] == 0) {
                    int retCode = pk[2];
                    if (retCode == 0)return true;
                    if (retCode == 1)return false;
                }
            }
            if (maxTry-- <= 0)return false;
        } while (true);
    }
}


bool reportFinalResult(int shape, int size, int distance, int color) {
    printf("reportFinalResult - ");
    uchar tmp8[8] = {0x75, 0x0B, (uchar) (color << 4 | shape), (uchar) (size >> 8), (uchar) (size & 0xFF),
                     (uchar) (distance >> 8), (uchar) (distance & 0xFF), (uchar) (~0x0B)};
    int maxTry = 5;
    do {
        write(serialFd, tmp8, 8);
        msleep(500);
        BasicPacket pk = {};
        if (next8bytePacketAsync(pk)) {
            if (!memcmp(pk + 1, tmp8 + 1, 6)) {
                sendGeneralReplyOK();
                printf("success\n");
                return true;
            }
        }
        if (maxTry-- <= 0) {
            printf("error\n");
            return false;
        }
    } while (true);
}

bool reportEndOfSolution() {
    printf("reportEndOfSolution - ");
    bool ret = sendPacket5(0x0F, 0, 0x00, 3);
    printf(ret ? "success\n" : "failed\n");
}

void sendGeneralReplyOK() {
    write(serialFd, REPLY_GENERAL_OK, 5);
}

float ab2c(float a, float b) {
    return sqrt(a * a + b * b);
}
