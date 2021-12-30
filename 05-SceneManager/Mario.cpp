#include <algorithm>
#include "Mario.h"
#include "debug.h"
#include "Game.h"
#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "Collision.h"
#include "Block.h"
#include "QuestionBrick.h"
#include "MushRoom.h"
#include "Koopas.h"
#include "Leaf.h"
#include "FireBullet.h"
#include "PiranhaPlant.h"
#include "Switch.h"
#include "Card.h"
#include "PlayScene.h"
#include "Point.h"


void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	this->marioDt = dt;

	if (!isFlying && !isTailFlying)
		HandleMarioJump();
	HandleFlying();
	HandleTransform(level);
	HandleTurning();
	HandleFlapping();
	HandleMarioKicking();
	HandleChangeDirection();
	HandleSpeedStack();
	HandleSwitchMap();
	HandleFinishScene();

	if (GetTickCount64() - start_score_time >= STACK_SCORE_TIME && isStackingScore) {
		isStackingScore = false;
		stackScoreTimes = 0;
	}

	if (abs(vx) > abs(maxVx)) vx = maxVx;

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	//isOnPlatform = false;

	//DebugOut(L"Mario->vx::%f\n", vx);
	tail->Update(dt, coObjects);
	//tail->ShowHitEffect();
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
	OnNoCollistionX(dt);
	OnNoCollistionY(dt);
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{

	if (!dynamic_cast<CBlock*>(e->obj)) {
		if (e->ny != 0 && e->obj->IsBlocking())
		{
			vy = 0;
			ay = MARIO_GRAVITY;
			if (e->ny < 0) {
				isOnPlatform = true;
			}
		}
		else
		{
			if (e->nx != 0 && e->obj->IsBlocking())
			{
				vx = 0;
			}
		}
	}

	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<QuestionBrick*>(e->obj))
		OnCollisionWithQuestionBrick(e);
	else if (dynamic_cast<CMushRoom*>(e->obj))
		OnCollisionWithMushRoom(e);
	else if (dynamic_cast<CKoopas*>(e->obj))
		OnCollisionWithKoopas(e);
	else if (dynamic_cast<CLeaf*>(e->obj))
		OnCollisionWithLeaf(e);
	else if (dynamic_cast<FireBullet*>(e->obj))
		OnCollisionWithFireBullet(e);
	else if (dynamic_cast<PiranhaPlant*>(e->obj))
		HandleBasicMarioDie();
	else if (dynamic_cast<Switch*>(e->obj))
		OnCollisionWithPSwitch(e);
	else if (dynamic_cast<CardItem*>(e->obj))
		OnCollisionWithPCardItem(e);
}

void CMario::OnCollisionWithPCardItem(LPCOLLISIONEVENT e) {
	CardItem* card = dynamic_cast<CardItem*>(e->obj);
	if (e->ny != 0 || e->nx != 0) {
		card->SetAppear(false);
		card->isDeleted = true;
		AddCard(card->state - 1);
		isFinish = true;
	}
}

void CMario::OnCollisionWithPSwitch(LPCOLLISIONEVENT e) {
	Switch* sw = dynamic_cast<Switch*>(e->obj);
	if (e->ny < 0) {
		if (sw->GetState() != SWITCH_STATE_PRESSED) {
			sw->SetState(SWITCH_STATE_PRESSED);
			sw->isDeleted = true;
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	// jump on top >> kill Goomba and deflect a bit 
	if (e->ny < 0)
	{
		if (goomba->GetState() != GOOMBA_STATE_DIE)
		{
			if (goomba->tag == GOOMBA_RED)
				goomba->SetTag(GOOMBA_RED_NORMAL);
			else if (goomba->tag == GOOMBA_SUPER)
				goomba->SetTag(GOOMBA_NORMAL);
			else
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
				HandleBasicMarioDie();
			}
		}
	}
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	AddScore(this->x, this->y, 100, false);
	AddCoin();
	coin++;
}

void CMario::OnCollisionWithFireBullet(LPCOLLISIONEVENT e) {
	e->obj->Delete();
	HandleBasicMarioDie();
}

void CMario::OnCollisionWithLeaf(LPCOLLISIONEVENT e)
{
	CLeaf* leaf = dynamic_cast<CLeaf*>(e->obj);
	if (e->ny != 0 || e->nx != 0) {
		if (level != MARIO_LEVEL_TAIL) StartTransform(MARIO_LEVEL_TAIL);
		leaf->SetAppear(false);
		//leaf->SetIsDestroyed(true);
		leaf->vy = 50.0f;
		AddScore(this->x, this->y, 1000);
		e->obj->Delete();
	}
}

void CMario::OnCollisionWithKoopas(LPCOLLISIONEVENT e) {
	CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
	if (e->nx != 0) {
		if (koopas->GetState() == KOOPAS_STATE_IN_SHELL || koopas->GetState() == KOOPAS_STATE_SHELL_UP) {
			if (isReadyToHold) {
				isHolding = true;
				koopas->SetIsBeingHeld(true);
			}
			else {
				SetState(MARIO_STATE_KICK);
				koopas->SetState(KOOPAS_STATE_SPINNING);
			}
		}
		else {
			HandleBasicMarioDie();
		}
	}
	if (e->ny > 0) {
		if (koopas->GetState() == KOOPAS_STATE_IN_SHELL || koopas->GetState() == KOOPAS_STATE_SHELL_UP) {
			//SetState(MARIO_STATE_KICK);
			koopas->SetState(KOOPAS_STATE_SPINNING);
		}
		else {
			koopas->x = this->x + nx * 2;
			HandleBasicMarioDie();
		}
	}
	if (e->ny < 0) {
		CGame* game = CGame::GetInstance();

		vy = -MARIO_JUMP_DEFLECT_SPEED;
		if (koopas->GetState() == KOOPAS_STATE_WALKING) {
			if (koopas->tag == KOOPAS_GREEN_PARA) {
				koopas->SetTag(KOOPAS_GREEN);
			}
			else koopas->SetState(KOOPAS_STATE_IN_SHELL);
		}
		else if (koopas->GetState() == KOOPAS_STATE_IN_SHELL) {
			koopas->SetState(KOOPAS_STATE_SPINNING);
		}
		else if (koopas->GetState() == KOOPAS_STATE_SPINNING) {
			koopas->SetState(KOOPAS_STATE_IN_SHELL);
		}
		AddScore(this->x, this->y, 100);
	}


}

void CMario::OnCollisionWithMushRoom(LPCOLLISIONEVENT e)
{
	CMushRoom* mushRoom = dynamic_cast<CMushRoom*>(e->obj);
	mushRoom->Delete();
	AddScore(this->x, this->y, 100);

	if (mushRoom->GetTypeMushRoom() != MUSHROOM_GREEN)
		StartTransform(MARIO_LEVEL_BIG);
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	portal = (CPortal*)e->obj;
	//CGame::GetInstance()->SwitchExtraScene(portal->GetSceneId(), portal->start_x, portal->start_y, portal->pipeUp);
}

void CMario::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e)
{
	QuestionBrick* qBrick = dynamic_cast<QuestionBrick*>(e->obj);

	// Hit from bottom
	if (e->ny > 0) {
		vy = 0;
		qBrick->SetState(QUESTION_BRICK_HIT);
	}
}

int CMario::GetAniIdSmall()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = MARIO_ANI_SMALL_JUMPINGUP_RIGHT;
			else
				aniId = MARIO_ANI_SMALL_JUMPINGUP_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = MARIO_ANI_SMALL_JUMPINGUP_RIGHT;
			else
				aniId = MARIO_ANI_SMALL_JUMPINGUP_LEFT;
		}
	}
	if (state == MARIO_STATE_JUMP || state == MARIO_STATE_RELEASE_JUMP || isHolding || isKick) {
		if (nx > 0) {
			aniId = MARIO_ANI_SMALL_JUMPINGUP_RIGHT;
			if (isFlying) {
				aniId = MARIO_ANI_SMALL_FLY_RIGHT;
			}
			if (isHolding) {
				aniId = MARIO_ANI_SMALL_HOLD_RUNNING_RIGHT;
			}
			else if (isKick)
			{
				aniId = MARIO_ANI_SMALL_KICKING_RIGHT;
			}
		}
		if (nx < 0) {
			aniId = MARIO_ANI_SMALL_JUMPINGUP_LEFT;
			if (isFlying) {
				aniId = MARIO_ANI_SMALL_FLY_LEFT;
			}
			if (isHolding) {
				aniId = MARIO_ANI_SMALL_HOLD_RUNNING_LEFT;
			}
			else if (isKick)
			{
				aniId = MARIO_ANI_SMALL_KICKING_LEFT;
			}
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = MARIO_ANI_SMALL_IDLE_RIGHT;
			else
				aniId = MARIO_ANI_SMALL_IDLE_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) {
					aniId = MARIO_ANI_SMALL_IDLE_RIGHT;
					if (isKick) {
						aniId = MARIO_ANI_SMALL_KICKING_RIGHT;
					}
				}
				else {
					aniId = MARIO_ANI_SMALL_IDLE_LEFT;
					if (isKick) {
						aniId = MARIO_ANI_SMALL_KICKING_LEFT;
					}
				}
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = MARIO_ANI_SMALL_BRAKING_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
					aniId = MARIO_ANI_SMALL_RUNNING_RIGHT;
				else if (ax == MARIO_ACCEL_WALK_X) {
					aniId = MARIO_ANI_SMALL_WALKING_RIGHT;
				}

				if (!isOnPlatform) {
					aniId = MARIO_ANI_SMALL_JUMPINGUP_RIGHT;
					if (isFlying) {
						aniId = MARIO_ANI_SMALL_FLY_RIGHT;
					}
				}
				if (isKick) {
					aniId = MARIO_ANI_SMALL_KICKING_RIGHT;
				}

			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = MARIO_ANI_SMALL_BRAKING_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X)
					aniId = MARIO_ANI_SMALL_RUNNING_LEFT;
				else if (ax == -MARIO_ACCEL_WALK_X)
					aniId = MARIO_ANI_SMALL_WALKING_LEFT;

				if (!isOnPlatform) {
					aniId = MARIO_ANI_SMALL_JUMPINGUP_LEFT;
					if (isFlying) {
						aniId = MARIO_ANI_SMALL_FLY_LEFT;
					}
				}
				if (isKick) {
					aniId = MARIO_ANI_SMALL_KICKING_LEFT;
				}
			}

	if (aniId == -1) aniId = MARIO_ANI_SMALL_IDLE_RIGHT;

	return aniId;
}

int CMario::GetAniIdBig()
{
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = MARIO_ANI_BIG_RUNNING_RIGHT;
			else
				aniId = MARIO_ANI_BIG_RUNNING_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = MARIO_ANI_BIG_WALKING_FAST_RIGHT;
			else
				aniId = MARIO_ANI_BIG_WALKING_FAST_LEFT;
		}
	}
	if (state == MARIO_STATE_JUMP || state == MARIO_STATE_RELEASE_JUMP) {
		if (nx > 0) {
			aniId = MARIO_ANI_BIG_JUMPINGUP_RIGHT;
			/*if (isFlying) {
				ani = MARIO_ANI_BIG_FLY_RIGHT;
			}*/
			if (isHolding) {
				aniId = MARIO_ANI_BIG_HOLD_JUMPINGUP_RIGHT;
			}
			else if (isKick)
			{
				aniId = MARIO_ANI_BIG_KICKING_RIGHT;

			}
		}
		if (nx < 0) {
			aniId = MARIO_ANI_BIG_JUMPINGUP_LEFT;
			/*if (isFlying) {
				ani = MARIO_ANI_SMALL_FLY_LEFT;
			}*/
			if (isHolding) {
				aniId = MARIO_ANI_BIG_HOLD_JUMPINGUP_LEFT;
			}
			else if (isKick)
			{
				aniId = MARIO_ANI_BIG_KICKING_LEFT;
			}
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0) {
				aniId = MARIO_ANI_BIG_SITTING_RIGHT;
			}
			else
				aniId = MARIO_ANI_BIG_SITTING_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) {
					aniId = MARIO_ANI_BIG_IDLE_RIGHT;
					if (isKick) {
						aniId = MARIO_ANI_BIG_KICKING_RIGHT;
					}
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_IDLE_RIGHT;
					}
				}
				else {
					aniId = MARIO_ANI_BIG_IDLE_LEFT;
					if (isKick) {
						aniId = MARIO_ANI_BIG_KICKING_LEFT;
					}
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_IDLE_LEFT;
					}
				}
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = MARIO_ANI_BIG_BRAKING_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
				{
					aniId = MARIO_ANI_BIG_RUNNING_RIGHT;
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_RUNNING_RIGHT;
					}
				}
				else if (ax == MARIO_ACCEL_WALK_X) {
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_WALKING_RIGHT;
					}
					aniId = MARIO_ANI_BIG_WALKING_RIGHT;
				}

				if (!isOnPlatform) {
					aniId = MARIO_ANI_BIG_JUMPINGUP_RIGHT;
					if (isFlying) {
						aniId = MARIO_ANI_BIG_FLY_RIGHT;
					}
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_JUMPINGUP_RIGHT;
					}
				}

				if (isKick) {
					aniId = MARIO_ANI_BIG_KICKING_RIGHT;
				}
			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = MARIO_ANI_BIG_BRAKING_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X) {
					aniId = MARIO_ANI_BIG_RUNNING_LEFT;
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_RUNNING_LEFT;
					}
				}
				else if (ax == -MARIO_ACCEL_WALK_X)
				{
					aniId = MARIO_ANI_BIG_WALKING_LEFT;
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_WALKING_LEFT;
					}
				}

				if (!isOnPlatform) {
					aniId = MARIO_ANI_BIG_JUMPINGUP_LEFT;
					if (isFlying) {
						aniId = MARIO_ANI_BIG_FLY_LEFT;
					}
					if (isHolding) {
						aniId = MARIO_ANI_BIG_HOLD_JUMPINGUP_LEFT;
					}
				}

				if (isKick) {
					aniId = MARIO_ANI_BIG_KICKING_LEFT;
				}
			}

	if (aniId == -1) aniId = MARIO_ANI_BIG_IDLE_RIGHT;

	return aniId;
}

int CMario::GetAniIdTail() {
	int aniId = -1;
	if (!isOnPlatform)
	{
		if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx >= 0)
				aniId = MARIO_ANI_TAIL_RUNNING_RIGHT;
			else
				aniId = MARIO_ANI_TAIL_RUNNING_LEFT;
		}
		else
		{
			if (nx >= 0)
				aniId = MARIO_ANI_TAIL_WALKING_FAST_RIGHT;
			else
				aniId = MARIO_ANI_TAIL_WALKING_FAST_LEFT;
		}
	}
	if (state == MARIO_STATE_JUMP || state == MARIO_STATE_RELEASE_JUMP || isHolding || isKick) {
		if (nx > 0) {
			aniId = MARIO_ANI_TAIL_JUMPINGUP_RIGHT;
			if (isTailFlying) {
				aniId = MARIO_ANI_TAIL_FLY_UP_RIGHT;
			}
			if (isFlapping)
				aniId = MARIO_ANI_TAIL_FLAPPING_RIGHT;
			else if (isFlappingTailFlying)
				aniId = MARIO_ANI_TAIL_FLY_FLAPPING_RIGHT;
			else if (isHolding) {
				aniId = MARIO_ANI_TAIL_HOLD_RUNNING_RIGHT;
			}
			else if (isKick)
			{
				aniId = MARIO_ANI_TAIL_KICKING_RIGHT;
			}
		}
		if (nx < 0) {
			aniId = MARIO_ANI_TAIL_JUMPINGUP_LEFT;
			if (isTailFlying) {
				aniId = MARIO_ANI_TAIL_FLY_UP_LEFT;
			}
			if (isFlapping)
				aniId = MARIO_ANI_TAIL_FLAPPING_LEFT;
			else if (isFlappingTailFlying)
				aniId = MARIO_ANI_TAIL_FLY_FLAPPING_LEFT;
			else if (isHolding) {
				aniId = MARIO_ANI_TAIL_HOLD_RUNNING_LEFT;
			}
			else if (isKick)
			{
				aniId = MARIO_ANI_TAIL_KICKING_LEFT;
			}
		}
	}
	else
		if (isSitting)
		{
			if (nx > 0)
				aniId = MARIO_ANI_TAIL_SITTING_RIGHT;
			else
				aniId = MARIO_ANI_TAIL_SITTING_LEFT;
		}
		else
			if (vx == 0)
			{
				if (nx > 0) aniId = MARIO_ANI_TAIL_IDLE_RIGHT;
				else aniId = MARIO_ANI_TAIL_IDLE_LEFT;
			}
			else if (vx > 0)
			{
				if (ax < 0)
					aniId = MARIO_ANI_TAIL_BRAKING_RIGHT;
				else if (ax == MARIO_ACCEL_RUN_X)
				{
					aniId = MARIO_ANI_TAIL_RUNNING_RIGHT;
				}
				else if (ax == MARIO_ACCEL_WALK_X) {
					aniId = MARIO_ANI_TAIL_WALKING_RIGHT;
				}

				if (!isOnPlatform) {
					aniId = MARIO_ANI_TAIL_JUMPINGUP_RIGHT;
					if (isTailFlying) {
						aniId = MARIO_ANI_TAIL_FLY_UP_RIGHT;
						if (isFlappingTailFlying)
							aniId = MARIO_ANI_TAIL_FLY_FLAPPING_RIGHT;
					}
					if (isFlapping)
						aniId = MARIO_ANI_TAIL_FLAPPING_RIGHT;
				}
			}
			else // vx < 0
			{
				if (ax > 0)
					aniId = MARIO_ANI_TAIL_BRAKING_LEFT;
				else if (ax == -MARIO_ACCEL_RUN_X)
					aniId = MARIO_ANI_TAIL_RUNNING_LEFT;
				else if (ax == -MARIO_ACCEL_WALK_X)
					aniId = MARIO_ANI_TAIL_WALKING_LEFT;

				if (!isOnPlatform) {
					aniId = MARIO_ANI_TAIL_JUMPINGUP_LEFT;
					if (isTailFlying) {
						aniId = MARIO_ANI_TAIL_FLY_UP_LEFT;
						if (isFlappingTailFlying)
							aniId = MARIO_ANI_TAIL_FLY_FLAPPING_LEFT;
					}
					if (isFlapping)
						aniId = MARIO_ANI_TAIL_FLAPPING_LEFT;
				}
			}

	if (aniId == -1) aniId = MARIO_ANI_TAIL_IDLE_RIGHT;

	return aniId;
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
	else if (level == MARIO_LEVEL_TAIL)
	{
		aniId = GetAniIdTail();
	}

	if (state == MARIO_STATE_TRANSFORM) {
		if (nx > 0) {
			aniId = MARIO_ANI_TRANSFORM_SMALL_RIGHT;
		}
		else aniId = MARIO_ANI_TRANSFORM_SMALL_LEFT;
		if (level == MARIO_LEVEL_TAIL || isBangAni || level == MARIO_LEVEL_FIRE) {
			aniId = MARIO_ANI_TRANSFORM_BANG;
		}

	}
	if (isSitting) {
		animation_set->at(aniId)->Render(x, y + 5);
	}
	else if (level == MARIO_LEVEL_TAIL) {
		if (state == MARIO_STATE_TAIL_ATTACK)
		{
			DebugOut(L"[TURNING_STACK]::%d\n", turningStack);
			if (isTuring && nx > 0) {
				if (turningStack == 1 || turningStack == 5) CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_RIGHT_1_ID)->Draw(x - 8, y);
				if (turningStack == 2) CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_RIGHT_2_ID)->Draw(x, y);
				if (turningStack == 3) CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_RIGHT_3_ID)->Draw(x, y);
				if (turningStack == 4) CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_RIGHT_4_ID)->Draw(x, y);
			}
			if (isTuring && nx < 0) {
				if (turningStack == 1 || turningStack == 5) CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_LEFT_1_ID)->Draw(x, y);
				if (turningStack == 2) CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_LEFT_2_ID)->Draw(x, y);
				if (turningStack == 3) CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_LEFT_3_ID)->Draw(x - 8, y);
				if (turningStack == 4)
					CSprites::GetInstance()->Get(MARIO_SPRITE_WHACK_LEFT_4_ID)->Draw(x, y);
			}
		}

		else animation_set->at(aniId)->Render(nx > 0 ? x - MARIO_DIFF : x + MARIO_DIFF, y);
		this->tail->Render();
	}
	else {
		animation_set->at(aniId)->Render(x, y);
	}
	RenderBoundingBox();

}

void CMario::SetState(int state)
{
	// DIE is the end state, cannot be changed! 
	if (this->state == MARIO_STATE_DIE) return;

	switch (state)
	{
	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) break;
		maxVx = MARIO_RUNNING_SPEED;
		ax = MARIO_ACCEL_RUN_X;
		nx = 1;
		isReadyToRun = true;
		//runningStack++;
		if (vx > MARIO_SPEED_STACK && isReadyToRun) {
			isRunning = true;
		}
		else {
			isRunning = false;

		}
		break;
	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) break;
		maxVx = -MARIO_RUNNING_SPEED;
		ax = -MARIO_ACCEL_RUN_X;
		nx = -1;
		isReadyToRun = true;
		//runningStack++;
		if (vx < MARIO_SPEED_STACK && isReadyToRun) {
			isRunning = true;
		}
		else {
			isRunning = false;
		}
		break;
	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		maxVx = MARIO_WALKING_SPEED;
		ax = MARIO_ACCEL_WALK_X;
		isRunning = false;
		nx = 1;
		if (ax < 0 && abs(vx) > MARIO_WALKING_SPEED_START) {
			isChangeDirection = true;
			runningStack = 0;
		}
		else {
			runningStack++;
		}
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) break;
		maxVx = -MARIO_WALKING_SPEED;
		ax = -MARIO_ACCEL_WALK_X;
		isRunning = false;
		nx = -1;
		if (ax > 0 && abs(vx) > MARIO_WALKING_SPEED_START) {
			isChangeDirection = true;
			runningStack = 0;
		}
		else {
			runningStack++;
		}
		break;
	case MARIO_STATE_JUMP:
		if (isSitting) break;
		if (isOnPlatform)
		{
			if (vy > -MARIO_JUMP_SPEED_MIN)
				vy = -MARIO_JUMP_SPEED_MIN;
			ay = -MARIO_ACCELERATION_JUMP;
			isJumping = true;
		}
		if (abs(ax) == MARIO_ACCEL_RUN_X) {
			if (level == MARIO_LEVEL_TAIL) {
				isTailFlying = true;
				StartTailFlying();
			}
			else {
				isFlying = true;
				StartFlying();
			}
		}
		normalFlyPullDown = false;
		isOnPlatform = false;
		break;

	case MARIO_STATE_RELEASE_JUMP:
		pullDown();
		break;

	case MARIO_STATE_SIT:
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			isSitting = true;
			vx = 0; vy = 0.0f;
			y += MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;

	case MARIO_STATE_IDLE:
		ax = 0.0f;
		vx = 0.0f;
		break;
	case MARIO_STATE_TRANSFORM:
		vx = 0;
		vy = 0;
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		break;
	case MARIO_STATE_KICK:
		StartKicking();
		break;
	case MARIO_STATE_TAIL_ATTACK:
		if (!isTuring) {
			turningStack = 0;
			//isTuring = true;
			StartTurning();
		}
		/*if (previousState == MARIO_STATE_SITDOWN)
			y -= MARIO_BIG_BBOX_HEIGHT - MARIO_BBOX_SIT_HEIGHT;*/
		break;
	}
	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	if (level != MARIO_LEVEL_SMALL)
	{

		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
		if (state == MARIO_STATE_SIT) {
			bottom = top + MARIO_BBOX_SIT_HEIGHT;
		}
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	// Adjust position to avoid falling off platform
	if (this->level == MARIO_LEVEL_SMALL)
	{
		y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT);
	}
	level = l;
}

void CMario::HandleSwitchMap() {
	if (pipeDownTimer > MARIO_PIPE_TIME && isPipeDown)
	{
		StopPipeDown();
		if (isSwitchMap)
		{
			StopPipeDown();
			CGame::GetInstance()->SwitchExtraScene(portal->GetSceneId(), portal->start_x, portal->start_y, portal->pipeUp);
		}
		else if (isSwitchMap && isBackScene)
		{
			CGame::GetInstance()->SwitchBackScene(portal->GetSceneId(), portal->start_x, portal->start_y);
			isBackScene = false;
		}
		else
		{
			vx = vy = 0;
			ay = MARIO_GRAVITY;
		}

	}

	else if (pipeUpTimer > MARIO_PIPE_TIME && isPipeUp)
	{
		StopPipeUp();
		if (isSwitchMap)
		{
			CGame::GetInstance()->SwitchExtraScene(portal->GetSceneId(), portal->start_x, portal->start_y, portal->pipeUp);
		}
		else if (isSwitchMap && isBackScene)
		{
			CGame::GetInstance()->SwitchBackScene(portal->GetSceneId(), portal->start_x, portal->start_y);
			isBackScene = false;
		}
		else
		{
			vx = vy = 0;
			ay = MARIO_GRAVITY;
		}
	}
}

void CMario::HandleMarioJump() {
	if (isJumping) {
		//DebugOut(L"ax::%f\n", 0.0f);
		//DebugOut(L"vy::%f\n", vy);
		// Dung yen nhay
		if (vx == 0)
		{
			if (vy < -MARIO_JUMP_MAX) {
				pullDown();
			}
		}
		// Di chuyen nhay phai
		if (vx > 0) {
			// vx lon nhat
			if (vx >= MARIO_SPEED_MAX) {
				// super jump
				if (vy < -MARIO_SUPER_JUMP_MAX) {
					pullDown();
				}
			}
			else if (vx < MARIO_SPEED_MAX && vx > 0) {
				if (vy < -MARIO_JUMP_MAX) {
					pullDown();
				}
			}
		}
		//Di chuyen nhay trai
		if (vx < 0) {
			// vx lon nhat
			if (abs(vx) >= MARIO_SPEED_MAX) {
				// super jump
				if (vy < -MARIO_SUPER_JUMP_MAX) {
					pullDown();
				}
			}
			else if (abs(vx) < MARIO_SPEED_MAX && vx < 0) {
				if (vy < -MARIO_JUMP_MAX) {
					pullDown();
				}
			}
		}

	}
}

void CMario::HandleBasicMarioDie() {

	if (level != MARIO_LEVEL_SMALL)
	{
		level -= 1;
		StartTransform(level);
		StartUntouchable();
		DebugOut(L">>> Mario TRANSFORM >>>%d \n", level);
	}
	else
	{
		DebugOut(L">>> Mario DIE >>> \n");
		SetState(MARIO_STATE_DIE);
	}
}

void CMario::HandleSpeedStack() {
	if (GetTickCount64() - start_running > MARIO_RUNNING_STACK_TIME && isRunning && vx != 0 && isReadyToRun) {
		start_running = GetTickCount64();
		speedStack++;
		DebugOut(L"HandleSpeedStack::%d\n", speedStack);
		if (speedStack > MARIO_RUNNING_STACKS) {
			speedStack = MARIO_RUNNING_STACKS;
			//isFlying = true;
		}
	}
	if (GetTickCount64() - running_stop > MARIO_SPEED_STACK_LOST_TIME && !isRunning)
	{
		running_stop = GetTickCount64();
		speedStack--;
		//isFlying = false;
		if (speedStack < 0)
		{
			speedStack = 0;
			isRunning = false;
			isFlying = false;
		}
	}
}

void CMario::HandleFlying() {
	if (level != -5) {
		if (isFlying || isTailFlying)
		{
			if (vy <= -MARIO_NORMAL_FLY_MAX) {
				normalFlyPullDown = true;
			}
		}
	}
	if (normalFlyPullDown && isFlying || normalFlyPullDown && isTailFlying) {
		ay = 0.001f;
	}
	if (GetTickCount64() - fly_start > MARIO_FLYING_TIME && fly_start != 0 && isFlying)
	{
		fly_start = 0;
		isRunning = false;
		isFlying = false;
	}
	if (GetTickCount64() - tail_fly_start > MARIO_FLYING_TIME && tail_fly_start != 0 && isTailFlying)
	{
		tail_fly_start = 0;
		speedStack = 0;
		isRunning = false;
		isTailFlying = false;
	}
}

void CMario::HandleChangeYTransform() {
	if (state == MARIO_STATE_TRANSFORM) {
		if (level == MARIO_LEVEL_SMALL && !isChangingY) {
			y -= MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT;
		}
		if (level == MARIO_LEVEL_BIG && !isChangingY) {
			y -= MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT + 3;
		}
		if (level == MARIO_LEVEL_TAIL && !isChangingY) {
			y -= MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT;
		}
		isChangingY = true;
	}
}

void CMario::HandleTransform(int level) {
	if (isTransforming) {
		SetState(MARIO_STATE_TRANSFORM);
		HandleChangeYTransform();
		if (GetTickCount64() - start_transform > MARIO_TRANSFORMING_TIME) {
			StopTransform();
			isBangAni = false;
			if (isAttacked) {
				StartUntouchable();
			}
		}
	}
}


void CMario::AddScore(float x, float y, int score, bool isStack) {
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();

	start_score_time = GetTickCount64();

	if (isStack) {
		if (!isStackingScore) {
			isStackingScore = true;
		}
		else {
			if (stackScoreTimes == 5) {
				score *= 10;
				stackScoreTimes = 5;
			}
			else {
				score = score * (int)(pow(2, stackScoreTimes));
			}
		}

		stackScoreTimes++;
	}

	Point* point = new Point(score);
	int previousScore = score;

	point->SetPosition(x, y);
	currentScene->AddObject(point);

	this->marioScore += score;

}

void CMario::HandleFinishScene() {
	if (isFinish) {
		ax = MARIO_ACCELERATION;
		ay = MARIO_GRAVITY;
		nx = 1;
		SetState(MARIO_STATE_WALKING_RIGHT);
	}
}

void CMario::HandleTurning() {

	if (GetTickCount64() - start_turning >= MARIO_TURNING_STATE_TIME && isTuring) {
		start_turning = GetTickCount64();
		turningStack++;
	}
	if (GetTickCount64() - start_turning_state > MARIO_TURNING_TAIL_TIME && isTuring) {
		isTuring = false;
		start_turning_state = 0;
		start_turning = 0;
		turningStack = 0;
	}

}

void CMario::HandleChangeDirection() {
	if (isChangeDirection && runningStack > 15) {
		runningStack = 0;
		isChangeDirection = false;
		vx = 0;
	}
}

void CMario::HandleFlapping() {
	if (level == MARIO_LEVEL_TAIL && isFlapping) {
		vy = MARIO_SLOW_FALLING_SPEED;
	}
}

void CMario::HandleMarioKicking() {
	if (isKick) {
		if (GetTickCount64() - start_kicking > MARIO_KICKING_TIME) {
			StopKicking();
			SetState(MARIO_STATE_IDLE);
		}
	}
}

