
#include<cstdio>
#include<cstdlib>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<MSWSock.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

//�¼�socket ���� ��С
#define SOCKET_EVENT_COUNT		1024

//������������С
#define WSA_IO_COUNT			1024
char strRecv[WSA_IO_COUNT];
char strSend[WSA_IO_COUNT] = "join success\n";

//�¼�socket ����
SOCKET g_socket[SOCKET_EVENT_COUNT];
OVERLAPPED g_overlappde[SOCKET_EVENT_COUNT];
int g_count;

//Ͷ���첽����
int postAccept();
int postRecv(int index);
int postSend(int index);

void clear();

//cpu����
int nProcessors;
HANDLE hPort;
//�̺߳���
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);
//�߳�����
HANDLE* g_handle;

//֧���̹߳ر� ���ƿ���		Ҳ�������˳��̺߳��� TerminateThread();��Ҫռ��һ����cpu��Դ  ����������Լ�����
BOOL g_flag = TRUE;

BOOL WINAPI fun(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		//�˳� ֧���߳�
		g_flag = FALSE;
		//�ͷ��߳̾��
		for (int i = 0; i < nProcessors; i++)
		{
										//TerminateThread()
			CloseHandle(g_handle[i]);
		}
		delete(g_handle);
		g_handle = NULL;

		clear();
		CloseHandle(hPort);
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
	WSADATA wsadata;
	int nRes = WSAStartup(wVersion, &wsadata);
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
	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
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

	//�Ž�������
	g_socket[0] = socketServer;
	g_overlappde[0].hEvent = WSACreateEvent();
	g_count++;


	//bind
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	inet_pton(AF_INET, "127.0.0.1", &si.sin_addr);
	if (SOCKET_ERROR == bind(socketServer, (sockaddr*)&si, sizeof(si)))
	{
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//������ɶ˿�
	hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (0 == hPort)
	{
		int a = GetLastError();   //ע������û��WSA
		printf("%d\n", a);
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	/************
	������ɶ˿�  ��һ���Ǻ�  �������ʱ��ڶ�������������null  �������Ǻ��Ե� ��ɶ���� ����0
	����4��0�Զ���ȡCPU����
	************/

	//��
	hPort = CreateIoCompletionPort((HANDLE)socketServer, hPort, 0, 0);
	if (0 == hPort)
	{
		int a = GetLastError();   //ע������û��WSA
		printf("%d\n", a);
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	/**********************************
	��   ����1	socket		����2	��ɶ˿ڱ���
		����3	����ɶ˿� �󶨵���Ϣ ��������  �õ���ɶ˿���Ӧʱ ͨ���˲���ȷ������ϵͳ���յ������ݹ�����һ��
		����4	����	��0
		����ֵ		ִ�гɹ�����  hPort�Լ�
					ʧ��  ����0
	**********************************/

	//listen 
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//Ͷ��accept
	int nResAccept = postAccept();
	if (0 != nResAccept)
	{
		printf("%d\n", nResAccept);
	}

	//�õ�����ϵͳ��Ϣ
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	nProcessors = SystemInfo.dwNumberOfProcessors;

	//�߳�����
	g_handle = new HANDLE[nProcessors];

	//�����߳�
	for (int i = 0; i < nProcessors; i++)
	{
		g_handle[i] = CreateThread(NULL, 0, ThreadProc, hPort, 0, NULL);
		if (NULL == g_handle[i])
		{
			int a = GetLastError();
			printf("%d\n", a);
			CloseHandle(hPort);
			closesocket(socketServer);
			WSACleanup();
			return 0;
		}
		/**********************
		����		����һ���߳�
		����1	�߳̾���Ƿ��ܱ��̳�	ָ���߳�ִ��Ȩ��		NULL ���̳�
		����2	�̴߳�С		��0 Ĭ�ϴ�С  ����1M		��λ���ֽ�
				��Ŀ����ϵͳ�� �����޸����̵߳Ĵ�С		�����Լ�������Ĭ�������߳�
		����3	�̺߳�����ַ		DWORD WINAPI *PTHREAD_START_ROUTINE( LPVOID lpThreadParameter );
				�̺߳����Ĳ����� createthread�Ĳ���4���ݽ�����
		����4	���ⲿ���ݴ��ݸ��߳��ڲ�
		����5	�߳�ִ��״̬  ����ִ�� �����
				0	����ִ��
				CREATE_SUSPENDED	����״̬	����ResumeThreadʱ�����߳�
				STACK_SIZE_PARAM_IS_A_RESERVATION	�����2 ����  
					�����˲���2 Ϊ ջ������С�������ڴ棩Ĭ��1M		û����ջ�ύ��С�������ڴ棩Ĭ��4kb
		����6	�߳�ID	������NULL
		����ֵ	�ɹ������߳̾��	�����Ҫ�ͷ�CloseHandle			ʧ��NULL
		**********************/
	}

	//���̲߳��ܽ���   Ҫ����
	while (1)
	{
		Sleep(1000);  //ͣһ��
	}


	//�˳� ֧���߳�
	g_flag = FALSE;
	//�ͷ��߳̾��
	for (int i = 0; i < nProcessors; i++)
	{
		CloseHandle(g_handle[i]);
	}
	delete(g_handle);
	g_handle = NULL;

	CloseHandle(hPort);
	clear();
	WSACleanup();
	return 0;
}

void clear()
{
	for (int i = 0; i < g_count; i++)
	{
		if (NULL == g_socket[i])
		{
			continue;
		}
		closesocket(g_socket[i]);
		WSACloseEvent(g_overlappde[i].hEvent);
	}
	g_count = 0;
}

int postAccept()
{
	g_socket[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	g_overlappde[g_count].hEvent = WSACreateEvent();
	char straccept[WSA_IO_COUNT];
	BOOL nResAccept = AcceptEx(g_socket[0], g_socket[g_count], straccept, 0,
		sizeof(sockaddr) + 16, sizeof(sockaddr) + 16, NULL, &g_overlappde[0]);
	//if (TRUE == nResAccept)
	//{
	//	//�������
	//
	//	printf("client connect\n");
	//	//�Ȱ�
	//	hPort = CreateIoCompletionPort((HANDLE)g_socket[g_count], hPort, g_count, 0);
	//	if (NULL == hPort)
	//	{
	//		int a = WSAGetLastError();
	//		printf("%d\n", a);
	//		//������  ɾ������ͻ���
	//		closesocket(g_socket[g_count]);
	//		WSACloseEvent(g_overlappde[g_count].hEvent);
	//	}
	//
	//	//Ͷ�� �첽 recv �� send
	//	postRecv(g_count);
	//	postSend(g_count);
	//	//�ٴ�Ͷ��accept
	//	g_count++;
	//	postAccept();
	//
	//	return 0;
	//}
	//else
	//{
	//	int a = WSAGetLastError();
	//	if (ERROR_IO_PENDING == a)
	//	{
	//		//�ӳ����
	//
	//		return 0;
	//	}
	//	else
	//	{
	//		//������
	//		return a;
	//	}
	//}
	if (FALSE == nResAccept)
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING != a)
		{
			//����ִ�г���
			printf("%d\n", a);
			return a;
		}
	}
	
	return 0;
}

int postRecv(int index)
{
	WSABUF buf;
	buf.buf = strRecv;
	buf.len = WSA_IO_COUNT;
	DWORD dword = 0;
	DWORD dwflag = 0;
	int nResRecv = WSARecv(g_socket[index], &buf, 1, &dword, &dwflag, &g_overlappde[index], NULL);
	//if (0 == nResRecv)
	//{
	//	//�������
	//	printf("%s\n", buf.buf);
	//	memset(buf.buf, 0, sizeof(buf.buf));
	//	postRecv(index);
	//	return 0;
	//}
	//else
	//{
	//	int a = WSAGetLastError();
	//	if (ERROR_IO_PENDING == a)
	//	{
	//		//�ӳ����
	//
	//		return 0;
	//	}
	//	else
	//	{
	//		//������
	//		return a;
	//	}
	//}
	if (0 != nResRecv)
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING != a)
		{
			//����ִ�г���
			printf("%d\n", a);
			return a;
		}
	}

	return 0;
}

int postSend(int index)
{
	WSABUF buf;
	buf.buf = strSend;
	buf.len = WSA_IO_COUNT;
	DWORD dword = 0;
	DWORD dwflag = 0;
	int nResSend = WSASend(g_socket[index], &buf, 1, &dword, dwflag, &g_overlappde[index], NULL);

	//if (0 == nResSend)
	//{
	//	return 0;
	//}
	//else
	//{
	//	int a = WSAGetLastError();
	//	if (WSA_IO_PENDING == a)
	//	{
	//		//�ӳ����
	//		return 0;
	//	}
	//	else
	//	{
	//		//ִ��ʧ��  ������
	//		int a = WSAGetLastError();
	//		return a;
	//	}
	//}
	if (0 != nResSend)
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING != a || 0 != a)
		{
			//����ִ�г���
			printf("%d\n", a);
			return a;
		}
	}
	
	return 0;
}

DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{
	HANDLE port = (HANDLE)lpThreadParameter;
	DWORD dword;
	ULONG_PTR CompletionKey;
	LPOVERLAPPED lpOverlapped;

	while (TRUE == g_flag)
	{
		BOOL bflag = GetQueuedCompletionStatus(port, &dword, &CompletionKey, &lpOverlapped, INFINITE);
		if (FALSE == bflag)
		{
			int a = GetLastError();
			if (64 == a)		//������������	 ���� ����10054
			{//ǿ������
				//�ͻ����˳�
				printf("client close\n");
				closesocket(g_socket[CompletionKey]);
				WSACloseEvent(g_overlappde[CompletionKey].hEvent);
				//g_count--;
				//g_socket[CompletionKey] = g_socket[g_count];
				//g_overlappde[CompletionKey].hEvent = g_overlappde[CompletionKey].hEvent;
				//���� ���±� ��ָ����  ���Բ���ͨ�����ַ�ʽ ɾ���ͻ���
				g_socket[CompletionKey] = NULL;
				g_overlappde[CompletionKey].hEvent = NULL;
			}
			else
			{
				printf("%d\n", a);
			}
			continue;
		}
		/**********************************
		��֪ͨʱ�̹߳��� ��ռ��CPU
		����  ���Գ��ԴӶ˿� ��ȡ �з�Ӧ�����ݰ�
		����1	��ɶ˿�
		����2	���ܻ��߷��͵��ֽ���
		����3	ͨ�� ��ɶ˿ڰ�  ����3���ݽ�����
		����4	�ص��ṹ
		����5	�ȴ�ʱ��				INFINITE һֱ��
		����ֵ	�ɹ�����	TRUE	ʧ�� FALSE
		**********************************/

		//���ദ��
		if (0 == CompletionKey)			//�˴�Ҳ����ͨ���ص��ṹ�ж�
		{
			//accept
			//��socket �󶨵� ��ɶ˿���
			hPort = CreateIoCompletionPort((HANDLE)g_socket[g_count], hPort, g_count, 0);
			if (NULL == hPort)
			{
				int a = WSAGetLastError();
				printf("%d\n", a);
				//������  ɾ������ͻ���
				closesocket(g_socket[g_count]);
				WSACloseEvent(g_overlappde[g_count].hEvent);
				continue;
			}
			//�¿ͻ���Ͷ�� recv ��send
			postRecv(g_count);
			postSend(g_count);

			printf("client connect\n");

			g_count++;
			postAccept();

		}
		else
		{
			if (0 == dword)
			{
				//�ͻ����˳�
				printf("client close\n");
				closesocket(g_socket[CompletionKey]);
				WSACloseEvent(g_overlappde[CompletionKey].hEvent);
				//g_count--;
				//g_socket[CompletionKey] = g_socket[g_count];
				//g_overlappde[CompletionKey].hEvent = g_overlappde[CompletionKey].hEvent;
				//���� ���±� ��ָ����  ���Բ���ͨ�����ַ�ʽ ɾ���ͻ���
				g_socket[CompletionKey] = NULL;
				g_overlappde[CompletionKey].hEvent = NULL;
				continue;
			}
			if (0 != dword)
			{
				//���ջ���
				if (0 != strRecv[0] && 0 != strRecv[1])
				{
					//����
					printf("%s\n", strRecv);
					memset(strRecv, 0, sizeof(strRecv));
					postRecv(CompletionKey);
				}
				else
				{
					//����
				}
			}
		}
	}
	return 0;
}

/***************************
����ɶ˿���  ������ ����ִ�� ���� �ӳ����  ������  ���֪ͨ
accept  recv��send ���Բ��� ���� �����ӳ���
***************************/