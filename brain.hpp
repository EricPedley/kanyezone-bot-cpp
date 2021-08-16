#pragma once
#include <opencv2/opencv.hpp>
#include "intersectionCalculator.hpp"


using cv::Mat;
using cv::Point;
class Brain {
    Mat kanyePic, zonePic, paddlePic;
    float scaleFactor;
    int zoneRadius,warpPreventionCounter,warpCooldown;
    Point previousKanyeLocation,canvasCenter,latestIntersection,previousPaddlePosition;
    IntersectionCalculator calc;
    bool previouslyWarped;
    public:
        Brain(Mat,Mat,Mat,float);
        bool* getMovementDecision(Mat);//get movement decision based on current game screenshot
    private:
        bool* getMovementDecision(Point,Point);//get movement decision based on calculated paddle and intersection locations
};