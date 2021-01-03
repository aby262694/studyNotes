
#include<cstdio>
#include<cstdlib>
#include<WinSock2.h>
#include<MSWSock.h>
#include<WS2tcpip.h>

#define WSA_ARRAY_MAX	1024	//�¼�socket ��������
#define WSA_IO_MAX		1024	//���������������

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

//�����¼�socket������
SOCKET g_socket[WSA_ARRAY_MAX];
OVERLAPPED g_overlapped[WSA_ARRAY_MAX];
int g_count;

//������Ϣ����
char strRecv[WSA_IO_MAX];
char strSend[WSA_IO_MAX] = "join success\n";

int postAccept();
int postRecv(int index);
int postSend(int index);

void clear();

//�ص�����
void CALLBACK backRecv(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);
//void CALLBACK backSend(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);

BOOL WINAPI fun(DWORD CtrlType)
{
	switch (CtrlType)
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
	WSADATA	WSAData;
	int nRes = WSAStartup(wVersion, &WSAData);
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
	if (2 != HIBYTE(WSAData.wVersion) || 2 != LOBYTE(WSAData.wVersion))
	{
		WSACleanup();
		return 0;
	}

	//����������socket
	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		WSACleanup();
		return 0;
	}

	//bind
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
	//listen
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		int a = WSAGetLastError();
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	
	//�Ž�����
	g_socket[0] = socketServer;
	g_overlapped[0].hEvent = WSACreateEvent();
	g_count++;

	if (0 != postAccept())
	{
		clear();
		WSACleanup();
		return 0;
	}


	while (1)
	{
		DWORD nRes = WSAWaitForMultipleEvents(1, &(g_overlapped[0].hEvent), FALSE, WSA_INFINITE, TRUE);
		//�������  ����5��TRUE	��ȡ��������� ִ����һ�ε��ź�  WSA_WAIT_IO_COMPLETION
		if (WSA_WAIT_FAILED == nRes || WSA_WAIT_IO_COMPLETION == nRes)
		{//ʧ��  ��ִ�����һ���������
			continue;
		}
		//�������ź�
		//�ź��ÿ�
		WSAResetEvent(g_overlapped[0].hEvent);
		
		printf("client connect\n");
		//Ͷ���첽recv
		postRecv(g_count);

		postSend(g_count);
		//�ٴ�Ͷ��accept
		g_count++;
		postAccept();
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
		WSACloseEvent(g_overlapped[i].hEvent);
	}
	g_count = 0;
}

int postAccept()
{
	g_socket[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	g_overlapped[g_count].hEvent = WSACreateEvent();
	char str[WSA_IO_MAX] = { 0 };
	BOOL nResAccept = AcceptEx(g_socket[0], g_socket[g_count], str, 0, sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16, NULL, &g_overlapped[0]);
	if (TRUE == nResAccept)
	{
		//��������
		printf("client connect\n");
		
		//Ͷ���첽recv
		postRecv(g_count);

		postSend(g_count);
		
		//�ٴ�Ͷ��accept
		g_count++;
		postAccept();
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//�ӳٽ���

			return 0;
		}
		else
		{
			//������
			return a;
		}
	}
}

int postRecv(int index)
{
	WSABUF buf;
	buf.buf = strRecv;
	buf.len = WSA_IO_MAX;
	DWORD dword = 0;
	DWORD dwflag = 0;
	int nResRecv = WSARecv(g_socket[index], &buf, 1, &dword, &dwflag, &g_overlapped[index], backRecv);

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
}

void CALLBACK backRecv(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	/**************************************
	����1	������   ����2	���ͻ�õ����ֽ���
	����3	�ص��ṹ	����4	ִ�з�ʽ
	**************************************/
	//�õ������±�
	//int i = 0;
	//for (i = 0; i < g_count; i++)
	//{
	//	if (lpOverlapped->hEvent == g_overlapped[i].hEvent)
	//	{
	//		break;
	//	}
	//}
	int i = lpOverlapped - &g_overlapped[0];  //�õ��±�ĸ�Ч�ʷ�ʽ	���и��õķ���
	

	if (10054 == dwError || 0 == cbTransferred)
	{
		//�ͻ����˳�
		printf("client close\n");
		//�ر�socket  �¼�
		closesocket(g_socket[i]);
		WSACloseEvent(g_overlapped[i].hEvent);
		//������ɾ��
		g_count--;
		g_socket[i] = g_socket[g_count];
		g_overlapped[i] = g_overlapped[g_count];

	}
	if( 0 != cbTransferred)
	{
		//��������
		printf("%s\n", strRecv);
		memset(strRecv, 0, sizeof(strRecv));
		postRecv(i);
	}
}

int postSend(int index)
{
	WSABUF buf;
	buf.buf = strSend;
	buf.len = WSA_IO_MAX;
	DWORD dword = 0;
	DWORD dwflag = 0;
	int nResSend = WSASend(g_socket[index], &buf, 1, &dword, dwflag, &g_overlapped[index], NULL);

	if (0 == nResSend)
	{
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

//void CALLBACK backSend(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
//{
//
//}