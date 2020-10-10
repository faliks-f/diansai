//
// Created by faliks on 2020/10/10.
//

#include "Uart.h"

using namespace std;

const char *PORT = "/dev/ttyUSB0";
int FD;
unsigned char buffer[32];

unsigned char selfInspectionReceiveArray[5] = {0x75, 0x01, 0x00, 0x00, 0xfe};
unsigned char selfInspectionSendArray[5] = {0x75, 0x01, 0x01, 0x00, 0xfe};
unsigned char checkQuestionOne[5] = {0x75, 0x02, 0x00, 0x00, 0xfd};

int UartOpen() {
    FD = open(PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (FD < 0) {
        cout << "Can't open";
        return 0;
    }
    struct termios Opt;
    tcgetattr(FD, &Opt);
    cfsetispeed(&Opt, B115200);
    cfsetospeed(&Opt, 115200);
    tcsetattr(FD, TCSANOW, &Opt);
    tcflush(FD, TCIOFLUSH);
    if (tcgetattr(FD, &Opt) != 0) {
        perror("SetupSerial 1");
        return 0;
    }
    Opt.c_cflag &= ~CSIZE;
    Opt.c_cflag |= CS8;
    Opt.c_cflag &= ~PARENB;
    Opt.c_cflag &= ~INPCK;
    Opt.c_cflag &= ~CSTOPB;
    tcflush(FD, TCIFLUSH);
    Opt.c_cc[VTIME] = 150;
    Opt.c_cc[VMIN] = 0;
    if (tcsetattr(FD, TCSANOW, &Opt) != 0) {
        perror("SetupSerial");
        return 0;
    }
    return 1;
}

bool compare(unsigned char *buffer, unsigned char *target, int len, int start) {
    for (int i = start; i < start + len; ++i)
        if (buffer[i] != target[i])
            return false;
    return true;
}