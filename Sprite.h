#ifndef SPRITE_H
#define SPRITE_H

#include "Windows.h"
#include "BitMap.h"

typedef WORD BOUNDSACTION;
const BOUNDSACTION BA_STOP = 0,
BA_WRAP = 1,
BA_BOUNCE = 2,
BA_DIE = 3;

typedef WORD SPRITEACTION;
const SPRITEACTION SA_NONE = 0x0000L,
SA_KILL = 0x0001L;

class Sprite {

protected:
	BitMap* bitmap;
	RECT position;
	POINT velocity;
	int zOrder;
	RECT bounds;
	BOUNDSACTION boundsAction;
	bool hidden;
	RECT collision;
	virtual void calcCollisionRect();
	int numFrames, curFrame;
	int frameDelay, frameTrigger;
	bool dying, oneCycle;

public:
	Sprite(BitMap*);
	Sprite(BitMap*, RECT& boundary, BOUNDSACTION ba = BA_STOP);
	Sprite(BitMap*, POINT position, POINT velocity, int zOrder,
		RECT& boundary, BOUNDSACTION ba = BA_STOP);

	virtual ~Sprite();

	virtual SPRITEACTION Update();

	void Draw(HDC);

	bool isPointInside(int x, int y);

	void kill() {

		dying = true;

	};

	void setNumFrames(int num, bool one = false);

	void setFrameDelay(int fDelay) {

		frameDelay = fDelay;

	};

	virtual void updateFrame();

	RECT& getPosition() {

		return position;

	};

	void setPosition(POINT);
	void setPosition(int x, int y);
	void setPosition(RECT& rectPosition) {

		CopyRect(&position, &rectPosition);
		calcCollisionRect();

	};

	void offSetPosition(int x, int y);

	POINT getVelocity() {

		return velocity;

	};

	void setVelocity(int x, int y);
	void setVelocity(POINT);

	int getZOrder() {

		return zOrder;

	};

	void setZOrder(int z) {

		zOrder = z;

	};

	void setBounds(RECT&);
	void setBoundsAction(BOUNDSACTION ba) {

		boundsAction = ba;

	};

	bool isHidden() {

		return hidden;

	};

	void setHidden(bool h) {

		hidden = h;

	};

	int getWidth() {

		return bitmap->getWidth();

	};

	int getHeight() {

		return bitmap->getHeight() / numFrames;

	};

	bool testCollision(Sprite*);

	RECT& getCollision() {
		return collision;
	};

};

#endif