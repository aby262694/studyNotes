
#include<cstdio>
#include<cstdlib>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<MSWSock.h>		//acceptex

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

#define MAX_COUNT 1024

SOCKET g_socket[MAX_COUNT];  //��������ûҪ��
OVERLAPPED g_allOverLapped[MAX_COUNT];
int g_count;
//��������
char strRecv[1024];
char strSend[1024] = "join success\n";
//�������麯��
void clear();

int postAccept();
int postRecv(int index);
int postSend(int index);

BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		clear();
		WSACleanup();
		break;
	}
	return TRUE;
}


int main()
{
	SetConsoleCtrlHandler(fun, TRUE);


	//�������
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRes = WSAStartup(wVersion, &wsaData);
	if (0 != nRes)   //�ɹ�����0
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("�����쳣,�������� �� ��������\n");
			break;
		case WSAVERNOTSUPPORTED:
			printf("�汾����\n");
			break;
		case WSAEPROCLIM:
			printf("���г������,�˿ڲ���\n");
			break;
		case WSAEINPROGRESS:
			printf("���ع��������� �ȴ���������������\n");
			break;
		case WSAEFAULT:
			//����2д����
			break;
		}
	}
	//У��汾
	if (2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))
	{
		WSACleanup();
		return -1;
	}

	//����socket
	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return -1;
	}
	/**********************
	ǰ����������socket һ��		����ֵ��socket һ��
	����4 ������ϸ���׽�������   �� ���������Ƿ���Ҫ���� �Ƿ���Ҫ�����ʹ�  �׽���Ȩ�޵�   ��ʹ����null
	����5 һ��socket����ID   ����   ��ʱ û���� �� null
	����6 ָ���׽�������WSA_FLAG_OVERLAPPED  �����ص�IO�ĺ�

			WSA_FLAG_MULTIPOINT_C_ROOT
			WSA_FLAG_MULTIPOINT_C_LEAF 
			WSA_FLAG_MULTIPOINT_D_ROOT
			WSA_FLAG_MULTIPOINT_D_LEAF		���ڶಥЭ��

			WSA_FLAG_ACCESS_SYSTEM_SECURITY	��������Ȩ��  ��Ҫ��ϲ���4 ʹ��

			WSA_FLAG_NO_HANDLE_INHERIT		�׽��ֲ��ɱ��̳�
	**********************/

	//�� �˿ں͵�ַ
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	inet_pton(AF_INET, "127.0.0.1", &si.sin_addr);
	if (SOCKET_ERROR == bind(socketServer, (sockaddr*)&si, sizeof(si)))
	{
		int a = WSAGetLastError();
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	//����
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		int a = WSAGetLastError();
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	//��������
	g_socket[g_count] = socketServer;
	g_allOverLapped[g_count].hEvent = WSACreateEvent();
	g_count++;

	//accept�ĸ�����AcceptEx		���ڵ��ø��ӷ�װ��һ������
	if (0 != postAccept())
	{
		clear();
		WSACleanup();
		return 0;
	}

	while (1)
	{
		for (int i = 0; i < g_count; i++)
		{
			int nResME = WSAWaitForMultipleEvents(1, &(g_allOverLapped[i].hEvent), false, 0, false);
			if (WSA_WAIT_FAILED == nResME || WSA_WAIT_TIMEOUT == nResME)
			{
				continue;
			}
			//���ź���
			DWORD transfer;
			DWORD flag;
			BOOL getResult = WSAGetOverlappedResult(g_socket[i], &g_allOverLapped[i], &transfer, TRUE, &flag);
			if (FALSE == getResult)
			{
				//ʧ��			//ֱ��ǿ�ƹرտͻ���Ҳ������
				int a = WSAGetLastError();
				if (10054 == a)
				{
					printf("client close\n");
					//�ر�socket���¼�
					closesocket(g_socket[i]);
					WSACloseEvent(g_allOverLapped[i].hEvent);
					//������ɾ��
					g_count--;
					g_socket[i] = g_socket[g_count];
					g_allOverLapped[i].hEvent = g_allOverLapped[g_count].hEvent;
					//Ϊ��ѭ��������������ù��������socket i��Ҫ��1
					i--;
				}
				continue;
			}
			/************************************
			����1	��Ӧsocket			����2 �ص��ṹ��ַ
			����3	���봫�����ݴ�С		����4 �¼�֪ͨ��true
			����5	���䷽ʽ
			����ֵ
			************************************/
			
			//�ź�����Ϊ��
			WSAResetEvent(g_allOverLapped[i].hEvent);

			//�ɹ�
			if (0 == i)
			{//�����������
				printf("client connet\n");
				//���¿ͻ���Ͷ��WSARecv
				int pR = postRecv(g_count);
				if ( 0 != pR )
				{
					//���� 
					printf("�����룺%d\n", pR);
				}
				postSend(g_count);

				g_count++;
				postAccept();
			}
			else if (0 == transfer)
			{//�ͻ�������
				printf("client close\n");
				//�ر�socket���¼�
				closesocket(g_socket[i]);
				WSACloseEvent(g_allOverLapped[i].hEvent);
				//������ɾ��
				g_count--;
				g_socket[i] = g_socket[g_count];
				g_allOverLapped[i].hEvent = g_allOverLapped[g_count].hEvent;
				//Ϊ��ѭ��������������ù��������socket i��Ҫ��1
				i--;
			}
			else if (0 != transfer)
			{
				//���ͻ���ճɹ�
				if (0 != strRecv[0])
				{//������Ϣ
					//���ܵ���Ϣ
					//���һ��
					printf("%s\n", strRecv);
					//�������
					memset(strRecv, 0, sizeof(strRecv));
					//�ٴ�Ͷ��
					postRecv(i);
				}
				else
				{//������Ϣ send
			
				}
			}
		}
		

	}



	clear();
	WSACleanup();
	return 0;
}

void clear()
{
	for (int i = 0; i < g_count; i++)
	{
		closesocket(g_socket[i]);
		WSACloseEvent(g_allOverLapped[i].hEvent);
	}
	g_count = 0;
}


int postAccept()
{
	g_socket[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	g_allOverLapped[g_count].hEvent = WSACreateEvent();
	//���ܺ����ԼӱȽϺ�g_count++;   ����ʧ�ܾͲ����Լ���
	char str[1024] = { 0 };
	BOOL nResAccept = AcceptEx(g_socket[0], g_socket[g_count], str, 0, sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16, NULL, &g_allOverLapped[0]);
	if (TRUE == nResAccept)
	{
		//���������
		//Ͷ��recv�첽����		�����Ҫ�ٴ�Ͷ��acceptex
		postRecv(g_count);
		//postSend(g_count);
		//�������Ͷ��send
		//�ͻ�������++  g_count++
		g_count++;
		printf("client connet\n");
		//�ٴ�Ͷ��acceptex
		postAccept();		//�����Լ����ݹ飩
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//�ӳٴ���

			return 0;
		}
		else
		{
			//������
			return a;
		}
	}
	/***************************
	���ܣ�Ͷ�ݷ�����socket �첽��������
		�ͻ��˷����ĵ�һ��������acceptex����  ����Ĳ���recv���� 
	����1	������    ����2 �µĿͻ���	   ����3		һ���ַ�����ָ�� �������óɿ�
	����4    ����3��������Ĵ�С	 ������0����ʾ����3����ʧЧ		��ʹ��Ч ����3Ҳ����������д ������NULL
	����5	���ص�ַ��Ϣ�����ֽ���+16���ֽ�
	����6	Զ�̵�ַ��Ϣ�����ֽ���+16���ֽ�
	����7	��ϲ���3��4ʹ�õģ� �����̽��ܵ���Ϣ  ͨ���ò����õ����ܵ����ֽڸ���������һ���������ַ��������NULL
	����8	�ص��ṹ		&g_allOverLapped[0]		�����Ƿ��������ص��ṹ
	����ֵ	TRUE ������ɷ���  ��ִ�о��пͻ���������
			FALSE	���� int a = WSAGetLastError();
						a == ERROR_IO_PENDING	�첽�ȴ�		�ȴ��ͻ�������
						�������ݴ�������
	***************************/
}

int postRecv(int index)
{
	WSABUF buf;
	buf.buf = strRecv;
	buf.len = sizeof(strRecv);
	DWORD dword;
	DWORD dwflag = 0;
	int nResRecv = WSARecv(g_socket[index], &buf, 1, &dword, &dwflag, &g_allOverLapped[index], NULL);
	if (0 == nResRecv)
	{
		//�������
		//���һ��
		printf("%s\n", buf.buf);
		//�������
		memset(buf.buf, 0, sizeof(buf.buf));
		//�ٴ�Ͷ��
		postRecv(index);
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING == a)
		{
			//�ӳ����

			return 0;
		}
		else
		{
			//ִ��ʧ��  ������
			int a = WSAGetLastError();
			return a;
		}
	}

	/********************************
	����1  �ͻ���socket	����2 ���ܵ���Ϣ��Ĵ洢λ��	�ṹ��WSABUF �ṹ������ַ������ָ��ʹ�С
	����3  �м����ṹ��	����4  ���ݳɹ������ֽ����ĸ���
	����5	ָ����ȡ��ʽ		����6	�ص��ṹ
	����7	�ص�����  ������NULL		���������ʹ��
	����ֵ   0�������
	********************************/
}

int postSend(int index)
{
	WSABUF buf;
	buf.buf = strSend;
	buf.len = sizeof(strSend);
	DWORD dword;
	DWORD dwflag = 0;
	int nResSend = WSASend(g_socket[index], &buf, 1, &dword, dwflag, &g_allOverLapped[index], NULL);
	if (0 == nResSend)
	{
		//�������

		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING == a)
		{
			//�ӳ����

			return 0;
		}
		else
		{
			//ִ��ʧ��  ������
			int a = WSAGetLastError();
			return a;
		}
	}
}