#pragma once
struct bits
{

};

union byte_bits
{
    bits b;
    unsigned char B;
};

struct position
{
    float X;
    float Y;
    float Theta = 1.57;
};

struct vw
{
    float V;
    float W;
};

struct vxy
{
    float Vx;
    float Vy;
};

//?
struct ability
{
    int counter;
};

struct points
{
    float score;
    int level = 1;
    int exp = 0;
};

struct game_info
{
    int mode;
    int second = 1 * 60 * 10;
    int counter = 0;
    bool game_over = 0;
    int bonus_second = 50;//for bonus point 3 sec for 2, 5 for 1.
    int ability_state = 9;
    char ability_state_text[20] = "None";
    char current_ability_text[20] = "None";
    int next_ability = 9;
    bool end = false;
    int renew_map = 0;
};

struct food_profile
{
    struct position pos;
    struct vxy V;
    bool enable = 1;
};

struct profile
{
    char name[20];
    struct position pos;
    struct vw vel;
    struct points point;
    union byte_bits status;
};