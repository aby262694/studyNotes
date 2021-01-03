#include"common.h"


char background[20][10] = { 0 };
char sqare[2][4] = { 0 };
char nSqare = -1;
char Sqare_nx = -1;
char Sqare_ny = -1;
int score = 0;

void messageCreate()
{
	PlaySound(TEXT("123.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //循环播放

	srand((unsigned int)time(0));
	//产生随机块
	CreateRandsqare();   //第一块
	//将随机块放进背景中
	showRandsqare();   //产生一次调用一次就行
}

void messagePaint(HDC hdc)
{//由于窗口上的东西很多时会造成闪 所以先画到另一张纸上在把这张画好的贴过来

 //创建兼容性dc    内核对象           兼容性dc可以存在多个
	HDC memdc = CreateCompatibleDC(hdc);
	//创建内存中的纸
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, 500, 600);
	//将内存中的图 跟 dc关联
	SelectObject(memdc, hbitmap);

	//画背景
	Rectangle(memdc, 0, 0,//参数2，3左上角起始点   坐标是对于 窗口的 不是桌面
		300, 600); //参数4,5 右下角终止位置    //画长方形
	
	
	//显示方块
	paintSqare(memdc);
	//显示 2 方块
	showSqare2(memdc);

	//显示分数
	showScore(memdc);

	//将内存中的图转移过来
	BitBlt(hdc, 0, 0, 500, 600, memdc, 0, 0, SRCCOPY); //前面是目标区域句柄

	//释放这张纸
	DeleteObject(hbitmap);
	//释放dc
	DeleteDC(memdc);
}
//显示方块
void paintSqare(HDC memdc)
{
	//创建画刷 涂颜色
	HBRUSH oldBrush;
	HBRUSH hbrush = CreateSolidBrush(RGB(227,151,23)); //参数是颜色,RGB(三原色)
	oldBrush = (HBRUSH)SelectObject(memdc, hbrush);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (1 == background[i][j])
			{
				Rectangle(memdc, j * 30, i * 30, j * 30 + 30, i * 30 + 30);  //注意行列别错了
			}
		}
	}
	hbrush = SelectObject(memdc, oldBrush);  //将画刷还原
	DeleteObject(hbrush);
}
//产生随机块
void CreateRandsqare()
{
	int n = rand() % 7;
		switch (n)
		{
		case 0:
			sqare[0][0] = 1; sqare[0][1] = 1; sqare[0][2] = 0; sqare[0][3] = 0;
			sqare[1][0] = 0; sqare[1][1] = 1; sqare[1][2] = 1; sqare[1][3] = 0;
			Sqare_nx = 0;
			Sqare_ny = 3;
			break;
		case 1:
			sqare[0][0] = 0; sqare[0][1] = 1; sqare[0][2] = 1; sqare[0][3] = 0;
			sqare[1][0] = 1; sqare[1][1] = 1; sqare[1][2] = 0; sqare[1][3] = 0;
			Sqare_nx = 0;
			Sqare_ny = 3;
			break;
		case 2:
			sqare[0][0] = 1; sqare[0][1] = 0; sqare[0][2] = 0; sqare[0][3] = 0;
			sqare[1][0] = 1; sqare[1][1] = 1; sqare[1][2] = 1; sqare[1][3] = 0;
			Sqare_nx = 0;
			Sqare_ny = 3;
			break; 
		case 3:
			sqare[0][0] = 0; sqare[0][1] = 0; sqare[0][2] = 1; sqare[0][3] = 0;
			sqare[1][0] = 1; sqare[1][1] = 1; sqare[1][2] = 1; sqare[1][3] = 0;
			Sqare_nx = 0;
			Sqare_ny = 3;
			break; 
		case 4:
			sqare[0][0] = 0; sqare[0][1] = 1; sqare[0][2] = 0; sqare[0][3] = 0;
			sqare[1][0] = 1; sqare[1][1] = 1; sqare[1][2] = 1; sqare[1][3] = 0;
			Sqare_nx = 0;
			Sqare_ny = 3;
			break; 
		case 5:
			sqare[0][0] = 1; sqare[0][1] = 1; sqare[0][2] = 0; sqare[0][3] = 0;
			sqare[1][0] = 1; sqare[1][1] = 1; sqare[1][2] = 0; sqare[1][3] = 0;
			break; 
		case 6:
			sqare[0][0] = 1; sqare[0][1] = 1; sqare[0][2] = 1; sqare[0][3] = 1;
			sqare[1][0] = 0; sqare[1][1] = 0; sqare[1][2] = 0; sqare[1][3] = 0;
			Sqare_nx = 0;
			Sqare_ny = 3;
			break;
		}
		nSqare = n;
}
//将随机块放进背景中
void showRandsqare()
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			background[i][j+3] = sqare[i][j];
		}
	}
}


//按键消息
void messageKey(HWND hwnd, WPARAM wparam)
{
	HDC hdc = GetDC(hwnd);
	switch (wparam)
	{
	case VK_RETURN:  //回车
		enterStart(hwnd); 
		break;
	case VK_LEFT:
		if (0 == judgeLeft() && 0 == judgeLeftSqare())
		{
			//向左移动
			leftkey(hdc);
			Sqare_ny--;
		}
		break;
	case VK_RIGHT:
		if (0 == judgeRight() && 0 == judgeRightSqare())
		{
			rightkey(hdc);
			Sqare_ny++;
		}
		break;
	case VK_DOWN:
		downkey(hwnd);
		break;
	case VK_UP:
		upkey(hdc);
		break;
	}
	ReleaseDC(hwnd, hdc);
}
//回车键  开始
void enterStart(HWND hwnd)
{
	//打开定时器    一定要有关闭
	SetTimer(hwnd, TIMER1, 500, NULL); //参数2 定时器ID 提前声明宏 参数3 间隔时间 
									  //参数4填NULL 的话每次刷新会产生wm timer消息 也可以填处理函数
}
//定时器消息处理
void messageTimer(HWND hwnd) 
{
	HDC hdc = GetDC(hwnd); //得到窗口的dc
	if (1 == sqareStop() && 0 == judgeFallOnSqare()) //没走到最后一行 并且 下一行没有2
	{
		//方块下落
		sqareDown();
		Sqare_nx++;
	}
	else //不能下落产生一个新的随机块
	{
		//将落在底部的方块变成2
		lastSqare();
		//满行删除
		deleteFallLine();  //每次有方块不能下落了 就检测一次
		//判断游戏结束
		if (1 == judgeGameOver())
		{
			KillTimer(hwnd, TIMER1);
			return;
		}

		//产生随机块
		CreateRandsqare();   //第一块
		//将随机块放进背景中
		showRandsqare();   //产生一次调用一次就行
	}
	
	
	//画方块
	messagePaint(hdc); //直接调用写好的函数

	ReleaseDC(hwnd, hdc); //内核对象一定要手动释放
}
//方块下落
void sqareDown()
{
	//思路 从下往上遍历 遇到1 向下移动 并归零
	for (int i = 19; i >= 0; i--)
	{
		for (int j = 0; j < 10; j++)
		{
			if (1 == background[i][j])
			{
				background[i + 1][j] = background[i][j];
				background[i][j] = 0;
			}
		}
	}
}
//方块是否走到最后一行
char sqareStop()
{
	for (int i = 0; i < 10; i++)
	{
		if (1 == background[19][i])
		{
			return 0;
		}
	}
	return 1;
}
//底部方块变成2
void lastSqare()
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (1 == background[i][j])
			{
				background[i][j] = 2;
			}
		}
	}
}
//显示2
void showSqare2(HDC memdc)
{
	//创建画刷 涂颜色
	HBRUSH oldBrush;
	HBRUSH hbrush = CreateSolidBrush(RGB(29,162,21)); //参数是颜色,RGB(三原色)
	oldBrush = (HBRUSH)SelectObject(memdc, hbrush);

	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (2 == background[i][j])
			{
				Rectangle(memdc, j * 30, i * 30, j * 30 + 30, i * 30 + 30);
			}
		}
	}
	hbrush = SelectObject(memdc, oldBrush);  //将画刷还原
	DeleteObject(hbrush);
}
//判断方块 是否落在方块上
char judgeFallOnSqare()
{
	for (int i = 19; i >= 0; i--)
	{
		for (int j = 0; j < 10; j++)
		{
			if ( 1 == background[i][j] && 2 == background[i + 1][j])
			{
				return 1;
			}
		}
	}
	return 0;
}
//向左移动
void leftkey(HDC hdc)
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (1 == background[i][j])
			{
				background[i][j - 1] = background[i][j];
				background[i][j] = 0;
			}
		}
	}
	messagePaint(hdc);
}
void rightkey(HDC hdc)
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 9; j >= 0; j--)
		{
			if (1 == background[i][j])
			{
				background[i][j + 1] = background[i][j];
				background[i][j] = 0;
			}
		}
	}
	messagePaint(hdc);
}
//判断是否走到左右最后一列
char judgeLeft()
{
	for (int i = 0; i < 20; i++)
	{
		if (1 == background[i][0])
		{
			return 1;
		}
	}
	return 0;
}
char judgeRight()
{
	for (int i = 0; i < 20; i++)
	{
		if (1 == background[i][9])
		{
			return 1;
		}
	}
	return 0;
}
//判断左右是否有2方块
char judgeLeftSqare()
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 9; j >= 0; j--)
		{
			if (1 == background[i][j] && 2 == background[i][j - 1])
			{
				return 1;
			}
		}
	}
	return 0;
}
char judgeRightSqare()
{
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (1 == background[i][j] && 2 == background[i][j + 1])
			{
				return 1;
			}
		}
	}
	return 0;
}
//按下键
void downkey(HWND hwnd)
{
	messageTimer(hwnd); //直接调用timer消息处理就行
}
//上键
void upkey(HDC hdc)
{
	//变形
	switch (nSqare)
	{
	case 0:		//前5种都可以放进3*3方块中 
	case 1:		//定义两个变量记录 这个方块左上角的坐标
	case 2:
	case 3:
	case 4:
		if (1 == judgeChange()) // && Sqare_ny >= 0 && Sqare_ny < 8)
		{						//可以用这两个条件来解决 边界变形出现的问题  这种就是直接不让变了
			changeSqare();
		}
		else
		{
			return;
		}
		break;
	case 5:  //正方形不用变形
		return;
	case 6:		//长条
		if (1 == judgeLongChange())
		{
			changeLongSqare();
		}
		else
		{
			return;
		}
		break;
	}
	messagePaint(hdc);//画到背景上
}
//变形
void changeSqare()
{
	if (Sqare_ny < 0)   //解决边界变形 问题
	{
		Sqare_ny = 0;
	}
	else if (Sqare_ny > 7)
	{
		Sqare_ny = 7;
	}
	//将背景中的 3*3块拿出来
	char tempSqare[3][3] = { 0 };
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{	
			tempSqare[i][j] = background[Sqare_nx + i][Sqare_ny + j];
		}
	}
	char temp;
	//改变放回
	for (int i = 0; i < 3; i++)
	{
		temp = 2;
		for (int j = 0; j < 3; j++)
		{
			
			background[Sqare_nx + i][Sqare_ny + j] = tempSqare[temp][i];
			temp--;
		}
	}
}
//判断能不能变形
char judgeChange()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (2 == background[Sqare_nx + i][Sqare_ny + j])
			{
				return 0;
			}
			if (Sqare_nx > 17)   //到最后一行后不能在变形了
			{
				return 0;
			}
		}
	}
	return 1;
}

//长条变形
void changeLongSqare()
{
	if (Sqare_ny < 0)
	{
		Sqare_ny = 0;
	}
	else if (Sqare_ny > 6)
	{
		Sqare_ny = 6;
	}
	char tempSqare[4][4] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			tempSqare[i][j] = background[Sqare_nx + i - 1][Sqare_ny + j];
		}
	}
	char temp;
	for (int i = 0; i < 4; i++)
	{
		temp = 3;
		for (int j = 0; j < 4; j++)
		{
			background[Sqare_nx + i - 1][Sqare_ny + j] = tempSqare[temp][i];
			temp--;
		}
	}
}

//判断长条能不能变形
char judgeLongChange()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (2 == background[Sqare_nx + i - 1][Sqare_ny + j])
			{
				return 0;
			}
			else if (Sqare_nx > 16)
			{
				return 0;
			}
			else if (Sqare_nx < 1)
			{
				return 0;
			}
		}
	}
	return 1;
}

//消除行
void deleteFallLine()
{
	int sum = 0;
	for (int i = 19; i >= 0; i--)
	{
		sum = 0;
		for (int j = 0; j < 10; j++)
		{
			sum += background[i][j];
		}
		if (20 == sum)   //满行
		{
			score++;
			//消除行
			for (int j = 0; j < 10; j++)
			{
				background[i][j] = 0;
			}
			for (int m = i - 1; m >= 0; m--)
			{
				for (int n = 0; n < 10; n++)
				{
					if (2 == background[m][n])
					{
						background[m + 1][n] = background[m][n];
						background[m][n] = 0;
					}
				}
			}
			i++;  //由于这行以上所有元素下落了一行 所以从下面一行 来进行遍历
		}
	}
}

//显示分数
void showScore(HDC memdc)
{
	Rectangle(memdc, 300, 0, 500, 600);
	TextOut(memdc, 370, 200, "分数：", strlen("分数："));//参数4有效字符数  
	char sqareScore[10] = { 0 };
	_itoa_s(score, sqareScore, 10, 10); //参数4进制
	TextOut(memdc, 375, 220, sqareScore, strlen(sqareScore));

	TextOut(memdc, 340, 270, "上 下 左 右 控制", strlen("上 下 左 右 控制"));
	TextOut(memdc, 350, 320, "  回车 开始  ", strlen("  回车 开始  "));
}

//判断结束
char judgeGameOver()
{
	for (int i = 0; i < 10; i++)
	{
		if (2 == background[0][i])
		{
			//游戏结束
			MessageBox(NULL, "GameOver","提示", MB_YESNO);
			//参数1填null 表示 这个消息不属于哪个窗口，属于当前应用程序
			//参数2 提示信息
			//参数3 对话框名字 
			//参数4 提示窗口风格
			return 1;
		}
	}
	return 0;
}