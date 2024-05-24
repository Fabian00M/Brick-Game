#include "Sprite.h"

using namespace std;

Sprite::Sprite(BitMap* bitmap) {
	this->bitmap = bitmap;
	SetRect(&position, 0, 0, bitmap->getWidth(), bitmap->getHeight());
	calcCollisionRect();
	velocity.x = velocity.y = 0;
	zOrder = 0;
	SetRect(&bounds, 0, 0, 640, 480);
	boundsAction = BA_STOP;
	hidden = false;
	numFrames = 1;
	curFrame = frameDelay = frameTrigger = 0;
	dying = oneCycle = false;
}

Sprite::Sprite(BitMap* bitmap, RECT& bounds, BOUNDSACTION boundsAction) {
	this->bitmap = bitmap;
	int xPos = rand() % (bounds.right - bounds.left - bitmap->getWidth()) + bounds.left;
	int yPos = rand() % (bounds.bottom - bounds.top - bitmap->getHeight()) + bounds.top;
	SetRect(&position, xPos, yPos, xPos + bitmap->getWidth(), yPos + bitmap->getHeight());
	calcCollisionRect();
	velocity.x = velocity.y = 0;
	zOrder = 0;
	CopyRect(&(this->bounds), &bounds);
	this->boundsAction = boundsAction;
	hidden = false;
	numFrames = 1;
	curFrame = frameDelay = frameTrigger = 0;
	dying = oneCycle = false;
}

Sprite::Sprite(BitMap* bitmap, POINT position, POINT velocity, int zOrder,
	RECT& boundary, BOUNDSACTION ba) {
	this->bitmap = bitmap;
	SetRect(&(this->position), position.x, position.y, position.x + bitmap->getWidth(),
		position.y + bitmap->getHeight());
	calcCollisionRect();
	this->velocity.x = velocity.x;
	this->velocity.y = velocity.y;
	this->zOrder = zOrder;
	CopyRect(&bounds, &boundary);
	boundsAction = ba;
	hidden = false;
	numFrames = 1;
	curFrame = frameDelay = frameTrigger = 0;
	dying = oneCycle = false;
}

SPRITEACTION Sprite::Update() {
	if (dying) {
		return SA_KILL;
	}
	POINT newPos, spriteSize, boundSize;
	newPos.x = position.left + velocity.x;
	newPos.y = position.top + velocity.y;
	spriteSize.x = position.right - position.left;
	spriteSize.y = position.bottom - position.top;
	boundSize.x = bounds.right - bounds.left;
	boundSize.y = bounds.bottom - bounds.top;

	if (boundsAction == BA_BOUNCE) {
		bool bounce = false;
		POINT newVelocity;
		newVelocity.x = velocity.x;
		newVelocity.y = velocity.y;
		if (newPos.x < bounds.left) {
			bounce = true;
			newPos.x = bounds.left;
			newVelocity.x = -newVelocity.x;
		}
		else if (newPos.x + bitmap->getWidth() > bounds.right) {
			bounce = true;
			newPos.x = bounds.right - bitmap->getWidth();
			newVelocity.x = -newVelocity.x;
		}

		if (newPos.y < bounds.top) {
			bounce = true;
			newPos.y = bounds.top;
			newVelocity.y = -newVelocity.y;
		}
		else if (newPos.y + bitmap->getHeight() > bounds.bottom) {
			bounce = true;
			newPos.y = bounds.bottom - bitmap->getHeight();
			newVelocity.y = -newVelocity.y;
		}

		if (bounce) {
			setVelocity(newVelocity);
		}

	}

	else if (boundsAction == BA_WRAP) {
		if (newPos.x + spriteSize.x < bounds.left) {
			newPos.x = bounds.right;
		}
		else if (newPos.x > bounds.right) {
			newPos.x = bounds.left - spriteSize.x;
		}

		if (newPos.y + spriteSize.y < bounds.top) {
			newPos.y = bounds.bottom;
		}
		else if (newPos.y > bounds.bottom) {
			newPos.y = bounds.top - spriteSize.y;
		}
	}

	else if (boundsAction == BA_DIE) {
		if (newPos.x + spriteSize.x < bounds.left ||
			newPos.x > bounds.right ||
			newPos.y + spriteSize.y < bounds.top ||
			newPos.y > bounds.bottom) {
			return SA_KILL;
		}
	}

	//	if (boundsAction == BA_STOP) {
	else {
		if (newPos.x < bounds.left) {
			setVelocity(0, 0);
			newPos.x = bounds.left;
		}
		else if (newPos.x + spriteSize.x > bounds.right) {
			setVelocity(0, 0);
			newPos.x = min(bounds.left, (bounds.right - spriteSize.x));
		}
		if (newPos.y < bounds.top) {
			setVelocity(0, 0);
			newPos.y = bounds.top;
		}
		else if (newPos.y + spriteSize.y > bounds.bottom) {
			setVelocity(0, 0);
			newPos.y = min(bounds.top, (bounds.bottom - spriteSize.y));
		}
	}

	setPosition(newPos);
	return SA_NONE;
}

void Sprite::Draw(HDC hdc) {
	if (bitmap != NULL && !hidden) {
		if (numFrames == 1)
			bitmap->draw(hdc, position.left, position.top, true);
		else {
			bitmap->drawPart(hdc, position.left, position.top,
				0, curFrame * getHeight(), getWidth(), getHeight(), true);
		}
	}
}

bool Sprite::isPointInside(int x, int y) {
	POINT p;
	p.x = x;
	p.y = y;
	return PtInRect(&position, p);
}

void Sprite::setNumFrames(int num, bool one) {

	numFrames = num;
	oneCycle = one;

	RECT rect = getPosition();
	rect.bottom = rect.top + ((rect.bottom - rect.top) / num);
	setPosition(rect);
	curFrame = 0;

}

void Sprite::updateFrame() {

	if (frameDelay >= 0 && (--frameTrigger <= 0)) {

		frameTrigger = frameDelay;
		if (++curFrame >= numFrames) {

			if (oneCycle) {
				dying = true;
			}
			else {
				curFrame = 0;
			}

		}

	}

}

void Sprite::setPosition(POINT p) {
	OffsetRect(&position, p.x - position.left, p.y - position.top);
	calcCollisionRect();
}

void Sprite::setPosition(int x, int y) {
	OffsetRect(&position, x - position.left, y - position.top);
	calcCollisionRect();
}

void Sprite::offSetPosition(int x, int y) {
	OffsetRect(&position, x, y);
	calcCollisionRect();
}

void Sprite::setVelocity(int x, int y) {
	velocity.x = x;
	velocity.y = y;
}

void Sprite::setVelocity(POINT p) {
	velocity.x = p.x;
	velocity.y = p.y;
}

void Sprite::setBounds(RECT& r) {
	CopyRect(&bounds, &r);
}

Sprite::~Sprite() {
	//nothing, yay!
}

bool Sprite::testCollision(Sprite* s) {
	RECT& test = s->getCollision();
	return collision.left <= test.right &&
		collision.right >= test.left &&
		collision.top <= test.bottom &&
		collision.bottom >= test.top;
}

void Sprite::calcCollisionRect() {
	int xShrink = (position.left - position.right) / 12;
	int yShrink = (position.top - position.bottom) / 12;
	CopyRect(&collision, &position);
	InflateRect(&collision, xShrink, yShrink);
}