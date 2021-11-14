#include "BreakPiece.h"

void BreakPiece::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
	CGameObject::Update(dt);

	x += vx * dt;
	y += vy * dt;

	if (GetTickCount64() - start_exist >= PIECE_TIME) {
		isDeleted = true;
	}
}

void BreakPiece::Render()
{
	if (isDeleted)
		return;
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
	//DebugOut(L"[Piece]\n");
}

void BreakPiece::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + PIECE_BBOX_WIDTH;
	b = y + PIECE_BBOX_HEIGHT;
}