
#define _WINSOCK_DEPRECATED_NO_WARNINGS

//#include<Windows.h>
#include<WinSock2.h>
#include<cstdio>
#include<cstdlib>
#include<WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

#define UM_AsyncSelectMessage  WM_USER+1

fd_set socketArray;
int y;

//���纯��
SOCKET internetFunction();
//�ص�����
LRESULT CALLBACK WinProc(HWND hwnd, UINT uIDmessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	//���ڽṹ��
	WNDCLASSEX wndclassex;
	wndclassex.cbClsExtra = 0; //����ռ�
	wndclassex.cbSize = sizeof(wndclassex);
	wndclassex.cbWndExtra = 0;
	wndclassex.hbrBackground = (HBRUSH)COLOR_BTNFACE;
	wndclassex.hCursor = NULL;
	wndclassex.hIcon = NULL;
	wndclassex.hIconSm = NULL;
	wndclassex.hInstance = hInstance;
	wndclassex.lpfnWndProc = WinProc;		//�ص�����
	wndclassex.lpszClassName = "AsyncSelect";
	wndclassex.lpszMenuName = NULL; //�˵�
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;

	//ע�ᴰ��
	if (0 == RegisterClassEx(&wndclassex))
	{
		//ע�ᴰ��ʧ��
		return 1;
	}

	//��������
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,		//���ڷ��
		"AsyncSelect",		//����������
		"�첽ѡ��ģ��",		//���ڵ�����
		WS_OVERLAPPEDWINDOW, //�����С���ȷ��
		500, 200,			//��ʼλ��
		600, 400,			//�� ��
		NULL,				//������
		NULL,				//�˵����
		hInstance,
		NULL);				//�Զ��庯��   ����ʵ�ֶ�����Ҫ�Ĺ���
	if (NULL == hwnd)
	{
		//����ʧ��
		return 1;
	}

	//��ʾ����
	ShowWindow(hwnd, nCmdShow);//���������һ������ Ĭ�Ͼ���1

	//������⵽��������			���Է�WM_CREATE�� ����switch�ṹcase�ж�������ᱨ��  ��Ҫ�������{}
	SOCKET socketSever = internetFunction();
	if (INVALID_SOCKET == socketSever)
	{
		//������⵽�����������д���
		return 1;
	}
	//�Ž�����
	socketArray.fd_array[socketArray.fd_count] = socketSever;
	socketArray.fd_count++;
	//��socket����Ϣ��
	if (SOCKET_ERROR == WSAAsyncSelect(socketSever, hwnd, UM_AsyncSelectMessage, FD_ACCEPT))
	{
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}

	//�ػ洰��
	UpdateWindow(hwnd);

	//��Ϣѭ��
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))    //һ��ȡһ��
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT uIDmessage, WPARAM wParam, LPARAM lParam)
{
	HDC hDc = GetDC(hwnd);
	char str[1024] = { 0 };
	switch (uIDmessage)
	{
	case UM_AsyncSelectMessage:
	{
		//MessageBox(NULL, L"�ͻ��˽���", L"��ʾ", MB_OK | MB_TOPMOST);
		//��ȡsocket  ͨ���ص������Ĳ���3
		SOCKET reactSocket = (SOCKET)wParam;
		//��ȡ��Ϣ	ͨ���ص������Ĳ���4
		//LOWORD(lParam);		//��Ϣ
		if(NULL != HIWORD(lParam))		//������
		{
			if (WSAECONNABORTED == HIWORD(lParam))
			{
				//ǿ������������
				//�رո�socket �� ��Ϣ
				WSAAsyncSelect(reactSocket, hwnd, NULL, NULL);
				closesocket(reactSocket);
				//������ɾ��
				for (unsigned int i = 0; i < socketArray.fd_count; i++)
				{
					if (reactSocket = socketArray.fd_array[i])
					{
						socketArray.fd_array[i] = socketArray.fd_array[socketArray.fd_count];
						socketArray.fd_count--;
					}
				}
				TextOut(hDc, 0, y, "client out false", sizeof("client out false") - 1);
				y += 20;
			}
			break;
		}
		//������Ϣ
		switch (LOWORD(lParam))
		{
		case FD_ACCEPT:
		{
			SOCKET socketClient = accept(reactSocket, NULL, NULL);
			if (INVALID_SOCKET == socketClient)
			{
				int a = WSAGetLastError();
				break;
			}
			if (SOCKET_ERROR == WSAAsyncSelect(socketClient, hwnd, UM_AsyncSelectMessage, FD_WRITE | FD_READ | FD_CLOSE))
			{
				int a = WSAGetLastError();
				closesocket(socketClient);
			}
			
			TextOut(hDc, 0, y, "client connet", sizeof("client connet") - 1);
			y += 20;
			//��¼������
			socketArray.fd_array[socketArray.fd_count] = socketClient;
			socketArray.fd_count++;
			
			if (SOCKET_ERROR == send(socketClient, "join success", sizeof("join success"), 0))
			{
				int a = WSAGetLastError();
			}
		}
			break;
		case FD_WRITE:
			TextOut(hDc, 0, y, "client write", sizeof("client write") - 1);
			y += 20;
			break;
		case FD_READ:
			if (SOCKET_ERROR == recv(reactSocket, str, sizeof(str), 0)) 
			{
				int a = WSAGetLastError();
			}
			TextOut(hDc, 0, y, str, strlen(str)); //������򵥵ķ�ʽ�Ǹĳɶ��ֽ��ַ���  ��Ȼ�͵��ú���ת��
			y += 20;
			break;
		case FD_CLOSE:
			//�رո�socket �� ��Ϣ
			WSAAsyncSelect(reactSocket, hwnd, NULL, NULL);
			closesocket(reactSocket);
			//������ɾ��
			for (unsigned int i = 0; i < socketArray.fd_count; i++)
			{
				if (reactSocket = socketArray.fd_array[i])
				{
					socketArray.fd_array[i] = socketArray.fd_array[socketArray.fd_count];
					socketArray.fd_count--;
				}
			}
			TextOut(hDc, 0, y, "client out", sizeof("client out") - 1);
			y += 20;
			break;
		}
		break;
	}


	case WM_CREATE:
		break;

	case WM_CLOSE:
		for (unsigned int i = 0; i < socketArray.fd_count; i++)
		{
			closesocket(socketArray.fd_array[i]);
		}
		WSACleanup();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);  //������quit��Ϣ ���������������		ʹ�����ʱҪ��getmassage �ڶ�������Ϊ0 ��Ȼ�����ѭ��
		break;
	}
	ReleaseDC(hwnd, hDc);
	return DefWindowProc(hwnd, uIDmessage, wParam, lParam);
}

SOCKET internetFunction()
{
	//�������
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRes = WSAStartup(wVersion, &wsaData);
	if (0 != nRes)
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("�����쳣,�������� �� ��������");
			break;
		case WSAVERNOTSUPPORTED:
			printf("�汾����");
			break;
		case WSAEPROCLIM:
			printf("���г������,�˿ڲ���");
			break;
		case WSAEINPROGRESS:
			printf("���ع��������� �ȴ���������������");
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
	SOCKET socketSever = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketSever)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return -1;
	}

	//��
	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(12345);
	inet_pton(AF_INET, "127.0.0.1", &si.sin_addr);
	int bi = bind(socketSever, (sockaddr*)&si, sizeof(si));
	if (SOCKET_ERROR == bi)
	{
		int a = WSAGetLastError();
		closesocket(socketSever);
		WSACleanup();
		return -1;
	}

	//����
	int li = listen(socketSever, SOMAXCONN);
	if (SOCKET_ERROR == li)
	{
		closesocket(socketSever);
		WSACleanup();
		return -1;
	}

	return socketSever;
}

/************************
WSAAsyncSelect(socketSever,hwnd,)
����1 socket  ����2 ���ھ��	
����3 �Զ���󶨵���Ϣ��ֵ��ע�ⲻ����ϵͳ�ڶ���Ϣ��ͻ��һ�㶨���  ��WM_USER ��+1 +2 +3�� (#define UM_ACCEPT  WM_USER+1)
����4 �󶨾������
************************/