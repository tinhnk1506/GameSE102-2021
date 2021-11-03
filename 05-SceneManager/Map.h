#pragma once
#include <d3d10.h>
#include"Textures.h"
#include "Sprites.h"
#include "Utils.h"

#define TILE_WIDTH 16
#define TILE_HEIGHT 16

#define TILE_DIFF 1

class CMap
{
	int TotalRowsOfMap, TotalColumnsOfMap;
	int TotalRowsOfTileSet, TotalColumnsOfTileSet;
	int TotalTiles;
	LPTEXTURE TileSet; //sceneX_bank.png
	vector<LPSPRITE> Tiles; // Danh sach Sprites cua TileSet
	int** TileMap;
	float CamX, CamY;
public:
	CMap(int mapTextureID, int TotalRowsOfMap, int TotalColumnsOfMap, int TotalRowsOfTileSet, int TotalColumnsOfTileSet, int TotalTiles);
	~CMap();
	void ExtractTileFromTileSet();
	void DrawMap();
	void SetTileMapData(int** TileMapData);
	int GetMapWidth() { return TotalColumnsOfMap * TILE_WIDTH; }
	int GetMapHeight() { return TotalRowsOfMap * TILE_HEIGHT; }
	void SetCamPos(float x, float y);
};