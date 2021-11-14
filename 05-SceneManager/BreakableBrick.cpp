#include "BreakableBrick.h"
#include "GameObject.h"
#include "Coin.h"
#include "PlayScene.h"
#include "BreakPiece.h"

void BreakableBrick::Render()
{
	if (isDeleted)
		return;
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}

void BreakableBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	/*CGameObject::Update(dt);
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	float mLeft, mTop, mRight, mBottom;
	if (mario != NULL) {
		mario->GetTail()->GetBoundingBox(mLeft, mTop, mRight, mBottom);
		if (isColliding(mLeft, mTop, mRight, mBottom) && mario->isTuring) {
			this->Break();
		}
	}

	if (isDestroyed)
		return;*/

}

void BreakableBrick::Break() {
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(PIECE_ANI_SET_ID);

	BreakPiece* bPieceTopLeft = new BreakPiece(-1, 1);
	bPieceTopLeft->SetPosition(x, y);
	bPieceTopLeft->SetAnimationSet(ani_set);

	BreakPiece* bPieceBottomLeft = new BreakPiece(-1, -1);
	bPieceBottomLeft->SetPosition(x, y);
	bPieceBottomLeft->SetAnimationSet(ani_set);

	BreakPiece* bPieceTopRight = new BreakPiece(1, 1);
	bPieceTopRight->SetPosition(x, y);
	bPieceTopRight->SetAnimationSet(ani_set);

	BreakPiece* bPieceBottomRight = new BreakPiece(1, -1);
	bPieceBottomRight->SetPosition(x, y);
	bPieceBottomRight->SetAnimationSet(ani_set);

	currentScene->AddObject(bPieceTopLeft);
	currentScene->AddObject(bPieceBottomLeft);
	currentScene->AddObject(bPieceTopRight);
	currentScene->AddObject(bPieceBottomRight);

	isDeleted = true;
}

void BreakableBrick::ChangeToCoin() {
	CPlayScene* currentScene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	vector<LPGAMEOBJECT> objects = currentScene->GetObjects();
	CAnimationSets* animation_sets = CAnimationSets::GetInstance();
	LPANIMATION_SET ani_set = animation_sets->Get(COIN_ANI_SET_ID);
	for (size_t i = 0; i < objects.size(); i++)
	{
		if (dynamic_cast<BreakableBrick*>(objects.at(i)) && !objects.at(i)->isDeleted) {
			BreakableBrick* bBrick = dynamic_cast<BreakableBrick*>(objects.at(i));
			CCoin* coin = new CCoin();
			coin->SetPosition(bBrick->x, bBrick->y);
			coin->SetAppear(true);
			coin->SetAnimationSet(ani_set);
			currentScene->AddObject(coin);
			bBrick->isDeleted = true;
		}
	}
}