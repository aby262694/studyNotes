#include"common.h"


char background[20][10] = { 0 };
char sqare[2][4] = { 0 };
char nSqare = -1;
char Sqare_nx = -1;
char Sqare_ny = -1;
int score = 0;

void messageCreate()
{
	PlaySound(TEXT("123.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); //ѭ������

	srand((unsigned int)time(0));
	//���������
	CreateRandsqare();   //��һ��
	//�������Ž�������
	showRandsqare();   //����һ�ε���һ�ξ���
}

void messagePaint(HDC hdc)
{//���ڴ����ϵĶ����ܶ�ʱ������� �����Ȼ�����һ��ֽ���ڰ����Ż��õ�������

 //����������dc    �ں˶���           ������dc���Դ��ڶ��
	HDC memdc = CreateCompatibleDC(hdc);
	//�����ڴ��е�ֽ
	HBITMAP hbitmap = CreateCompatibleBitmap(hdc, 500, 600);
	//���ڴ��е�ͼ �� dc����
	SelectObject(memdc, hbitmap);

	//������
	Rectangle(memdc, 0, 0,//����2��3���Ͻ���ʼ��   �����Ƕ��� ���ڵ� ��������
		300, 600); //����4,5 ���½���ֹλ��    //��������
	
	
	//��ʾ����
	paintSqare(memdc);
	//��ʾ 2 ����
	showSqare2(memdc);

	//��ʾ����
	showScore(memdc);

	//���ڴ��е�ͼת�ƹ���
	BitBlt(hdc, 0, 0, 500, 600, memdc, 0, 0, SRCCOPY); //ǰ����Ŀ��������

	//�ͷ�����ֽ
	DeleteObject(hbitmap);
	//�ͷ�dc
	DeleteDC(memdc);
}
//��ʾ����
void paintSqare(HDC memdc)
{
	//������ˢ Ϳ��ɫ
	HBRUSH oldBrush;
	HBRUSH hbrush = CreateSolidBrush(RGB(227,151,23)); //��������ɫ,RGB(��ԭɫ)
	oldBrush = (HBRUSH)SelectObject(memdc, hbrush);
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (1 == background[i][j])
			{
				Rectangle(memdc, j * 30, i * 30, j * 30 + 30, i * 30 + 30);  //ע�����б����
			}
		}
	}
	hbrush = SelectObject(memdc, oldBrush);  //����ˢ��ԭ
	DeleteObject(hbrush);
}
//���������
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
//�������Ž�������
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


//������Ϣ
void messageKey(HWND hwnd, WPARAM wparam)
{
	HDC hdc = GetDC(hwnd);
	switch (wparam)
	{
	case VK_RETURN:  //�س�
		enterStart(hwnd); 
		break;
	case VK_LEFT:
		if (0 == judgeLeft() && 0 == judgeLeftSqare())
		{
			//�����ƶ�
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
//�س���  ��ʼ
void enterStart(HWND hwnd)
{
	//�򿪶�ʱ��    һ��Ҫ�йر�
	SetTimer(hwnd, TIMER1, 500, NULL); //����2 ��ʱ��ID ��ǰ������ ����3 ���ʱ�� 
									  //����4��NULL �Ļ�ÿ��ˢ�»����wm timer��Ϣ Ҳ���������
}
//��ʱ����Ϣ����
void messageTimer(HWND hwnd) 
{
	HDC hdc = GetDC(hwnd); //�õ����ڵ�dc
	if (1 == sqareStop() && 0 == judgeFallOnSqare()) //û�ߵ����һ�� ���� ��һ��û��2
	{
		//��������
		sqareDown();
		Sqare_nx++;
	}
	else //�����������һ���µ������
	{
		//�����ڵײ��ķ�����2
		lastSqare();
		//����ɾ��
		deleteFallLine();  //ÿ���з��鲻�������� �ͼ��һ��
		//�ж���Ϸ����
		if (1 == judgeGameOver())
		{
			KillTimer(hwnd, TIMER1);
			return;
		}

		//���������
		CreateRandsqare();   //��һ��
		//�������Ž�������
		showRandsqare();   //����һ�ε���һ�ξ���
	}
	
	
	//������
	messagePaint(hdc); //ֱ�ӵ���д�õĺ���

	ReleaseDC(hwnd, hdc); //�ں˶���һ��Ҫ�ֶ��ͷ�
}
//��������
void sqareDown()
{
	//˼· �������ϱ��� ����1 �����ƶ� ������
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
//�����Ƿ��ߵ����һ��
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
//�ײ�������2
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
//��ʾ2
void showSqare2(HDC memdc)
{
	//������ˢ Ϳ��ɫ
	HBRUSH oldBrush;
	HBRUSH hbrush = CreateSolidBrush(RGB(29,162,21)); //��������ɫ,RGB(��ԭɫ)
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
	hbrush = SelectObject(memdc, oldBrush);  //����ˢ��ԭ
	DeleteObject(hbrush);
}
//�жϷ��� �Ƿ����ڷ�����
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
//�����ƶ�
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
//�ж��Ƿ��ߵ��������һ��
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
//�ж������Ƿ���2����
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
//���¼�
void downkey(HWND hwnd)
{
	messageTimer(hwnd); //ֱ�ӵ���timer��Ϣ�������
}
//�ϼ�
void upkey(HDC hdc)
{
	//����
	switch (nSqare)
	{
	case 0:		//ǰ5�ֶ����ԷŽ�3*3������ 
	case 1:		//��������������¼ ����������Ͻǵ�����
	case 2:
	case 3:
	case 4:
		if (1 == judgeChange()) // && Sqare_ny >= 0 && Sqare_ny < 8)
		{						//��������������������� �߽���γ��ֵ�����  ���־���ֱ�Ӳ��ñ���
			changeSqare();
		}
		else
		{
			return;
		}
		break;
	case 5:  //�����β��ñ���
		return;
	case 6:		//����
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
	messagePaint(hdc);//����������
}
//����
void changeSqare()
{
	if (Sqare_ny < 0)   //����߽���� ����
	{
		Sqare_ny = 0;
	}
	else if (Sqare_ny > 7)
	{
		Sqare_ny = 7;
	}
	//�������е� 3*3���ó���
	char tempSqare[3][3] = { 0 };
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{	
			tempSqare[i][j] = background[Sqare_nx + i][Sqare_ny + j];
		}
	}
	char temp;
	//�ı�Ż�
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
//�ж��ܲ��ܱ���
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
			if (Sqare_nx > 17)   //�����һ�к����ڱ�����
			{
				return 0;
			}
		}
	}
	return 1;
}

//��������
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

//�жϳ����ܲ��ܱ���
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

//������
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
		if (20 == sum)   //����
		{
			score++;
			//������
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
			i++;  //����������������Ԫ��������һ�� ���Դ�����һ�� �����б���
		}
	}
}

//��ʾ����
void showScore(HDC memdc)
{
	Rectangle(memdc, 300, 0, 500, 600);
	TextOut(memdc, 370, 200, "������", strlen("������"));//����4��Ч�ַ���  
	char sqareScore[10] = { 0 };
	_itoa_s(score, sqareScore, 10, 10); //����4����
	TextOut(memdc, 375, 220, sqareScore, strlen(sqareScore));

	TextOut(memdc, 340, 270, "�� �� �� �� ����", strlen("�� �� �� �� ����"));
	TextOut(memdc, 350, 320, "  �س� ��ʼ  ", strlen("  �س� ��ʼ  "));
}

//�жϽ���
char judgeGameOver()
{
	for (int i = 0; i < 10; i++)
	{
		if (2 == background[0][i])
		{
			//��Ϸ����
			MessageBox(NULL, "GameOver","��ʾ", MB_YESNO);
			//����1��null ��ʾ �����Ϣ�������ĸ����ڣ����ڵ�ǰӦ�ó���
			//����2 ��ʾ��Ϣ
			//����3 �Ի������� 
			//����4 ��ʾ���ڷ��
			return 1;
		}
	}
	return 0;
}