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
    for (int i = -30; i < 30; i += 6) {

        bool turn =setCameraYaw(i);

        msleep(700);

    }


    //end
    printf("Leave question 3\n");
}