#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "data_format.h"
#include "rigid_body.h"

void base_class::Update_Motion()
{
	if (this->enable == 1)
	{
		this->pos.X = this->pos.X + this->rate * this->V.Vx * 0.15;
		this->pos.Y = this->pos.Y + this->rate * this->V.Vy * 0.15;
	}
}

struct position base_class::Get_Position()
{
	return this->pos;
}

bool base_class::Get_Moving_Status()
{
	return is_moving;
}

/*
void base_class::Interaction(struct points *s_point)
{
	s_point->score = s_point->score + this->point.score;
	this->enable = 0;
}
*/

void base_class::Interaction(struct points* s_point, struct game_info time)
{
	//eat food score
	if (time.bonus_second / 10 >= 3)
	{
		s_point->score = s_point->score + 2 + 2;//base point + time bonus
	}
	else if (time.bonus_second / 10 < 3 && time.bonus_second / 10 >= 0)
	{
		s_point->score = s_point->score + 2 + 1;//base point + time bonus
	}
	else
	{
		s_point->score = s_point->score + 2;
	}

	//	this->enable = 0;
}

bool base_class::IsEnable()
{
	return this->enable;
}

bool base_class::IsEat(struct position target)
{
	float dis = sqrt((target.X - pos.X) * (target.X - pos.X)
		+ (target.Y - pos.Y) * (target.Y - pos.Y));

	if (dis < diameter)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

bool base_class::IsHit(const std::list<struct position>& body)
{
	if (body.size() <= 15)
	{
		return 0;
	}

	auto it = std::begin(body);

	if (std::distance(it, std::end(body)) >= 4)
	{
		std::advance(it, 3);

		for (; it != std::end(body); it++)
		{
			if (it->X == pos.X && it->Y == pos.Y)
			{
				return 1;
			}
		}
	}

	return 0;
}

struct points base_class::Get_Point(void)
{
	return this->point;
}

void base_class::Set_Enable(bool b)
{
	this->enable = b;
}

//EXP====================
void class_snake::EXP(int exp)
{
	this->point.exp += exp;
}
//level==================
void class_snake::LV(int lv)// lv = 1
{
	int(this->point.level) = this->point.level + lv;//¨C¤TÃö¤Éµ¥
}

// snake
void class_snake::Motion_Init(struct position p, struct vw v, struct points poi)
{
	this->pos = p;
	this->VW = v;
	this->point = poi;
}

void class_snake::Update_Motion_VW(void)
{
	Set_Moving_Status(1);

	this->pos.X = this->pos.X + this->rate * this->VW.V * 0.1;
	this->pos.Y = this->pos.Y + this->rate * this->VW.W * 0.1;

	body.push_front(pos);
	while (body.size() > static_cast<size_t>(body_length))
	{
		body.pop_back();
	}
}

const std::list<struct position> class_snake::Get_Body() const
{
	return body;
}

void class_snake::Set_Velocity(struct vxy V)
{
	this->VW.V = V.Vx;
	this->VW.W = V.Vy;
}

void class_snake::Set_Position(struct position posi)
{
	this->pos.X = posi.X;
	this->pos.Y = posi.Y;
	this->pos.Theta = posi.Theta;
}

void class_snake::Grow_up(struct position pos)
{
	if (shrink_state == 0)
	{
		body_length -= 8;
	}
	else
	{
		body_length += 5;
	}
}

void class_snake::Shrink_Length()
{
	if (this->shrink_state == 1)
	{
		this->shrink_state = 0;
	}
}

void class_snake::Set_Moving_Status(bool moving)
{
	is_moving = moving;
}

void class_snake::Reverse()
{
	this->reverse = -1;
}

void class_snake::Reset_Ability(class class_food food)
{
	this->reverse = 1;
	this->shrink_state = 1;
	food.dodge_time = 1;
	//
	//
}

bool class_snake::EXP_Check(game_info game)
{
	if (game.ability_state == 1 || game.ability_state == 2 || game.ability_state == 4)//exp達標和反效果
	{
		if (this->point.exp >= 5)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}



// food
void class_food::Set_Position(struct position posi)
{
	this->pos.X = posi.X;
	this->pos.Y = posi.Y;
	this->pos.Theta = posi.Theta;
}

void class_food::Freeze()
{
	this->V.Vx = 0;
	this->V.Vy = 0;
}

void class_food::Dodge(class class_snake snake)
{
	float dis = sqrt((snake.Get_Position().X - this->pos.X) * (snake.Get_Position().X - this->pos.X)
		+ (snake.Get_Position().Y - this->pos.Y) * (snake.Get_Position().Y - this->pos.Y));

	if (dis < 30 && this->dodge_time == 1)
	{
		this->pos.X = rand() % 420;
		this->pos.Y = rand() % 420;
		this->dodge_time = 0;
	}
}
void class_food::Tracking(class class_snake snake)
{
	// 计算物体到目标的向量
	double dx = snake.Get_Position().X - this->pos.X;
	double dy = snake.Get_Position().Y - this->pos.Y;

	// 计算向量的单位向量
	float dist = sqrt((snake.Get_Position().X - pos.X) * (snake.Get_Position().X - pos.X) + (snake.Get_Position().Y - pos.Y) * (snake.Get_Position().Y - pos.Y));
	double unitVectorX = dx / dist;
	double unitVectorY = dy / dist;

	// 计算速度向量
	this->V.Vx = unitVectorX * 15;
	this->V.Vy = unitVectorY * 15;
}