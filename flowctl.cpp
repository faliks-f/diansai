#include "flowctl.h"
#include "unistd.h"
#include <fcntl.h>
#include <termios.h>
#include <cstdio>
#include "string.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"


#define BIT_RATE B115200

#define SERIAL_DEV "/dev/ttyUSB0"
//#define SERIAL_DEV "/dev/ttyS0"

#define VIDEO_CAP_INDEX 0

int serialFd = -1;
cv::VideoCapture videoIn;

using namespace cv;

int startupInitInternal() {
    //open port
    serialFd = open(SERIAL_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
    //O_NONBLOCK设置为非阻塞模式，在read时不会阻塞住，在读的时候将read放在while循环中，下一节篇文档将详细讲解阻塞和非阻塞
    printf("fd=%d\n", serialFd);
    if (serialFd == -1) {
        perror("Can't Open SerialPort");
    }
    /*恢复串口为阻塞状态*/
    if (fcntl(serialFd, F_SETFL, 0) < 0)
        printf("fcntl failed!\n");
    else
        printf("fcntl=%d\n", fcntl(serialFd, F_SETFL, 0));
    struct termios newtio, oldtio;
    /*保存测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/
    if (tcgetattr(serialFd, &oldtio) != 0) {
        perror("SetupSerial 1");
        printf("tcgetattr( fd,&oldtio) -> %d\n", tcgetattr(serialFd, &oldtio));
        return -1;
    }
    bzero(&newtio, sizeof(newtio));
    /*步骤一，设置字符大小*/
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    /*设置停止位*/
    newtio.c_cflag |= CS8;
    /*设置奇偶校验位*/
    /*奇数
    newtio.c_cflag |= PARENB;
    newtio.c_cflag |= PARODD;
    newtio.c_iflag |= (INPCK | ISTRIP);
    /*
    //偶数
    newtio.c_iflag |= (INPCK | ISTRIP);
    newtio.c_cflag |= PARENB;
    newtio.c_cflag &= ~PARODD;
    //无奇偶校验位
     */
    newtio.c_cflag &= ~PARENB;

    /*设置波特率*/
    cfsetispeed(&newtio, BIT_RATE);
    cfsetospeed(&newtio, BIT_RATE);
    /*设置停止位*/
    //newtio.c_cflag &= ~CSTOPB;//1
    newtio.c_cflag |= CSTOPB;//2
    /*设置等待时间和最小接收字符*/
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;
    /*处理未接收字符*/
    tcflush(serialFd, TCIFLUSH);
    /*激活新配置*/
    if ((tcsetattr(serialFd, TCSANOW, &newtio)) != 0) {
        perror("com set error");
        return -1;
    }
    printf("Serial port " SERIAL_DEV " set done!\n");
    //open video
    videoIn = VideoCapture(VIDEO_CAP_INDEX);
    if (!videoIn.isOpened()) {
        printf("Unable to open video%d\n", VIDEO_CAP_INDEX);
        return -1;
    }
    //done
    return 0;
}

//Without 0x75 header
typedef uchar BasicPacket[5];


uchar cmdbuf[256];
int cmdbuf_start = 0, cmdbuf_len = 0;

bool nextBasicPacketAsync(BasicPacket &pk) {
    //check buffer
    if (cmdbuf_len < 32 && (256 - cmdbuf_start - cmdbuf_len < 64)) {
        printf("Reset buffer\n");
        memcpy(cmdbuf, cmdbuf + cmdbuf_start, cmdbuf_len);
        cmdbuf_start = 0;
    }
    int rl = read(serialFd, cmdbuf + cmdbuf_start, 256 - cmdbuf_start - cmdbuf_len);
    if (rl != 0) {
        for (int i = 0; i < rl; i++) {
            printf("%02x, ", cmdbuf[(cmdbuf_start + i) % 256]);
        }
        cmdbuf_len += rl;
        printf("\n");
    }
    //skip bad value
    while (cmdbuf_len > 0 && *(cmdbuf + cmdbuf_start) != 0x75) {
        cmdbuf_len--;
        cmdbuf_start++;
    }
    if (cmdbuf_len > 5 && *(cmdbuf + cmdbuf_start) == 0x75) {
        //start from 6
        memcpy(pk, cmdbuf + cmdbuf_start + 1, 5);
        cmdbuf_len -= 6;
        cmdbuf_start += 6;
        return true;
    }
    return false;
}

/* 毫秒级 延时 */
void msleep(int ms) {
    struct timeval delay = {};
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000;
    select(0, nullptr, nullptr, nullptr, &delay);
}


void startupAndLoop(const bool showGui) {
    startupInitInternal();


}