#pragma once

#include <Windows.h>
#include <d3dx10.h>
#include <vector>

#include "Animations.h"
#include "Sprites.h"
#include "Collision.h"

using namespace std;
#define STATIC	0
#define MOVING	1
#define IGNORE_DEFINE	2

#define ID_TEX_BBOX -100		// special texture to draw object bounding box
#define BBOX_ALPHA 0.25f		// Bounding box transparency

class CGameObject
{
protected:
	int isBlocking = 1;

	float z = 0; // For render coin in brick and other items or enemies inside Brick and Pipe



	float start_x;
	float start_y;





	LPANIMATION_SET animation_set;

public:
	bool isDeleted;

	void SetPosition(float x, float y) {
		this->x = x, this->y = y;
		this->start_x = x, this->start_y = y;
	}
	float x;
	float y;
	int nx;
	float vx;
	float vy;
	int state;
public:
	int tag = 0;
	int type = 0;

	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
	void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }
	void SetTag(int tag) { this->tag = tag; }
	void SetType(int type) { this->type = type; }

	int GetState() { return this->state; }
	virtual void Delete() { isDeleted = true; }
	bool IsDeleted() { return isDeleted; }

	void SetZIndex(int z) {
		this->z = z;
	}

	int GetZIndex() {
		return this->z;
	}

	void RenderBoundingBox();

	CGameObject();
	CGameObject(float x, float y) :CGameObject() { this->x = x; this->y = y; }

	void SetAnimationSet(LPANIMATION_SET ani_set) { animation_set = ani_set; }
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) = 0;
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = NULL) {};
	virtual void Render() = 0;
	virtual void SetState(int state) { this->state = state; }

	//
	// Collision ON or OFF ? This can change depending on object's state. For example: die
	//
	virtual int IsCollidable() { return 0; };

	// When no collision has been detected (triggered by CCollision::Process)
	virtual void OnNoCollision(DWORD dt) {};

	// When collision with an object has been detected (triggered by CCollision::Process)
	virtual void OnCollisionWith(LPCOLLISIONEVENT e) {};

	// Is this object blocking other object? If YES, collision framework will automatically push the other object
	virtual int IsBlocking() { return this->isBlocking; }
	virtual void SetIsBlocking(int blocking) {
		this->isBlocking = blocking;
	}

	//GET
	float getX() { return x; }
	float getY() { return y; }

	~CGameObject();

	static bool IsDeleted(const LPGAMEOBJECT& o) { return o->isDeleted; }
};
