//
// Created by faliks on 2020/10/10.
//

#ifndef BALLCTLUPPERTEST_QUESTIONONE_H
#define BALLCTLUPPERTEST_QUESTIONONE_H

#include "opencv4/opencv2/opencv.hpp"
#include "opencv4/opencv2/core.hpp"



class QuestionOne {
public:
    enum Color{ RED = 0, GREEN = 1, BLUE = 2};
    QuestionOne() {}

    ~QuestionOne() {}

    void totalSolve(cv::Mat img);

    Color getColor(cv::Mat img);

private:

    Color color;
};


#endif //BALLCTLUPPERTEST_QUESTIONONE_H
