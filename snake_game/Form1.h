#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <cmath>
#include <list>
#include <vcclr.h> 
#include "data_format.h"
#include "rigid_body.h"
using namespace System::Runtime::InteropServices;

// global declaration
int system_counter = 0;
struct game_info game;
class_snake snake;
class_food food;

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections::Generic;
	using namespace System::Drawing::Drawing2D;

	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			this->KeyPreview = true;
			this->KeyDown += gcnew KeyEventHandler(this, &Form1::Form1_KeyDown);
			this->Focus();

			game.mode = -1;
			comboBox1->Items->Add("Classic");
			comboBox1->Items->Add("Time-Limited");
			comboBox1->Items->Add("Challenge");

			this->SetStyle(System::Windows::Forms::ControlStyles::AllPaintingInWmPaint |
				System::Windows::Forms::ControlStyles::DoubleBuffer |
				System::Windows::Forms::ControlStyles::UserPaint, true);

			this->DoubleBuffered = true;

			pictureBox1 = gcnew PictureBox();
			pictureBox1->Location = System::Drawing::Point(0, 0);
			pictureBox1->Size = System::Drawing::Size(420, 420);
			pictureBox1->BackColor = Color::WhiteSmoke;
			pictureBox1->Size = System::Drawing::Size(this->Width, this->Height);
			pictureBox1->SizeMode = PictureBoxSizeMode::StretchImage;

			this->Controls->Add(pictureBox1);
			bmp = gcnew Bitmap(pictureBox1->Width, pictureBox1->Height);
		}

	protected:
		void motion(void)
		{
			// snake
			struct position pos;
			struct vxy V;

			snake.Update_Motion_VW();

			// food
			if (food.IsEnable() == 1)
			{
				food.Update_Motion();
				if (food.Get_Position().X > 390)
				{
					pos.X = 390;
					pos.Y = food.Get_Position().Y;
					pos.Theta = food.Get_Position().Theta;
					food.Set_Position(pos);
				}
				if (food.Get_Position().X < 0)
				{
					pos.X = 0;
					pos.Y = food.Get_Position().Y;
					pos.Theta = food.Get_Position().Theta;
					food.Set_Position(pos);
				}
				if (food.Get_Position().Y > 390)
				{
					pos.X = food.Get_Position().X;
					pos.Y = 390;
					pos.Theta = food.Get_Position().Theta;
					food.Set_Position(pos);
				}
				if (food.Get_Position().Y < 0)
				{
					pos.X = food.Get_Position().X;
					pos.Y = 0;
					pos.Theta = food.Get_Position().Theta;
					food.Set_Position(pos);
				}
			}
		}
		// ==================================================================================
		void plot(void)
		{
			Graphics^ g = Graphics::FromImage(bmp);

			if (game.game_over == 0)
			{
				//Graphics^ e1 = this->CreateGraphics();
				g->Clear(Color::WhiteSmoke);
				SolidBrush^ head = gcnew SolidBrush(Color::ForestGreen);
				SolidBrush^ body_Pen = gcnew SolidBrush(Color::ForestGreen);
				SolidBrush^ food_Pen = gcnew SolidBrush(Color::Navy);
				SolidBrush^ sight = gcnew SolidBrush(Color::Black);
				Pen^ edge = gcnew Pen(Color::Gray, 4);

				int R_size = 15;
				int grid_width = 400;
				int grid_height = 400;
				int grid_resolution = 1;
				int shift_x = (grid_width)-390;
				int shift_y = (grid_height)+0;
				int Y_shift = 0;
				int width = 420;
				int height = 420;

				// snake
				int head_x = (snake.Get_Position().X) / grid_resolution;
				int head_y = (snake.Get_Position().Y + Y_shift) / grid_resolution;
				int x1 = (snake.Get_Position().X) / grid_resolution;
				int y1 = (snake.Get_Position().Y + Y_shift) / grid_resolution;
				g->FillEllipse(head, shift_x + x1, shift_y - 10 - y1, R_size * 1.5, R_size * 1.5);

				// snake body
				std::list<struct position> body_ = snake.Get_Body();

				for (auto segment : body_)
				{
					x1 = (segment.X) / grid_resolution;
					y1 = (segment.Y + Y_shift) / grid_resolution;
					g->FillEllipse(body_Pen, shift_x + x1, shift_y - 10 - y1, R_size * 1.5, R_size * 1.5);
				}

				// edge
				g->DrawRectangle(edge, 0, 0, width, height);

				// food
				if (food.IsEnable() == 1)
				{
					x1 = (food.Get_Position().X) / grid_resolution;
					y1 = (food.Get_Position().Y + Y_shift) / grid_resolution;
					g->FillEllipse(food_Pen, shift_x + 0 + x1, shift_y - y1, R_size, R_size);
				}

				// map
				if (game.mode == 0)
				{
					int box_size = 15;

					Bitmap^ buffer = gcnew Bitmap(grid_width, grid_height);
					Graphics^ bufferGraphics = Graphics::FromImage(buffer);
					String^ mapFile;

					// simple
					if (snake.Get_Point().level <= 3)
					{
						plot_map_coord(g, "line");
					}

					//  median
					else if (snake.Get_Point().level > 3 && snake.Get_Point().level <= 5)
					{
						plot_map_coord(g, "F");
					}

					// difficult
					else
					{
						array<String^>^ lines = System::IO::File::ReadAllLines("C.txt");
						plot_map(lines, bufferGraphics, box_size);
					}

					g->DrawImage(buffer, 0, 0);
					delete bufferGraphics;
					delete buffer;
				}

				// for ability
				if (game.ability_state == 4 && snake.point.exp < 5) {

					GraphicsPath^ path = gcnew GraphicsPath();
					path->AddEllipse(shift_x + head_x - R_size - 50, shift_y - head_y - R_size - 60, R_size * 10, R_size * 10);
					g->SetClip(path, CombineMode::Exclude);
					g->SetClip(path, CombineMode::Exclude);
					g->FillRectangle(sight, 0, 0, width, height);
					g->ResetClip();
				}
				pictureBox1->Image = bmp;
			}

			else
			{
				g->Clear(Color::Black);

				label1->Visible = false;
				label2->Visible = false;
				label3->Visible = false;
				label4->Visible = false;
				label5->Visible = true;
				label6->Visible = false;
				label7->Visible = false;
				label8->Visible = false;
				label9->Visible = false;
				label17->Visible = false;
				label10->Visible = false;

				String^ text = "Your score is " + snake.Get_Point().score;
				System::Drawing::Font^ font = gcnew System::Drawing::Font("Times New Roman", 18);
				SolidBrush^ brush = gcnew SolidBrush(Color::PapayaWhip);
				PointF point = PointF(203.0f, 154.0f);
				g->DrawString(text, font, brush, point);
				// Update the PictureBox with the new bitmap
				pictureBox1->Image = bmp;

				//ranking
				if (game.mode == 0)
				{
					if (game.end == false)
					{
						RANK(snake.Get_Point().score, game);
						game.end = true;
					}
					else
					{
						int score1, score2, score3, score4, score5;
						panel1->Visible = true;
						this->label16->Text = ("Rank");
						FILE* pFile;
						pFile = fopen("score.dat", "r");
						fscanf(pFile, "%d", &score1);
						fscanf(pFile, "%d", &score2);
						fscanf(pFile, "%d", &score3);
						fscanf(pFile, "%d", &score4);
						fscanf(pFile, "%d", &score5);

						this->label11->Text = ("No.1: " + Convert::ToString(score1));
						this->label12->Text = ("No.2: " + Convert::ToString(score2));
						this->label13->Text = ("No.3: " + Convert::ToString(score3));
						this->label14->Text = ("No.4: " + Convert::ToString(score4));
						this->label15->Text = ("No.5: " + Convert::ToString(score5));
					}
				}
			}

			// Clean up resources
			delete g;
		}
		// ==================================================================================
		void plot_map(array<String^>^ lines, Graphics^ graphics, int box_size)
		{
			for (int i = 0; i < lines->Length; i++)
			{
				String^ line = lines[i];

				for (int j = 0; j < line->Length; j++)
				{
					if (line[j] == '1')
					{
						int x = j * box_size;
						int y = i * box_size;
						graphics->FillRectangle(Brushes::Gray, x, y, box_size, box_size);
					}
				}
			}
		}
		// ==================================================================================
		void plot_map_coord(Graphics^ g, String^ map)
		{
			SolidBrush^ pen = gcnew SolidBrush(Color::Gray);
			// simple
			if (map == "line")
			{
				g->FillRectangle(pen, 170, 150, 250, 30);
				g->FillRectangle(pen, 0, 300, 250, 30);
			}

			// median
			if (map == "F")
			{
				g->FillRectangle(pen, 55, 290, 310, 30);
				g->FillRectangle(pen, 55, 110, 30, 180);
				g->FillRectangle(pen, 175, 110, 30, 180);
			}
		}
		// ==================================================================================
		void collision(void)
		{
			// snake touches itself
			std::list<struct position> body = snake.Get_Body();

			if (snake.Get_Moving_Status() == 1)
			{
				if (snake.IsHit(body))
				{
					game.game_over = 1;
				}
			}

			// out of the edge
			if (snake.Get_Position().X > 390 || snake.Get_Position().X < 0
				|| snake.Get_Position().Y > 390 || snake.Get_Position().Y < 0)
			{
				game.game_over = 1;
			}

			// crash obstacles(map)
			if (game.mode == 0)
			{
				int box_size = 15;
				int headX = snake.Get_Position().X;// box_size;
				int headY = snake.Get_Position().Y; //box_size;

				struct position initpos;
				initpos.X = 0;
				initpos.Y = 0;

				// simple
				// lines
				if (snake.Get_Point().level <= 3 && game.ability_state != 5)
				{
					if ((headX >= 0 && headX <= 225 && headY >= 70 && headY <= 110)
						|| (headX >= 150 && headX <= 400 && headY >= 215 && headY <= 255))
					{
						game.game_over = 1;
					}
				}

				// median
				// F
				else if (snake.Get_Point().level > 3 && snake.Get_Point().level <= 5 && game.ability_state != 5)
				{
					//System::Diagnostics::Debug::WriteLine("pos (" + Convert::ToString(headX) + ", " + Convert::ToString(headY) + ")");

					if ((headX >= 35 && headX <= 345 && headY >= 70 && headY <= 110)
						|| (headX >= 30 && headX <= 70 && headY >= 110 && headY <= 298)
						|| (headX >= 145 && headX <= 175 && headY >= 110 && headY <= 298))
					{
						game.game_over = 1;
					}
				}

				// difficult
				else if (game.ability_state != 5)
				{
					int adjustedY = (headY / box_size) + 1;
					int adjustedX = (headX / box_size) + 1;
					array<String^>^ lines = System::IO::File::ReadAllLines("C.txt");

					if (adjustedY >= 0 && adjustedY < lines->Length && adjustedX >= 0 && adjustedX < lines[adjustedY]->Length)
					{
						if (lines[adjustedY][adjustedX] == '1')
						{
							game.game_over = 1;
						}
					}
				}
			}
			// snake and food
			if (food.IsEat(snake.Get_Position()) == 1)
			{
				food.Interaction(&snake.point, game);
				snake.Grow_up(snake.Get_Position());

				// new food
				struct position new_pos;
				struct vxy new_v;
				struct points new_point;
				new_pos.X = rand() % 420;
				new_pos.Y = rand() % 420;
				new_v.Vx = rand() % 20 - 10;
				new_v.Vy = rand() % 20 - 10;
				food.Motion_Init(new_pos, new_v, new_point);
				food.Set_Enable(1);
				game.bonus_second = 50;
				snake.EXP(1);// cumulate 1 exp when eating food   ## every five exp offset the negative effect automatively
				snake.LV(1);

				snake.Reset_Ability(food);

				srand(time(NULL));
				game.ability_state = game.next_ability;//rand()
				game.next_ability = rand() % 6;

			}
		}
		// ==================================================================================
		void load_profile(void)
		{
			FILE* pFile;
			pFile = fopen("profile.dat", "r");

			char name[20];
			struct points point;
			struct position pos;
			struct vw VW;
			VW.V = 0;
			VW.W = 0;

			fscanf(pFile, "%s", &name);
			fscanf(pFile, "%f", &pos.X);
			fscanf(pFile, "%f", &pos.Y);
			fscanf(pFile, "%f", &pos.Theta);
			fscanf(pFile, "%i", &system_counter);
			fscanf(pFile, "%f", &point.score);
			fscanf(pFile, "%f", &point.level);

			snake.Motion_Init(pos, VW, point);
			fclose(pFile);
		}
		// ==================================================================================
		void RANK(int final_score, game_info game)
		{
			FILE* pFile1;
			pFile1 = fopen("score.dat", "r");

			int dataArray[6];
			int value;

			// 讀取前五筆資料
			for (int i = 0; i < 6; ++i) {
				if (i <= 4)
				{
					fscanf(pFile1, "%d", &value);
					dataArray[i] = value;
				}
				else
				{
					//加新資料
					dataArray[5] = final_score;
				}
			}
			for (int i = 0; i < 5; i++)
			{
				printf("before sorting %d\n", dataArray[i]);
			}
			// sort
			for (int i = 0; i < 6 - 1; ++i) {
				for (int j = 0; j < 6 - i - 1; ++j) {
					if (dataArray[j] < dataArray[j + 1]) {
						int temp = dataArray[j];
						dataArray[j] = dataArray[j + 1];
						dataArray[j + 1] = temp;
					}
				}
			}
			FILE* pFile2;
			//寫入新排名 前五筆
			pFile2 = fopen("score.dat", "w");
			for (int i = 0; i < 5; i++)
			{
				fprintf(pFile2, "%d\n", dataArray[i]);
			}
			fclose(pFile2);
		}
		// ==================================================================================
		void text_displayer(void)
		{
			this->label1->Text = ("SCORE: " + Convert::ToString(snake.Get_Point().score));
			this->label2->Text = ("TIME: " + Convert::ToString(game.second / 600) + ":" + Convert::ToString((game.second / 10) % 60));
			this->label3->Text = ("TIMES UP!");
			this->label6->Text = ("TIME: " + Convert::ToString(game.counter / 600) + ":" + Convert::ToString((game.counter / 10) % 60));
			this->label7->Text = ("LEVEL: " + Convert::ToString(snake.point.level));
			this->label8->Text = ("EXP: " + Convert::ToString(snake.point.exp));
			this->label10->Text = ("CURE");

		}
		// ==================================================================================
		void Time_Bonus_Counter(void)
		{
			game.bonus_second--;
		}
		//================================================================ABILITY
		//======啟動條件food collision
		void Ability(game_info game)
		{
			if (snake.EXP_Check(game) && game.game_over == 0)//exp達標和反效果
			{
				label10->Visible = true;
			}

			switch (game.ability_state)
			{
			case 0:
				// 冰凍food
				food.Tracking(snake);//ok
				strcpy(game.current_ability_text, "追蹤");
				break;
			case 1:
				//反向操作
				snake.Reverse();//ok
				strcpy(game.current_ability_text, "反向");
				break;
			case 2://閃躲
				food.Dodge(snake);
				strcpy(game.current_ability_text, "閃躲");
				break;
			case 3:
				// 縮短蛇長度
				snake.Shrink_Length();
				strcpy(game.current_ability_text, "縮短");
				break;
			case 4:
				// 限制視野
				strcpy(game.current_ability_text, "限制視野");
				break;
			case 5://穿牆
				strcpy(game.current_ability_text, "穿牆");
				break;
			}
			String^ prefix = "CURRENT: ";
			this->label17->Text = String::Concat(prefix, gcnew String(game.current_ability_text));
		}
		// ==================================================================================
		void NEXT(game_info game)
		{
			switch (game.next_ability)
			{
			case 0:
				strcpy(game.ability_state_text, "追蹤");
				break;
			case 1:
				strcpy(game.ability_state_text, "反向");
				break;
			case 2:
				strcpy(game.ability_state_text, "閃躲");
				break;
			case 3:
				strcpy(game.ability_state_text, "縮短");
				break;
			case 4:
				strcpy(game.ability_state_text, "限制視野");
				break;
			case 5:
				strcpy(game.ability_state_text, "穿牆");
				break;
			}
			String^ prefix = "NEXT: ";
			this->label9->Text = String::Concat(prefix, gcnew String(game.ability_state_text));
		}
		// ==================================================================================
		void initialize(void)
		{
			load_profile();

			struct position pos;
			struct vxy V;
			struct points point;

			srand(time(NULL));
			pos.X = rand() % 420;
			pos.Y = rand() % 420;
			V.Vx = rand() % 20 - 10;
			V.Vy = rand() % 20 - 10;
			food.point.score = 1;
			food.Motion_Init(pos, V, point);
			food.Set_Enable(1);
			snake.point.exp = 0;
			snake.point.level = 1;
			game.next_ability = rand() % 6;

		}

		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Timer^ timer1;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::ComboBox^ comboBox1;
	private: System::Windows::Forms::Button^ button1;
	private:System::Drawing::Graphics^ graphics;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::Label^ label5;
	private: System::Windows::Forms::Label^ label6;
	private: System::Windows::Forms::Label^ label7;
	private: System::Windows::Forms::Label^ label8;
	private: System::Windows::Forms::Label^ label9;
	private: System::Windows::Forms::Label^ label10;

	protected:

	private: System::ComponentModel::IContainer^ components;

	private:
		PictureBox^ pictureBox1;
		Bitmap^ bmp;
	private: System::Windows::Forms::Label^ label11;
	private: System::Windows::Forms::Label^ label12;
	private: System::Windows::Forms::Label^ label13;
	private: System::Windows::Forms::Label^ label14;
	private: System::Windows::Forms::Label^ label15;
	private: System::Windows::Forms::Panel^ panel1;
	private: System::Windows::Forms::Label^ label16;
	private: System::Windows::Forms::Label^ label17;
		   String^ currentMapFile;

#pragma region Windows Form Designer generated code

		   void InitializeComponent(void)
		   {
			   this->components = (gcnew System::ComponentModel::Container());
			   this->timer1 = (gcnew System::Windows::Forms::Timer(this->components));
			   this->label1 = (gcnew System::Windows::Forms::Label());
			   this->label2 = (gcnew System::Windows::Forms::Label());
			   this->label3 = (gcnew System::Windows::Forms::Label());
			   this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			   this->button1 = (gcnew System::Windows::Forms::Button());
			   this->label4 = (gcnew System::Windows::Forms::Label());
			   this->label5 = (gcnew System::Windows::Forms::Label());
			   this->label6 = (gcnew System::Windows::Forms::Label());
			   this->label7 = (gcnew System::Windows::Forms::Label());
			   this->label8 = (gcnew System::Windows::Forms::Label());
			   this->label9 = (gcnew System::Windows::Forms::Label());
			   this->label10 = (gcnew System::Windows::Forms::Label());
			   this->label11 = (gcnew System::Windows::Forms::Label());
			   this->label12 = (gcnew System::Windows::Forms::Label());
			   this->label13 = (gcnew System::Windows::Forms::Label());
			   this->label14 = (gcnew System::Windows::Forms::Label());
			   this->label15 = (gcnew System::Windows::Forms::Label());
			   this->panel1 = (gcnew System::Windows::Forms::Panel());
			   this->label16 = (gcnew System::Windows::Forms::Label());
			   this->label17 = (gcnew System::Windows::Forms::Label());
			   this->panel1->SuspendLayout();
			   this->SuspendLayout();
			   // 
			   // timer1
			   // 
			   this->timer1->Enabled = true;
			   this->timer1->Tick += gcnew System::EventHandler(this, &Form1::timer1_Tick);
			   // 
			   // label1
			   // 
			   this->label1->AutoSize = true;
			   this->label1->Enabled = false;
			   this->label1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9));
			   this->label1->Location = System::Drawing::Point(968, 19);
			   this->label1->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label1->Name = L"label1";
			   this->label1->Size = System::Drawing::Size(70, 27);
			   this->label1->TabIndex = 0;
			   this->label1->Text = L"label1";
			   this->label1->Visible = false;
			   // 
			   // label2
			   // 
			   this->label2->AutoSize = true;
			   this->label2->Enabled = false;
			   this->label2->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label2->Location = System::Drawing::Point(968, 80);
			   this->label2->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label2->Name = L"label2";
			   this->label2->Size = System::Drawing::Size(70, 27);
			   this->label2->TabIndex = 1;
			   this->label2->Text = L"label2";
			   this->label2->Visible = false;
			   // 
			   // label3
			   // 
			   this->label3->BackColor = System::Drawing::SystemColors::Desktop;
			   this->label3->Font = (gcnew System::Drawing::Font(L"Times New Roman", 18, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label3->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			   this->label3->Location = System::Drawing::Point(409, 193);
			   this->label3->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label3->Name = L"label3";
			   this->label3->Size = System::Drawing::Size(404, 105);
			   this->label3->TabIndex = 0;
			   this->label3->Text = L"Times Up!";
			   this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			   this->label3->Visible = false;
			   // 
			   // comboBox1
			   // 
			   this->comboBox1->FormattingEnabled = true;
			   this->comboBox1->Location = System::Drawing::Point(95, 103);
			   this->comboBox1->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			   this->comboBox1->Name = L"comboBox1";
			   this->comboBox1->Size = System::Drawing::Size(195, 32);
			   this->comboBox1->TabIndex = 2;
			   // 
			   // button1
			   // 
			   this->button1->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9));
			   this->button1->Location = System::Drawing::Point(351, 95);
			   this->button1->Margin = System::Windows::Forms::Padding(4, 5, 4, 5);
			   this->button1->Name = L"button1";
			   this->button1->Size = System::Drawing::Size(94, 49);
			   this->button1->TabIndex = 3;
			   this->button1->Text = L"Start";
			   this->button1->UseVisualStyleBackColor = true;
			   this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			   // 
			   // label4
			   // 
			   this->label4->AutoSize = true;
			   this->label4->Font = (gcnew System::Drawing::Font(L"Times New Roman", 16));
			   this->label4->Location = System::Drawing::Point(88, 19);
			   this->label4->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label4->Name = L"label4";
			   this->label4->Size = System::Drawing::Size(395, 49);
			   this->label4->TabIndex = 4;
			   this->label4->Text = L"Choose a game mode:";
			   // 
			   // label5
			   // 
			   this->label5->AutoSize = true;
			   this->label5->BackColor = System::Drawing::SystemColors::Desktop;
			   this->label5->Font = (gcnew System::Drawing::Font(L"Times New Roman", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label5->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			   this->label5->Location = System::Drawing::Point(430, 205);
			   this->label5->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label5->Name = L"label5";
			   this->label5->Size = System::Drawing::Size(350, 73);
			   this->label5->TabIndex = 5;
			   this->label5->Text = L"Game Over";
			   this->label5->Visible = false;
			   // 
			   // label6
			   // 
			   this->label6->AutoSize = true;
			   this->label6->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label6->Location = System::Drawing::Point(968, 80);
			   this->label6->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label6->Name = L"label6";
			   this->label6->Size = System::Drawing::Size(70, 27);
			   this->label6->TabIndex = 6;
			   this->label6->Text = L"label6";
			   this->label6->Visible = false;
			   // 
			   // label7
			   // 
			   this->label7->AutoSize = true;
			   this->label7->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label7->Location = System::Drawing::Point(968, 667);
			   this->label7->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label7->Name = L"label7";
			   this->label7->Size = System::Drawing::Size(70, 27);
			   this->label7->TabIndex = 7;
			   this->label7->Text = L"label7";
			   this->label7->Visible = false;
			   // 
			   // label8
			   // 
			   this->label8->AutoSize = true;
			   this->label8->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label8->Location = System::Drawing::Point(968, 720);
			   this->label8->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label8->Name = L"label8";
			   this->label8->Size = System::Drawing::Size(70, 27);
			   this->label8->TabIndex = 8;
			   this->label8->Text = L"label8";
			   this->label8->Visible = false;
			   // 
			   // label9
			   // 
			   this->label9->AutoSize = true;
			   this->label9->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label9->Location = System::Drawing::Point(968, 815);
			   this->label9->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label9->Name = L"label9";
			   this->label9->Size = System::Drawing::Size(70, 27);
			   this->label9->TabIndex = 9;
			   this->label9->Text = L"label9";
			   this->label9->Visible = false;
			   // 
			   // label10
			   // 
			   this->label10->AutoSize = true;
			   this->label10->Font = (gcnew System::Drawing::Font(L"新細明體", 9));
			   this->label10->Location = System::Drawing::Point(969, 857);
			   this->label10->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label10->Name = L"label10";
			   this->label10->Size = System::Drawing::Size(75, 24);
			   this->label10->TabIndex = 10;
			   this->label10->Text = L"label10";
			   this->label10->Visible = false;
			   // 
			   // label11
			   // 
			   this->label11->AutoSize = true;
			   this->label11->Font = (gcnew System::Drawing::Font(L"新細明體", 14));
			   this->label11->Location = System::Drawing::Point(133, 115);
			   this->label11->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label11->Name = L"label11";
			   this->label11->Size = System::Drawing::Size(123, 38);
			   this->label11->TabIndex = 11;
			   this->label11->Text = L"label11";
			   // 
			   // label12
			   // 
			   this->label12->AutoSize = true;
			   this->label12->Font = (gcnew System::Drawing::Font(L"新細明體", 14));
			   this->label12->Location = System::Drawing::Point(133, 171);
			   this->label12->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label12->Name = L"label12";
			   this->label12->Size = System::Drawing::Size(123, 38);
			   this->label12->TabIndex = 12;
			   this->label12->Text = L"label12";
			   // 
			   // label13
			   // 
			   this->label13->AutoSize = true;
			   this->label13->Font = (gcnew System::Drawing::Font(L"新細明體", 14));
			   this->label13->Location = System::Drawing::Point(133, 224);
			   this->label13->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label13->Name = L"label13";
			   this->label13->Size = System::Drawing::Size(123, 38);
			   this->label13->TabIndex = 13;
			   this->label13->Text = L"label13";
			   // 
			   // label14
			   // 
			   this->label14->AutoSize = true;
			   this->label14->Font = (gcnew System::Drawing::Font(L"新細明體", 14));
			   this->label14->Location = System::Drawing::Point(133, 281);
			   this->label14->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label14->Name = L"label14";
			   this->label14->Size = System::Drawing::Size(123, 38);
			   this->label14->TabIndex = 14;
			   this->label14->Text = L"label14";
			   // 
			   // label15
			   // 
			   this->label15->AutoSize = true;
			   this->label15->Font = (gcnew System::Drawing::Font(L"新細明體", 14));
			   this->label15->Location = System::Drawing::Point(133, 335);
			   this->label15->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label15->Name = L"label15";
			   this->label15->Size = System::Drawing::Size(123, 38);
			   this->label15->TabIndex = 15;
			   this->label15->Text = L"label15";
			   // 
			   // panel1
			   // 
			   this->panel1->Controls->Add(this->label16);
			   this->panel1->Controls->Add(this->label15);
			   this->panel1->Controls->Add(this->label11);
			   this->panel1->Controls->Add(this->label14);
			   this->panel1->Controls->Add(this->label12);
			   this->panel1->Controls->Add(this->label13);
			   this->panel1->Location = System::Drawing::Point(390, 501);
			   this->panel1->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			   this->panel1->Name = L"panel1";
			   this->panel1->Size = System::Drawing::Size(404, 421);
			   this->panel1->TabIndex = 16;
			   this->panel1->Visible = false;
			   // 
			   // label16
			   // 
			   this->label16->AutoSize = true;
			   this->label16->Font = (gcnew System::Drawing::Font(L"新細明體", 16));
			   this->label16->Location = System::Drawing::Point(133, 27);
			   this->label16->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label16->Name = L"label16";
			   this->label16->Size = System::Drawing::Size(137, 43);
			   this->label16->TabIndex = 16;
			   this->label16->Text = L"label16";
			   // 
			   // label17
			   // 
			   this->label17->AutoSize = true;
			   this->label17->Font = (gcnew System::Drawing::Font(L"Times New Roman", 9));
			   this->label17->Location = System::Drawing::Point(968, 767);
			   this->label17->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			   this->label17->Name = L"label17";
			   this->label17->Size = System::Drawing::Size(82, 27);
			   this->label17->TabIndex = 17;
			   this->label17->Text = L"label17";
			   this->label17->Visible = false;
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(13, 24);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(1202, 1083);
			   this->Controls->Add(this->label17);
			   this->Controls->Add(this->panel1);
			   this->Controls->Add(this->label10);
			   this->Controls->Add(this->label9);
			   this->Controls->Add(this->label8);
			   this->Controls->Add(this->label7);
			   this->Controls->Add(this->label6);
			   this->Controls->Add(this->label5);
			   this->Controls->Add(this->label4);
			   this->Controls->Add(this->label3);
			   this->Controls->Add(this->button1);
			   this->Controls->Add(this->comboBox1);
			   this->Controls->Add(this->label2);
			   this->Controls->Add(this->label1);
			   this->Name = L"Form1";
			   this->Text = L"Form1";
			   this->panel1->ResumeLayout(false);
			   this->panel1->PerformLayout();
			   this->ResumeLayout(false);
			   this->PerformLayout();

		   }
#pragma endregion
	private: System::Void timer1_Tick(System::Object^ sender, System::EventArgs^ e)
	{
		if (system_counter == 0)
		{
			initialize();
		}

		switch (game.mode)
		{
		case 0:
			label1->Visible = true;
			label2->Visible = false;
			comboBox1->Visible = false;
			button1->Visible = false;
			label4->Visible = false;
			label7->Visible = true;
			label8->Visible = true;
			label9->Visible = true;
			label17->Visible = true;
			label10->Visible = false;
			panel1->Visible = false;


			plot();
			motion();
			collision();
			text_displayer();
			Time_Bonus_Counter();
			NEXT(game);
			Ability(game);
			if (snake.EXP_Check(game))//exp達標和反效果
			{
				snake.point.exp -= 5;
				snake.Reset_Ability(food);
				game.ability_state = 9;
			};
			system_counter++;

			break;

		case 1:
			label1->Visible = true;
			label2->Visible = true;
			comboBox1->Visible = false;
			button1->Visible = false;
			label4->Visible = false;
			label7->Visible = true;
			label8->Visible = true;
			label9->Visible = true;
			label17->Visible = true;
			label10->Visible = false;
			panel1->Visible = false;

			plot();
			motion();
			collision();
			text_displayer();
			Time_Bonus_Counter();
			NEXT(game);
			Ability(game);
			if (snake.EXP_Check(game))//exp達標和反效果
			{
				snake.point.exp -= 5;
				snake.Reset_Ability(food);
				game.ability_state = 9;
			};
			system_counter++;

			if (game.second > 0)
			{
				game.second--;
			}

			else
			{
				label3->Visible = true;
				game.game_over = 1;
				this->label3->BringToFront();
			}

			break;

		case 2:
			label1->Visible = true;
			comboBox1->Visible = false;
			button1->Visible = false;
			label4->Visible = false;
			label6->Visible = true;
			label7->Visible = true;
			label8->Visible = true;
			label9->Visible = true;
			label17->Visible = true;
			label10->Visible = false;
			panel1->Visible = false;

			game.counter++;

			for (int i = 1; i < 10; i++)
			{
				if (game.counter >= 0.5 * 600 * i)  // 30 seconds
				{
					snake.rate = 1 + 0.5 * i;
				}
			}

			plot();
			motion();
			collision();
			text_displayer();
			Time_Bonus_Counter();
			NEXT(game);
			Ability(game);
			if (snake.EXP_Check(game))//exp達標和反效果
			{
				snake.point.exp -= 5;
				snake.Reset_Ability(food);
				game.ability_state = 9;
			};
			system_counter++;
			break;

		default:
			break;
		}
	}

	private: System::Void Form1_KeyDown(System::Object^ sender, System::Windows::Forms::KeyEventArgs^ e)
	{
		struct vxy V;

		switch (e->KeyCode)
		{
		case Keys::W:
			V.Vx = 0;
			V.Vy = 5 * 5 * 3 * snake.reverse;
			break;

		case Keys::S:
			V.Vx = 0;
			V.Vy = -5 * 5 * 3 * snake.reverse;
			break;

		case Keys::A:
			V.Vx = -5 * 5 * 3 * snake.reverse;
			V.Vy = 0;
			break;

		case Keys::D:
			V.Vx = 5 * 5 * 3 * snake.reverse;
			V.Vy = 0;
			break;

		case Keys::Q:
			if (snake.EXP_Check(game))//exp達標和反效果
			{
				snake.point.exp -= 3;
				snake.Reset_Ability(food);
			};
			break;
		}

		snake.Set_Velocity(V);
		if (food.IsEat(snake.Get_Position()) == 1)
		{
			snake.Grow_up(snake.Get_Position());
		}

		this->Invalidate();
	}

	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e)
	{
		String^ selectedGameMode = safe_cast<String^>(comboBox1->SelectedItem);

		if (selectedGameMode == "Classic")
		{
			game.mode = 0;
		}
		else if (selectedGameMode == "Time-Limited")
		{
			game.mode = 1;
		}
		else if (selectedGameMode == "Challenge")
		{
			game.mode = 2;
		}
	}
	};
}