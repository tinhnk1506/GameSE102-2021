#pragma once
#include "Brick.h"

#define BREAKABLEBRICK_ANI_SET_ID 12
#define PIECE_ANI_SET_ID	126
#define COIN_ANI_SET_ID 6

class BreakableBrick :
	public CBrick
{
public:
	BreakableBrick(float x, float y, int type, int item, int totalItems) :CBrick(x, y, type, item, totalItems) {}
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* colliable_objects = NULL);
	virtual void Render();
	void Break();
	void ChangeToCoin();

	virtual int IsCollidable() { return 1; };
	virtual int IsBlocking() { return 1; }
	//virtual void OnCollisionWith(LPCOLLISIONEVENT e);
};