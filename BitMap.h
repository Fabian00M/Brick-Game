#ifndef BITMAP_H
#define BITMAP_H

#include "Windows.h"
#include <string>

class BitMap {
protected:
	HBITMAP hbitmap;
	int width, height;
	void free();
public:
	BitMap();
	BitMap(HDC, std::wstring);
	BitMap(HDC, UINT, HINSTANCE);
	BitMap(HDC, int, int, COLORREF crColor = RGB(0, 0, 0));
	~BitMap();
	bool create(HDC, std::wstring);
	bool create(HDC, UINT, HINSTANCE);
	bool create(HDC, int, int, COLORREF);
	void drawPart(HDC, int x, int y, int xPart, int yPart, int wPart,
		int hPart, bool btrans = false, COLORREF ctrans = RGB(255, 0, 255));
	void draw(HDC, int, int, bool btrans = false,
		COLORREF ctrans = RGB(255, 0, 255));
	int getWidth() { return width; };
	int getHeight() { return height; };
};

#endif