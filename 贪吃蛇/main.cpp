#include"common.h"

int main()
{
	//封面
	titlePage();
	//播放音乐
	music();
	//按空格开始
	start();
	////显示背景
	//background();  //这种方式不太好 会有明显的刷新痕迹

	//蛇初始位置随机产生
	CreateSnakeRandFirst();
	//画到背景上
	drawSnake();
	//产生食物
	food a = createRandFood();

	bool judge;
	while (1)
	{
		//清理之前的
		system("cls");
		//改变方向
		changeDirection();				

		//清理上一条蛇
		clearSnake();
		//蛇移动
		snakeMove();

		//判断 吃了
		bool eat = judgeEat(a);
		if (true == eat)
		{
			//产生新的食物
			a = createRandFood();
			//蛇增长
			snakeGrow();
			//分数++
			mark++;
		}
		
		//显示
		show();
		//计分
		score(mark);

		//死亡判断
		judge = judgeDeath();
		if (false == judge)
		{
			//设置光标位置
			COORD coo;
			coo.X = 0;
			coo.Y = 21;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coo);
			cout << "游戏结束\n";

			//释放链表
			snakeBody.clear();
			break;
		}

		//使界面停止一会
		Sleep(500);
	}
	

	system("pause");
	return 0;
}