#include "flowctl.h"
#include "unistd.h"
#include <fcntl.h>
#include <termios.h>
#include <cstdio>
#include "string.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "question_one_routine.h"
#include "question_three_routine.h"
#include "common.h"
#include "QuestionOne.h"

#define BIT_RATE B115200

//#define ALLOW_RUN_WITHOUT_USART true

#if ALLOW_RUN_WITHOUT_USART
#warning ALLOW_RUN_WITHOUT_USART is set!!!
#endif

#define SERIAL_DEV "/dev/ttyUSB0"
//#define SERIAL_DEV "/dev/ttyS0"

#define VIDEO_CAP_INDEX 2

int serialFd = -1;
cv::VideoCapture videoIn;
const uchar REPLY_GENERAL_OK[5] = {0x75, 0x00, 0x00, 0x00, static_cast<uchar>(~0x00u)};

using namespace cv;

int startupInitInternal() {
#ifndef ALLOW_RUN_WITHOUT_USART
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
#endif
    //open video
    videoIn = VideoCapture(VIDEO_CAP_INDEX);
    if (!videoIn.isOpened()) {
        printf("Unable to open video%d\n", VIDEO_CAP_INDEX);
        return -1;
    }
    Mat pTest;
    videoIn >> pTest;
    if (pTest.empty()) {
        std::cerr << "VideoCapture: unable to read frame!" << std::endl;
        return 1;
    }
    //done
    printf("startupInitInternal done\n");
    return 0;
}


uchar cmdbuf[256];
int cmdbuf_start = 0, cmdbuf_len = 0;

void dropPendingRxPacket() {
    cmdbuf_len = cmdbuf_start = 0;
}

bool nextBasicPacketAsync(BasicPacket &pk) {
    //check buffer
    if (cmdbuf_len < 32 && (256 - cmdbuf_start - cmdbuf_len < 64)) {
        printf("Reset buffer\n");
        memcpy(cmdbuf, cmdbuf + cmdbuf_start, cmdbuf_len);
        cmdbuf_start = 0;
    }
    int rl = read(serialFd, cmdbuf + cmdbuf_start, 256 - cmdbuf_start - cmdbuf_len);
    if (rl > 0) {
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
    if (cmdbuf_len >= 5 && *(cmdbuf + cmdbuf_start) == 0x75) {
        //start from 6
        memcpy(pk, cmdbuf + cmdbuf_start, 5);
        cmdbuf_len -= 5;
        cmdbuf_start += 5;
        if (((uchar) pk[1]) == ((uchar) (~pk[4]))) {
            return true;
        } else if (pk[1] != 0) {
            uchar tmp5[5] = {0x75, 0x00, 0xFF, 0x00, (uchar) ~0x00u};
            write(serialFd, tmp5, 5);
            return false;
        }
    }
    return false;
}

bool next8bytePacketAsync(BasicPacket &pk) {
    //check buffer
    if (cmdbuf_len < 32 && (256 - cmdbuf_start - cmdbuf_len < 64)) {
        printf("Reset buffer\n");
        memcpy(cmdbuf, cmdbuf + cmdbuf_start, cmdbuf_len);
        cmdbuf_start = 0;
    }
    int rl = read(serialFd, cmdbuf + cmdbuf_start, 256 - cmdbuf_start - cmdbuf_len);
    if (rl > 0) {
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
    if (cmdbuf_len >= 8 && *(cmdbuf + cmdbuf_start) == 0x75) {
        //start from 6
        memcpy(pk, cmdbuf + cmdbuf_start, 8);
        cmdbuf_len -= 8;
        cmdbuf_start += 8;
        if (((uchar) pk[1]) == ((uchar) (~pk[7]))) {
            return true;
        } else if (pk[1] != 0) {
            uchar tmp5[5] = {0x75, 0x00, 0x00, 0x00, (uchar) ~0x00u};
            write(serialFd, tmp5, 5);
            return false;
        }
    }
    return false;
}


void drawDemo(cv::Mat img) {
    GaussianBlur(img, img, Size(3, 3), 1, 1);
    std::vector<Mat> hsvImg;
    QuestionOne q1;
    QuestionOne::Color color = q1.getColor(img, hsvImg);
    //显示文字信息
    Vec3b getColor = img.at<Vec3b>(img.cols / 2, img.rows / 2);
    //cout << ((color == Color::RED) ? "R" : color == Color::GREEN ? "G" : color == Color::BLUE ? "B" : "None");
    //cout << "  (" << (int) getColor[0] << ", " << (int) getColor[1] << ", " << (int) getColor[2] << ")" << endl;
    circle(img, Point(img.cols / 2, img.rows / 2), 2, Scalar(0, 0, 0), -1);

    //blur(img, img, Size(7, 7));
    //二值化
    Mat binaryImg = Mat(img.rows, img.cols, CV_8U);

    if (color == QuestionOne::Color::RED)
        q1.getPureColorImg(hsvImg, binaryImg, 0);
    else if (color == QuestionOne::Color::GREEN)
        q1.getPureColorImg(hsvImg, binaryImg, 1);
    else
        q1.getPureColorImg(hsvImg, binaryImg, 2);

    std::vector<std::vector<Point>> contours, approxSet;
    std::vector<Vec4i> hierarchy;
    //begin bg


    //begin shape
    findContours(binaryImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    drawContours(img, contours, -1, Scalar(0, 0, 255), 1, 8);

    float minDistance = 114514;
    std::vector<Point> bestApprox;
    std::vector<Point> bestRawContour;
    for (const std::vector<Point> &cnt:contours) {
        float epsilon = 0.03f * arcLength(cnt, true);
        if (arcLength(cnt, true) < 16)continue;
        std::vector<Point> approx;
        approxPolyDP(cnt, approx, epsilon, true);
        approxSet.push_back(approx);
        Mat tmp(cnt);
        Moments moment = moments(tmp, false);
        //除数不能为0
        if (moment.m00 != 0) {
            int x = cvRound(moment.m10 / moment.m00);//计算重心横坐标
            int y = cvRound(moment.m01 / moment.m00);//计算重心纵坐标
            float dist = ab2c(x - img.cols / 2, y - img.rows / 2);
            if (minDistance > dist) {
                bestApprox = approx;
                bestRawContour = cnt;
                minDistance = dist;
            }
        }
    }
    drawContours(img, approxSet, -1, Scalar(0, 255, 0), 1, 8);
}

/* sleep for ms */
void msleep(int ms) {
    struct timeval delay = {};
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000;
    select(0, nullptr, nullptr, nullptr, &delay);
}

void doHandleQuestion(int qid) {
    switch (qid) {
        case 1:
        case 2: {
            executeQuestionOne();
            break;
        }
        case 3: {
            executeQuestionThree();
            break;
        }
        default: {
            printf("doHandleQuestion: unknown id %d\n", qid);
        }
    }
}

bool SHOW_GUI = false;

void startupAndLoop() {
    int __i = startupInitInternal();
    if (__i) {
        std::cerr << "startupInitInternal error!" << std::endl;
        exit(__i);
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    Mat cameraPreview;
    while (true) {
        //enter main loop
        //read a 5-byte packet and handle it
        BasicPacket pk;
        bool hasRecv;
        if (!(hasRecv = nextBasicPacketAsync(pk))) {
            msleep(100);
        }
        if (SHOW_GUI) {
            videoIn >> cameraPreview;
            flip(cameraPreview, cameraPreview, -1);
            drawDemo(cameraPreview);
            circle(cameraPreview, Point(cameraPreview.cols / 2, cameraPreview.rows / 2), 2, Scalar(0, 0, 0), -1);
            imshow("prev", cameraPreview);
            waitKey(20);
        }
        if (hasRecv) {
            switch (pk[1]) {
                case 1: {
                    uchar tmp5[5] = {0x75, 0x01, 0x01, 0x00, (uchar) (~0x01u)};
                    write(serialFd, tmp5, 5);
                    break;
                }
                case 2: {
                    int questionId = pk[2];
                    write(serialFd, REPLY_GENERAL_OK, 5);
                    doHandleQuestion(questionId);
                    break;
                }
            }
        }
    }
#pragma clang diagnostic pop

}