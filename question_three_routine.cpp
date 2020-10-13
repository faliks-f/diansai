#include "question_three_routine.h"


#include "question_one_routine.h"
#include "stdio.h"
#include "serial_cmd.h"
#include "flowctl.h"
#include "QuestionOne.h"
#include "iostream"
#include "Blob.h"
#include "algorithm"
#include "common.h"

using namespace cv;
using namespace std;

struct DegreeDistanceHolder {
    int degree;
    int distance;
};

void executeQuestionThree() {
    printf("Enter question 3\n");
    setLaserPowerOn(false);
    Mat img;
    videoIn >> img;
    if (img.empty()) {
        std::cerr << "FATAL: read frame failed!" << endl;
        return;
    }
    //begin
    if (setCameraYaw(-30)) {
        msleep(3000);
        printf("Turned to left\n");
    }
    int currentDistance = 114514;
    int currentDegree = 0;
    for (int i = -27; i < 30; i += 6) {
        int dist = 0;
        bool turnSc = setCameraYaw(i);
        if (!turnSc) {
            std::cerr << "E: turn to deg " << i << " failure" << std::endl;
            continue;
        }
        msleep(600);
        bool distSc = measureDistanceAndWaitForReply(dist);
        if (!distSc) {
            std::cerr << "E: measure distance at deg " << i << " failure" << std::endl;
            continue;
        }
        if (dist < 1500 || dist > 3500)continue;
        if (dist < currentDistance) {
            currentDistance = dist;
            currentDegree = i;
        }
    }
    if (currentDistance > 10000) {
        std::cerr << "FATAL: min distance NOT found" << std::endl;
        return;
    }
    cout << "Select degree = " << currentDegree << ", distance = " << currentDistance << endl;
    if (!setCameraYaw(currentDegree)) {
        std::cerr << "FATAL: fail to go to degree " << currentDegree << std::endl;
        return;
    }
    msleep(2000);
    //end of first measure

    int currentSrcDistance = 1114514;

    const int SRC_DISTANCE = min(img.cols, img.rows) / 30;
    int iColor = 0;
    vector<Point> bestApprox;
    vector<Point> bestRawContour;
    int failureCount = 0;
    int correctCount = 0;
    int lastWentDegree = currentDegree;
    do {
        videoIn >> img;
        flip(img, img, -1);
        GaussianBlur(img, img, Size(3, 3), 1, 1);
        vector<Mat> hsvImg;

        QuestionOne::Color color = QuestionOne::getColor(img, hsvImg);
        //显示文字信息
        Vec3b getColor = img.at<Vec3b>(img.cols / 2, img.rows / 2);
        //cout << ((color == Color::RED) ? "R" : color == Color::GREEN ? "G" : color == Color::BLUE ? "B" : "None");
        //cout << "  (" << (int) getColor[0] << ", " << (int) getColor[1] << ", " << (int) getColor[2] << ")" << endl;
        circle(img, Point(img.cols / 2, img.rows / 2), 2, Scalar(0, 0, 0), -1);
        if (color == QuestionOne::Color::NONE) {
            cout << "No Color!" << endl;
            return;
        }


        //blur(img, img, Size(7, 7));
        //二值化
        Mat binaryImg = Mat(img.rows, img.cols, CV_8U);
        if (color == QuestionOne::Color::RED) {
            iColor = 1;
            QuestionOne::getPureColorImg(hsvImg, binaryImg, 0);
        } else if (color == QuestionOne::Color::GREEN) {
            iColor = 2;
            QuestionOne::getPureColorImg(hsvImg, binaryImg, 1);
        } else {
            iColor = 3;
            QuestionOne::getPureColorImg(hsvImg, binaryImg, 2);
        }
        vector<vector<Point>> contours, approxSet;
        vector<Vec4i> hierarchy;
        //begin bg
        //begin shape
        findContours(binaryImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
        drawContours(img, contours, -1, Scalar(0, 0, 255), 1, 8);

        float minDistance = 114514;

        bestApprox.clear();
        bestRawContour.clear();

        Point bestCenter;
        for (const vector<Point> &cnt:contours) {
            float epsilon = 0.03f * arcLength(cnt, true);
            if (arcLength(cnt, true) < 64)continue;
            vector<Point> approx;
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
                    minDistance = dist;
                    bestApprox = approx;
                    bestRawContour = cnt;
                    bestCenter = Point(x, y);
                }
            }
        }
        if (minDistance > 10000) {
            failureCount++;
            if (failureCount > 2) {
                cerr << "Target NOT found!!!" << endl;
                return;
            } else {
                continue;
            }
        }
        cout << "CENTER " << bestCenter << endl;
        currentSrcDistance = minDistance;
        int currentX = bestCenter.x;
        int currentY = bestCenter.y;
        if (currentSrcDistance > SRC_DISTANCE) {
            int deltaX = currentX - img.cols / 2;
            if (deltaX > 0)lastWentDegree--;
            else lastWentDegree++;
            setCameraYaw(lastWentDegree);
//            moveCamera(, 0);
//            moveCamera(currentX - img.cols / 2, 0);
        }
        correctCount++;
        if (SHOW_GUI) {
            imshow("original", img);
            imshow("binary", binaryImg);
        }
    } while (currentSrcDistance > SRC_DISTANCE && correctCount < 50);

    int phyDistance = 2600;

    if (!measureDistanceAndWaitForReply(phyDistance)) {
        printf("measureDistanceAndWaitForReply return false!!!\n");
    }

    const float PHY_SCALE_FACTOR = 0.001150;
    const float FXXK_RECT_CORRECT_FACTOR = 1.09;
    const float FXXK_TRI_CORRECT_FACTOR = 1.100;

    if (!bestApprox.empty()) {
        float pixSize = 0;
        int shape = 0;
        if (bestApprox.size() > 7) {
            //circle
            shape = 1;
            pixSize = (float) (arcLength(bestRawContour, true) / CV_PI);
        } else {
            if (bestApprox.size() == 3 || bestApprox.size() == 5) {
                //tri
                shape = 2;
                pixSize = (float) (FXXK_TRI_CORRECT_FACTOR * arcLength(bestApprox, true) / 3);
            } else {
                //rect
                shape = 3;
                pixSize = (float) (FXXK_RECT_CORRECT_FACTOR * arcLength(bestApprox, true) / 4);
            }
        }

        float phySize = pixSize * PHY_SCALE_FACTOR * (float) phyDistance;
        {
            const char *strshape = "Unknown";
            switch (shape) {
                case 1: {
                    strshape = "Round";
                    break;
                }
                case 2: {
                    strshape = "Triangle";
                    break;
                }
                case 3: {
                    strshape = "Rect";
                    break;
                }
            }
            printf("%s -> s=%.1fmm d=%dmm pixSize=%.2f\n", strshape, phySize, phyDistance, pixSize);
        }
        reportFinalResult(shape, (int) phySize, phyDistance, iColor);
        setLaserPowerOn(true);
        reportEndOfSolution();

    }

    //end
    printf("Leave question 3\n");
}