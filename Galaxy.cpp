#include "GameEngine.h"
#include "Windows.h"
#include "Resource.h"
#include "Sprite.h"
#include "BitMap.h"

GameEngine* game;
BitMap* planets[3];
BitMap* bck;
Sprite* dragPlanet;

HDC offScreen;
HBITMAP offScreenBitMap;

BOOL GameInitialize(HINSTANCE currInstance){
	game = new GameEngine(currInstance, L"Planets", L"Planets", IDI_GALAXY,
		IDI_GALAXY_SM, 600, 400);
	if (game == NULL) {
		return FALSE;
	}
	game->setFrameRate(30);
	return TRUE;
}

void GameLoop(){
	game->updateSprites();
	HWND hwnd = game->getWnd();
	HDC hdc = GetDC(hwnd);
	GamePaint(offScreen);
	BitBlt(hdc, 0, 0, game->getWidth(), game->getHeight(), offScreen,
		0, 0, SRCCOPY);
	ReleaseDC(hwnd, hdc);
}

void GameEnd(){
	DeleteObject(offScreenBitMap);
	DeleteDC(offScreen);
	delete bck;
	for (int i = 0; i < 3; i++) {
		delete planets[i];
	}
	game->cleanupSprites();
	delete game;
}

void GameStart(HWND hwnd){
	srand(GetTickCount());
	offScreen = CreateCompatibleDC(GetDC(hwnd));
	offScreenBitMap = CreateCompatibleBitmap(GetDC(hwnd), game->getWidth(),
		game->getHeight());
	SelectObject(offScreen, offScreenBitMap);
	HDC hdc = GetDC(hwnd);
	bck = new BitMap(hdc, L"Res/Galaxy.bmp");
	planets[0] = new BitMap(hdc, L"Res/Planet1.bmp");
	planets[1] = new BitMap(hdc, L"Res/Planet2.bmp");
	planets[2] = new BitMap(hdc, L"Res/Planet3.bmp");

	RECT bounds = { 0, 0, 600, 400 };
	Sprite* temp = new Sprite(planets[0], bounds, BA_WRAP);
	temp->setVelocity(3, 2);
	// game->addSprite(temp);
	Sprite* temp2 = new Sprite(planets[1], bounds, BA_WRAP);
	temp2->setVelocity(4, 1);
	// game->addSprite(temp);
	bounds.right = 200;
	bounds.bottom = 160;
	Sprite* temp3 = new Sprite(planets[2], bounds, BA_BOUNCE);
	temp3->setVelocity(-4, 2);
	// game->addSprite(temp);
	bounds.left = 400;
	bounds.top = 240;
	bounds.right = 600;
	bounds.bottom = 400;
	Sprite*  temp4 = new Sprite(planets[2], bounds, BA_BOUNCE);
	temp4->setVelocity(7, -3);
	// game->addSprite(temp);

	bounds.left = 0;
	bounds.top = 0;
	bounds.right = 600;
	bounds.bottom = 400;

	while (temp->testCollision(temp2) || temp->testCollision(temp3) ||
		temp->testCollision(temp4)) {
		int xPos = rand() % (bounds.right - bounds.left - temp->getWidth()) + bounds.left;
		int yPos = rand() % (bounds.bottom - bounds.top - temp->getHeight()) + bounds.top;
		temp->setPosition(xPos, yPos);
	}

	while (temp2->testCollision(temp) || temp2->testCollision(temp3) ||
		temp2->testCollision(temp4)) {
		int xPos = rand() % (bounds.right - bounds.left - temp2->getWidth()) + bounds.left;
		int yPos = rand() % (bounds.bottom - bounds.top - temp2->getHeight()) + bounds.top;
		temp2->setPosition(xPos, yPos);
	}

	game->addSprite(temp);
	game->addSprite(temp2);
	game->addSprite(temp3);
	game->addSprite(temp4);

	dragPlanet = NULL;
}

void GameActivate(HWND hwnd){
	//Blank
}

void GameDeactivate(HWND hwnd){
	//Blank
}

void GamePaint(HDC hdc){
	bck->draw(hdc, 0, 0);
	game->drawSprites(hdc);
}

void HandleKeys(){
	// Blank
}

void MouseButtonDown(int x, int y, bool left){
	if (dragPlanet == NULL) {
		if (dragPlanet = game->isPointInSprite(x, y)) {
			SetCapture(game->getWnd());
			MouseMove(x, y);
		}
	}
}

void MouseButtonUp(int x, int y, bool left){
	ReleaseCapture();
	dragPlanet = NULL;
}

void MouseMove(int x, int y){
	if (dragPlanet != NULL) {
		dragPlanet->setPosition(
			x - (dragPlanet->getWidth() / 2),
			y - (dragPlanet->getHeight() / 2)
		);
		InvalidateRect(game->getWnd(), NULL, FALSE);
	}
}

bool SpriteCollision(Sprite* hitter, Sprite* hittee){
	RECT hitterCollision = hitter->getCollision();
	RECT hitteeCollision = hittee->getCollision();
	POINT hitterVelo = hitter->getVelocity();
	if (hitterCollision.left <= hitteeCollision.right ||
		hitterCollision.right >= hitteeCollision.left) {
		hitterVelo.x = -hitterVelo.x;
	}
	if (hitterCollision.top <= hitteeCollision.bottom ||
		hitterCollision.bottom >= hitteeCollision.top) {
		hitterVelo.y = -hitterVelo.y;
	}
	hitter->setVelocity(hitterVelo);
	return true;
}
