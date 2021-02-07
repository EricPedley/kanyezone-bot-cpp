#pragma once
#include <opencv2/opencv.hpp>

using cv::Point;
using cv::Mat;

Point findImageCenter(Mat img, Mat templ) {//not a method of Brain because it doesn't need the scale factor or any of the images
    cv::Size targetSize = templ.size();
    int width = targetSize.width;
    int height = targetSize.height; 
    //try this to speed this up: also, the python version of matchTemplate is faster for some reason by 0.1 to 0.2 seconds, which is significant because the total processing time is 0.08 seconds max.
    //https://web.archive.org/web/20160228151016/http://opencv-code.com/tutorials/fast-template-matching-with-image-pyramid
    Mat matchOutput;
    cv::matchTemplate(img,templ,matchOutput,0);
    double minVal, maxVal;
    Point minLoc, maxLoc;
    cv::minMaxLoc(matchOutput,&minVal, &maxVal, &minLoc, &maxLoc);//earlier I was using matchOutput.reshape(1) but I replaced it with matchOutput and it still works
    Point topLeft = minLoc;
    return topLeft + Point(width/2,height/2);
}

//overriden function to find exclude a certain region from the matching
Point findImageCenter(Mat img, Mat templ, int exclusionRadius) {
    cv::Size targetSize = templ.size();
    int width = targetSize.width;
    int height = targetSize.height; 
    //instead of masking (couldn't figure it out), we just draw a yellow circle over the zone, which is the opposite of blue
    //POSSIBLE ERROR? this has the side effect of making the zone yellow in the screenshot for everything after this function
    cv::circle(img,cv::Point(img.cols/2,img.rows/2),exclusionRadius,cv::Scalar(0,255,255),cv::FILLED);//this is where we exclude the zone
    //try this to speed this up: also, the python version of matchTemplate is faster for some reason by 0.1 to 0.2 seconds, which is significant because the total processing time is 0.08 seconds max.
    //https://web.archive.org/web/20160228151016/http://opencv-code.com/tutorials/fast-template-matching-with-image-pyramid
    //Mat mask = Mat::zeros(img.size(),CV_8U);
    Mat matchOutput;
    cv::matchTemplate(img,templ,matchOutput,0);
   
    double minVal, maxVal;
    Point minLoc, maxLoc;
    cv::imshow("match output after masking",matchOutput);
    cv::minMaxLoc(matchOutput,&minVal, &maxVal, &minLoc, &maxLoc);//earlier I was using matchOutput.reshape(1) but I replaced it with matchOutput and it still works
    // std::cout<<"min: "<<minVal<<", max: "<<maxVal<<", data type: "<<matchOutput.type()<<std::endl;
    Point topLeft = minLoc;
    return topLeft + Point(width/2,height/2);
}