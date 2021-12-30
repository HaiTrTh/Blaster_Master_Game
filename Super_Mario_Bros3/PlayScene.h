#pragma once
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "SOPHIA.h"
#include "Eye.h"
#include "Koopas.h"
#include "Map.h"
#include "SOPHIAWHEELS.h"
#include "MapObj.h"
#include "LaserGuard.h"
#include "BallCarry.h"
#include "Ballbot.h"
#include "DRAP.h"
#include "GX680.h"
#include "GX680S.h"
#include "STUKA.h"
#include "Eyelet.h"
#include "Interrupt.h"
#include "SOPHIABULLET.h"
#include "EvenType.h"
#include "INTERRUPT_BULLET.h"
#include "REDWORM.h"
#include "SOPHIABODY.h"
#include "SOPHIATURRET.h"
#include "EFFECT.h"
#include "JASON.h"
#include "BOOM.h"
#include "NoCollisionObject.h"

#include "Utils.h"
#include "Game.h"
#include <iostream>
#include <fstream>
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
#include "DF.h"
#include "CWAVE_BULLET.h"
#include "GRENADE.h"
#include "GX_BULLET.h"
#include "LASER_BULLET.h"
#include "MapCamera.h"
#include "HP.h"
#include "MINI_JASON.h"
#include "SOPHIADOOR.h"
#include "Items.h"

#define QUADTREE_SECTION_SETTINGS	1
#define QUADTREE_SECTION_OBJECTS	2
#define MAX_QUADTREE_LINE 1024

class CQuadTree
{
	bool isLeaf = false;
	float x = 0;
	float y = 0;
	float cellWidth = 0;
	float  cellHeight = 0;
	CQuadTree* BrachTL = NULL;
	CQuadTree* BrachTR = NULL;
	CQuadTree* BrachBL = NULL;
	CQuadTree* BrachBR = NULL;
	MapObj* obj;
	vector<LPGAMEOBJECT> listObjects;
	

	void _ParseSection_SETTINGS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_MapObj(string line);
public:
	CQuadTree(float cellWidth, float cellHeight, float x, float y);
	CQuadTree(LPCWSTR filePath);
	void GetObjects(vector<LPGAMEOBJECT>& listObject, int playerX, int playerY);
	void Load(LPCWSTR filePath);
	void Unload();
	void Add(CGameObject* obj)
	{
		listObjects.push_back(obj);
	}
	int getVollunm(){
		return listObjects.size();
	}
	void Render();
	void Plit();
	void ObjectPlit(CQuadTree* brach);
	bool inRange(float ox, float oy, float x, float y, float width, float height);
	void Pop(vector<LPGAMEOBJECT>& listObject, int CamX, int CamY);

};

class CPlayScene : public CScene
{
protected:
	CSOPHIA* player;				// A play scene has to have player, right? 
	JASON* player2;
	MINI_JASON* player3;
	vector<LPGAMEOBJECT> objects;
	vector<LPGAMEOBJECT> secondLayer;
	int mapHeight;
	Map* map;
	CQuadTree* quadtree;
	vector<EvenType*> InterruptBulletMng ;
	vector<EvenType*> WormSpamMng;
	vector<EvenType*> KaboomMng;
	vector<EvenType*> BoomCarryMng;
	vector<EvenType*> CGXMng;
	vector<EvenType*> ItemsMng;
	vector<MapCamera*> MapCam;
	
	int filming_duration = 1000;
	DWORD filming_start = 0;

	int camState = 0;

	bool piloting = true;

	void _ParseSection_TEXTURES(string line);
	void _ParseSection_SPRITES(string line);
	void _ParseSection_ANIMATIONS(string line);
	void _ParseSection_ANIMATION_SETS(string line);
	void _ParseSection_OBJECTS(string line);
	void _ParseSection_MAP(string line);
	void _ParseSection_QUADTREE(string line);
	void _ParseSection_SETTING(string line);
	void _ParseSection_MAPCAM(string line);
public:
	CPlayScene(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(DWORD dt);
	virtual void Render();
	virtual void Unload();

	bool IsInUseArea(float Ox, float Oy);
	bool IsInside(float Ox, float Oy, float xRange, float yRange, float tx, float ty);

	CSOPHIA* GetPlayer() { return player; }
	JASON* GetPlayer2() { return player2; }
	MINI_JASON* GetPlayer3() { return player3; }

	void setpiloting(int value)
	{
		piloting = value;
	}

	int getpiloting()
	{
		return piloting;
	}
	void StartFilming()
	{
		if (filming_start == 0)
			filming_start = (DWORD)GetTickCount64();
	}
	void setCamState(int value)
	{
		camState = value;
	}

	int getCamState()
	{
		return camState;
	}
	void setMapheight(int height)
	{
		mapHeight = height;
	}

	int getMapheight()
	{
		return mapHeight;
	}
	
	/////////////////ItemsMng
	void AddItemsMng(float x, float y, int num)
	{
		EvenType* obj = new EvenType(x, y, num);
		this->ItemsMng.push_back(obj);
	}
	EvenType* GetItemsMng()
	{
		return ItemsMng.at(0);
	}
	bool CheckItemsMng()
	{
		if (ItemsMng.size() != 0)
			return true;
		return false;
	}
	void DeleteItemsMng()
	{
		this->ItemsMng.erase(ItemsMng.begin());
	}
	/////////////////CGXMng
	void AddCGXMng(float x, float y, float vx, float vy)
	{
		EvenType* obj = new EvenType(x, y, 0, vx, vy);
		this->CGXMng.push_back(obj);
	}
	EvenType* GetCGXMng()
	{
		return CGXMng.at(0);
	}
	bool CheckCGXMng()
	{
		if (CGXMng.size() != 0)
			return true;
		return false;
	}
	void DeleteCGXMng()
	{
		this->CGXMng.erase(CGXMng.begin());
	}
	/////////////////BoomCarryMng
	void AddBoomCarryMng(float x, float y)
	{
		EvenType* obj = new EvenType(x, y);
		this->BoomCarryMng.push_back(obj);
	}
	void CheckStackBoomCarryMng()
	{
		if (BoomCarryMng.at(0)->getCEventStack() < 4)
		{
			BoomCarryMng.at(0)->setCEventStack(BoomCarryMng.at(0)->getCEventStack() + 1);
		}
		else 
		{
			DeleteBoomCarryMng();
		}
	}
	EvenType* GetBoomCarryMng()
	{
		return BoomCarryMng.at(0);
	}
	bool CheckBoomCarryMng()
	{
		if (BoomCarryMng.size() != 0)
			return true;
		return false;
	}
	void DeleteBoomCarryMng()
	{
		this->BoomCarryMng.erase(BoomCarryMng.begin());
	}
	/////////////////KaboomMng
	void AddKaboomMng(float x, float y)
	{
		EvenType* obj = new EvenType(x, y);
		this->KaboomMng.push_back(obj);
	}
	EvenType* GetKaboomMng()
	{
		return KaboomMng.at(0);
	}
	bool CheckKaboomMng()
	{
		if (KaboomMng.size() != 0)
			return true;
		return false;
	}
	void DeleteKaboomMng()
	{
		this->KaboomMng.erase(KaboomMng.begin());
	}
	/////////////////InterruptBulletMng
	void AddInterruptBulletMng(float x, float y)
	{
		EvenType* obj = new EvenType(x, y);
		this->InterruptBulletMng.push_back(obj);
	}
	EvenType* GetInterruptBulletMng()
	{
		return InterruptBulletMng.at(0);
	}
	bool CheckInterruptBulletMng()
	{
		if (InterruptBulletMng.size() != 0)
			return true;
		return false;
	}
	void DeleteInterruptBulletMng()
	{
		this->InterruptBulletMng.erase(InterruptBulletMng.begin());
	}
	//////////////////////////WormSpamMng
	void AddWormSpamMng(float x, float y)
	{
		EvenType* obj = new EvenType(x, y);
		this->WormSpamMng.push_back(obj);
	}
	EvenType* GetWormSpamMng()
	{
		return WormSpamMng.at(0);
	}
	bool CheckWormSpamMng()
	{
		if (WormSpamMng.size() != 0)
			return true;
		return false;
	}
	void DeleteWormSpamMng()
	{
		this->WormSpamMng.erase(WormSpamMng.begin());
	}
	///////////////////////////////
	//friend class CPlayScenceKeyHandler;
};

class CPlayScenceKeyHandler : public CScenceKeyHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	CPlayScenceKeyHandler(CScene* s) :CScenceKeyHandler(s) {};
};

