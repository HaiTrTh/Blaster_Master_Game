#include "BallBot.h"
#include "PlayScene.h"
BALLBOT::BALLBOT()
{
	SetState(CBALLBOT_STATE_IDLE);
}

void BALLBOT::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + CBALLBOT_BBOX_WIDTH;

	if (state == CBALLBOT_STATE_DIE)
		bottom = y + CBALLBOT_BBOX_HEIGHT_DIE;
	else
		bottom = y + CBALLBOT_BBOX_HEIGHT;
	if (state == CBALLBOT_STATE_DIE)
	{
		left = top = right = bottom = 0;
	}
}

void BALLBOT::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CPlayScene* playscene = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene());
	CGameObject::Update(dt, coObjects);
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	if (!spammed && state == STATE_DIE)
	{
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->AddKaboomMng(x, y);
		int chance = rand() % 100;
		srand(time(NULL));
		if (chance >= 70)
			playscene->AddItemsMng(x, y, 0);
		spammed = true;
	}

	if (!triggered) {
	if (switch_state != 0 && state != CBALLBOT_STATE_IDLE)
	{
		if ((DWORD)GetTickCount64() - pre_tickcount >= 50)
		{
			tickcount_diff += (DWORD)GetTickCount64() - pre_tickcount;
		}

		pre_tickcount = (DWORD)GetTickCount64();

		if ((DWORD)GetTickCount64() - switch_state >= CBALLBOT_SWITCH_STATE_TIME + tickcount_diff)
		{
			SetState(CBALLBOT_STATE_FLY_UP);
			if (x > playscene->GetPlayer()->GetPositionX())
			{
				vx = -2*CBALLBOT_FLYING_SPEED;
			}
			else 
			{
				vx = 2 * CBALLBOT_FLYING_SPEED;
			}
				switch_state = 0;
				tickcount_diff = 0;
		}
	}

	if (state == CBALLBOT_STATE_IDLE && playscene->IsInside(x - 20, y, x + 20, y + 100, playscene->GetPlayer()->GetPositionX(), playscene->GetPlayer()->GetPositionY()))
	{
		SetState(CBALLBOT_STATE_FALLING);
		StartSwitch_state();
	}

	if (state != CBALLBOT_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (dynamic_cast<CBrick*>(e->obj)) 
			{
				if (ny < 0 && nx == 0) 
				{
					SetState(CBALLBOT_STATE_IDLE);
					switch_state = 0;
					tickcount_diff = 0;
					pre_tickcount = 0;
					triggered = true;
				}
				if (nx != 0)
				{
					vx = 0;
				}
			}
			CGame* game = CGame::GetInstance();
			if (dynamic_cast<CSOPHIA*>(e->obj) && !playscene->GetPlayer()->getUntouchable())
			{
				playscene->GetPlayer()->StartUntouchable();
				game->setheath(game->Getheath() - 100);
			}

		}
		CGame* game = CGame::GetInstance();
		if (playscene->IsInside(x - SOPHIA_BIG_BBOX_WIDTH, y - SOPHIA_BIG_BBOX_HEIGHT, x + CBALLBOT_BBOX_WIDTH, y + CBALLBOT_BBOX_HEIGHT, playscene->GetPlayer()->GetPositionX(), playscene->GetPlayer()->GetPositionY()) && !playscene->GetPlayer()->getUntouchable())
		{
			playscene->GetPlayer()->StartUntouchable();
			game->setheath(game->Getheath() - 100);
		}
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	}
	}
}

void BALLBOT::CalcPotentialCollisions(
	vector<LPGAMEOBJECT>* coObjects,
	vector<LPCOLLISIONEVENT>& coEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));
		if (!dynamic_cast<CBrick*>(e->obj))
		{
			continue;
		}
		if (e->t > 0 && e->t <= 1.0f)
		{
			//if (dynamic_cast<CSOPHIA*>(e->obj))
				coEvents.push_back(e);
		}
		else
			delete e;
	}
	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void BALLBOT::Render()
{
	int ani = 0;
	if (state != CBALLBOT_STATE_DIE)
	{
		if (vx > 0)
		{
			int ani = CBALLBOT_ANI_RIGHT;
			pre_ani = ani;
		}
		if (vx == 0)
			ani = pre_ani;
		
		animation_set->at(ani)->Render(x, y);
		//RenderBoundingBox();
	}
	
}

void BALLBOT::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case CBALLBOT_STATE_IDLE:
		vx = 0;
		vy = 0;
		break;
	case CBALLBOT_STATE_FALLING:
		vy = -2*CBALLBOT_FLYING_SPEED;
		vx = 0;
		break;
	case CBALLBOT_STATE_FLY_UP:
		vy = CBALLBOT_FLYING_SPEED;
		vx = 0;
		break;
	case CBALLBOT_STATE_DIE:
		vy = -10*DIE_PULL;
		break;
	}
}
