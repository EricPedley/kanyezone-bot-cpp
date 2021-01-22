#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include "screencap.cpp"

int main() {
    cv::Mat screenshot = captureScreenMat(FindWindow(NULL,"Don't let Kanye into his zone: Kanye Zone - Google Chrome"));
    cv::imshow("screen capture",screenshot);
    cv::waitKey(0);
}