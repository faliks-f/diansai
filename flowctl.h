#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "opencv2/videoio.hpp"

//WITH 0x75 header
typedef uchar BasicPacket[8];

typedef unsigned char uchar;

extern int serialFd;

extern bool SHOW_GUI;

extern const uchar REPLY_GENERAL_OK[5];

extern cv::VideoCapture videoIn;

void startupAndLoop();

void msleep(int ms);

void dropPendingRxPacket();

bool nextBasicPacketAsync(BasicPacket &pk);

bool next8bytePacketAsync(BasicPacket &pk);
