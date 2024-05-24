#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include "Windows.h"
#include <string>
#include "Sprite.h"
#include <vector>

int WINAPI WinMain(HINSTANCE currInstance, HINSTANCE prevInstance,
	PSTR szCmdLine, int showCmd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam,
	LPARAM lparam);

BOOL GameInitialize(HINSTANCE currInstance);
void GameLoop();
void GameEnd();
void GameStart(HWND hwnd);
void GameActivate(HWND hwnd);
void GameDeactivate(HWND hwnd);
void GamePaint(HDC hdc);

void HandleKeys();
void MouseButtonDown(int x, int y, bool left);
void MouseButtonUp(int x, int y, bool left);
void MouseMove(int x, int y);

bool SpriteCollision(Sprite* hitter, Sprite* hittee);


class GameEngine
{
protected:
	static GameEngine* gameEngine;
	HINSTANCE instance;
	HWND hwnd;
	std::wstring wndClass;
	std::wstring title;
	WORD icon;
	WORD smIcon;
	int width;
	int height;
	int frameDelay;
	BOOL sleep;
	std::vector<Sprite*> sprites;
	bool checkSpriteCollision(Sprite* testSprite);

public:
	GameEngine(HINSTANCE instance, std::wstring wndClass,
		std::wstring title, WORD icon, WORD smIcon, int width = 640,
		int height = 480);
	virtual ~GameEngine();

	static GameEngine* GetEngine() { return gameEngine; };

	BOOL initialize(int showCmd);
	LRESULT HandleEvent(HWND hwnd, UINT msg, WPARAM wparam,
		LPARAM lparam);
	HINSTANCE getInstance() { return instance; };
	HWND getWnd() { return hwnd; };
	void setWnd(HWND win) { hwnd = win; };
	std::wstring getTitle() { return title; };
	WORD getIcon() { return icon; };
	WORD getSmIcon() { return smIcon; };
	int getWidth() { return width; };
	int getHeight() { return height; };
	int getFrameDelay() { return frameDelay; };
	void setFrameRate(int frameRate) { frameDelay = 1000 / frameRate; };
	BOOL getSleep() { return sleep; };
	void setSleep(BOOL s) { sleep = s; };
	LPPOINT drawLine(HDC hdc, int startx, int starty, int endx, int endy) {

		LPPOINT point = NULL;

		MoveToEx(hdc, startx, starty, point);

		LineTo(hdc, endx, endy);

		return point;

	};
	void drawBitMap(std::wstring filename, int x, int y) {

		HBITMAP image = (HBITMAP)LoadImage(0, filename.c_str(), IMAGE_BITMAP,
			0, 0, LR_LOADFROMFILE);

		BITMAP bm;

		GetObject(image, sizeof(BITMAP), &bm);

		HDC hdc = CreateCompatibleDC(GetDC(hwnd));

		SelectObject(hdc, image);

		BitBlt(GetDC(hwnd), x, y, bm.bmWidth, bm.bmHeight, hdc, 0, 0, SRCCOPY);

		DeleteDC(hdc);

		DeleteObject((HBITMAP)image);
	}

	void addSprite(Sprite*);
	void drawSprites(HDC);
	void updateSprites();
	void cleanupSprites();
	Sprite* isPointInSprite(int x, int y);

};

#endif

