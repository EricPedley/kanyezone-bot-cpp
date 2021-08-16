#pragma once
#include <opencv2/opencv.hpp>

using cv::Point;

class IntersectionCalculator {
    float scaleFactor;
    public:
        IntersectionCalculator(float);
        IntersectionCalculator();
        Point getIntersection(Point,Point,int,int);
    private:
        Point getLineCircleIntersection(Point,Point,int);
};