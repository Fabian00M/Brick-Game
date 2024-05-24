#include "GameEngine.h"

GameEngine* GameEngine::gameEngine = NULL;

int WINAPI WinMain(HINSTANCE currInstance, HINSTANCE prevInstance,
	PSTR szCmdLine, int showCmd) {
	MSG msg;
	static long ticTrigger = 0;
	long ticCounter;

	if (GameInitialize(currInstance)) {
		if (!GameEngine::GetEngine()->initialize(showCmd))
			return FALSE;
		while (true) {

			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

				if (msg.message == WM_QUIT) {

					break;

				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}
			else {

				if (!GameEngine::GetEngine()->getSleep()) {

					ticCounter = GetTickCount();

					if (ticCounter > ticTrigger) {

						ticTrigger = ticCounter + GameEngine::GetEngine()->getFrameDelay();
						HandleKeys();
						GameLoop();

					}

				}

			}

		}

		return msg.wParam;

	}

	GameEnd();

	return TRUE;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam,
	LPARAM lparam) {

	return GameEngine::GetEngine()->HandleEvent(hwnd, msg, wparam, lparam);

}

BOOL GameEngine::initialize(int showCmd) {

	WNDCLASSEX wndClass;
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = instance;
	wndClass.hIcon = LoadIcon(instance, MAKEINTRESOURCE(getIcon()));
	wndClass.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(getSmIcon()));
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = (this->wndClass).c_str();

	if (!RegisterClassEx(&wndClass)) {

		return FALSE;

	}

	int wndWidth = width + GetSystemMetrics(SM_CXFIXEDFRAME) * 2;
	int wndHeight = height + GetSystemMetrics(SM_CYFIXEDFRAME) * 2
		+ GetSystemMetrics(SM_CYCAPTION);

	if (wndClass.lpszMenuName != NULL) {

		wndHeight += GetSystemMetrics(SM_CYMENU);

	}

	int xWndPos = (GetSystemMetrics(SM_CXSCREEN) - wndWidth) / 2;
	int yWndPos = (GetSystemMetrics(SM_CYSCREEN) - wndHeight) / 2;

	hwnd = CreateWindow(this->wndClass.c_str(), this->wndClass.c_str(),
		WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX,
		xWndPos, yWndPos, wndWidth, wndHeight, NULL, NULL,
		instance, NULL);

	if (!hwnd)
		return FALSE;

	ShowWindow(hwnd, showCmd);
	UpdateWindow(hwnd);

	return TRUE;
}

GameEngine::GameEngine(HINSTANCE instance, std::wstring wndClass,
	std::wstring title, WORD icon, WORD smIcon, int width, int height) {

	gameEngine = this;
	this->instance = instance;
	hwnd = NULL;

	if (wndClass.length() > 0) {
		this->wndClass = wndClass;
	}
	else {
		this->wndClass = L"";
	}

	if (title.length() > 0)
		this->title = title;
	else
		this->title = L"";

	this->icon = icon;
	this->smIcon = smIcon;
	this->width = width;
	this->height = height;
	frameDelay = 50;
	sleep = TRUE;
}

GameEngine::~GameEngine() {

}

LRESULT GameEngine::HandleEvent(HWND hwnd, UINT msg, WPARAM wparam,
	LPARAM lparam) {
	switch (msg) {
	case WM_CREATE:
		setWnd(hwnd);
		GameStart(hwnd);
		return 0;
	case WM_SETFOCUS:
		GameActivate(hwnd);
		setSleep(FALSE);
		return 0;
	case WM_KILLFOCUS:
		GameDeactivate(hwnd);
		setSleep(TRUE);
		return 0;
	case WM_PAINT:
		HDC hdc;
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		GamePaint(hdc);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		GameEnd();
		PostQuitMessage(0);
		return 0;
	case WM_MOUSEMOVE:
		MouseMove(LOWORD(lparam), HIWORD(lparam));
		return 0;
	case WM_LBUTTONDOWN:
		MouseButtonDown(LOWORD(lparam), HIWORD(lparam), true);
		return 0;
	case WM_LBUTTONUP:
		MouseButtonUp(LOWORD(lparam), HIWORD(lparam), true);
		return 0;
	case WM_RBUTTONDOWN:
		MouseButtonDown(LOWORD(lparam), HIWORD(lparam), false);
		return 0;
	case WM_RBUTTONUP:
		MouseButtonUp(LOWORD(lparam), HIWORD(lparam), false);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void GameEngine::addSprite(Sprite* s) {
	if (s == NULL) {
		return;
	}
	if (sprites.size() > 0) {
		std::vector<Sprite*>::iterator vecIter;
		for (vecIter = sprites.begin(); vecIter != sprites.end(); vecIter++) {
			if (s->getZOrder() < (*vecIter)->getZOrder()) {
				sprites.insert(vecIter, s);
				return;
			}
		}
	}
	sprites.push_back(s);
}

void GameEngine::drawSprites(HDC hdc) {
	for (auto vecIter = sprites.begin(); vecIter != sprites.end(); vecIter++) {
		(*vecIter)->Draw(hdc);
	}
}

void GameEngine::updateSprites() {
	if (sprites.size() >= sprites.capacity() / 2) {
		sprites.reserve(sprites.capacity() * 2);
	}
	RECT oldSpritePos;
	SPRITEACTION sa;
	for (auto vecIter = sprites.begin(); vecIter != sprites.end(); vecIter++) {
		oldSpritePos = (*vecIter)->getPosition();
		sa = (*vecIter)->Update();
		if (sa & SA_KILL) {
			delete (*vecIter);
			vecIter = sprites.erase(vecIter);
			vecIter--;
			continue;
		}
		if (checkSpriteCollision(*vecIter)) {
			(*vecIter)->setPosition(oldSpritePos);
		}
	}
}

void GameEngine::cleanupSprites() {
	for (auto vecIter = sprites.begin(); vecIter != sprites.end();
		vecIter = sprites.begin()) {
		delete (*vecIter);
		sprites.erase(vecIter);
	}
}

Sprite* GameEngine::isPointInSprite(int x, int y) {
	for (auto vecIter = sprites.begin(); vecIter != sprites.end();
		vecIter++) {
		if (!(*vecIter)->isHidden() && (*vecIter)->isPointInside(x, y)) {
			return (*vecIter);
		}
	}
	return NULL;
}

bool GameEngine::checkSpriteCollision(Sprite* testSprite) {
	for (auto vecIter = sprites.begin(); vecIter != sprites.end();
		vecIter++) {
		if ((*vecIter) == testSprite) {
			continue;
		}
		if (testSprite->testCollision(*vecIter)) {
			return SpriteCollision(testSprite, (*vecIter));
		}
	}
	return false;
}