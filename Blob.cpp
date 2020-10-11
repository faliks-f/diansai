//
// Created by faliks on 2020/10/11.
//

#include "Blob.h"
#include "queue"
#include "algorithm"

using namespace std;
using namespace cv;

Blob::Blob(cv::Point point) {
    left = right = point.x;
    up = down = point.y;
}

std::vector<Blob> findBlob(cv::Mat img) {
    const int row = img.rows, col = img.cols;
    //0代表没用过，1代表使用过
    vector<vector<int>> record(col, vector<int>(row, 0));
    vector<Blob> result;
    for (int i = 0; i < col; ++i) {
        for (int j = 0; j < row; ++j) {
            if (*(img.data + img.step[0] * j + i) == 0)
                continue;
            if (record[i][j])
                continue;
            queue<Point> myQueue;
            Blob blob(Point(i, j));
            myQueue.push(Point(i, j));
            while (!myQueue.empty()) {
                int m = myQueue.front().x;
                int n = myQueue.front().y;
                if (n + 1 < row && *(img.data + img.step[0] * m + n + 1) == 255) {
                    if (!record[m][n + 1]) {
                        record[m][n + 1] = 1;
                        blob.up = max(blob.up, n + 1);
                        myQueue.push(Point(m, n + 1));
                    }
                }
                if (n > 0 && *(img.data + img.step[0] * m + n - 1) == 255) {
                    if (!record[m][n - 1]) {
                        record[m][n - 1] = 1;
                        blob.down = min(blob.down, n - 1);
                        myQueue.push(Point(m, n - 1));
                    }
                }
                if (m + 1 < col && *(img.data + img.step[0] * (m + 1) + n) == 255) {
                    if (!record[m + 1][n]) {
                        record[m + 1][n] = 1;
                        blob.right = max(blob.right, m + 1);
                        myQueue.push(Point(m + 1, n));
                    }
                }
                if (m > 0 && *(img.data + img.step[0] * (m - 1) + n) == 255) {
                    if (!record[m - 1][n]) {
                        record[m - 1][n] = 1;
                        blob.left = min(blob.left, m - 1);
                        myQueue.push(Point(m - 1, n));
                    }
                }
                myQueue.pop();
            }
            if (blob.area())
                result.push_back(blob);
        }
    }
    return result;
}

void turnBlack(cv::Mat img, Blob blob) {
    for (int i = blob.left; i < blob.right; ++i) {
        for (int j = blob.down; j < blob.up; ++j) {
            *(img.data + img.step[0] * j + i) = 0;
        }
    }
}
