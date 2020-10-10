//
// Created by faliks on 2020/10/10.
//

#include "QuestionOne.h"
#include "iostream"

using namespace cv;
using namespace std;

void QuestionOne::totalSolve(cv::Mat img) {
    color = getColor(img);
    Mat binaryImg;
    cvtColor(img, img, COLOR_RGB2GRAY);

    imshow("a", binaryImg);
    //cout << color << endl;
}

QuestionOne::Color QuestionOne::getColor(cv::Mat img) {
    int cols = img.cols, rows = img.rows;
    int sumR = 0, sumG = 0, sumB = 0;
    for (int i = cols / 2 - 3; i < cols  / 2 + 3; ++i) {
        for (int j = rows / 2 - 3; j < rows / 2 + 3; ++j) {
            Vec3b getColor = img.at<Vec3b>(i, j);
            sumR += getColor[0];
            sumG += getColor[1];
            sumB += getColor[2];
        }
    }
    if (sumR > (sumB + sumG))
        return Color::RED;
    else if (sumG > (sumR + sumB))
        return Color::GREEN;
    else
        return Color::BLUE;
}

int QuestionOne::getThreshold(cv::Mat img) {
    int cols = img.cols, rows = img.rows;
    int sum = 0;
    for (int i = cols / 2 - 3; i < cols / 2 + 3; ++i) {
        for (int j = rows / 2 - 3; j < rows / 2 + 3; ++j) {
            Vec3b getGray = img.at<Vec3b>(i, j);
            sum += getGray[0];
        }
    }
    return sum / 36;
}
