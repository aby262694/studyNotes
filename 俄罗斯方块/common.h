#pragma once
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")
#include<time.h>
#include<wingdi.h>
#include<string.h>
#define TIMER1   12345 


//背景数组
extern char background[20][10];  //思路 0 不显示 1显示方块
//方块数组
extern char sqare[2][4];//这里可以用char 会节省一部分空间 背景也是

//声明 回调函数
LRESULT CALLBACK Pels(HWND hwnd, UINT nMsg, WPARAM wparam, LPARAM lparam);

///////////////////////////////////////////////////////
//消息
void messageCreate();
void messagePaint(HDC hdc);
//按键消息
void messageKey(HWND hwnd, WPARAM wparam);
//定时器消息处理
void messageTimer(HWND hwnd);

/////////////////////////////////////////////////
//产生随机块
void CreateRandsqare();
//将随机块放进背景中
void showRandsqare();
//显示背景方块
void paintSqare(HDC memdc);


////////////////////////////////////////////
//按键
//回车键  开始
void enterStart(HWND hwnd);
//向左移动
void leftkey(HDC hdc);
void rightkey(HDC hdc);
//按下键
void downkey(HWND hwnd);
//上 键
void upkey(HDC hdc);

///////////////////////////////////////////////////
//方块下落
void sqareDown();
//方块是否走到最后一行
char sqareStop();  //整个返回值看能否下落
//底部方块变成2
void lastSqare();
//显示2
void showSqare2(HDC memdc);
//判断方块 是否落在方块上
char judgeFallOnSqare();
///////////////////////////////////////////////////
//判断是否走到左右最后一列
char judgeLeft();
char judgeRight();
//判断左右是否有2方块
char judgeLeftSqare();
char judgeRightSqare();
/////////////////////////////////////////////////////
//变形
void changeSqare();
//判断能不能变形
char judgeChange();

//长条变形
void changeLongSqare();
//判断长条能不能变形
char judgeLongChange();
////////////////////////////////////////////////////

//消除行
void deleteFallLine();
//判断结束
char judgeGameOver();
//显示分数
void showScore(HDC memdc);