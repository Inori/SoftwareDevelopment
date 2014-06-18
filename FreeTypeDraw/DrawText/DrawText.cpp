// DrawText.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DrawText.h"

#pragma comment(lib, "Msimg32.lib")

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_STROKER_H

FT_Library library;
FT_Face face;

HBITMAP hBitmap;


#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	HANDLE hfile = CreateFile("simhei.ttf", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD filesize = GetFileSize(hfile, NULL);
	DWORD sizeread = 0;
	BYTE *fontdata = new BYTE[filesize];
	ReadFile(hfile, (LPVOID)fontdata, filesize, &sizeread, NULL);
	CloseHandle(hfile);

	FT_Init_FreeType(&library);
	FT_New_Memory_Face(library, fontdata, filesize, 0, &face);

	const unsigned int size = 500;
	FT_Set_Char_Size(face, size << 6, size << 6, 90, 90);


	hBitmap = (HBITMAP)LoadImageA(NULL, "40639484.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);

	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DRAWTEXT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DRAWTEXT));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAWTEXT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DRAWTEXT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void DrawBmp(HDC hDC, HBITMAP bitmap, int nWidth, int nHeight)
{
	BITMAP			bm;
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

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	FT_GlyphSlot slot = face->glyph; /* 一个小捷径 */
	FT_Bitmap bmp;
	FT_Error error;
	FT_UInt glyph_index;


	glyph_index = FT_Get_Char_Index(face, 0x6211);
	error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
	error = FT_Render_Glyph(face->glyph, ft_render_mode_normal);
	
	bmp = slot->bitmap;

	DWORD nWidth = bmp.width;
	DWORD nHeight = bmp.rows;

	BITMAPINFO bi;
	PVOID buffer;
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = nWidth;
	bi.bmiHeader.biHeight = nHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = 0;
	HBITMAP hbm = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&buffer, NULL, NULL);

	BYTE *src = bmp.buffer;
	BYTE *dst = (BYTE*)buffer;
	const int pix_size = 4;

	for (int y = 0; y < nHeight; y++)
	{
		BYTE *srcline = &src[(nHeight - y - 1)*nWidth];
		BYTE *dstline = &dst[y * nWidth * pix_size];
		for (int x = 0; x < nWidth; x++)
		{
			dstline[x*pix_size] = srcline[x];
			dstline[x*pix_size + 1] = srcline[x];
			dstline[x*pix_size + 2] = srcline[x];
			dstline[x*pix_size + 3] = srcline[x];
		}
	}

	
	HBITMAP hbmOld;
	HDC mdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// 绘制背景
		DrawBmp(hdc, hBitmap, 1280, 720);


		mdc = CreateCompatibleDC(hdc);
		hbmOld = (HBITMAP)::SelectObject(mdc, hbm);

		BLENDFUNCTION blend;
		blend.BlendOp = AC_SRC_OVER;
		blend.AlphaFormat = AC_SRC_ALPHA;
		blend.BlendFlags = 0;
		blend.SourceConstantAlpha = 255;
		AlphaBlend(hdc, 0, 0, nWidth, nHeight, mdc, 0, 0, nWidth, nHeight, blend);

		SelectObject(mdc, hbmOld);

		DeleteDC(mdc);
		ReleaseDC(hWnd, hdc);
		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
