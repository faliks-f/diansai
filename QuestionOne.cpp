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
    color = getColor(img);
    //显示文字信息
    Vec3b getColor = img.at<Vec3b>(img.cols / 2, img.rows / 2);
    cout << ((color == Color::RED) ? "R" : color == Color::GREEN ? "G" : color == Color::BLUE ? "B" : "None");
    cout << "  (" << (int) getColor[0] << ", " << (int) getColor[1] << ", " << (int) getColor[2] << ")" << endl;
    circle(img, Point(img.cols / 2, img.rows / 2), 2, Scalar(0, 0, 0), -1);
    if (color == Color::NONE) {
        cout << "No Color!" << endl;
        return;
    }

    //blur(img, img, Size(7, 7));
    //二值化
    Mat binaryImg = Mat(img.rows, img.cols, CV_8U);
    if (color == Color::BLUE)
        getPureColorImg(img, binaryImg, 0);
    else if (color == Color::GREEN)
        getPureColorImg(img, binaryImg, 1);
    else
        getPureColorImg(img, binaryImg, 2);
//    medianBlur(binaryImg, binaryImg, 5);

    getRidOfConor(binaryImg);
    getRidOfOthers(binaryImg);

    imshow("original", img);
    imshow("binary", binaryImg);

}

QuestionOne::Color QuestionOne::getColor(cv::Mat img) {
    Mat hsvImg;
    cvtColor(img, hsvImg, COLOR_BGR2HSV);
    vector<Mat> hsvSplit;
    split(hsvImg, hsvSplit);
    int countR = 0, countG = 0, countB = 0;
    for (int i = img.cols / 2 - 3; i < img.cols / 2 + 4; ++i) {
        for (int j = img.rows / 2 - 3; j < img.rows / 2 + 4; ++j) {
            cout << (int)hsvSplit[0].at<uchar>(i, j) << endl;
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
        }
    }
    cout << endl;
    if (countR > countG && countR > countB)
        return Color::RED;
    if (countG > countR && countG > countB)
        return Color::GREEN;
    if (countB > countG && countB > countR)
        return Color::BLUE;
    return Color::NONE;
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
    //cout << blob.size() << endl;
    for (auto x:blob) {
        //printf("( %d, %d, %d, %d)   ", x.left, x.right, x.down, x.up);
        if (x.area() < maxBlob.area()) {
            turnBlack(img, x);
        } else {
            turnBlack(img, maxBlob);
            maxBlob = x;
        }
    }
    //cout << endl;
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

