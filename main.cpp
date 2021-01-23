#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include "screencap.cpp"

using std::cout;

cv::Point findImageCenter(cv::Mat img, cv::Mat targetImage) {//img is the larger image and targetImage is what you want to find within img.
    cv::Size targetSize = targetImage.size();
    int width = targetSize.width;
    int height = targetSize.height; 
    cv::Mat matchOutput;
    cv::matchTemplate(img,targetImage,matchOutput,0);
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(matchOutput.reshape(1),&minVal, &maxVal, &minLoc, &maxLoc);
    cv::Point topLeft = minLoc;
    return topLeft + cv::Point(width/2,height/2);
}

int main() {//you need to start the program when the zone is just created, so it looks exactly like zone.png
    HWND chromeWindow = FindWindow(NULL,"Don't let Kanye into his zone: Kanye Zone - Google Chrome");
    cv::Mat zonepic = cv::imread("zone.png");//87x87 image
    //cv::waitKey(0);
    cv::Mat screenshot1 = captureScreenMat(chromeWindow);
    cv::cvtColor(screenshot1,screenshot1,cv::COLOR_BGRA2BGR);//drops the alpha channel
    cv::Point origin = findImageCenter(screenshot1,zonepic);
    while(1) {
        cv::Mat screenshot = captureScreenMat(chromeWindow,origin.x-465/2,origin.y-466/2,465,466);
        cv::cvtColor(screenshot,screenshot,cv::COLOR_BGRA2BGR);//drops the alpha channel
        cv::imshow("screen capture",screenshot);
        int keyPressed = cv::waitKey(1);//returns ascii code of key pressed
        if(keyPressed==27){//27 is the esc key.
            break;
        }
        //std::cout << "key pressed(char,int)" << (char)keyPressed << "," << keyPressed << std::endl;
    }
}