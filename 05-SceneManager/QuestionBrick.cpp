#include "QuestionBrick.h"
#include "Utils.h"
#include "Coin.h"
#include "PlayScene.h"
#include "Game.h"
#include "Coin.h"
#include "Mario.h"

QuestionBrick::QuestionBrick(int tag, int type) : CGameObject() {
	state = QUESTION_BRICK_NORMAL;
	this->tag = tag;
	this->type = type;
}

void QuestionBrick::Render() {
	int ani = -1;

	if (state == QUESTION_BRICK_NORMAL) {
		ani = QUESTION_BRICK_ANI_NORMAL;
	}
	if (state == QUESTION_BRICK_HIT) {
		ani = QUESTION_BRICK_ANI_HIT;
	}
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void QuestionBrick::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
};

void QuestionBrick::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (!e->obj->IsBlocking()) return;
	if (dynamic_cast<CGoomba*>(e->obj)) return;

	if (e->ny != 0)
	{
		vy = 0;
	}
	else if (e->nx != 0)
	{
		vx = -vx;
	}
}

void QuestionBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	float mLeft, mTop, mRight, mBottom;

	x += vx * dt;
	y += vy * dt;

	/*if (mario != NULL && totalItems > 0 && state != QUESTION_BRICK_HIT && mario->isTuring) {
		mario->GetTail()->GetBoundingBox(mLeft, mTop, mRight, mBottom);
		if (isColliding(mLeft, mTop, mRight, mBottom)) {
			SetState(QUESTION_BRICK_HIT);
		}

	}*/

	if (state == QUESTION_BRICK_HIT) {
		if (isBeingPushedUp && start_y - y >= QUESTIONBRICK_PUSH_MAX_HEIGHT) {
			stopPushedUp();
		}
		if (isFallingDown && y >= start_y) {
			y = start_y;
			isFallingDown = false;
			vy = 0;
			/*if (tag != ITEM_COIN_QUESTION_BRICK_COIN) {
				DebugOut(L"item::%d\n", tag);
				CreateItem(tag);
			}*/
		}
		/*if (tag == ITEM_COIN_QUESTION_BRICK_COIN) {
			CreateItem(tag);
		}*/
	}

	//DebugOut(L"[BRICK vy]::%f\n", vy);
}


void QuestionBrick::GetBoundingBox(float& l, float& t, float& r, float& b) {
	l = x;
	t = y;
	r = x + 16;
	b = y + 16;
}


void QuestionBrick::startPushedUp() {
	isBeingPushedUp = true;
	vy = -QUESTIONBRICK_SPEED;
}

void QuestionBrick::stopPushedUp() {
	isBeingPushedUp = false;
	isFallingDown = true;
	vy = QUESTIONBRICK_SPEED;
}

void QuestionBrick::CreateItem(int itemType) {
	this->obj = SetUpItem(itemType);
	if (this->obj == NULL) {
		return;
	}
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	if (dynamic_cast<Coin*>(this->obj)) {
		Coin* obj = dynamic_cast<Coin*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y - COIN_BBOX_HEIGHT - 1);
		obj->SetState(COIN_STATE_UP);
		currentScene->AddSpecialObject(obj);
	}
	/*if (dynamic_cast<MushRoom*>(this->obj)) {
		MushRoom* obj = dynamic_cast<MushRoom*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y);
		obj->SetState(MUSHROOM_STATE_UP);
		currentScene->AddSpecialObject(obj);
	}
	if (dynamic_cast<Switch*>(this->obj)) {
		Switch* obj = dynamic_cast<Switch*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y);
		obj->SetState(SWITCH_STATE_UP);
		currentScene->AddSpecialObject(obj);
	}
	if (dynamic_cast<Leaf*>(this->obj)) {
		Leaf* obj = dynamic_cast<Leaf*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y);
		obj->SetState(LEAF_STATE_UP);
		currentScene->AddSpecialObject(obj);
	}
	if (dynamic_cast<FireFlower*>(this->obj)) {
		FireFlower* obj = dynamic_cast<FireFlower*>(this->obj);
		obj->isAppear = true;
		obj->SetPosition(x, y);
		obj->SetState(FIRE_FLOWER_STATE_UP);
		currentScene->AddSpecialObject(obj);
	}*/
}


void QuestionBrick::SetState(int state) {
	CGameObject::SetState(state);
	switch (state)
	{
	case QUESTION_BRICK_NORMAL:
		vy = 0;
		break;
	case QUESTION_BRICK_HIT:
		/*if (totalItems > 0) startPushedUp();*/
		startPushedUp();
		break;
	}
}