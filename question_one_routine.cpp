#include "question_one_routine.h"
#include "stdio.h"

#include "flowctl.h"
#include "QuestionOne.h"
#include "iostream"
#include "Blob.h"
#include "algorithm"
#include "common.h"

using namespace cv;
using namespace std;

void executeQuestionOne() {
    printf("Enter question 1\n");
    Mat img;
    videoIn >> img;
    if (img.empty()) {
        std::cerr << "FATAL: read frame failed!" << endl;
        return;
    }
    QuestionOne questionOne;
    questionOne.totalSolve(img);
    printf("Leave question 1\n");
}