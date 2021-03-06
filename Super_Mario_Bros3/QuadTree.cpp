#include "PlayScene.h"

CQuadTree::CQuadTree(LPCWSTR filePath)
{
	Load(filePath);
}
CQuadTree::CQuadTree(float cellWidth, float cellHeight, float x, float y) {
	this->cellHeight = cellHeight;
	this->cellWidth = cellWidth;
	this->x = x;
	this->y = y;
}

void CQuadTree::Plit() {
	if (listObjects.size() == 0)
		return;
	CGame* game = CGame::GetInstance();
	if (cellHeight > game->GetScreenHeight() / 8 || cellWidth > game->GetScreenWidth() / 8)
	{
		BrachTL = new CQuadTree(cellWidth / 2, cellHeight / 2, x, y);
		BrachTR = new CQuadTree(cellWidth / 2, cellHeight / 2, x + cellWidth / 2, y);
		BrachBL = new CQuadTree(cellWidth / 2, cellHeight / 2, x, y + cellHeight / 2);
		BrachBR = new CQuadTree(cellWidth / 2, cellHeight / 2, x + cellWidth / 2, y + cellHeight / 2);
		ObjectPlit(BrachTL);
		ObjectPlit(BrachTR);
		ObjectPlit(BrachBL);
		ObjectPlit(BrachBR);
		BrachTL->Plit();
		BrachTR->Plit();
		BrachBL->Plit();
		BrachBR->Plit();
		listObjects.clear();
		return;
	}
	isLeaf = true;
	DebugOut(L"LEAF : %d %f %f %f %f \n", listObjects.size(), cellWidth, cellHeight, x, y);
	return;
}

bool CQuadTree::inRange(float ox, float oy, float x, float y, float width, float height)
{
	if (x <= ox && ox <= x + width + CAM_X_BONUS/2 && y <= oy && oy <= y + height)
		return true;
	return false;
}
void CQuadTree::ObjectPlit(CQuadTree* brach)
{
	int SL = listObjects.size();
	for (int i = 0; i < SL; i++)
	{
		if (brach->inRange(listObjects[i]->GetPositionX(), listObjects[i]->GetPositionY(), brach->x, brach->y, brach->cellWidth, brach->cellHeight))
		{
			brach->Add(listObjects[i]);
		}

	}
}

void CQuadTree::_ParseSection_SETTINGS(string line)
{
	vector<string> tokens = split(line);
	DebugOut(L"--> %s\n", ToWSTR(line).c_str());

	if (tokens.size() < 2) return; // skip invalid lines

	cellWidth = atoi(tokens[0].c_str());
	cellHeight = atoi(tokens[1].c_str());
}

void CQuadTree::_ParseSection_OBJECTS(string line)
{
	vector<string> tokens = split(line);
	DebugOut(L"--> %s\n", ToWSTR(line).c_str());

	CAnimationSets* animation_sets = CAnimationSets::GetInstance();

	if (tokens.size() < 4) return; // skip invalid lines

	int x = atoi(tokens[1].c_str());
	int y = atoi(tokens[2].c_str());

	/*int cellX = atoi(tokens[5].c_str());
	int cellY = atoi(tokens[6].c_str());*/

	int object_type = atoi(tokens[0].c_str());

	int ani_set_id = atoi(tokens[3].c_str());

	/*int renderLayer = atoi(tokens[4].c_str());*/

	CGameObject* obj = NULL;

	switch (object_type)
	{
	case OBJECT_TYPE_GOOMBA: obj = new CGoomba(); break;
	case OBJECT_TYPE_BRICK: obj = new CBrick(); break;
	case OBJECT_TYPE_LASERGUARD: obj = new LASERGUARD(); break;
	case OBJECT_TYPE_BALLCARRY: obj = new BALLCARRY(); break;
	case OBJECT_TYPE_BALLBOT: obj = new BALLBOT(); break;
	case OBJECT_TYPE_DRAP: obj = new DRAP(); break;
	case OBJECT_TYPE_CGX680: obj = new GX680(); break;
	case OBJECT_TYPE_CGX680S: obj = new GX680S(); break;
	case OBJECT_TYPE_CSTUKA: obj = new STUKA(); break;
	case OBJECT_TYPE_NoCollisionObject: obj = new CNoCollisionObject(); break;
	case OBJECT_TYPE_EYELET: 
	{
		float kill_point = atoi(tokens[4].c_str());
		obj = new EYELET(kill_point);
	}
	break;
	case OBJECT_TYPE_CINTERCRUPT: obj = new INTERRUPT(); break;
	
	default:
		DebugOut(L"[ERR] Invalid object type: %d\n", object_type);
		return;
	}
	// General object setup
	LPANIMATION_SET ani_set = animation_sets->Get(ani_set_id);
	if (obj != NULL)
	{
		CPlayScene* playscene = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene());
		int add = 0;
		obj->SetPosition((float)x, playscene->getMapheight() - (float)y);
		obj->SetAnimationSet(ani_set);
		obj->SetOrigin((float)x, (float)y, obj->GetState());
		/*obj->SetrenderLayer(renderLayer);*/
		Add(obj);
	}
}

void CQuadTree::Load(LPCWSTR filePath)
{
	DebugOut(L"[INFO] Start loading quadtree resources from : %s \n", filePath);

	ifstream f;
	f.open(filePath);

	// current resource section flag
	int section = 0;

	char str[MAX_QUADTREE_LINE];
	while (f.getline(str, MAX_QUADTREE_LINE))
	{
		string line(str);

		if (line[0] == '#') continue;	// skip comment lines	

		if (line == "[MapObj]") { section = SCENE_SECTION_MapObj; continue; }

		if (line == "[SETTINGS]") {
			section = QUADTREE_SECTION_SETTINGS; continue;
		}
		if (line == "[OBJECTS]") {
			section = QUADTREE_SECTION_OBJECTS; continue;
		}
		//
		// data section
		//
		switch (section)
		{
		case SCENE_SECTION_MapObj: _ParseSection_MapObj(line); break;
		case QUADTREE_SECTION_SETTINGS: _ParseSection_SETTINGS(line); break;
		case QUADTREE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
		}
	}

	f.close();
	Plit();
	DebugOut(L"[INFO] Done loading scene resources %s\n", filePath);
}

void CQuadTree::_ParseSection_MapObj(string line)
{
	vector<string> tokens = split(line);

	if (tokens.size() < 3) return; // skip invalid lines

	int totalRowsMap = atoi(tokens[0].c_str());
	int totalColumnsMap = atoi(tokens[1].c_str());
	wstring file_path = ToWSTR(tokens[2]);

	obj = new MapObj(totalRowsMap, totalColumnsMap);
	obj->LoadMapObj(file_path.c_str());
	obj->Render(listObjects);
}

void CQuadTree::Render()
{
	for (int i = 0; i < listObjects.size(); i++)
		listObjects[i]->Render();
}

void CQuadTree::GetObjects(vector<LPGAMEOBJECT>& listObject, int CamX, int CamY)
{
	Pop(listObject, CamX, CamY);
}

void CQuadTree::Pop(vector<LPGAMEOBJECT>& Object, int CamX, int CamY)
{
	if (this == NULL)
		return;
	if (isLeaf)
	{
		for (int i = 0; i < listObjects.size(); i++)
		{
			if (inRange(x + cellWidth, y + cellHeight, CamX, CamY, CGame::GetInstance()->GetScreenWidth(), CGame::GetInstance()->GetScreenHeight()) && listObjects[i]->GetisAlive())
			if (!listObjects[i]->GetActive())
			{
				float Ox, Oy;
				listObjects[i]->GetOriginLocation(Ox, Oy);
				/*if (!inRange(Ox, Oy, CamX, CamY, CGame::GetInstance()->GetScreenWidth(), CGame::GetInstance()->GetScreenHeight()))
					listObjects[i]->reset();*/
				Object.push_back(listObjects[i]);
				listObjects[i]->SetActive(true);
			}
		}
		return;
	}

	BrachTL->Pop(Object, CamX, CamY);
	BrachTR->Pop(Object, CamX, CamY);
	BrachBL->Pop(Object, CamX, CamY);
	BrachBR->Pop(Object, CamX, CamY);
		
}

void CQuadTree::Unload()
{
	//if (this != nullptr)
	//	if (cells)
	//	{
	//		for (unsigned int i = 0; i < numCol; i++)
	//		{
	//			for (unsigned int j = 0; j < numRow; j++)
	//			{
	//				cells[i][j].Unload();
	//			}
	//		}
	//		delete cells;
	//		cells = NULL;
	//	}
}