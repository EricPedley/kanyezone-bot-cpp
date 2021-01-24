#include <iostream>
#include <opencv2/opencv.hpp>
#include <windows.h>

#include "screencap.cpp"
#include "movementMath.cpp"
#include "intersectionMath.cpp"

cv::Point findImageCenter(cv::Mat img, cv::Mat targetImage) {//img is the larger image and targetImage is what you want to find within img.
    cv::Size targetSize = targetImage.size();
    int width = targetSize.width;
    int height = targetSize.height; 
    cv::Mat matchOutput;
    cv::matchTemplate(img,targetImage,matchOutput,0);
    double minVal, maxVal;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(matchOutput.reshape(1),&minVal, &maxVal, &minLoc, &maxLoc);//idk why I needed matchOutput.reshape(1)
    cv::Point topLeft = minLoc;
    return topLeft + cv::Point(width/2,height/2);
}

int main() {//you need to start the program when the zone is just created, so it looks exactly like zone.png
    cv::Mat zonePic = cv::imread("zone.png");//87x87 image
    cv::Mat kanyePic = cv::imread("kanye.png");

    HWND chromeWindow = FindWindow(NULL,"Don't let Kanye into his zone: Kanye Zone - Google Chrome");

    cv::Mat screenshot1 = captureScreenMat(chromeWindow);
    cv::cvtColor(screenshot1,screenshot1,cv::COLOR_BGRA2BGR);//drops the alpha channel. it's cvtcolor instead of reshape(3) because reshape doesn't drop the alpha, it just rearranges the matrix. https://docs.opencv.org/4.1.1/d8/d01/group__imgproc__color__conversions.html
   
    cv::Point origin = findImageCenter(screenshot1,zonePic);
    
    cv::Point previousKanyeLocation = cv::Point(465/2,466/2);//for the first frame we assume kanye used to be at the center of the map. no real reason to do this, it just needs to be initialized so I don't have to check that.

    while(1) {
        
        cv::Mat screenshot = captureScreenMat(chromeWindow,origin.x-465/2,origin.y-466/2,465,466);//screenshot is cropped to the game window if you started the program at the right time, when matchtemplate can find zone.png.
        cv::cvtColor(screenshot,screenshot,cv::COLOR_BGRA2BGR);//drops the alpha channel
        
        cv::Point kanyeLocation = findImageCenter(screenshot,kanyePic);
        cv::Point kanyeSize = cv::Point(56,76);

        cv::rectangle(screenshot,cv::Rect(kanyeLocation-kanyeSize/2,kanyeLocation+kanyeSize/2),cv::Scalar(255,255,0));//draw cyan rectangle around kanye

        cv::Point delta = kanyeLocation-previousKanyeLocation;//delta is the change in position
        
        cv::Point intersection = getIntersection(delta,kanyeLocation);
        if(intersection!=cv::Point(-1,-1))
            cv::circle(screenshot,intersection,10,cv::Scalar(0,255,0));//draw green dot at intersection point

        cv::imshow("screen capture",screenshot);


        previousKanyeLocation = kanyeLocation;

        int keyPressed = cv::waitKey(1);//returns ascii code of key pressed
        if(keyPressed==27){//27 is the esc key.
            break;
        }
        //std::cout << "key pressed(char,int)" << (char)keyPressed << "," << keyPressed << std::endl;
    }
    std::cout<<"exiting\n";
}