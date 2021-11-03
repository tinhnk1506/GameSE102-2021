#include <algorithm>
#include "debug.h"

#include "Mario.h"
#include "Game.h"

#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"

#include "Collision.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;
	DebugOut(L"[INFO] CMario::Update  %f ::\n", vx);
	if (abs(vx) > abs(maxVx)) vx = maxVx;

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	isOnPlatform = false;

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else
		if (e->nx != 0 && e->obj->IsBlocking())
		{
			vx = 0;
		}

	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			goomba->SetState(GOOMBA_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else // hit by Goomba
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != GOOMBA_STATE_DIE)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					DebugOut(L">>> Mario DIE >>> \n");
					SetState(MARIO_STATE_DIE);
				}
			}
		}
	}
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	coin++;
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

//
// Get animation ID for small Mario
//
int CMario::GetAniIdSmall()
{
	int aniId = -1;
	//if (!isOnPlatform)
	//{
	//	if (abs(ax) == MARIO_ACCEL_RUN_X)
	//	{
	//		if (nx >= 0)
	//			aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
	//		else
	//			aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
	//	}
	//	else
	//	{
	//		if (nx >= 0)
	//			aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
	//		else
	//			aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
	//	}
	//}
	//else
	//	if (isSitting)
	//	{
	//		if (nx > 0)
	//			aniId = ID_ANI_MARIO_SIT_RIGHT;
	//		else
	//			aniId = ID_ANI_MARIO_SIT_LEFT;
	//	}
	//	else
	//		if (vx == 0)
	//		{
	//			if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
	//			else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
	//		}
	//		else if (vx > 0)
	//		{
	//			if (ax < 0)
	//				aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
	//			else if (ax == MARIO_ACCEL_RUN_X)
	//				aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
	//			else if (ax == MARIO_ACCEL_WALK_X)
	//				aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
	//		}
	//		else // vx < 0
	//		{
	//			if (ax > 0)
	//				aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
	//			else if (ax == -MARIO_ACCEL_RUN_X)
	//				aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
	//			else if (ax == -MARIO_ACCEL_WALK_X)
	//				aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
	//		}

	//if (aniId == -1) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;

	return MARIO_ANI_SMALL_IDLE_RIGHT;
}


//
// Get animdation ID for big Mario
//
int CMario::GetAniIdBig()
{
	int aniId = -1;
	//if (!isOnPlatform)
	//{
	//	if (abs(ax) == MARIO_ACCEL_RUN_X)
	//	{
	//		if (nx >= 0)
	//			aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
	//		else
	//			aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
	//	}
	//	else
	//	{
	//		if (nx >= 0)
	//			aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
	//		else
	//			aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
	//	}
	//}
	//else
	//	if (isSitting)
	//	{
	//		if (nx > 0)
	//			aniId = ID_ANI_MARIO_SIT_RIGHT;
	//		else
	//			aniId = ID_ANI_MARIO_SIT_LEFT;
	//	}
	//	else
	//		if (vx == 0)
	//		{
	//			if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
	//			else aniId = ID_ANI_MARIO_IDLE_LEFT;
	//		}
	//		else if (vx > 0)
	//		{
	//			if (ax < 0)
	//				aniId = ID_ANI_MARIO_BRACE_RIGHT;
	//			else if (ax == MARIO_ACCEL_RUN_X)
	//				aniId = ID_ANI_MARIO_RUNNING_RIGHT;
	//			else if (ax == MARIO_ACCEL_WALK_X)
	//				aniId = ID_ANI_MARIO_WALKING_RIGHT;
	//		}
	//		else // vx < 0
	//		{
	//			if (ax > 0)
	//				aniId = ID_ANI_MARIO_BRACE_LEFT;
	//			else if (ax == -MARIO_ACCEL_RUN_X)
	//				aniId = ID_ANI_MARIO_RUNNING_LEFT;
	//			else if (ax == -MARIO_ACCEL_WALK_X)
	//				aniId = ID_ANI_MARIO_WALKING_LEFT;
	//		}

	//if (aniId == -1) aniId = ID_ANI_MARIO_IDLE_RIGHT;

	return MARIO_ANI_BIG_IDLE_RIGHT;
}

void CMario::Render()
{
	int aniId = -1;

	if (state == MARIO_STATE_DIE)
		aniId = MARIO_ANI_DIE;
	else if (level == MARIO_LEVEL_BIG)
		aniId = GetAniIdBig();

	else if (level == MARIO_LEVEL_SMALL)
		aniId = GetAniIdSmall();

	animation_set->at(aniId)->Render(x, y);
	//RenderBoundingBox();

	//DebugOutTitle(L"Coins: %d", coin);
}

void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return;

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		//maxVx = MARIO_WALKING_SPEED;
		ax = -MARIO_ACCELERATION;
		nx = 1;
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) break;
		//maxVx = -MARIO_WALKING_SPEED;
		ax = -MARIO_ACCELERATION;
		nx = -1;
		break;
	case MARIO_STATE_JUMPING:
		if (isSitting) break;

		break;

		

	case MARIO_STATE_IDLE:
		ax = 0.0f;
		vx = 0.0f;
		break;

	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;
	}

	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	if (level == MARIO_LEVEL_SMALL)
	{
		right = left + MARIO_BIG_BBOX_WIDTH;
		bottom = top + MARIO_BIG_BBOX_HEIGHT;
		if (state == MARIO_STATE_SITTING)
		{
			bottom -= MARIO_BIG_BBOX_HEIGHT - MARIO_BIG_BBOX_SITTING_HEIGHT;
		}
	}
	else
	{
		/*if (transformTimer.IsStarted())
		{
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}*/
		//else {
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
		//}
	}
}

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
	}
	level = l;
}

