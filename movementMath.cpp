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

bool* getMovementDecision(Point paddleLocation, Point targetLocation) {
    Point canvasCenter = Point(465/2,466/2);
    Point relativePaddleLocation = paddleLocation=canvasCenter;
    Point relativeTargetLocation = targetLocation-canvasCenter;

    Point ccwReferencePoint = rotatedPoint(relativeTargetLocation,PI/2);
    double dotProduct = relativePaddleLocation.dot(relativeTargetLocation);
    double dotProduct2 = ccwReferencePoint.dot(relativePaddleLocation);
    bool* movementDecision = new bool[2];//since we're returning a local variable, it gets destroyed and replaced with an empty variable, which is why this code doesn't work.
    //the asteric thing and new operator mean we're allocating memory, so I have to free the memory after we use it in the main method
    //still doesn't work but returns something different. it returned [0,1] before and now it constantly returns [1,1]. well, idk if it returns those but that's what prints out 
    //in the main method. try adding a cout in this method to compare what we're returning to what the main loop is receiving.
    movementDecision[1] = dotProduct<=0;//basically, if they are in opposite directions then press space to teleport the paddle
    movementDecision[0] = dotProduct2>0;//if it's in the same direction as the reference point to the counter clockwise direction, then go counter-clockwise
    if(movementDecision[1])//if we press space
        movementDecision[0]!=movementDecision[0];//invert the movement direction
    // std::cout << "local var: "<< movementDecision[0] << "," << movementDecision[1] << std::endl; //local variable is working
    return movementDecision;
}
