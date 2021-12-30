#pragma once

#include "GameObject.h"
#include "Animations.h"

#define ID_ANI_COIN 11000

#define	COIN_WIDTH 10
#define COIN_BBOX_WIDTH 10
#define COIN_BBOX_HEIGHT 16

#define COIN_STATE_IDLE		0
#define COIN_STATE_UP		100
#define COIN_STATE_DOWN		111

#define COIN_TYPE_NORMAL	6
#define COIN_TYPE_INBRICK	61
#define COIN_TYPE_TRANSFORM	60

#define COIN_FALLING_TIME			300
#define COIN_BEING_PUSHEDUP_TIME	300
#define COIN_EXIST_TIME				10000

#define COIN_SPEED 0.2f

class CCoin : public CGameObject {
	ULONGLONG timing_start = 0;
	ULONGLONG exist_start = 0;
	bool isAppear = false;
	//int coinPoint = 0;

public:
	CCoin(float x, float y) : CGameObject(x, y) {}
	CCoin(int type = COIN_TYPE_NORMAL);

	virtual int IsCollidable() { return 0; };
	void Render();
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void OnNoCollision(DWORD dt);

	void SetAppear(bool appear) { isAppear = appear; }
	void SetState(int state);

	bool GetAppear() { return isAppear; }

	void StartTiming()
	{
		timing_start = GetTickCount64();
	}

	void StartExist()
	{
		exist_start = GetTickCount64();
	}

	int IsBlocking() { return 0; }
	int IsBlocking(bool isBlocking) { return isBlocking; }
};