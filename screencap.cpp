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
	BITMAPINFOHEADER  bi;

	// create a bitmap
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
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

/**ripped from https://superkogito.github.io/blog/CaptureScreenUsingOpenCv.html, but modified
 * Capture a screen window as a matrix.
 *
 * @param hwnd : window handle.
 *
 * @return Mat (Mat of the captured image)
 */
using cv::Mat;
Mat captureScreenMat(HWND hwnd)
{
	Mat src;

	// get handles to a device context (DC)
	HDC hwindowDC = GetDC(hwnd);
	HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	// define x,y,height and width
    RECT r;
    GetWindowRect(hwnd,&r);
	int screenx = r.left;//GetSystemMetrics(SM_XVIRTUALSCREEN);
	int screeny = r.top;//GetSystemMetrics(SM_YVIRTUALSCREEN);
	int width = r.right-r.left;//GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int height = r.bottom-r.top;//GetSystemMetrics(SM_CYVIRTUALSCREEN);

	// create mat object
	src.create(height, width, CV_8UC4);

	// create a bitmap
	HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	BITMAPINFOHEADER bi = createBitmapHeader(width, height);

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);

	// copy from the window device context to the bitmap device context
    //This is what the problem was. stretchblt was producing a completely black image so I replaced it with bitblt and now it works
	//StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
	BitBlt(hwindowCompatibleDC, 0, 0, width, height, GetDC(0), screenx, screeny, SRCCOPY);

    //this line is what copies the hbitmap image information to the opencv matrix object
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);            //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}