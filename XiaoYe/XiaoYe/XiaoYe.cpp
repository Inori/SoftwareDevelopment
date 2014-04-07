// XiaoYe.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "XiaoYe.h"

#define WndTimer 1
#define BmpTimer 2
#define CmdTimer 3

#define OPENEYE 10
#define CLOSEEYE 20
#define OPENMOUSE 30
#define CLOSEMOUSE 40
#define ALPHACHANGE 50
#define INVALIDCMD 0
typedef unsigned int  uint;
typedef unsigned char uchar;

typedef struct cmd_s
{
	uchar flag;
	uchar val;
} cmd_t;


cmd_t GetSerialCmd(CSerialPort &sp)
{
	uchar *cache = new uchar[2];
	cmd_t cmd;
	uint num = sp.GetBytesInCOM();
	double temp_alp = 0;

	if (num != 2)
	{
		sp.ClearPort();
		cmd.flag = INVALIDCMD;
		cmd.val = 0;
		return cmd;
	}

	for (uint i = 0; i < num; i++)
		sp.ReaduChar(cache[i]);

	if (!memcmp("eo", cache, 2))
	{
		cmd.flag = OPENEYE;
		cmd.val = 0;
		return cmd;
	}
	if (!memcmp("ec", cache, 2))
	{
		cmd.flag = CLOSEEYE;
		cmd.val = 0;
		return cmd;
	}
	if (!memcmp("mo", cache, 2))
	{
		cmd.flag = OPENMOUSE;
		cmd.val = 0;
		return cmd;
	}
	if (!memcmp("mc", cache, 2))
	{
		cmd.flag = CLOSEMOUSE;
		cmd.val = 0;
		return cmd;
	}
	if (!memcmp("\xFF", cache, 1))
	{
		cmd.flag = ALPHACHANGE;
		temp_alp = (cache[1] - 200) * 4.6;
		cmd.val = (uchar)temp_alp > 255 ? 255 : (uchar)temp_alp; //为了是效果明显进行放大
		return cmd;
	}

	delete[]cache;
	cmd.flag = INVALIDCMD;
	cmd.val = 0;
	return cmd;
}

CSerialPort gPort;

COLORREF rgb;
HWND ghWnd;
int gHeight = 720;
int gWidth = 465;

HBITMAP hBitmap;
HBITMAP eomo;
HBITMAP eomc;
HBITMAP ecmo;
HBITMAP ecmc;
int flag = 0;
int CALLBACK TimerProc()
{
	static int wndAlp = 0;
	if (flag)
	{
		wndAlp -= 5;
		SetLayeredWindowAttributes(ghWnd, -1, wndAlp, LWA_ALPHA);
		if (wndAlp == 0)
			DestroyWindow(ghWnd);
		
	}
	else
	{
		if (wndAlp < 255)
		{
			wndAlp += 5;
			SetLayeredWindowAttributes(ghWnd, -1, wndAlp, LWA_ALPHA);
		}
		//else
			//KillTimer(ghWnd, 1003);
	}


	return 0;
}
void DrawBmp(HDC hDC, HBITMAP bitmap, int nWidth, int nHeight)
{
	BITMAP	bm;
	HDC hdcImage;
	HDC hdcMEM;
	hdcMEM = CreateCompatibleDC(hDC);
	hdcImage = CreateCompatibleDC(hDC);
	HBITMAP bmp = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
	GetObject(bitmap, sizeof(bm), (LPSTR)&bm);
	SelectObject(hdcMEM, bmp);
	SelectObject(hdcImage, bitmap);
	StretchBlt(hdcMEM, 0, 0, nWidth, nHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	StretchBlt(hDC, 0, 0, nWidth, nHeight, hdcMEM, 0, 0, nWidth, nHeight, SRCCOPY);

	DeleteObject(hdcImage);
	DeleteDC(hdcImage);
	DeleteDC(hdcMEM);
}

void DrawAlpBmp(HDC hDC, HBITMAP bitmap, int nWidth, int nHeight ,BYTE alp)
{
	BITMAP	bm;
	HDC hdcImage;
	HDC hdcMEM;
	hdcMEM = CreateCompatibleDC(hDC);
	hdcImage = CreateCompatibleDC(hDC);
	HBITMAP bmp = ::CreateCompatibleBitmap(hDC, nWidth, nHeight);
	GetObject(bitmap, sizeof(bm), (LPSTR)&bm);
	SelectObject(hdcMEM, bmp);
	SelectObject(hdcImage, bitmap);
	StretchBlt(hdcMEM, 0, 0, nWidth, nHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.AlphaFormat = 0;
	blend.BlendFlags = 0;
	blend.SourceConstantAlpha = alp;
	AlphaBlend(hDC, 0, 0, nWidth, nHeight, hdcMEM, 0, 0, nWidth, nHeight, blend);

	DeleteObject(hdcImage);
	DeleteDC(hdcImage);
	DeleteDC(hdcMEM);
}

BYTE gBmpAlp = 0;
int CALLBACK TimerBmp()
{
	if (gBmpAlp < 255)
	{
		gBmpAlp += 1;
		DrawAlpBmp(GetDC(ghWnd), hBitmap, gWidth, gHeight, gBmpAlp);
	}
	return 0;
}

void DrawLayeredBmp(HBITMAP bitmap)
{
	hBitmap = bitmap;
	gBmpAlp = 0;
}

int CALLBACK TimerCmd()
{
	cmd_t cmd = GetSerialCmd(gPort);
	if (cmd.flag == INVALIDCMD)
		return 0;

	switch (cmd.flag)
	{
	case OPENEYE:
		if (hBitmap == ecmo)
			DrawLayeredBmp(eomo);
		else if (hBitmap == ecmc)
			DrawLayeredBmp(eomc);
		break;
	case CLOSEEYE:
		if (hBitmap == eomo)
			DrawLayeredBmp(ecmo);
		else if (hBitmap == eomc)
			DrawLayeredBmp(ecmc);
		break;
	case OPENMOUSE:
		if (hBitmap == eomc)
			DrawLayeredBmp(eomo);
		else if (hBitmap == ecmc)
			DrawLayeredBmp(ecmo);
		break;
	case CLOSEMOUSE:
		if (hBitmap == eomo)
			DrawLayeredBmp(eomc);
		else if (hBitmap == ecmo)
			DrawLayeredBmp(ecmc);
		break;
	case ALPHACHANGE:
		SetLayeredWindowAttributes(ghWnd, -1, cmd.val, LWA_ALPHA);
		break;
	default:
		break;
	}
	
	return 0;
}

LRESULT CALLBACK WindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	static HDC compDC = 0;
	static RECT rect;
	if (uMsg == WM_CREATE)
	{
		ghWnd = hwnd;
		SetLayeredWindowAttributes(hwnd, -1, 0, LWA_ALPHA);
		SetTimer(hwnd, WndTimer, 1, (TIMERPROC)TimerProc);
		SetTimer(NULL, BmpTimer, 15, (TIMERPROC)TimerBmp);
		SetTimer(NULL, CmdTimer, 1, (TIMERPROC)TimerCmd);
		int scrWidth, scrHeight;

		scrWidth = GetSystemMetrics(SM_CXSCREEN);
		scrHeight = GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(hwnd, &rect);
		rect.left = (scrWidth - rect.right) / 2;
		rect.top = (scrHeight - rect.bottom) / 2;
		SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW);
		//DrawBmp(GetDC(hwnd), hBitmap, gWidth, gHeight);
	}
	if (uMsg == WM_PAINT)
	{
		RECT rect;
		GetWindowRect(hwnd, &rect);
		DrawBmp(GetDC(hwnd), hBitmap, gWidth, gHeight);
	}
	if (uMsg == WM_CLOSE)
	{
		DestroyWindow(hwnd);
	}
	if (uMsg == WM_MOUSEMOVE)
	{
		WORD y = HIWORD(lParam);
		uchar yPos[2];
		yPos[0] = y >> 8; //高位
		yPos[1] = y & 0xFF;
		gPort.WriteData(yPos, 2);
	}
	if (uMsg == WM_LBUTTONDBLCLK)
	{
		flag = 1;
	}
	if (uMsg == WM_RBUTTONDOWN)
	{
		if (hBitmap == eomo)
			DrawLayeredBmp(ecmo);
		else
			DrawLayeredBmp(eomo);
	}
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
	}
	if (uMsg == WM_LBUTTONDOWN)
	{
		PostMessage(hwnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(LOWORD(lParam), HIWORD(lParam)));
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HINSTANCE hInst;
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	ecmc = (HBITMAP)LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	ecmo = (HBITMAP)LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	eomc = (HBITMAP)LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP3), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	eomo = (HBITMAP)LoadImageA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP4), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
	hBitmap = eomo;

	gPort.InitPort(3);

	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = hInst;
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszClassName = "XiaoYe";
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.cbWndExtra = DLGWINDOWEXTRA;
	rgb = 0xFFFFFFFF;
	RegisterClassExA(&wcex);
	HWND hWnd = CreateWindowExA(WS_EX_LAYERED | WS_EX_TOPMOST, "XiaoYe", "XiaoYe", WS_POPUP | WS_SYSMENU | WS_SIZEBOX, 0, 0, gWidth, gHeight, NULL, NULL, hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
