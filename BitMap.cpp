#include "BitMap.h"

using namespace std;

bool BitMap::create(HDC hdc, wstring fileName) {
	free();
	HANDLE hfile = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		return false;
	}
	BITMAPFILEHEADER bmfh;
	DWORD dwBytesRead;
	bool bok = ReadFile(hfile, &bmfh, sizeof(BITMAPFILEHEADER),
		&dwBytesRead, NULL);
	if (!bok || (dwBytesRead != sizeof(BITMAPFILEHEADER)) ||
		bmfh.bfType != 0x4d42) {
		CloseHandle(hfile);
		return false;
	}
	BITMAPINFO* info = new BITMAPINFO;
	if (info != NULL) {
		bok = ReadFile(hfile, info, sizeof(BITMAPINFO),
			&dwBytesRead, NULL);
		if (!bok || dwBytesRead != sizeof(BITMAPINFO)) {
			CloseHandle(hfile);
			free();
			return false;
		}
		width = (int)(info->bmiHeader.biWidth);
		height = (int)(info->bmiHeader.biHeight);
		PBYTE bitmapBits;
		hbitmap = CreateDIBSection(hdc, info, DIB_RGB_COLORS,
			(PVOID*)(&bitmapBits), NULL, 0);
		if (hbitmap != NULL && bitmapBits != NULL) {
			SetFilePointer(hfile, bmfh.bfOffBits, NULL, FILE_BEGIN);
			bok = ReadFile(hfile, bitmapBits, info->bmiHeader.biSizeImage,
				&dwBytesRead, NULL);
			if (bok) {
				return true;
			}
		}
	}
	free();
	return false;
}

bool BitMap::create(HDC hdc, UINT resID, HINSTANCE hinstance) {
	free();
	HRSRC resinfo = FindResource(hinstance, MAKEINTRESOURCE(resID),
		RT_BITMAP);
	if (resinfo == NULL) {
		return false;
	}
	HGLOBAL memBitMap = LoadResource(hinstance, resinfo);
	if (memBitMap == NULL) {
		return false;
	}
	PBYTE bitMapImage = (BYTE*)LockResource(memBitMap);
	if (bitMapImage == NULL) {
		FreeResource(memBitMap);
		return false;
	}
	BITMAPINFO* info = (BITMAPINFO*)bitMapImage;
	width = info->bmiHeader.biWidth;
	height = info->bmiHeader.biHeight;
	PBYTE bitMapBits;
	hbitmap = CreateDIBSection(hdc, info, DIB_RGB_COLORS,
		(PVOID*)&bitMapBits, NULL, 0);
	if (hbitmap != NULL && bitMapBits != NULL) {
		const PBYTE TEMPBITS = bitMapImage + info->bmiHeader.biSize
			+ info->bmiHeader.biClrUsed + sizeof(RGBQUAD);
		CopyMemory(bitMapBits, TEMPBITS, info->bmiHeader.biSizeImage);
		UnlockResource(memBitMap);
		FreeResource(memBitMap);
		return true;
	}
	UnlockResource(memBitMap);
	FreeResource(memBitMap);
	free();
	return false;
}

bool BitMap::create(HDC hdc, int w, int h, COLORREF color) {

	hbitmap = CreateCompatibleBitmap(hdc, w, h);

	if (hbitmap == NULL) {

		return false;

	}

	width = w;
	height = h;

	HDC memDC = CreateCompatibleDC(hdc);

	HBRUSH brush = CreateSolidBrush(color);

	HBITMAP oldBitMap = (HBITMAP)SelectObject(memDC, hbitmap);

	RECT rect = { 0, 0, w, h };

	FillRect(memDC, &rect, brush);

	SelectObject(memDC, oldBitMap);

	DeleteObject(brush);
	DeleteDC(memDC);

	return true;

}

BitMap::BitMap() {

	hbitmap = NULL;
	width = 0;
	height = 0;

}

BitMap::BitMap(HDC hdc, wstring fileName) {

	hbitmap = NULL;
	width = 0;
	height = 0;

	create(hdc, fileName);

}

BitMap::BitMap(HDC hdc, UINT resID, HINSTANCE instance) {

	hbitmap = NULL;
	width = 0;
	height = 0;

	create(hdc, resID, instance);

}

BitMap::BitMap(HDC hdc, int w, int h, COLORREF color) {

	hbitmap = NULL;
	width = 0;
	height = 0;

	create(hdc, w, h, color);

}

BitMap::~BitMap() {

	free();

}

void BitMap::free() {

	if (hbitmap != NULL) {

		DeleteObject(hbitmap);
		hbitmap = NULL;

	}

}

void BitMap::draw(HDC hdc, int x, int y, bool btrans, COLORREF ctrans) {
	drawPart(hdc, x, y, 0, 0, getWidth(), getHeight(), btrans, ctrans);
}

void BitMap::drawPart(HDC hdc, int x, int y, int xPart, int yPart, int wPart, int hPart,
	bool btrans, COLORREF ctrans) {

	if (hbitmap == NULL) {

		return;

	}

	HDC memDC = CreateCompatibleDC(hdc);

	HBITMAP oldBitMap = (HBITMAP)SelectObject(memDC, hbitmap);

	if (btrans) {

		TransparentBlt(hdc, x, y, wPart, hPart, memDC, xPart, yPart,
			wPart, hPart, ctrans);

	}
	else {

		BitBlt(hdc, x, y, wPart, hPart, memDC, xPart, yPart, SRCCOPY);

	}

	DeleteObject(oldBitMap);
	DeleteDC(memDC);

}
