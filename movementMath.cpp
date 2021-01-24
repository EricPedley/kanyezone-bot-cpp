#ifndef cv
#include <opencv2/opencv.hpp>
#endif
#ifndef PI
#include <math.h>
#define PI 3.14159265
#endif

using cv::Point;
using std::vector;

Point rotatedPoint(Point v, double theta) {
    return Point(cos(theta)*v.x-sin(theta)*v.y,sin(theta*v.x+cos(theta)*v.y));
}

vector<bool> getMovementDecision(Point paddleLocation, Point targetLocation) {
    Point canvasCenter = Point(465/2,466/2);
    Point relativePaddleLocation = paddleLocation=canvasCenter;
    Point relativeTargetLocation = targetLocation-canvasCenter;

    Point ccwReferencePoint = rotatedPoint(relativeTargetLocation,PI/2);
    double dotProduct = relativePaddleLocation.dot(relativeTargetLocation);
    double dotProduct2 = ccwReferencePoint.dot(relativePaddleLocation);

    if(dotProduct>0) {
        if(dotProduct2>0)
            return vector<bool>(true,false);
        else
            return vector<bool>(false,false);
    } else {
        if(dotProduct2>0)
            return vector<bool>(false,true);
        else
            return vector<bool>(true,true);

    }
}
