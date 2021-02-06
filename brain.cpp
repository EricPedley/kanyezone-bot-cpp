#include <opencv2/opencv.hpp>
#include "intersectionMath.cpp"
#include "templMatching.cpp"

#ifndef PI
#include <math.h>
#define PI 3.14159265
#endif

using cv::Mat;
using cv::Point;

class Brain {
    Mat kanyePic, zonePic, paddlePic;
    float scaleFactor;
    int zoneRadius;
    Point previousKanyeLocation,canvasCenter;
    IntersectionCalculator calc;
    public:
        Brain(Mat,Mat,Mat,float);
        bool* getMovementDecision(Mat);//get movement decision based on current game screenshot
    private:
        bool* getMovementDecision(Point,Point);//get movement decision based on calculated paddle and intersection locations
};

cv::Size getAdjustedSize(Mat img, float scaleFactor) {
    cv::Size inputSize = img.size();
    return cv::Size(int(inputSize.width*scaleFactor),int(inputSize.height*scaleFactor));
}

Brain::Brain(Mat kanye, Mat zone, Mat paddle, float scaleFac=1) {//when scalefac isn't 1, the algorithm isn't finding kanye correctly
    scaleFactor=scaleFac;
    cv::resize(kanye,kanyePic,getAdjustedSize(kanye,scaleFactor));
    std::cout<<"kanye dimensions 1:"<<kanyePic.size()<<std::endl;
    cv::resize(zone,zonePic,getAdjustedSize(zone,scaleFactor));
    cv::resize(paddle,paddlePic,getAdjustedSize(paddle,scaleFactor));
    previousKanyeLocation = Point(int(scaleFactor*465/2),int(scaleFactor*466/2));//for the first frame we assume kanye used to be at the center of the map. this means that for the first frame, the paddle will just move toward kanye
    zoneRadius=int(scaleFactor*39);//39 is the radius, in pixels, when the scale is 1:1
    calc=IntersectionCalculator(scaleFac);
    canvasCenter = Point(int(scaleFactor*465/2),int(scaleFactor*466/2));//TODO figure out if you actually need to cast to integer here (probably not, at least in this function)
}

Point rotatedPoint(Point v, double theta) {
    return Point(cos(theta)*v.x-sin(theta)*v.y,sin(theta*v.x+cos(theta)*v.y));
}

bool* Brain::getMovementDecision(Point paddleLocation, Point targetLocation) {
    Point relativePaddleLocation = paddleLocation=canvasCenter;
    Point relativeTargetLocation = targetLocation-canvasCenter;
    
    Point ccwReferencePoint = rotatedPoint(relativeTargetLocation,PI/2);
    double dotProduct = relativePaddleLocation.dot(relativeTargetLocation);
    double dotProduct2 = ccwReferencePoint.dot(relativePaddleLocation);
    bool* movementDecision = new bool[2];//creates pointer to array on the heap.
    movementDecision[1] = dotProduct<=0;//basically, if they are in opposite directions then press space to teleport the paddle
    movementDecision[0] = dotProduct2>0;//if it's in the same direction as the reference point to the counter clockwise direction, then go counter-clockwise
    if(movementDecision[1])//if we press space
        movementDecision[0]!=movementDecision[0];//invert the movement direction
    // std::cout << "local var: "<< movementDecision[0] << "," << movementDecision[1] << std::endl; //local variable is working
    return movementDecision;
}


bool* Brain::getMovementDecision(Mat screenshot) {
    cv::Size scaledSize = cv::Size(int(screenshot.size().width*scaleFactor),int(screenshot.size().height*scaleFactor));
    cv::resize(screenshot,screenshot,scaledSize);
    cv::Vec3b* rowPtr = screenshot.ptr<cv::Vec3b>(466/2);
    for(int radius=zoneRadius;radius<zoneRadius+20;radius++) {//TODO maybe do this every other frame or something
        int c = int(scaleFactor*465/2+radius);//column
        cv::Vec3b bgr = rowPtr[c];
        int b=bgr[0];
        int g = bgr[1];
        int r = bgr[2];
        if(r>g+20 and b>g+20)//if the pixel is purple
            zoneRadius=radius;
        else
            break;
    }
    cv::circle(screenshot,cv::Point(int(scaleFactor*465/2),int(scaleFactor*466/2)),zoneRadius,cv::Scalar(0,255,0),2);

    //scale down for int
    //find radius of the zone
    

    //get location of kanye and his velocity
    Point kanyeLocation = findImageCenter(screenshot,kanyePic);
    Point delta = kanyeLocation-previousKanyeLocation;//delta is the change in position
    
    //draw box around kanye
    Point kanyeSize = Point(int(scaleFactor*56),int(scaleFactor*76));
    cv::rectangle(screenshot,cv::Rect(kanyeLocation-kanyeSize/2,kanyeLocation+kanyeSize/2),cv::Scalar(255,255,0));//draw cyan rectangle around kanye

    
    Point intersection = calc.getIntersection(delta,kanyeLocation,zoneRadius);//find intersection
    bool* decision = nullptr;
    if(intersection!=Point(-1,-1)) {//if there is an intersection
        cv::circle(screenshot,intersection,10,cv::Scalar(0,255,0));//draw green dot at intersection point

        cv::Rect croppedRegion(100,200,87,87);
        Point paddleLocation = findImageCenter(screenshot,paddlePic);//need to crop screenshot so it only sees the center where the paddle can be, not the whole sceen
        cv::circle(screenshot,paddleLocation,20,cv::Scalar(0,0,255));
        decision = getMovementDecision(paddleLocation,intersection);
    }
        
    cv::imshow("screen capture",screenshot);


    previousKanyeLocation = kanyeLocation;

    return decision;
}

