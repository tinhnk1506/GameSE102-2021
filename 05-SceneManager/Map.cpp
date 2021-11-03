#include "Map.h"
#include "Game.h"
#include "debug.h"

CMap::CMap(int TileSetID, int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int TotalColumnsOfTileSet, int TotalTiles) {
	CGame* game = CGame::GetInstance();
	TileSet = CTextures::GetInstance()->Get(TileSetID);
	this->TotalRowsOfMap = TotalRowsOfMap;
	this->TotalColumnsOfMap = TotalColumnsOfMap;
	this->TotalRowsOfTileSet = TotalRowsOfTileSet;
	this->TotalColumnsOfTileSet = TotalColumnsOfTileSet;
	this->TotalTiles = TotalTiles;
	CamX = 0;
	CamY = 0;
	TileMap = NULL;
}

CMap::~CMap()
{
}


void CMap::DrawMap() {
	CGame* game = CGame::GetInstance();
	int FirstColumn = (int)floor(CamX / TILE_WIDTH);
	int LastColumn = (int)ceil((CamX + game->GetBackBufferWidth()) / TILE_WIDTH);
	if (LastColumn >= TotalColumnsOfMap)
		LastColumn = TotalColumnsOfMap - 1;

	int d = 0;
	for (int CurrentRow = 0; CurrentRow < TotalRowsOfMap; CurrentRow++)
		for (int CurrentColumn = FirstColumn; CurrentColumn <= LastColumn; CurrentColumn++)
		{
			int index = TileMap[CurrentRow][CurrentColumn] - 1;
			if (index < TotalTiles)
				Tiles.at(index)->Draw((float)(CurrentColumn * TILE_WIDTH), (float)(CurrentRow * TILE_HEIGHT));
		}
}

void CMap::SetTileMapData(int** TileMapData) {
	TileMap = TileMapData;
}

void CMap::ExtractTileFromTileSet() {
	for (int TileNum = 0; TileNum < TotalTiles; TileNum++) {
		int left = TileNum % TotalColumnsOfTileSet * TILE_WIDTH;
		int top = TileNum / TotalColumnsOfTileSet * TILE_HEIGHT;
		int right = left + TILE_WIDTH - TILE_DIFF;
		int bottom = top + TILE_HEIGHT - TILE_DIFF;
		LPSPRITE NewTile = new CSprite(TileNum + 5000, left, top, right, bottom, TileSet);
		this->Tiles.push_back(NewTile);
	}
}

void CMap::SetCamPos(float x, float y) {
	this->CamX = x;
	this->CamY = y;
}