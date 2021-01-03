#pragma once
#include<Windows.h>
#include<mmsystem.h>
#pragma comment(lib,"Winmm.lib")
#include<time.h>
#include<wingdi.h>
#include<string.h>
#define TIMER1   12345 


//��������
extern char background[20][10];  //˼· 0 ����ʾ 1��ʾ����
//��������
extern char sqare[2][4];//���������char ���ʡһ���ֿռ� ����Ҳ��

//���� �ص�����
LRESULT CALLBACK Pels(HWND hwnd, UINT nMsg, WPARAM wparam, LPARAM lparam);

///////////////////////////////////////////////////////
//��Ϣ
void messageCreate();
void messagePaint(HDC hdc);
//������Ϣ
void messageKey(HWND hwnd, WPARAM wparam);
//��ʱ����Ϣ����
void messageTimer(HWND hwnd);

/////////////////////////////////////////////////
//���������
void CreateRandsqare();
//�������Ž�������
void showRandsqare();
//��ʾ��������
void paintSqare(HDC memdc);


////////////////////////////////////////////
//����
//�س���  ��ʼ
void enterStart(HWND hwnd);
//�����ƶ�
void leftkey(HDC hdc);
void rightkey(HDC hdc);
//���¼�
void downkey(HWND hwnd);
//�� ��
void upkey(HDC hdc);

///////////////////////////////////////////////////
//��������
void sqareDown();
//�����Ƿ��ߵ����һ��
char sqareStop();  //��������ֵ���ܷ�����
//�ײ�������2
void lastSqare();
//��ʾ2
void showSqare2(HDC memdc);
//�жϷ��� �Ƿ����ڷ�����
char judgeFallOnSqare();
///////////////////////////////////////////////////
//�ж��Ƿ��ߵ��������һ��
char judgeLeft();
char judgeRight();
//�ж������Ƿ���2����
char judgeLeftSqare();
char judgeRightSqare();
/////////////////////////////////////////////////////
//����
void changeSqare();
//�ж��ܲ��ܱ���
char judgeChange();

//��������
void changeLongSqare();
//�жϳ����ܲ��ܱ���
char judgeLongChange();
////////////////////////////////////////////////////

//������
void deleteFallLine();
//�жϽ���
char judgeGameOver();
//��ʾ����
void showScore(HDC memdc);