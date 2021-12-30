#pragma once
#include "GameObject.h"

#define SWITCH_BBOX_WIDTH			16
#define SWITCH_BBOX_HEIGHT			16
#define SWITCH_BBOX_PRESSED_HEIGHT	7

#define SWITCH_ANI_SET_ID		77

#define SWITCH_ANI_IDLE		0
#define SWITCH_ANI_PRESSED	1


#define SWITCH_STATE_IDLE		0
#define SWITCH_STATE_UP			111
#define SWITCH_STATE_PRESSED	122

class Switch :public CGameObject
{
	bool isAppear = false;
public:
	float start_y = 0;
	virtual void Render();
	bool GetIsAppear() { return isAppear; }
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	void SetState(int state);
	void SetAppear(bool appear) { this->isAppear = appear; }
	void ChangeBrickToCoin();
	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 0; }
};
