#include"common.h"

//封面
void titlePage()
{
	cout << endl << endl << endl << endl << endl;
	cout << "\t\t\t欢迎进入贪吃蛇游戏" << endl;
	cout << "\t\t\t上 下 左 右控制方向" << endl;
	cout << "\t\t\t 按空格 开始游戏 " << endl;
	cout << endl << endl << endl;
}

//播放音乐
void music()
{
	PlaySound(L"123.wav", NULL, SND_FILENAME | SND_ASYNC);   //只能播放wav类型的
	//音乐要与.exe 文件放在同一目录
	//这种加载路径比较常用  比较容易替换

	//还有一种方式是添加到资源文件
	//PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_FILENAME | SND_ASYNC);
	//这种方式 不可替换 加载进了程序中
}

//按空格开始
void start()
{
	char a;
	while (1)    //可以略过 输入其他数据  直到 检测到需要的输入
	{
		a = _getch();  //不需要按回车 直接读入  不显示在控制台
		if (' ' == a)
		{
			break;
		}
	}
	//停止音乐
	PlaySound(NULL, NULL, NULL);  //全是null就停止了
	//开始游戏
	//清理之前页面 
	system("cls");
}

//显示背景
void background()  //刷新时间超过20毫秒左右 人眼就会感受到闪烁
{
	int back[20][23] = { 
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},   //■占两个字符位
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	};
	int(*p)[23] = back;
	while (1)     //蛇移动 后需要反复打印 所以要循环
	{
		system("cls");
		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 23; j++)
			{
				if (1 == p[i][j])
				{
					cout << "■";
				}
				else if(0 == p[i][j])
				{
					cout << "  ";
				}
			}
			cout << '\n';
		}
		Sleep(1000);   //以毫秒为单位
	}
}

//显示
void show()
{
	for (int i = 0; i < 20; i++)
	{
			cout << backFrame[i];
	}
}

//蛇初始位置随机产生
void CreateSnakeRandFirst()
{
	int x = -1,
		y = -1;
	//随机数种子
	srand((unsigned int)time(NULL));
	x = rand() % 17 + 2;   //行
	y = (rand() % 15 + 4)*2;//列
	//初始化三个节点
	snakeBody.push_back(snakeNode(x, y, enum_right));
	snakeBody.push_back(snakeNode(x, y-2, enum_right));
	snakeBody.push_back(snakeNode(x, y-4, enum_right));
}
//画蛇
void drawSnake()
{
	//由于是字符串 组成的背景  所以用strncpy 来放进去
	for(list<snakeNode>::iterator it = snakeBody.begin(); it != snakeBody.end(); it++)//遍历蛇
	{
		strncpy(&backFrame[it->index_x][it->index_y], "●",2);
	}
}
//移动
void snakeMove()
{
	list<snakeNode>::iterator it = --snakeBody.end();
	list<snakeNode>::iterator it1 = --snakeBody.end();
	list<snakeNode>::iterator it2 = snakeBody.begin();
	//从后往前遍历  用前面的赋值后面的
	do
	{
		it--;
		it1->index_x = it->index_x;
		it1->index_y = it->index_y;
		it1->forward = it->forward;
		it1 = it;
		
	} while (it != it2);
	//头往前动
	if (enum_right == snakeBody.begin()->forward)
	{
		if(snakeBody.begin()->index_y <44)
		snakeBody.begin()->index_y += 2;
	}
	else if (enum_left == snakeBody.begin()->forward)
	{
		if (snakeBody.begin()->index_y > 0)
		snakeBody.begin()->index_y -= 2;
	}
	else if (enum_up == snakeBody.begin()->forward)
	{
		if (snakeBody.begin()->index_x > 0)
		snakeBody.begin()->index_x -= 1;
	}
	else if (enum_down == snakeBody.begin()->forward)
	{
		if (snakeBody.begin()->index_x < 20)
			snakeBody.begin()->index_x += 1;
	}
	//画蛇
	drawSnake();
}
//清理之前的蛇   逻辑上最容易实现的
void clearSnake()
{
	for (list<snakeNode>::iterator it = snakeBody.begin(); it != snakeBody.end(); it++)//遍历蛇
	{
		strncpy(&backFrame[it->index_x][it->index_y], "  ", 2);
	}
}
//改变方向
void changeDirection()
{
	//_getch();    //这个是同步检测  必须要有输入才会继续执行程序   在这里不适用

	if (GetAsyncKeyState(VK_UP))      //异步是不等你的   返回short类型 
	{								//正在按着 高字节装着非零   如果执行前按过了低字节会返回一个1
									//按了返回值 就该是非0 的   没按是0
									//若需要处理 是不是一直按着 可以&1  按位与个1
		if (enum_down != snakeBody.front().forward)
		{
			snakeBody.begin()->forward = enum_up;
		}	
	}
	else if (GetAsyncKeyState(VK_DOWN))    //检测字符的话只能检测大写 如 'w'这样
	{
		if (enum_up != snakeBody.front().forward)
		{
			snakeBody.begin()->forward = enum_down;
		}
	}
	else if (GetAsyncKeyState(VK_LEFT))   //核心在于异步读取函数
	{
		if (enum_right != snakeBody.front().forward)
		{
			snakeBody.begin()->forward = enum_left;
		}

	}
	else if (GetAsyncKeyState(VK_RIGHT))
	{
		if (enum_left != snakeBody.front().forward)
		{
			snakeBody.begin()->forward = enum_right;
		}

	}
}
//死亡判断 
bool judgeDeath()
{	//判断边框
	switch (snakeBody.front().index_x)
	{
	case 0:
	case 19:
		return false;
	}
	switch (snakeBody.front().index_y)
	{
	case 0:
	case 44:
		return false;
	}
	//判断撞自己
	for (list<snakeNode>::iterator it = ++snakeBody.begin(); it != snakeBody.end(); it++)
	{//从第二个开始遍历
		if ((snakeBody.front().index_x == it->index_x) && (snakeBody.front().index_y == it->index_y))
		{
			return false;
		}
	}
	return TRUE;
}
//产生食物
 food createRandFood()
{
	food crfood;
	srand((unsigned int)time(NULL));
	while (1)
	{
		crfood.index_x = rand() % 18 + 1;
		crfood.index_y = (rand() % 21 + 1) * 2;
		//加个判断  食物不能与蛇重合
		for (list<snakeNode>::iterator it = snakeBody.begin(); it != snakeBody.end(); it++)
		{
			if ((it->index_x != crfood.index_x) || (it->index_y != crfood.index_y))
			{
				goto step;
			}
		}
		break;
	}
	step:
	//画上
	strncpy(&backFrame[crfood.index_x][crfood.index_y], "★", 2);
	return crfood;
}
//判断吃到食物
bool judgeEat(food a)
{
	if ((a.index_x == snakeBody.front().index_x) && (a.index_y == snakeBody.front().index_y))
	{
		return true;
	}
	return false;
}
//蛇增长
void snakeGrow()
{
	snakeNode node(snakeBody.back().index_x,snakeBody.back().index_y,snakeBody.back().forward);
	snakeBody.push_back(node);
}
//计分
void score(int mark)
{
	//设置光标位置
	COORD coo;
	coo.X = 54;
	coo.Y = 8;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coo);  //参数1  当前控制台句柄
	//输出分数
	cout << "得分：" << mark;
}