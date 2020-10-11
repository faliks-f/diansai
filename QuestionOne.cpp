//
// Created by faliks on 2020/10/10.
//

#include "QuestionOne.h"
#include "iostream"
#include "Blob.h"
#include "algorithm"

using namespace cv;
using namespace std;

void QuestionOne::totalSolve(cv::Mat img) {
    GaussianBlur(img, img, Size(3, 3), 1, 1);
    vector<Mat> hsvImg;
    color = getColor(img, hsvImg);
    //显示文字信息
    Vec3b getColor = img.at<Vec3b>(img.cols / 2, img.rows / 2);
    //cout << ((color == Color::RED) ? "R" : color == Color::GREEN ? "G" : color == Color::BLUE ? "B" : "None");
    //cout << "  (" << (int) getColor[0] << ", " << (int) getColor[1] << ", " << (int) getColor[2] << ")" << endl;
    circle(img, Point(img.cols / 2, img.rows / 2), 2, Scalar(0, 0, 0), -1);
    if (color == Color::NONE) {
        cout << "No Color!" << endl;
        return;
    }

    //blur(img, img, Size(7, 7));
    //二值化
    Mat binaryImg = Mat(img.rows, img.cols, CV_8U);
    if (color == Color::RED)
        getPureColorImg(hsvImg, binaryImg, 0);
    else if (color == Color::GREEN)
        getPureColorImg(hsvImg, binaryImg, 1);
    else
        getPureColorImg(hsvImg, binaryImg, 2);
//    medianBlur(binaryImg, binaryImg, 5);


//    erode(binaryImg, binaryImg, getStructuringElement(MORPH_RECT, Size(3, 3)));
//    dilate(binaryImg, binaryImg, getStructuringElement(MORPH_RECT, Size(5, 5)));
//    erode(binaryImg, binaryImg, getStructuringElement(MORPH_RECT, Size(2, 2)));

    vector<vector<Point>> contours, approxSet;
    vector<Vec4i> hierarchy;
    findContours(binaryImg, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    drawContours(img, contours, -1, Scalar(0, 0, 255), 1, 8);
    for (const vector<Point> &cnt:contours) {
        float epsilon = 0.03f * arcLength(cnt, true);
        vector<Point> approx;
        approxPolyDP(cnt, approx, epsilon, true);
        approxSet.push_back(approx);
    }
    drawContours(img, approxSet, -1, Scalar(0, 255, 0), 1, 8);

    //getRidOfConor(binaryImg);
    //getRidOfOthers(binaryImg);

    imshow("original", img);
    imshow("binary", binaryImg);

}

QuestionOne::Color QuestionOne::getColor(cv::Mat img, vector<Mat> &out) {
    Mat hsvImg;
    cvtColor(img, hsvImg, COLOR_BGR2HSV);
    vector<Mat> hsvSplit;
    split(hsvImg, hsvSplit);
    out = hsvSplit;
    int countR = 0, countG = 0, countB = 0;
    for (int i = img.cols / 2 - 3; i < img.cols / 2 + 4; ++i) {
        for (int j = img.rows / 2 - 3; j < img.rows / 2 + 4; ++j) {
            switch (calDistance(hsvSplit[0], i, j)) {
                case 1:
                    ++countR;
                    cout << "R" << " ";
                    break;
                case 2:
                    ++countG;
                    cout << "G" << " ";
                    break;
                case 3:
                    ++countB;
                    cout << "B" << " ";
                    break;
            }
            cout << (int) hsvSplit[0].at<uchar>(i, j) << endl;
        }
    }
    if (countR > countG && countR > countB)
        return Color::RED;
    if (countG > countR && countG > countB)
        return Color::GREEN;
    if (countB > countG && countB > countR)
        return Color::BLUE;
    return Color::NONE;
}

void QuestionOne::getPureColorImg(vector<Mat> &imgIn, cv::Mat &imgOut, int colorIndex) {
    int colorLow[3] = {156, 50, 98};
    int colorHigh[3] = {190, 77, 124};
    int SLow[3] = {83, 69, 110};
    int SHigh[3] = {255, 255, 255};
    int VLow[3] = {105, 115, 51};
    int VHigh[3] = {255, 255, 255};
    for (int i = 0; i < imgIn[0].cols; ++i) {
        for (int j = 0; j < imgIn[0].rows; ++j) {
            uchar *pH = imgIn[0].data + imgIn[0].step[0] * j + i;
            uchar *pS = imgIn[1].data + imgIn[1].step[0] * j + i;
            uchar *pV = imgIn[2].data + imgIn[2].step[0] * j + i;
            uchar *pOut = imgOut.data + imgOut.step[0] * j + i;
            if (*pH <= 10)
                *pH += 180;
            if (colorLow[colorIndex] <= *pH && *pH <= colorHigh[colorIndex])
                if (SLow[colorIndex] <= *pS && *pS <= SHigh[colorIndex] && VLow[colorIndex] <= *pV &&
                    *pV <= VHigh[colorIndex])
                    *pOut = 255;
                else
                    *pOut = 0;
            else
                *pOut = 0;
        }
    }

}

void QuestionOne::getRidOfConor(cv::Mat img) {
    int row = img.rows, col = img.cols;
    for (int j = 0; j < row; ++j) {
        if (j < row / 4 || j > row * 3 / 4) {
            for (int i = 0; i < col; ++i) {
                *(img.data + img.step[0] * j + i) = 0;
            }
        } else {
            for (int i = 0; i < col; ++i) {
                if (i < col / 4 || i > col * 3 / 4)
                    *(img.data + img.step[0] * j + i) = 0;
            }
        }
    }
}

void QuestionOne::getRidOfOthers(cv::Mat img) {
    vector<Blob> blob = findBlob(img);
    Blob maxBlob(Point(0, 0));
    for (auto x:blob) {
        if (x.area() < maxBlob.area()) {
            turnBlack(img, x);
        } else {
            turnBlack(img, maxBlob);
            maxBlob = x;
        }
    }
}

int QuestionOne::calDistance(cv::Mat img, int i, int j) {
    int data = *(img.data + j * img.step[0] + i);
    int disR = min(min(abs(data - 156), abs(data - 180)), min(abs(data - 0), abs(data - 10)));
    int disG = min(abs(data - 33), abs(data - 77));
    int disB = min(abs(data - 100), abs(data - 124));
    if (disR < disG && disR < disB)
        return 1;
    if (disG < disR && disG < disB)
        return 2;
    if (disB < disR && disB < disG)
        return 3;
    return 0;
}

