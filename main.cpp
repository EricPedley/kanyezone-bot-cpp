#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>
#include <ctime>

#include "screencap.cpp"
#include "brain.cpp"
#include "intersectionMath.cpp"
#include "keyInput.cpp"
#ifndef findImageCenter
#include "templMatching.cpp"
#endif

int getTime() {//returns current time since unix epoch in nanoseconds
    return clock();// this is for include <chrono> std::chrono::system_clock::now().time_since_epoch().count();
}


int main() {//you need to start the program when the zone is just created, so it looks exactly like zone.png
    std::cout<<"starting program"<<std::endl;

    //Initialize image matrices and chrome window handle
    cv::Mat zonePic = cv::imread("zone.png");//87x87 image
    cv::Mat kanyePic = cv::imread("kanye.png");
    cv::Mat paddlePic = cv::imread("paddle.png");
    HWND chromeWindow = FindWindow(NULL,"Don't let Kanye into his zone: Kanye Zone - Google Chrome");
    //figure out where the game is on the screen and save the center location
    cv::Mat screenshot1 = ScreenCapturer(chromeWindow).getScreenCap();
    cv::cvtColor(screenshot1,screenshot1,cv::COLOR_BGRA2BGR);//drops the alpha channel. it's cvtcolor instead of reshape(3) because reshape doesn't drop the alpha, it just rearranges the matrix. https://docs.opencv.org/4.1.1/d8/d01/group__imgproc__color__conversions.html
    cv::Point origin = findImageCenter(screenshot1,zonePic);
    ScreenCapturer cap = ScreenCapturer(chromeWindow,origin.x-465/2,origin.y-466/2,465,466);


    //set up scale factor
    float scaleFactor=0.2;//TODO mess with scale factor to get best speed with acceptable accuracy loss
    Brain brain = Brain(kanyePic,zonePic,paddlePic,scaleFactor);

    int prev = getTime();

    while(1) {
        // int now=getTime();
        // std::cout<<float(now-prev)/CLOCKS_PER_SEC<<std::endl;
        // prev=now;
        int t1 = clock();
        cv::Mat screenshot = cap.getScreenCap();//screenshot is cropped to the game window if you started the program at the right time, when matchtemplate can find zone.png.
        int t2 = clock();
        std::cout<<float(t2-t1)/CLOCKS_PER_SEC<<std::endl;
        cv::cvtColor(screenshot,screenshot,cv::COLOR_BGRA2BGR);//drops the alpha channel
        bool *movementDecision = brain.getMovementDecision(screenshot);
        if(movementDecision!=nullptr) {
            if(movementDecision[1]) {
                tapKey(' ');
            }
            if(movementDecision[0]) {
                releaseKey('d');
                pressKey('a');
            } else {
                releaseKey('a');
                pressKey('d');
            }
            delete[] movementDecision;
        }

        int keyPressed = cv::waitKey(1);//returns ascii code of key pressed
        if(keyPressed==27){//27 is the esc key.
            break;
        }
    }
    std::cout<<"exiting\n";
}