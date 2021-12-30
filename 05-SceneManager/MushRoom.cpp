#include "MushRoom.h"
#include "PlayScene.h"
#include "Block.h"

CMushRoom::CMushRoom(int type) {
	SetAppear(false);
	typeMushRoom = type;
}

void CMushRoom::Render() {
	if (isAppear && !isDeleted) {
		if (typeMushRoom == MUSHROOM_GREEN) {
			animation_set->at(1)->Render(x, y);
		}
		else animation_set->at(0)->Render(x, y);
	}
	RenderBoundingBox();
}

void CMushRoom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	if (isDeleted) return;

	if (state == MUSHROOM_STATE_UP)
	{
		y += vy * dt;
		if (start_y - y >= MUSHROOM_BBOX_HEIGHT)
		{
			vy = 0;
			y = start_y - MUSHROOM_BBOX_HEIGHT - 0.1f;
			SetState(MUSHROOM_STATE_RIGHT);
		}
	}

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}


void CMushRoom::OnNoCollision(DWORD dt) {
	if (state == MUSHROOM_STATE_RIGHT) {
		x += vx * dt;
		y += vy * dt;
		vy = MUSHROOM_GRAVITY;
	}
}


void CMushRoom::OnCollisionWith(LPCOLLISIONEVENT e)
{

	CBlock* block = dynamic_cast<CBlock*>(e->obj);

	if (state == MUSHROOM_STATE_RIGHT) {
		if (e->ny != 0 && e->obj->IsBlocking() || block)
		{
			vy = 0;
		}
		else
			if (e->nx != 0 && e->obj->IsBlocking())
			{
				vx = -vx;
			}
	}
}

void CMushRoom::SetState(int state) {
	CGameObject::SetState(state);
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	switch (state)
	{
	case MUSHROOM_STATE_IDLE:
		vy = vx = 0;
		break;
	case MUSHROOM_STATE_UP:
		vy = -0.05f;
		start_y = y;
		break;
	case MUSHROOM_STATE_RIGHT:
		vy = MUSHROOM_GRAVITY;
		vx = -mario->GetMarioDirection() * MUSHROOM_SPEED;
		break;
	case MUSHROOM_STATE_LEFT:
		vy = MUSHROOM_GRAVITY;
		break;
	}
}