#pragma once
#define _CRT_SECURE_NO_WARNINGS  //strncpy ��

#include<iostream>
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")
#include<conio.h>
#include<time.h>
#include<list>
#include<string>
using namespace std;

//����
void titlePage();
//��������
void music();
//���ո�ʼ  
void start();
//��ʾ����
void background();
//��ǿ�� ��ʾ���� 
static char backFrame[20][48] = {
{"����������������������������������������������\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"��                                          ��\n"},
{"����������������������������������������������\n"}
};
//��ʾ
void show();
//�߳�ʼλ���������
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
//��������
enum MyEnum
{
	enum_up = -1,
	enum_down = 1,
	enum_left = -2,
	enum_right = 2
};
//����
void drawSnake();
//�ƶ�
void snakeMove();
//����֮ǰ����   �߼���������ʵ�ֵ�
void clearSnake();
//�ı䷽��
void changeDirection();
//�����ж� 
bool judgeDeath();
//ʳ��
struct food
{
	int index_x;
	int index_y;
};
//����ʳ��
food createRandFood();
//�жϳԵ�ʳ��
bool judgeEat(food a);
//������
void snakeGrow();
//����
static int mark;
//�Ʒ�
void score(int mark);


