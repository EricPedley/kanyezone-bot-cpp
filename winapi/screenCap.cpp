#ifndef HWND
#include <windows.h>
#endif
#ifndef cv
#include <opencv2/opencv.hpp>
#endif
/** ripped from https://superkogito.github.io/blog/CaptureScreenUsingOpenCv.html
 * Create a Bitmap file header..
 *
 * @param hwindowDC : window handle.
 * @param widht	    : image width.
 * @param height    : image height.
 *
 * @return Bitmap header.
 */
BITMAPINFOHEADER createBitmapHeader(int width, int height)
{
	BITMAPINFOHEADER bi;

	// create a bitmap
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height; //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	return bi;
}

using cv::Mat;

class ScreenCapturer {
	HWND hwnd;
	int height, width, screenx, screeny;
	HDC hwindowDC,hwindowCompatibleDC;
	BITMAPINFOHEADER bi;
	HBITMAP hbwindow;
	Mat src;
	public:
		ScreenCapturer(HWND, int, int, int, int);
		~ScreenCapturer();
		Mat getScreenCap();
};

ScreenCapturer::ScreenCapturer(HWND hwndParameter, int offsetX = 0, int offsetY = 0, int cropWidth = -1, int cropHeight = -1) {
	hwnd=hwndParameter;
	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	//SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	// define x,y,height and width
	RECT r;
	GetWindowRect(hwnd, &r);	   //passes window handle and pointer to r, and this function modifies r.
	screenx = r.left+offsetX;		   //GetSystemMetrics(SM_XVIRTUALSCREEN);
	screeny = r.top+offsetY;		   //GetSystemMetrics(SM_YVIRTUALSCREEN);
	width;  //GetSystemMetrics(SM_CXVIRTUALSCREEN);
	if(cropWidth==-1)
		width = r.right - r.left;  
	else
		width = cropWidth;
	height;
	if(cropHeight==-1)
		height = r.bottom - r.top; //GetSystemMetrics(SM_CYVIRTUALSCREEN);
	else
		height=cropHeight;
	bi = createBitmapHeader(width, height);
	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// create mat object
	src.create(height, width, CV_8UC4);

}

ScreenCapturer::~ScreenCapturer() {
	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);
}

Mat ScreenCapturer::getScreenCap()
{
	//This is what the problem was. stretchblt was producing a completely black image so I replaced it with bitblt and now it works
	//StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
	BitBlt(hwindowCompatibleDC, 0, 0, width, height, GetDC(0), screenx, screeny, SRCCOPY);

	//this line is what copies the hbitmap image information to the opencv matrix object
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS); //copy from hwindowCompatibleDC to hbwindow

	return src;
}