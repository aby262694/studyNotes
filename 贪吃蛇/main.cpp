#include"common.h"

int main()
{
	//����
	titlePage();
	//��������
	music();
	//���ո�ʼ
	start();
	////��ʾ����
	//background();  //���ַ�ʽ��̫�� �������Ե�ˢ�ºۼ�

	//�߳�ʼλ���������
	CreateSnakeRandFirst();
	//����������
	drawSnake();
	//����ʳ��
	food a = createRandFood();

	bool judge;
	while (1)
	{
		//����֮ǰ��
		system("cls");
		//�ı䷽��
		changeDirection();				

		//������һ����
		clearSnake();
		//���ƶ�
		snakeMove();

		//�ж� ����
		bool eat = judgeEat(a);
		if (true == eat)
		{
			//�����µ�ʳ��
			a = createRandFood();
			//������
			snakeGrow();
			//����++
			mark++;
		}
		
		//��ʾ
		show();
		//�Ʒ�
		score(mark);

		//�����ж�
		judge = judgeDeath();
		if (false == judge)
		{
			//���ù��λ��
			COORD coo;
			coo.X = 0;
			coo.Y = 21;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coo);
			cout << "��Ϸ����\n";

			//�ͷ�����
			snakeBody.clear();
			break;
		}

		//ʹ����ֹͣһ��
		Sleep(500);
	}
	

	system("pause");
	return 0;
}