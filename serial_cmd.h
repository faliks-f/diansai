#include "common.h"

void moveCamera(int dX, int dY);

bool setCameraYaw(int yaw);

bool setLaserPowerOn(bool on);

bool measureDistanceAndWaitForReply(int &distanceMM);

bool sendPacket5(uchar cmd, uchar arg1, uchar arg2, int maxTry);

bool reportFinalResult(int shape, int size, int distance, int color);

bool reportEndOfSolution();

void sendGeneralReplyOK();