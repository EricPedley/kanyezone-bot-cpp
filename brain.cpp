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

cv::Size getAdjustedSize(Mat img, float scaleFactor) {
    cv::Size inputSize = img.size();
    return cv::Size(int(inputSize.width*scaleFactor),int(inputSize.height*scaleFactor));
}

Brain::Brain(Mat kanye, Mat zone, Mat paddle, float scaleFac=1) {//when scalefac isn't 1, the algorithm isn't finding kanye correctly
    scaleFactor=scaleFac;
    cv::resize(kanye,kanyePic,getAdjustedSize(kanye,scaleFactor));
    std::cout<<"kanye dimensions 1:"<<kanyePic.size()<<std::endl;
    cv::resize(zone,zonePic,getAdjustedSize(zone,scaleFactor));//zone is 87x87 default
    cv::resize(paddle,paddlePic,getAdjustedSize(paddle,scaleFactor));//42x42 default
    previousKanyeLocation = Point(int(scaleFactor*465/2),int(scaleFactor*466/2));//for the first frame we assume kanye used to be at the center of the map. this means that for the first frame, the paddle will just move toward kanye
    zoneRadius=int(scaleFactor*39);//39 is the radius, in pixels, when the scale is 1:1
    calc=IntersectionCalculator(scaleFac);
    canvasCenter = Point(int(scaleFactor*465/2),int(scaleFactor*466/2));//TODO figure out if you actually need to cast to integer here (probably not, at least in this function)
    latestIntersection=Point(-1,-1);
    previouslyWarped=false;
    warpCooldown=0;
    warpPreventionCounter=0;
}

Point rotatedPoint(Point v, double theta) {
    return Point(cos(theta)*v.x-sin(theta)*v.y,sin(theta)*v.x+cos(theta)*v.y);
}

double distSq(Point a, Point b) {
    return pow(a.x-b.x,2)+pow(a.y-b.y,2);
}

bool* Brain::getMovementDecision(Point paddleLocation, Point targetLocation) {
    Point relativePaddleLocation = paddleLocation-canvasCenter;
    Point relativeTargetLocation = targetLocation-canvasCenter;

    Point ccwReferencePoint = rotatedPoint(relativeTargetLocation,PI/2);
    double dotProduct = relativePaddleLocation.dot(relativeTargetLocation);
    double dotProduct2 = ccwReferencePoint.dot(relativePaddleLocation);
    bool* movementDecision = new bool[2];//creates pointer to array on the heap.
    movementDecision[1] = dotProduct<=0;//basically, if they are in opposite directions then press space to teleport the paddle
    movementDecision[0] = dotProduct2>0;//if it's in the same direction as the reference point to the counter clockwise direction, then go counter-clockwise
    if(movementDecision[1])//if we press space
        movementDecision[0]=!movementDecision[0];//invert the movement direction
    // std::cout << "local var: "<< movementDecision[0] << "," << movementDecision[1] << std::endl; //local variable is working
    return movementDecision;
}


bool* Brain::getMovementDecision(Mat screenshot) {
    cv::Size scaledSize = cv::Size(int(screenshot.size().width*scaleFactor),int(screenshot.size().height*scaleFactor));
    cv::resize(screenshot,screenshot,scaledSize);

    cv::Vec3b* rowPtr = screenshot.ptr<cv::Vec3b>(int(233*scaleFactor));
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

    //get location of kanye and his velocity
    Point kanyeLocation = findImageCenter(screenshot,kanyePic);
    Point delta = kanyeLocation-previousKanyeLocation;//delta is the change in position
    if(delta==Point(0,0))//duplicate images, skip procesing and key input
        return nullptr;
    
    //draw box around kanye
    Point kanyeSize = Point(int(scaleFactor*56),int(scaleFactor*76));

    Point intersection = calc.getIntersection(delta,kanyeLocation,zoneRadius,2);//find intersection
    bool* decision = nullptr;
    //bool wacky = false;
    if(intersection!=Point(-1,-1)) {//if there is an intersection
        latestIntersection=intersection;
        // if(intersection.x<1) {
        //     latestIntersection=-1*intersection;
        //     std::cout<< "wacky intercept" <<std::endl;
        //     wacky = true;
        // }
    }
    float cropDiameter = 2*(42+30+zoneRadius/scaleFactor);
    Point cropStart = Point(int((465-cropDiameter)*scaleFactor/2),int((466-cropDiameter)*scaleFactor/2));
    Point bottomRight = cropStart+Point(int(cropDiameter*scaleFactor),int(cropDiameter*scaleFactor));
    cv::Rect croppedRegion(cropStart,bottomRight);//140(above 87+42)px square region centered at the game center
    Mat croppedScreenshot = screenshot(croppedRegion);
    Point paddleLocation = cropStart+findImageCenter(croppedScreenshot,paddlePic,zoneRadius);
    decision = getMovementDecision(paddleLocation,latestIntersection);
    // if(warpCooldown>0) {//if on cooldown
    //     decision[1]=false;
    //     warpCooldown++;//increment cooldown
    //     if(warpCooldown>10) {//if done, take it off cooldown
    //         warpCooldown=0;
    //     }
    // }
    // if(decision[1]&&warpCooldown==0) {//if not on cooldown and we warp, put it on cooldown. 
    //     warpCooldown=1;
    // }
    double paddleDistance = distSq(paddleLocation,previousPaddlePosition);
    bool originalWarpDecision=decision[1];
    //we only prevent a warp a certain amount of times in a row, then let it go.
    if(previouslyWarped && warpPreventionCounter<3 && paddleDistance<pow(2*zoneRadius,2)) {//if we supposedly warped but the paddle hasn't moved yet
        std::cout<<"preventing double warp"<<std::endl;
        decision[1]=false;
        warpPreventionCounter++;
    }
    previouslyWarped=originalWarpDecision;
    previousPaddlePosition = paddleLocation;
    if(decision[1]) {
        if(warpPreventionCounter>=3)
            std::cout<<"overrided warp prevention"<<std::endl;
        warpPreventionCounter=0;
        std::cout<<"paddle distance: "<<paddleDistance<<std::endl;
        //cv::circle(screenshot,cv::Point(20,20),8,cv::Scalar(255,255,255),-1);
    }
    //cv::circle(screenshot,cv::Point(int(scaleFactor*465/2),int(scaleFactor*466/2)),zoneRadius,cv::Scalar(0,255,0));
    cv::rectangle(screenshot,croppedRegion,cv::Scalar(255,255,255));//white rectangle around crop region for paddle
    cv::rectangle(screenshot,cv::Rect(kanyeLocation-kanyeSize/2,kanyeLocation+kanyeSize/2),cv::Scalar(255,255,0));//draw cyan rectangle around kanye
    cv::circle(screenshot,paddleLocation,int(20*scaleFactor),cv::Scalar(0,0,255));
    cv::circle(screenshot,latestIntersection,int(20*scaleFactor),cv::Scalar(0,255,0),cv::FILLED);//draw dot at intersection point
    cv::resize(screenshot,screenshot,cv::Size(465,466));
    cv::imshow("screen capture",screenshot);


    previousKanyeLocation = kanyeLocation;

    return decision;
}

