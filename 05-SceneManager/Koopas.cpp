#include "Koopas.h"
#include "Brick.h"
#include "Block.h"
#include "Mario.h"
#include "PlayScene.h"
#include "BreakableBrick.h"
#include "QuestionBrick.h"

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
	if (GetTickCount64() - shell_start >= KOOPAS_SHELL_TIME && shell_start != 0 && state != KOOPAS_STATE_SPINNING) {
		shell_start = 0;
		StartReviving();
	}

	if (GetTickCount64() - reviving_start >= KOOPAS_REVIVE_TIME && reviving_start != 0 && state != KOOPAS_STATE_SPINNING && shell_start == 0)
	{
		reviving_start = 0;
		y -= (KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_SHELL_HEIGHT) + 1.0f;
		/*	if (isBeingHeld)
			{
				isBeingHeld = false;
				mario->SetIsHolding(false);
			}*/
		SetState(KOOPAS_STATE_WALKING);
	}
	this->dt = dt;
	vy += KOOPAS_GRAVITY * dt;


	CGameObject::Update(dt, coObjects);
	CCollision::GetInstance()->Process(this, dt, coObjects);
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
}

void CKoopas::OnCollisionWithBreakableBrick(LPCOLLISIONEVENT e) {
	if (state == KOOPAS_STATE_SPINNING) {
		BreakableBrick* tmp = dynamic_cast<BreakableBrick*>(e->obj);
		tmp->Break();
		//mario->AddScore(x, y, 150);
	}
}

void CKoopas::OnCollisionWithQuestionBrick(LPCOLLISIONEVENT e) {
	QuestionBrick* qBrick = dynamic_cast<QuestionBrick*>(e->obj);
	if (qBrick->state != QUESTION_BRICK_HIT)
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
		//y += KOOPAS_BBOX_HEIGHT - KOOPAS_BBOX_HEIGHT + 1;
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