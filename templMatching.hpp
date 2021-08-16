#pragma once
#include <opencv2/opencv.hpp>

using cv::Point;
using cv::Mat;

Point findImageCenter(Mat, Mat);
Point findImageCenter(Mat, Mat, int, cv::Scalar);