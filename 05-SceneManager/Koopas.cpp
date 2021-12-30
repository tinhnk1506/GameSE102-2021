#include "Koopas.h"
#include "Brick.h"
#include "Block.h"
#include "PlayScene.h"
#include "BreakableBrick.h"
#include "QuestionBrick.h"
#include "debug.h"
#include "Mario.h"
#include "PiranhaPlant.h"
#include "PiranhaPlantFire.h"

CKoopas::CKoopas(int tag)
{
	this->start_x = x;
	this->start_y = y;
	this->start_tag = tag;
	if (tag == KOOPAS_GREEN || tag == KOOPAS_GREEN_PARA) {
		this->nx = -1;
	}
	this->nx = -1;
	this->SetState(KOOPAS_STATE_WALKING);
}

void CKoopas::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (GetTickCount64() - shell_start >= KOOPAS_SHELL_TIME && shell_start != 0 && state != KOOPAS_STATE_SPINNING) {
		shell_start = 0;
		StartReviving();
	}

	if (GetTickCount64() - reviving_start >= KOOPAS_REVIVE_TIME && reviving_start != 0 && state != KOOPAS_STATE_SPINNING && shell_start == 0)
	{
		reviving_start = 0;
		y -= (KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_SHELL_HEIGHT) + 1.0f;
		if (isBeingHeld)
		{
			isBeingHeld = false;
			mario->isHolding = false;
		}
		SetState(KOOPAS_STATE_WALKING);
	}

	float mLeft, mTop, mRight, mBottom;
	//float oLeft, oTop, oRight, oBottom;
	if (mario != NULL) {
		if (mario->isTuring && mario->GetLevel() == MARIO_LEVEL_TAIL) {
			mario->tail->GetBoundingBox(mLeft, mTop, mRight, mBottom);
			if (isColliding(floor(mLeft), floor(mTop), ceil(mRight), ceil(mBottom))) {
				SetState(KOOPAS_STATE_SHELL_UP);
				if (tag == KOOPAS_GREEN_PARA)
					tag = KOOPAS_GREEN;
				mario->tail->ShowHitEffect();
			}
		}
	}

	this->dt = dt;
	vy += KOOPAS_GRAVITY * dt;

	if (!isBeingHeld)
	{
		if (tag == KOOPAS_GREEN_PARA)
			vy += KOOPAS_PARA_GRAVITY * dt;
		if (tag == KOOPAS_RED || tag == KOOPAS_GREEN)
			vy += KOOPAS_GRAVITY * dt;
	}

	HandleBeingHeld(mario);

	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CKoopas::HandleBeingHeld(LPGAMEOBJECT player) {

	CMario* mario = dynamic_cast<CMario*>(player);

	if (isBeingHeld && mario->isHolding) {
		if (state == KOOPAS_STATE_IN_SHELL || state == KOOPAS_STATE_SHELL_UP) {
			if (mario->nx > 0) {
				x = mario->x + MARIO_BIG_BBOX_WIDTH * mario->nx - 3.0f;
			}
			else x = mario->x + MARIO_BIG_BBOX_WIDTH * mario->nx;
			if (mario->GetLevel() != MARIO_LEVEL_SMALL) {
				y = mario->y - 2.0f;
			}
			else {
				y = mario->y - 2.0f;
			}
			vy = 0;
		}
	}
	else if (isBeingHeld && !mario->isHolding) {
		if (state == KOOPAS_STATE_SHELL_UP || state == KOOPAS_STATE_IN_SHELL) {
			this->nx = mario->nx;
			isBeingHeld = false;
			//mario->StartKicking();
			SetState(KOOPAS_STATE_SPINNING);
		}
	}
}

void CKoopas::OnNoCollision(DWORD dt) {
	x += vx * dt;
	y += vy * dt;
}

void CKoopas::OnCollisionWith(LPCOLLISIONEVENT e) {
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
	}

	else
	{
		if (e->nx != 0 && e->obj->IsBlocking())
		{
			if (!dynamic_cast<CBlock*>(e->obj)) {
				vx = -vx;
				nx = -nx;
			}
			else {
				CBlock* block = dynamic_cast<CBlock*>(e->obj);
				block->SetIsBlocking(0);
			}
		}
	}

	if (dynamic_cast<CBrick*>(e->obj))
		OnCollisionWithBrick(e);
	if (dynamic_cast<CBlock*>(e->obj))
		OnCollisionWithBlock(e);
	if (dynamic_cast<BreakableBrick*>(e->obj))
		OnCollisionWithBreakableBrick(e);
	if (dynamic_cast<QuestionBrick*>(e->obj))
		OnCollisionWithQuestionBrick(e);
	if (dynamic_cast<CKoopas*>(e->obj))
		OnCollisionWithKoopas(e);
	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	if (dynamic_cast<PiranhaPlant*>(e->obj) || dynamic_cast<PiranhaPlantFire*>(e->obj))
		OnCollisionWithPlan(e);
	//if (dynamic_cast<PiranhaPlantFire*>(e->obj))
	//	OnCollisionWithPlan(e);
}

void CKoopas::OnCollisionWithPlan(LPCOLLISIONEVENT e) {

	PiranhaPlant* piranhaPlant = dynamic_cast<PiranhaPlant*>(e->obj);
	PiranhaPlantFire* piranhaPlantFire = dynamic_cast<PiranhaPlantFire*>(e->obj);

	if ((piranhaPlant->GetState() != PIRANHAPLANT_STATE_DEATH ||
		piranhaPlantFire->GetState() != PIRANHAPLANT_STATE_DEATH) &&
		this->GetState() == KOOPAS_STATE_SPINNING)
	{
		piranhaPlant->SetState(PIRANHAPLANT_STATE_DEATH);
		piranhaPlantFire->SetState(PIRANHAPLANT_STATE_DEATH);

	}

}

void CKoopas::OnCollisionWithBreakableBrick(LPCOLLISIONEVENT e) {
	if (state == KOOPAS_STATE_SPINNING) {
		if (e->nx != 0) {
			BreakableBrick* tmp = dynamic_cast<BreakableBrick*>(e->obj);
			tmp->Break();
		}
	}
}

void CKoopas::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e) {
	QuestionBrick* qBrick = dynamic_cast<QuestionBrick*>(e->obj);
	if (qBrick->state != QUESTION_BRICK_HIT && state == KOOPAS_STATE_SPINNING)
		qBrick->SetState(QUESTION_BRICK_HIT);
}

void CKoopas::OnCollisionWithBrick(LPCOLLISIONEVENT e) {
	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;

	GetBoundingBox(mLeft, mTop, mRight, mBottom);
	e->obj->GetBoundingBox(oLeft, oTop, oRight, oBottom);

	if (e->ny < 0) {
		this->vy = 0;
		if (state == KOOPAS_STATE_SHELL_UP)
			vx = 0;
		if (tag == KOOPAS_RED && state == KOOPAS_STATE_WALKING)
		{
			if (this->nx > 0 && x >= e->obj->x + KOOPAS_TURN_DIFF)
				if (CalTurnable(e->obj))
				{
					this->nx = -1;
					vx = this->nx * KOOPAS_WALKING_SPEED;
				}
			if (this->nx < 0 && x <= e->obj->x - KOOPAS_TURN_DIFF)
				if (CalTurnable(e->obj))
				{
					this->nx = 1;
					vx = this->nx * KOOPAS_WALKING_SPEED;
				}
		}
		if (tag == KOOPAS_GREEN_PARA || KOOPAS_GREEN) {
			this->nx = -1;
			vx = this->nx * KOOPAS_WALKING_SPEED;
		}
	}
	if (e->nx != 0)
	{
		if (ceil(mBottom) != oTop)
		{
			//vx = -vx;
			this->nx = -this->nx;
		}
	}
}

void CKoopas::OnCollisionWithBlock(LPCOLLISIONEVENT e) {
	if (e->ny < 0)
	{
		vy = 0;
		if (state == KOOPAS_STATE_SHELL_UP)
			vx = 0;
		if (tag == KOOPAS_RED && state == KOOPAS_STATE_WALKING)
		{
			if (this->nx > 0 && x >= e->obj->x + KOOPAS_TURN_DIFF)
				if (CalTurnable(e->obj))
				{
					this->nx = -1;
					vx = this->nx * KOOPAS_WALKING_SPEED;
				}
			if (this->nx < 0 && x <= e->obj->x - KOOPAS_TURN_DIFF)
				if (CalTurnable(e->obj))
				{
					this->nx = 1;
					vx = this->nx * KOOPAS_WALKING_SPEED;
				}
		}
		if (tag == KOOPAS_GREEN_PARA)
		{
			y = e->obj->y - KOOPAS_BBOX_HEIGHT;
			vy = -KOOPAS_JUMP_SPEED;
			vx = vx = this->nx * KOOPAS_WALKING_SPEED;
			this->nx = -1;
		}
	}
	else
	{
		if (e->nx != 0)
			x += vx * this->dt;
		if (state == KOOPAS_STATE_SHELL_UP && e->ny > 0)
			y += vy * this->dt;
	}

}

void CKoopas::OnCollisionWithKoopas(LPCOLLISIONEVENT e) {

	CKoopas* koopas = dynamic_cast<CKoopas*>(e->obj);
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (koopas->state == KOOPAS_STATE_SPINNING)
	{
		if (koopas->tag == KOOPAS_GREEN_PARA)
			koopas->tag = KOOPAS_GREEN;
		SetState(KOOPAS_STATE_DEATH);
		mario->AddScore(x, y, 100, true);
	}
	else
	{
		if ((koopas->state == KOOPAS_STATE_SHELL_UP || koopas->state == KOOPAS_STATE_IN_SHELL)
			&& state == KOOPAS_STATE_WALKING)
		{
			DebugOut(L"OnCollisionWithKoopas else 1 \n");
		}
		if (koopas->state == KOOPAS_STATE_WALKING)
		{
			DebugOut(L"OnCollisionWithKoopas - else 2 \n");
			this->vx = -this->vx;
			this->nx = -this->nx;
			koopas->vx = -koopas->vx;
			koopas->nx = -koopas->nx;
		}
	}
}

void CKoopas::OnCollisionWithGoomba(LPCOLLISIONEVENT e) {
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	if (this->GetState() == KOOPAS_STATE_SPINNING)
	{
		goomba->SetState(GOOMBA_STATE_DIE);
		vy = -MARIO_JUMP_DEFLECT_SPEED;
	}
	else
	{
		goomba->vx = -goomba->vx;
		goomba->nx = -goomba->nx;
		this->vx = -this->vx;
		this->nx = -this->nx;
	}
}

void CKoopas::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + KOOPAS_BBOX_WIDTH;
	if (state == KOOPAS_STATE_IN_SHELL || state == KOOPAS_STATE_SPINNING || state == KOOPAS_STATE_SHELL_UP)
	{
		bottom = y + KOOPAS_BBOX_SHELL_HEIGHT;
	}
	else
		bottom = y + 20;
}

void CKoopas::Render()
{
	int ani = -1;
	if (state == KOOPAS_STATE_SHELL_UP || state == KOOPAS_STATE_DEATH)
		ani = KOOPAS_ANI_SHELL_UP;
	else if (state == KOOPAS_STATE_IN_SHELL)
		ani = KOOPAS_ANI_SHELL;
	else if (state == KOOPAS_STATE_SPINNING)
	{
		if (vx < 0)
			ani = KOOPAS_ANI_SPIN_LEFT;
		else
			ani = KOOPAS_ANI_SPIN_RIGHT;
	}
	else
	{
		if (this->nx < 0)
			ani = KOOPAS_ANI_WALKING_LEFT;
		else
			ani = KOOPAS_ANI_WALKING_RIGHT;
	}
	if (state != KOOPAS_STATE_DEATH) {
		if (tag == KOOPAS_GREEN_PARA || tag == KOOPAS_RED_PARA)
			if (this->nx < 0)
				ani = KOOPAS_ANI_PARA_LEFT;
			else
				ani = KOOPAS_ANI_PARA_RIGHT;
		if (reviving_start != 0)
		{
			if (state == KOOPAS_STATE_IN_SHELL)
				ani = KOOPAS_ANI_SHAKE;
			if (state == KOOPAS_STATE_SHELL_UP)
				ani = KOOPAS_ANI_SHAKE_UP;
		}
	}
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

bool CKoopas::CalTurnable(LPGAMEOBJECT object)
{
	//if (!IsInViewPort())
	//	return false;
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	vector<LPGAMEOBJECT> coObjects = currentScene->GetObjects();
	for (UINT i = 0; i < coObjects.size(); i++)
		if (dynamic_cast<CBrick*>(coObjects[i]) || dynamic_cast<CBlock*>(coObjects[i]))
			if (abs(coObjects[i]->y == object->y))
			{
				if (nx > 0)
					if (coObjects[i]->x > object->x && coObjects[i]->x - 16 < object->x + 16)
						return false;
				if (nx < 0)
					if (coObjects[i]->x + 16 > object->x - 16 && coObjects[i]->x < object->x)
						return false;
			}
	return true;
}

void CKoopas::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case KOOPAS_STATE_INACTIVE:
		vx = 0;
		vy = 0;
		break;
	case KOOPAS_STATE_DEATH:
		y += KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT + 1;
		vx = 0;
		vy = 0;
		break;
	case KOOPAS_STATE_WALKING:
		vx = this->nx * KOOPAS_WALKING_SPEED;
		break;
	case KOOPAS_STATE_SPINNING:
		CMario* mario;
		mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
		//if (nx > 0)
		vx = mario->nx * KOOPAS_WALKING_SPEED * 5;
		/*else
			vx = -KOOPAS_WALKING_SPEED * 5;*/
		break;
	case KOOPAS_STATE_IN_SHELL:
		vx = 0;
		vy = 0;
		StartShell();
		break;
	case KOOPAS_STATE_SHELL_UP:
		vy = -KOOPAS_SHELL_DEFLECT_SPEED;
		/*if (x <= mario->x)
			nx = -1;
		else*/
		nx = 1;
		vx = nx * KOOPAS_WALKING_SPEED;
		StartShell();
		break;
	}
}