//
// Created by faliks on 2020/10/10.
//

#ifndef BALLCTLUPPERTEST_UART_H
#define BALLCTLUPPERTEST_UART_H

#include "unistd.h"
#include "iostream"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "termios.h"
#include "errno.h"
#include "string.h"
#include "common.h"

extern const char *PORT;
extern int FD;
extern unsigned char buffer[32];

extern unsigned char selfInspectionReceiveArray[5];
extern unsigned char selfInspectionSendArray[5];
extern unsigned char checkQuestionOne[5];

int UartOpen();

bool compare(unsigned char *buffer, unsigned char *target, int len, int start = 0);

#endif //BALLCTLUPPERTEST_UART_H
