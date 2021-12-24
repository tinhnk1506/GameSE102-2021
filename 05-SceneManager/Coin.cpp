#include "Coin.h"
#include "PlayScene.h"
#include "Mario.h"

void CCoin::Render()
{
	if (!isAppear || isDeleted) {
		return;
	}

	animation_set->at(0)->Render(x, y);

	//RenderBoundingBox();
}

CCoin::CCoin(int tag) : CGameObject() {
	if (tag == COIN_TYPE_INBRICK)
		isAppear = false;
	else
		isAppear = true;
	if (tag == COIN_TYPE_TRANSFORM)
		StartExist();
	state = COIN_STATE_IDLE;
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - COIN_BBOX_WIDTH / 2;
	t = y - COIN_BBOX_HEIGHT / 2;
	r = l + COIN_BBOX_WIDTH;
	b = t + COIN_BBOX_HEIGHT;
}

void CCoin::OnNoCollision(DWORD dt) {
	y += vy * dt;
}

void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	

	if (isDeleted)
		return;

	y += vy * dt;

	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CMario* mario = currentScene->GetPlayer();

	if (state == COIN_STATE_UP)
	{
		if (GetTickCount64() - timing_start >= COIN_FALLING_TIME)
		{
			SetState(COIN_STATE_DOWN);
			StartTiming();
		}
	}
	if (state == COIN_STATE_DOWN)
	{
		if (GetTickCount64() - timing_start >= COIN_FALLING_TIME)
		{
			isAppear = false;
			SetState(COIN_STATE_IDLE);
			mario->AddScore(this->x, this->y, 100, false);
			mario->AddCoin();
			Delete();
		}
	}
}

void CCoin::SetState(int state) {
	CGameObject::SetState(state);
	switch (state)
	{
	case COIN_STATE_IDLE:
		vx = vy = 0;
		break;
	case COIN_STATE_UP:
		vy = -COIN_SPEED;
		StartTiming();
		break;
	case COIN_STATE_DOWN:
		vy = COIN_SPEED;
		break;
	}
}
