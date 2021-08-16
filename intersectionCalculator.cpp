#include <opencv2/opencv.hpp>

using cv::Point;

#include "intersectionCalculator.hpp"

IntersectionCalculator::IntersectionCalculator(float scaleFac) {
    std::cout<<"scale factor is: "<<scaleFac<<std::endl;
    scaleFactor=scaleFac;
}

IntersectionCalculator::IntersectionCalculator() {
    IntersectionCalculator(1);
}

Point IntersectionCalculator::getLineCircleIntersection(Point delta, Point center, int radius=39) {//delta is a vector not a point but I'm using the point object to store its x and y components
    
    int c = scaleFactor*465/2;//x coordinate of center of circle
    int d = scaleFactor*466/2;// y coordinate of center of circle
    int e = radius;//radius of circle
    if(delta.x==0) {
        int cSquared = pow(center.x-c,2);//this represents the (x-c)^2 term 
        int eSquared = e*e;
        if(eSquared>cSquared) {
            int yIntercept;
            if(delta.y>0) {
                return Point(center.x,d-sqrt(eSquared-cSquared));
            } else {
                return Point(center.x,d+sqrt(eSquared-cSquared));
            }
        } else {
            return Point(-1,-1);
        }

    }
    
    //all this math is basically finding a solution to the system of equations y=ax+b and e^2 = (x-c)^2 + (y-d)^2
    double a = delta.y/delta.x;
    double b = center.y-a*center.x;
    double negb = -2*(a*(b-d)-c);
    double bsq = pow(negb,2);
    double neg4ac = -4*(a*a+1)*(b*b-2*b*d+d*d+c*c-e*e);
    if(bsq+neg4ac>0) {//two solutions
        double x1 = (negb+sqrt(bsq+neg4ac))/(2*(a*a+1));//x coord on right side
        double x2 = (negb-sqrt(bsq+neg4ac))/(2*(a*a+1));//x coord on left size
        if(a<=1) {//if the slope is 1 or less
            double xdist1 = abs(x1-center.x);
            double xdist2 = abs(x2-center.x);
            if(xdist1<=xdist2)
                return Point(x1,x1*a+b);
            else
                return Point(x2,x2*a+b);
        } else {//if the slope is greater than 1
            double y1 = a*x1+b;
            double y2 = a*x2+b;
            double ydist1 = abs(y1-center.y);
            double ydist2 = abs(y2-center.y);
            if(ydist1<=ydist2)
                return Point(x1,y1);
            else
                return Point(x2,y2);
        }
    } else if( (bsq+neg4ac) == 0) {//one solution
        double x = negb/(2*(a*a)+1);
        return Point(x,x*a+b);
    } else {//no solution
        return Point(-1,-1);//tried returning null but it was giving me an error. This point should be impossible because it's outside the bounds of the game, so i can use it as a placeholder for no intersection.
    }
}

Point IntersectionCalculator::getIntersection(Point delta, Point center, int radius=39, int numBounces=3) {//center is the center of kanye's head
    for(int i=0;i<numBounces;i++) {
        Point intersection = getLineCircleIntersection(delta,center,radius);
        if(intersection!=Point(-1,-1)&&((intersection.x>center.x) == (delta.x>0))) {//this means there's an intersection. I programmed getLineCircleIntersection to return (-1,-1) if there was no intersection.
           return intersection;
        } else {
            if(delta.x==0) {//if travelling vertically, the next point is the top or bottom depending on delta.y
                if(delta.y<0) {
                    center = Point(center.x,scaleFactor*466-scaleFactor*76/2);
                    delta.y=-delta.y;
                } else {
                    center = Point(center.x,scaleFactor*76/2);
                    delta.y=-delta.y;
                }
                continue;
            }
            double slope = delta.y/delta.x;
            int yIntercept;
            if(delta.x>0)//both of these calculate yIntercept using the equation of Kanye's motion in point-slope form rearranged to solve for the y intercept.
                yIntercept=slope*(scaleFactor*465-scaleFactor*56/2-center.x)+center.y;//scaleFactor*56 is the width in pixels of Kanye's head.
            else
                yIntercept = slope*(scaleFactor*56/2-center.x)+center.y;
            if(yIntercept>scaleFactor*466-scaleFactor*76/2) {//scaleFactor*76 is the height of kanye's head. this condition checks if we are gonna intersect the bottom of the map.
                delta.y=-delta.y;//reflect velocity over x axis
                if(slope!=0)//don't know why slope ever hits zero.
                    center = Point(center.x+(scaleFactor*466-scaleFactor*76/2-center.y)/slope,scaleFactor*466-scaleFactor*76/2);
                else
                    std::cout << "I don't know why, but the slope of the velocity of Kanye's head is zero." <<std::endl;
            } else if(yIntercept<scaleFactor*76/2) {//intersection with the top of the map
                delta.y=-delta.y;
                if(slope!=0)//don't know why slope ever hits zero. this is almost the same logic as in the case where we intersect the top, except the y coord is different
                    center = Point(center.x+(scaleFactor*76/2-center.y)/slope,scaleFactor*76/2);
                else
                    std::cout << "I don't know why, but the slope of the velocity of Kanye's head is zero."<<std::endl;
            } else {//bouncing off side wall
                delta.x=-delta.x;
                center = Point((delta.x<0)? scaleFactor*56/2:scaleFactor*465-scaleFactor*56/2,yIntercept);
            }
        }
    }
    return Point(-1,-1);
}
