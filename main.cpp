#include <iostream>
#include "Uart.h"
#include "QuestionOne.h"

using namespace std;
using namespace cv;

bool isStartWork = false;

void selfInspection();

int getQuestion();

int main(int argc, char **argv) {
    //while (!UartOpen()) {

    //}
    //selfInspection();

    VideoCapture capture(0);
    Mat img;
    capture >> img;
    imshow("a", img);
    waitKey(0);
    while (1) {
        int table = getQuestion();
        table = 1;
        if (table)
            isStartWork = true;
        while (isStartWork) {
            capture >> img;
            switch (table) {
                case 1:
                    QuestionOne questionOne;
                    questionOne.totalSolve(img);

            }
            waitKey(0);
        }
    }


    return 0;
}

int getQuestion() {
    int readArray;
    readArray = read(FD, buffer, 5);
    if (readArray && compare(buffer, checkQuestionOne, 2, 0) && compare(buffer, checkQuestionOne, 3, 2))
        return buffer[2];
    return 0;
}

void selfInspection() {
    int readArray;
    while (1) {
        readArray = read(FD, buffer, 5);
        if (readArray && compare(buffer, selfInspectionReceiveArray, readArray)) {
            write(FD, selfInspectionReceiveArray, 5);
            return;
        }
    }
}