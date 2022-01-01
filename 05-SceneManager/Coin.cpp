#include "Coin.h"
#include "PlayScene.h"
#include "Mario.h"
#include "BreakableBrick.h"
#include "debug.h"

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
	if (tag == COIN_TYPE_TRANSFORM) {
		StartExist();
		this->tag = tag;
	}
		
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

	if (GetTickCount64() - exist_start > COIN_EXIST_TIME && this->tag == COIN_TYPE_TRANSFORM) {
		DebugOut(L"IN HERE WITH TAG 60");
		ChangeCoinToBrick();
	}

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

void CCoin::ChangeCoinToBrick() {
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	vector<LPGAMEOBJECT> objects = currentScene->GetObjects();
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(BREAKABLEBRICK_ANI_SET_ID);
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<CCoin*>(objects.at(i)) && !objects.at(i)->isDeleted) {
			CCoin* coin = dynamic_cast<CCoin*>(objects.at(i));
			if (coin->tag == COIN_TYPE_TRANSFORM) {
				DebugOut(L"coin->tag::%d\n", coin->tag);
				BreakableBrick* brick = new BreakableBrick(coin->x, coin->y);
				brick->SetAnimationSet(ani_set);
				currentScene->AddObject(brick);
				coin->isDeleted = true;
			}
			//CCoin* coin = new CCoin(COIN_TYPE_TRANSFORM);
			//coin->SetPosition(bBrick->x, bBrick->y);
			//coin->SetAppear(true);
		}
	}
}