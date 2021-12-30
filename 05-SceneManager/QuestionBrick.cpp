#include "QuestionBrick.h"
#include "Utils.h"
#include "Coin.h"
#include "PlayScene.h"
#include "Game.h"
#include "Coin.h"
#include "MushRoom.h"
#include "Leaf.h"
#include "Switch.h"

QuestionBrick::QuestionBrick(int tag, int type) : CGameObject() {
	state = QUESTION_BRICK_NORMAL;
	this->tag = tag;
	this->type = type;
	this->item = NULL;
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
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario != NULL && totalItems > 0 && state != QUESTION_BRICK_HIT && mario->isTuring) {
		mario->tail->GetBoundingBox(mLeft, mTop, mRight, mBottom);
		if (isColliding(mLeft, mTop, mRight, mBottom)) {
			SetState(QUESTION_BRICK_HIT);
		}

	}

	if (state == QUESTION_BRICK_HIT) {
		if (isBeingPushedUp && start_y - y >= QUESTIONBRICK_PUSH_MAX_HEIGHT) {
			stopPushedUp();
		}
		if (isFallingDown && y >= start_y) {
			y = start_y;
			isFallingDown = false;
			vy = 0;
			if (tag != ITEM_COIN_QUESTION_BRICK_COIN) {
				CreateItem(tag);
			}
		}
		if (tag == ITEM_COIN_QUESTION_BRICK_COIN) {
			CreateItem(tag);
		}
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
	if (dynamic_cast<CCoin*>(this->obj)) {
		CCoin* obj = dynamic_cast<CCoin*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y - COIN_BBOX_HEIGHT - 1);
		obj->SetState(COIN_STATE_UP);
		obj->SetZIndex(-1);
		currentScene->AddObject(obj);
	}
	if (dynamic_cast<CMushRoom*>(this->obj)) {
		CMushRoom* obj = dynamic_cast<CMushRoom*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y);
		obj->SetState(MUSHROOM_STATE_UP);
		obj->SetZIndex(-1);
		currentScene->AddObject(obj);
	}
	if (dynamic_cast<CLeaf*>(this->obj)) {
		CLeaf* obj = dynamic_cast<CLeaf*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y);
		obj->SetState(LEAF_STATE_UP);
		obj->SetZIndex(-1);
		currentScene->AddObject(obj);
	}
	if (dynamic_cast<Switch*>(this->obj)) {
		Switch* obj = dynamic_cast<Switch*>(this->obj);
		obj->SetAppear(true);
		obj->SetPosition(x, y);
		obj->SetState(SWITCH_STATE_UP);
		obj->SetZIndex(-1);
		currentScene->AddObject(obj);
	}
}

CGameObject* QuestionBrick::SetUpItem(int itemType) {
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CMario* mario = currentScene->GetPlayer();
	int ani_set_id = -1;
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	if (totalItems >= 1) {
		totalItems--;
	}
	else {
		return NULL;
	}
	if (itemType == ITEM_COIN_QUESTION_BRICK_COIN) {
		obj = new CCoin(COIN_TYPE_INBRICK);
		ani_set_id = COIN_ANI_SET_ID;
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);
	}
	if (itemType == ITEM_CUSTOM || itemType == ITEM_LEAF) {
		if (mario->GetLevel() == MARIO_LEVEL_SMALL) {
			obj = new CMushRoom();
			ani_set_id = ITEM_MUSHROOM_ANI_SET_ID;
			LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
			obj->SetAnimationSet(ani_set);
		}
		if (mario->GetLevel() == MARIO_LEVEL_BIG) {
			obj = new CLeaf();
			ani_set_id = LEAF_ANI_SET_ID;
			LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
			obj->SetAnimationSet(ani_set);
		}
		if (mario->GetLevel() == MARIO_LEVEL_TAIL) {
			obj = new CMushRoom(MUSHROOM_GREEN);
			ani_set_id = MUSHROOM_ANI_GREEN_ID;
			LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
			obj->SetAnimationSet(ani_set);
		}
	}
	if (itemType == ITEM_SWITCH) {
		obj = new Switch();
		ani_set_id = SWITCH_ANI_SET_ID;
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);
	}
	if (itemType == ITEM_MUSHROOM_GREEN) {
		obj = new CMushRoom(ITEM_MUSHROOM_GREEN);
		ani_set_id = ITEM_MUSHROOM_ANI_SET_ID;
		LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
		obj->SetAnimationSet(ani_set);
	}
	return obj;
}


void QuestionBrick::SetState(int state) {
	CGameObject::SetState(state);
	switch (state)
	{
	case QUESTION_BRICK_NORMAL:
		vy = 0;
		break;
	case QUESTION_BRICK_HIT:
		if (totalItems > 0) startPushedUp();
		startPushedUp();
		break;
	}
}