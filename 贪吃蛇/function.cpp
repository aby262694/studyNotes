#include"common.h"

//����
void titlePage()
{
	cout << endl << endl << endl << endl << endl;
	cout << "\t\t\t��ӭ����̰������Ϸ" << endl;
	cout << "\t\t\t�� �� �� �ҿ��Ʒ���" << endl;
	cout << "\t\t\t ���ո� ��ʼ��Ϸ " << endl;
	cout << endl << endl << endl;
}

//��������
void music()
{
	PlaySound(L"123.wav", NULL, SND_FILENAME | SND_ASYNC);   //ֻ�ܲ���wav���͵�
	//����Ҫ��.exe �ļ�����ͬһĿ¼
	//���ּ���·���Ƚϳ���  �Ƚ������滻

	//����һ�ַ�ʽ����ӵ���Դ�ļ�
	//PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_FILENAME | SND_ASYNC);
	//���ַ�ʽ �����滻 ���ؽ��˳�����
}

//���ո�ʼ
void start()
{
	char a;
	while (1)    //�����Թ� ������������  ֱ�� ��⵽��Ҫ������
	{
		a = _getch();  //����Ҫ���س� ֱ�Ӷ���  ����ʾ�ڿ���̨
		if (' ' == a)
		{
			break;
		}
	}
	//ֹͣ����
	PlaySound(NULL, NULL, NULL);  //ȫ��null��ֹͣ��
	//��ʼ��Ϸ
	//����֮ǰҳ�� 
	system("cls");
}

//��ʾ����
void background()  //ˢ��ʱ�䳬��20�������� ���۾ͻ���ܵ���˸
{
	int back[20][23] = { 
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},   //��ռ�����ַ�λ
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
	while (1)     //���ƶ� ����Ҫ������ӡ ����Ҫѭ��
	{
		system("cls");
		for (int i = 0; i < 20; i++)
		{
			for (int j = 0; j < 23; j++)
			{
				if (1 == p[i][j])
				{
					cout << "��";
				}
				else if(0 == p[i][j])
				{
					cout << "  ";
				}
			}
			cout << '\n';
		}
		Sleep(1000);   //�Ժ���Ϊ��λ
	}
}

//��ʾ
void show()
{
	for (int i = 0; i < 20; i++)
	{
			cout << backFrame[i];
	}
}

//�߳�ʼλ���������
void CreateSnakeRandFirst()
{
	int x = -1,
		y = -1;
	//���������
	srand((unsigned int)time(NULL));
	x = rand() % 17 + 2;   //��
	y = (rand() % 15 + 4)*2;//��
	//��ʼ�������ڵ�
	snakeBody.push_back(snakeNode(x, y, enum_right));
	snakeBody.push_back(snakeNode(x, y-2, enum_right));
	snakeBody.push_back(snakeNode(x, y-4, enum_right));
}
//����
void drawSnake()
{
	//�������ַ��� ��ɵı���  ������strncpy ���Ž�ȥ
	for(list<snakeNode>::iterator it = snakeBody.begin(); it != snakeBody.end(); it++)//������
	{
		strncpy(&backFrame[it->index_x][it->index_y], "��",2);
	}
}
//�ƶ�
void snakeMove()
{
	list<snakeNode>::iterator it = --snakeBody.end();
	list<snakeNode>::iterator it1 = --snakeBody.end();
	list<snakeNode>::iterator it2 = snakeBody.begin();
	//�Ӻ���ǰ����  ��ǰ��ĸ�ֵ�����
	do
	{
		it--;
		it1->index_x = it->index_x;
		it1->index_y = it->index_y;
		it1->forward = it->forward;
		it1 = it;
		
	} while (it != it2);
	//ͷ��ǰ��
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
	//����
	drawSnake();
}
//����֮ǰ����   �߼���������ʵ�ֵ�
void clearSnake()
{
	for (list<snakeNode>::iterator it = snakeBody.begin(); it != snakeBody.end(); it++)//������
	{
		strncpy(&backFrame[it->index_x][it->index_y], "  ", 2);
	}
}
//�ı䷽��
void changeDirection()
{
	//_getch();    //�����ͬ�����  ����Ҫ������Ż����ִ�г���   �����ﲻ����

	if (GetAsyncKeyState(VK_UP))      //�첽�ǲ������   ����short���� 
	{								//���ڰ��� ���ֽ�װ�ŷ���   ���ִ��ǰ�����˵��ֽڻ᷵��һ��1
									//���˷���ֵ �͸��Ƿ�0 ��   û����0
									//����Ҫ���� �ǲ���һֱ���� ����&1  ��λ���1
		if (enum_down != snakeBody.front().forward)
		{
			snakeBody.begin()->forward = enum_up;
		}	
	}
	else if (GetAsyncKeyState(VK_DOWN))    //����ַ��Ļ�ֻ�ܼ���д �� 'w'����
	{
		if (enum_up != snakeBody.front().forward)
		{
			snakeBody.begin()->forward = enum_down;
		}
	}
	else if (GetAsyncKeyState(VK_LEFT))   //���������첽��ȡ����
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
//�����ж� 
bool judgeDeath()
{	//�жϱ߿�
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
	//�ж�ײ�Լ�
	for (list<snakeNode>::iterator it = ++snakeBody.begin(); it != snakeBody.end(); it++)
	{//�ӵڶ�����ʼ����
		if ((snakeBody.front().index_x == it->index_x) && (snakeBody.front().index_y == it->index_y))
		{
			return false;
		}
	}
	return TRUE;
}
//����ʳ��
 food createRandFood()
{
	food crfood;
	srand((unsigned int)time(NULL));
	while (1)
	{
		crfood.index_x = rand() % 18 + 1;
		crfood.index_y = (rand() % 21 + 1) * 2;
		//�Ӹ��ж�  ʳ�ﲻ�������غ�
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
	//����
	strncpy(&backFrame[crfood.index_x][crfood.index_y], "��", 2);
	return crfood;
}
//�жϳԵ�ʳ��
bool judgeEat(food a)
{
	if ((a.index_x == snakeBody.front().index_x) && (a.index_y == snakeBody.front().index_y))
	{
		return true;
	}
	return false;
}
//������
void snakeGrow()
{
	snakeNode node(snakeBody.back().index_x,snakeBody.back().index_y,snakeBody.back().forward);
	snakeBody.push_back(node);
}
//�Ʒ�
void score(int mark)
{
	//���ù��λ��
	COORD coo;
	coo.X = 54;
	coo.Y = 8;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coo);  //����1  ��ǰ����̨���
	//�������
	cout << "�÷֣�" << mark;
}