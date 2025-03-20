#pragma once
#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include "data_format.h"
#include <list>

class base_class
{
public:
	void Update_Motion();
	struct position Get_Position();
	struct points Get_Point(void);
	void Interaction(struct points*, struct game_info time);//
	bool IsEnable();
	bool IsEat(struct position);
	bool IsHit(const std::list<struct position>& body);  // snake touches itself
	bool Get_Moving_Status();
	void Set_Enable(bool);
	void Motion_Init(struct position p, struct vxy v, struct points poi)
	{
		pos = p;
		V = v;
		point = poi;
	}
	struct points point;
	bool enable = 0;
	char name[20];
	float rate = 1;

private:
	float Velocity;

protected:
	struct position pos;
	struct vxy V;
	int counter;
	float diameter = 15;
	bool is_moving = 0;
};

class class_snake :public base_class
{
public:
	void Update_Motion_VW(void);
	void Set_Velocity(struct vxy);
	void Motion_Init(struct position p, struct vw v, struct points point);
	void Set_Position(struct position);
	void Grow_up(struct position pos);
	void Shrink_Length();
	void Set_Moving_Status(bool moving);
	void EXP(int exp);
	void LV(int lv);
	void Reset_Ability(class class_food food);
	void Reverse();
	bool EXP_Check(game_info game);
	struct vw VW;
	//ability
	int reverse = 1;//1代表正常,-1代表inverse
	int shrink_state = 1;//每輪food只縮短一次
	const std::list<struct position> Get_Body() const;

private:
	int body_length = 15;
	std::list<struct position> body;
};

class class_food :public base_class
{
public:
	void Set_Position(struct position);
	void Freeze();
	void Dodge(class class_snake snake);
	int dodge_time = 1;//閃躲一次
	void Tracking(class class_snake snake);
};