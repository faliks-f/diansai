//
// Created by faliks on 2020/10/11.
//

#ifndef ELECTRONICCOMP_BLOB_H
#define ELECTRONICCOMP_BLOB_H

#include "opencv4/opencv2/opencv.hpp"
#include "vector"

class Blob {
public:
    Blob(cv::Point point);
    int area() const {return (right - left) * (up - down);}
public:
    int left;
    int right;
    int up;
    int down;
};

std::vector<Blob> findBlob(cv::Mat img);
void turnBlack(cv::Mat img, Blob blob);

#endif //ELECTRONICCOMP_BLOB_H
