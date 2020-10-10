//
// Created by faliks on 2020/10/10.
//

#include "QuestionOne.h"
#include "iostream"

using namespace cv;
using namespace std;

void QuestionOne::totalSolve(cv::Mat img) {
    color = getColor(img);
    Mat binaryImg = Mat(img.rows, img.cols, CV_8U);
    //cvtColor(img, img, COLOR_RGB2GRAY);
    blur(img, img, Size(7, 7));
    Vec3b getColor = img.at<Vec3b>(img.cols / 2, img.rows / 2);
    cout << "(" << (int) getColor[0] << ", " << (int) getColor[1] << ", " << (int) getColor[2] << ")" << endl;
    circle(img, Point(img.cols / 2, img.rows / 2), 2, Scalar(0, 0, 0), -1);
    imshow("a", img);
    getPureColorImg(img, binaryImg, 1);
    imshow("b", binaryImg);
    //cout << color << endl;
}

QuestionOne::Color QuestionOne::getColor(cv::Mat img) {
    int cols = img.cols, rows = img.rows;
    int sumR = 0, sumG = 0, sumB = 0;
    for (int i = cols / 2 - 3; i < cols / 2 + 3; ++i) {
        for (int j = rows / 2 - 3; j < rows / 2 + 3; ++j) {
            Vec3b getColor = img.at<Vec3b>(i, j);
            sumB += getColor[0];
            sumG += getColor[1];
            sumR += getColor[2];
        }
    }
    if (sumR > (sumB + sumG))
        return Color::RED;
    else if (sumG > (sumR + sumB))
        return Color::GREEN;
    else
        return Color::BLUE;
}

void QuestionOne::getPureColorImg(cv::Mat &imgIn, cv::Mat &imgOut, int colorIndex) {
    const float THRES_FACTOR = 0.92f;
    const int MAX_EMMM = 252;
    const int thres_over_val = 20;
    for (int i = 0; i < imgIn.cols; ++i) {
        for (int j = 0; j < imgIn.rows; ++j) {
            uchar *pIn = imgIn.data + imgIn.step.p[0] * j + i * 3;
            uchar *pOut = imgOut.data + imgOut.step.p[0] * j + i;
            float thres_factor;
            if (pIn[colorIndex] > MAX_EMMM) {
                thres_factor = 0.95f;
            } else {
                thres_factor = THRES_FACTOR;
            }
            switch (colorIndex) {
                case 0: {//B
                    int raw = pIn[colorIndex];
                    int th = (int) (((float) raw) * thres_factor);
                    *pOut = ((th > pIn[1] && th > pIn[2]) ||
                             (raw - pIn[1] > thres_over_val && raw - pIn[2] > thres_over_val)) ? 255 : 0;
                    break;
                }
                case 1: {//B
                    int raw = pIn[colorIndex];
                    int th = (int) (((float) raw) * thres_factor);
                    *pOut = ((th > pIn[0] && th > pIn[2]) ||
                             (raw - pIn[0] > thres_over_val && raw - pIn[2] > thres_over_val)) ? 255 : 0;
                    break;
                }
                case 2: {//B
                    int raw = pIn[colorIndex];
                    int th = (int) (((float) raw) * thres_factor);
                    *pOut = ((th > pIn[1] && th > pIn[0]) ||
                             (raw - pIn[1] > thres_over_val && raw - pIn[0] > thres_over_val)) ? 255 : 0;
                    break;
                }

            }
        }
    }
}

