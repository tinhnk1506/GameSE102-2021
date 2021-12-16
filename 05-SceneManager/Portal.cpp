#include "Portal.h"
#include "Game.h"
#include "Textures.h"
#include "debug.h"
#include "Mario.h"
#include "PlayScene.h"

CPortal::CPortal(float x, float y, int scene_id)
{
	this->scene_id = scene_id;
	this->start_x = x;
	this->start_y = y;
	SetType(IGNORE_DEFINE);
}

void CPortal::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPTEXTURE bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	float cx, cy;
	CGame::GetInstance()->GetCamPos(cx, cy);

	CGame::GetInstance()->Draw(x - cx, y - cy, bbox, nullptr, BBOX_ALPHA, rect.right - 1, rect.bottom - 1);
	//DebugOut(L"Portal::renderboundingbox::\n");
}

void CPortal::Render()
{
	RenderBoundingBox();
}

void CPortal::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x - PORTAL_BBOX_WIDTH;
	t = y - PORTAL_BBOX_HEIGHT;
	r = x + PORTAL_BBOX_WIDTH;
	b = y + PORTAL_BBOX_HEIGHT;
}

void CPortal::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CMario* mario = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	float mLeft, mTop, mRight, mBottom;
	float oLeft, oTop, oRight, oBottom;

	if (mario != NULL)
	{
		mario->GetBoundingBox(mLeft, mTop, mRight, mBottom);
		GetBoundingBox(oLeft, oTop, oRight, oBottom);

		if (isColliding(floor(mLeft), floor(mTop), ceil(mRight), ceil(mBottom))
			&& mario->isSwitchMap
			&& mLeft >= oLeft && mRight <= oRight)
		{
			mario->portal = this;
			if (tag == BACKTOPLAYSCENE)
			{
				//DebugOut(L"CPortal::BACKTOPLAYSCENE %f %f %f %f\n");
				if (scene_id == 1)
					mario->StartPipeUp();
				//else if (scene_id == 3)
				//	mario->StartPipeDown();
				mario->isBackScene = true;
				mario->isSwitchMap = true;
			}
			if (tag == TOEXTRASCENE && scene_id == 2)
			{
				//DebugOut(L"CPortal::TOEXTRASCENE %f %f %f %f\n");

				mario->StartPipeDown();
				mario->isSwitchMap = true;
			}
			return;
		}
		/*	if (tag == TOEXTRASCENE && scene_id == 4 && mario->isJumpMusicBrick)
			{
				mario->portal = this;
				mario->isJumpMusicBrick = false;
				mario->pipeUpTimer.Start();
				mario->wannaTele = true;
				mario->isTravel = true;
				DebugOut(L"tele map");
			}*/
	}
}
