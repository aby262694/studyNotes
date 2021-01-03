
#include<cstdio>
#include<cstdlib>
#include<winsock2.h>
#include <ws2tcpip.h> //inet_pton

#pragma comment(lib,"ws2_32.lib")

struct numberList
{
	unsigned short count;
	SOCKET socketarray[WSA_MAXIMUM_WAIT_EVENTS];  // ������64����  Ĭ��ÿ�δ���64�� 
	WSAEVENT eventarray[64];
}interfaceNumber;

BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		//�ͷ��¼�����
		for (int i = 0; i < interfaceNumber.count; i++)
		{
			closesocket(interfaceNumber.socketarray[i]);
			WSACloseEvent(interfaceNumber.eventarray[i]);
			interfaceNumber.count = 0;
		}
		WSACleanup();
	}
	return TRUE;
}

int main()
{
	//���������ܴ�����Ϣ
	SetConsoleCtrlHandler(fun, TRUE);

	//�������
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA WSAData;
	int startRes = WSAStartup(wVersionRequested, &WSAData);
	if (0 != startRes)
	{//ʧ��������Ӧ��  �ر������
		WSACleanup();
		switch (startRes)
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
	if ( 2 != HIBYTE(WSAData.wVersion) || 2 != LOBYTE(WSAData.wVersion))
	{
		WSACleanup();
		return 0;
	}
	
	//����socket
	SOCKET socketSever = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketSever)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return 0;
	}

	//��
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(12345);
	inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr);
	int biRes = bind(socketSever, (sockaddr*)&saddr, sizeof(saddr));
	if (SOCKET_ERROR == biRes)
	{
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}

	//����
	int liRes = listen(socketSever, SOMAXCONN);
	if (SOCKET_ERROR == liRes)
	{
		int a = WSAGetLastError();
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}

	//�����¼�����
	WSAEVENT eventSever = WSACreateEvent();
	if (WSA_INVALID_EVENT == eventSever)
	{
		int a = WSAGetLastError();
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}
	/*************************
	��غ���
	WSACloseEvent(eventSever); �ͷ��¼�������
	WSAResetEvent(); �����źŵ� �¼���������Ϊû�źŵ�
	WSASetEvent();   �����źŵ� �¼� �ó����źŵģ����ܾ����������źţ�
	*************************/
	
	//��������socket �� �¼� װ��ṹ����
	interfaceNumber.socketarray[interfaceNumber.count] = socketSever;
	interfaceNumber.eventarray[interfaceNumber.count] = eventSever;
	interfaceNumber.count++;

	//�󶨲�Ͷ�ݸ�ϵͳ
	int ESRes = WSAEventSelect(socketSever, eventSever, FD_ACCEPT);
	if (SOCKET_ERROR == ESRes)
	{
		int a = WSAGetLastError();
		WSACloseEvent(eventSever);
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}
	/*************************
	���� ���¼�����socket������룬��Ͷ�ݸ�����ϵͳ
	����1 ���󶨵�socket   ����2 ���󶨵��¼�����
	����3 �����¼�����			��������� | ����			������һ���ź�  ��ȻҲ��֪�����嵽�����ĸ������ź���Ӧ
		FD_ACCEPT   �пͻ�������   ���ڰ� ������socket
		FD_READ		�пͻ��˷�����Ϣ	��ͻ���socket��
		FD_CLOSE	�ͻ�������	���ڰ� �ͻ���socket
		FD_WRITE	���Ը��ͻ��˷���Ϣ		��ͻ��˰�				ֻ���ڿͻ�������ʱ����һ����Ϣ
		FD_CONNECT		�ڿͻ��˴����� ����������󶨷�����
		0		ȡ���¼�����
	FD_QOS�׽��ַ�������״̬�����仯֪ͨ
	�õ�����������Ϣ		WSAloctl();		����������Լ�����������ı仯

	����ֵ  �ɹ�����0    ʧ�ܷ���socket_error
	*************************/

	//ѭ��
	while (1)
	{
		//�鿴�¼��Ƿ����ź�
		DWORD getEventNumber = WSAWaitForMultipleEvents(interfaceNumber.count, interfaceNumber.eventarray, FALSE, WSA_INFINITE, FALSE);
		if (WSA_WAIT_FAILED == getEventNumber)
		{
			int a = WSAGetLastError();
			printf("�����룺%d\n", a);
			break;
		}
		////����4���ó�ʱʱ��ʱʹ��
		//if (WSA_WAIT_TIMEOUT == getEventNumber)
		//{
		//	continue;
		//}
		DWORD nIndex = getEventNumber - WSA_WAIT_EVENT_0;
		/********************
		����1 ��Ч�¼�����			����2 �¼��б�
		����3 �¼��ȴ���ʽ		TRUE �ȴ������¼��������źŲŷ���
			FALSE�κ�һ���¼������ź���������		����ֵ��ȥWSA_WAIT_EVENT_0��ʾ�¼�����������������з�Ӧ���¼�������±�
			����¼������ź�		�����±���С��
		����4 ��ʱ��� ��select�Ǹ�����һ�� ��λ����  ��ʱ���¼���Ӧ����WSA_WAIT_TIMEOUT
			������0 ���ȴ�		��WSA_INFINITE	�ȴ����¼�����
		����5	TRUE	�ص�IO��ʹ��			FALSE	�������ģ����ʹ��

		����ֵ	������±�ֵ		����3ΪTRUE ʱ �����¼������ź�	
								����3ΪFALSEʱ����ֵ��ȥWSA_WAIT_EVENT_0 == �������¼��±�
				����5Ϊ	TRUEʱ	����WSA_WAIT_IO_COMPLETION
				��ʱ���� WSA_WAIT_TIMEOUT
		********************/

		//���źŵķ��ദ��
		WSANETWORKEVENTS NetworkEvents;
		int ENERes = WSAEnumNetworkEvents(interfaceNumber.socketarray[nIndex], interfaceNumber.eventarray[nIndex], &NetworkEvents);
		if (SOCKET_ERROR == ENERes)
		{
			break;
		}
		/*********************
		����		��ȡ�¼����Ͳ����¼��ϵ��ź�����
		����1	��Ӧ��socket			����2	��Ӧ���¼�
		����3	��һ���ṹ��ָ��	�������¼�����װ������ṹ����	��Ա1 ��������¼�
							��Ա2 ������  FD_ACCEPT�¼�������װ��FD_ACCEPT_BIT�±���		û�д����Ӧ����0
									�õ�����  socket���ֵĴ�����Ϣ  ���Ǻ������еĴ���
		����ֵ	ִ�гɹ�����0		ʧ�ܷ���socket_error
		*********************/

		//��Ϣ����	��if�ṹ		else if ��switch ���߼���������һ���źŷ������ֻ���ֲ���ʱ��
		//�ж�FD_ACCEPT
		if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
		{
			//�ж��ǿͻ����������� ����	 �ͻ��˳�������
			if ( 0 == NetworkEvents.iErrorCode[FD_ACCEPT_BIT])
			{//����0˵��û�д�������������
				SOCKET socketClient = accept(socketSever, NULL, NULL);
				if (INVALID_SOCKET == socketClient)
				{
					continue;
				}
				//�����ͻ��˵��¼�����
				WSAEVENT eventClient = WSACreateEvent();
				if (WSA_INVALID_EVENT == eventClient)
				{
					closesocket(socketClient);
					continue;
				}
				//Ͷ�ݸ�ϵͳ
				int CnRes = WSAEventSelect(socketClient, eventClient, FD_READ | FD_CLOSE | FD_WRITE);
				if (SOCKET_ERROR == CnRes)
				{
					int a = WSAGetLastError();
					WSACloseEvent(eventClient);
					closesocket(socketClient);
					continue;
				}
				//���ͻ��˷Ž��ṹ��������
				interfaceNumber.socketarray[interfaceNumber.count] = socketClient;
				interfaceNumber.eventarray[interfaceNumber.count] = eventClient;
				interfaceNumber.count++;

				printf("client accept\n");
			}
			else
			{//�ͻ�������������ⲻ��Ӱ�����������������		ֱ�ӽ�����һ��ѭ������
				continue;
			}
		}

		//�ж�FD_WRITE		������if�߼��ϸ��Ͻ�		else ifҲ����ֻ����ĳЩ�����������ƫ����ϲ��õ�elseif��
		if (NetworkEvents.lNetworkEvents & FD_WRITE) //ֻ������ʱ����һ��	�����ڴ˴������ݳ�ʼ��
		{
			if (0 == NetworkEvents.iErrorCode[FD_WRITE_BIT])
			{
				printf("client write\n");
				if (SOCKET_ERROR == send(interfaceNumber.socketarray[nIndex], "join success", sizeof("connect success"), NULL))
				{
					int a = WSAGetLastError();
					printf("send error code:%d\n", a);
					continue;
				}
			}
			else
			{	//����
				printf("socket error code:%d", NetworkEvents.iErrorCode[FD_WRITE_BIT]);
				continue;
			}
		}
		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			if (0 == NetworkEvents.iErrorCode[FD_READ_BIT])
			{
				char rebuf[1500] = { 0 };
				if (SOCKET_ERROR == recv(interfaceNumber.socketarray[nIndex], rebuf, 1499, NULL))
				{
					int a = WSAGetLastError();
					printf("recv error code:%d\n", a);
					continue;
				}
				printf("recv : %s\n", rebuf);
			}
			else
			{	//����
				printf("socket error code:%d", NetworkEvents.iErrorCode[FD_READ_BIT]);
				continue;
			}
		}
		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			
			//�ͻ��˶Ͽ����ӣ�ɾ������ͻ���
			closesocket(interfaceNumber.socketarray[nIndex]);
			WSACloseEvent(interfaceNumber.eventarray[nIndex]);
			//��ɾ�������λ�÷�����������һ������		������-1
			--interfaceNumber.count;
			interfaceNumber.socketarray[nIndex] = interfaceNumber.socketarray[interfaceNumber.count];
			interfaceNumber.eventarray[nIndex] = interfaceNumber.eventarray[interfaceNumber.count];
			printf("this client close\n");
		
		}
	}
	

	//�ͷ��¼�����
	for (int i = 0; i < interfaceNumber.count; i++)
	{
		closesocket(interfaceNumber.socketarray[i]);
		WSACloseEvent(interfaceNumber.eventarray[i]);
		interfaceNumber.count = 0;
	}
	WSACleanup();
	return 0;
}


/***************************
�ɽ��������Ż�����			�������
	��õķ�ʽ������߳����̳߳�	��������߳� ���ܳ�Ϊ�������õ�����ģ��
	Ҳ������ѭ�� һ��һ���Ĳ鿴�źţ�WSAWaitForMultipleEvents������һ���̶��ϱ��������������

�����¼���������
	һ��һ���� ͨ���߳� �̳߳��������Ż���� ÿ���߳�һ��64��
	Ҳ����һ��һ��ѯ�ʣ�WSAWaitForMultipleEvents��	�Ϳ��Գ�������������
	һ��һ��Ҳ������forѭ������  ���¼�����ṹ������ ���������������
***************************/