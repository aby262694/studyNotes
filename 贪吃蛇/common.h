#pragma once
#define _CRT_SECURE_NO_WARNINGS  //strncpy 的

#include<iostream>
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")
#include<conio.h>
#include<time.h>
#include<list>
#include<string>
using namespace std;

//封面
void titlePage();
//播放音乐
void music();
//按空格开始  
void start();
//显示背景
void background();
//加强版 显示背景 
static char backFrame[20][48] = {
{"■■■■■■■■■■■■■■■■■■■■■■■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■                                          ■\n"},
{"■■■■■■■■■■■■■■■■■■■■■■■\n"}
};
//显示
void show();
//蛇初始位置随机产生
void CreateSnakeRandFirst();

struct snakeNode
{
	int index_x;
	int index_y;
	int forward;
	snakeNode(int a, int b , int c):index_x(a),index_y(b),forward(c)
	{

	}
};
static list<snakeNode> snakeBody;
//方向命名
enum MyEnum
{
	enum_up = -1,
	enum_down = 1,
	enum_left = -2,
	enum_right = 2
};
//画蛇
void drawSnake();
//移动
void snakeMove();
//清理之前的蛇   逻辑上最容易实现的
void clearSnake();
//改变方向
void changeDirection();
//死亡判断 
bool judgeDeath();
//食物
struct food
{
	int index_x;
	int index_y;
};
//产生食物
food createRandFood();
//判断吃到食物
bool judgeEat(food a);
//蛇增长
void snakeGrow();
//分数
static int mark;
//计分
void score(int mark);


