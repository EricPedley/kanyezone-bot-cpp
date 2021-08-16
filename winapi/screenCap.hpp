#pragma once
#ifndef HWND
#include <windows.h>
#endif
#ifndef cv
#include <opencv2/opencv.hpp>
#endif

using cv::Mat;

class ScreenCapturer {
	HWND hwnd;
	int height, width, screenx, screeny;
	HDC hwindowDC,hwindowCompatibleDC;
	BITMAPINFOHEADER bi;
	HBITMAP hbwindow;
	Mat src;
	public:
		ScreenCapturer(HWND);//TODO: this feels like a hack. idk how to specify that the ints are optional.
		ScreenCapturer(HWND, int, int, int, int);
		~ScreenCapturer();
		Mat getScreenCap();
};