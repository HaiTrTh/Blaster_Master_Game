#include "GX680.h"
GX680::GX680()
{
	SetState(STATE_IDLE);
}

void GX680::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == STATE_DIE)
		return;
	left = x;
	top = y;
	right = x + CGX680_BBOX_WIDTH;

	if (state == CGX680_STATE_DIE)
		bottom = y + CGX680_BBOX_HEIGHT_DIE;
	else
		bottom = y + CGX680_BBOX_HEIGHT;
}

void GX680::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt);

	CPlayScene* playscene = ((CPlayScene*)CGame::GetInstance()->GetCurrentScene());
	CGame* game = CGame::GetInstance();

	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;

	if (state != STATE_DIE)
	{
		if (playscene->IsInside(x - 200, y - 200, x + 200, y + 200, playscene->GetPlayer2()->GetPositionX(), playscene->GetPlayer2()->GetPositionY()))
		{
			StartSwitch_state();
			StartAttack();
		}

		if ((DWORD)GetTickCount64() - switch_state >= CGX680_WALKING_TIME && switch_state != 0)
		{
			switch_state = 0;

			StartSwitch_state();

			vx = (playscene->GetPlayer2()->GetPositionX() - x) / abs(playscene->GetPlayer2()->GetPositionX() - x) * CGX680_WALKING_SPEED;
			vy = -(playscene->GetPlayer2()->GetPositionY() - y) / abs(playscene->GetPlayer2()->GetPositionY() - y) * CGX680_WALKING_SPEED;
		}

		if ((DWORD)GetTickCount64() - attacking >= CGX680_ATTACKING_TIME && attacking != 0)
		{
			attacking = 0;

			StartAttack();

			float distant = (abs(playscene->GetPlayer2()->GetPositionX()) - abs(x)) / sqrt(pow(playscene->GetPlayer2()->GetPositionX(), 2) + pow(x, 2));
			float distant2 = (abs(playscene->GetPlayer2()->GetPositionY()) - JASON_SMALL_BBOX_HEIGHT / 2 - abs(y)) / sqrt(pow(playscene->GetPlayer2()->GetPositionY(), 2) + pow(y, 2));

			float bx = distant / 6;
			float by = -distant2 / 6;

			playscene->AddCGXMng(x, y, bx, by);
		}
	}
	else
	{
		if (!spammed)
		{
			int chance = rand() % 100;
			srand(time(NULL));
			if (chance >= 70)
				playscene->AddItemsMng(x, y, 0);
			spammed = true;
		}
	}
	if (state != STATE_DIE)
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

		// block every object first!
		//x += min_tx * dx + nx * 0.4f;
		//y += min_ty * dy + ny * 0.4f;

		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		//
		// Collision logic with other objects
		//

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<JASON*>(e->obj) && !playscene->GetPlayer2()->getUntouchable())
			{
				playscene->GetPlayer2()->StartUntouchable();
				game->setheath(game->Getheath() - 100);
			}
		}
	}

	// clean up collision events
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void GX680::Render()
{
	if (state != STATE_DIE)
	{
		int ani = CGX680_ANI;

		animation_set->at(ani)->Render(x, y);

		//RenderBoundingBox();
	}
}

void GX680::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case STATE_IDLE:
		vx = 0;
		vy = 0;
		break;
	case STATE_DIE:
		((CPlayScene*)CGame::GetInstance()->GetCurrentScene())->AddKaboomMng(x, y);
		vy = DIE_PULL;
		break;
	}
}
