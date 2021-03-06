//
// Created by faliks on 2020/10/10.
//

#ifndef BALLCTLUPPERTEST_QUESTIONONE_H
#define BALLCTLUPPERTEST_QUESTIONONE_H

#include "opencv4/opencv2/opencv.hpp"
#include "opencv4/opencv2/core.hpp"
#include "Blob.h"

static cv::Scalar redLow = cv::Scalar(0, 0, 100);
static cv::Scalar redHigh = cv::Scalar(128, 128, 255);
//static cv::Scalar redLow = cv::Scalar(20, 100, 100) ;

class QuestionOne {
public:
    enum Color {
        RED = 0, GREEN = 1, BLUE = 2, NONE = 3
    };

    QuestionOne() {}

    ~QuestionOne() {}

    void totalSolve(cv::Mat img);

    static void getPureColorImg(std::vector<cv::Mat> &img, cv::Mat &imgOut, int colorIndex);

    static Color getColor(cv::Mat img, std::vector<cv::Mat> &out);

    void equalizeColor(cv::Mat img);

    void getRidOfConor(cv::Mat img);

    void getRidOfOthers(cv::Mat img);

private:
    static int calDistance(cv::Mat img, int i, int j);

    Color color;
};

void findWhiteBackground(std::vector<cv::Mat> &hsvIn, cv::Mat &binOut);

#endif //BALLCTLUPPERTEST_QUESTIONONE_H
