#include "Goomba.h"
CGoomba::CGoomba()
{
	SetState(GOOMBA_STATE_WALKING);
}

// l?y vùng xung quanh
void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + GOOMBA_BBOX_WIDTH;

	if (state == GOOMBA_STATE_DIE)
		bottom = y + GOOMBA_BBOX_HEIGHT_DIE;
	else
		bottom = y + GOOMBA_BBOX_HEIGHT;
}

// v? l?i tr?ng thái
void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	//
	// TO-DO: make sure Goomba can interact with the world and to each of them too!
	// 

	x += dx;
	y += dy;


	// trái dướii
	if (vx < 0 && x <= 0) {
		x = 0; vx = 0; vy = -GOOMBA_WALKING_SPEED;
	}

	//phải trên
	if (vx > 0 && x >= 360 - 32) {
		x = 360 - 32; vy = GOOMBA_WALKING_SPEED; vx = 0;
	}

	//trái trên
	if (vy < 0 && y <= 0) {
		y = 0; vy = 0; vx = GOOMBA_WALKING_SPEED;
	}

	//phải dưới
	if (vy > 0 && y >= 240 - 33) {
		y = 240 - 33; vy = 0; vx = -GOOMBA_WALKING_SPEED;
	}
}	

/// <summary>
/// vẽ theo trạng thái
/// </summary>
void CGoomba::Render()
{
	int ani = GOOMBA_ANI_WALKING;
	if (state == GOOMBA_STATE_DIE) {
		ani = GOOMBA_ANI_DIE;
	}

	animation_set->at(ani)->Render(x, y);

	//RenderBoundingBox();
}

// xét trạng thái

void CGoomba::SetState(int state)
{
	CGameObject::SetState(state);
	switch (state)
	{
	case GOOMBA_STATE_DIE:
		y += GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE + 1;
		vx = 0;
		vy = 0;
		break;
	case GOOMBA_STATE_WALKING:
		vx = GOOMBA_WALKING_SPEED;
	}
}
