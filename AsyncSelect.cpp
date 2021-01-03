
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

//网络函数
SOCKET internetFunction();
//回调函数
LRESULT CALLBACK WinProc(HWND hwnd, UINT uIDmessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
	//窗口结构体
	WNDCLASSEX wndclassex;
	wndclassex.cbClsExtra = 0; //额外空间
	wndclassex.cbSize = sizeof(wndclassex);
	wndclassex.cbWndExtra = 0;
	wndclassex.hbrBackground = (HBRUSH)COLOR_BTNFACE;
	wndclassex.hCursor = NULL;
	wndclassex.hIcon = NULL;
	wndclassex.hIconSm = NULL;
	wndclassex.hInstance = hInstance;
	wndclassex.lpfnWndProc = WinProc;		//回调函数
	wndclassex.lpszClassName = "AsyncSelect";
	wndclassex.lpszMenuName = NULL; //菜单
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;

	//注册窗口
	if (0 == RegisterClassEx(&wndclassex))
	{
		//注册窗口失败
		return 1;
	}

	//创建窗口
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,		//窗口风格
		"AsyncSelect",		//窗口类名字
		"异步选择模型",		//窗口的名字
		WS_OVERLAPPEDWINDOW, //最大化最小化等风格
		500, 200,			//起始位置
		600, 400,			//宽 高
		NULL,				//父窗口
		NULL,				//菜单句柄
		hInstance,
		NULL);				//自定义函数   用于实现额外想要的功能
	if (NULL == hwnd)
	{
		//创建失败
		return 1;
	}

	//显示窗口
	ShowWindow(hwnd, nCmdShow);//主参数最后一个参数 默认就是1

	//打开网络库到监听过程			可以放WM_CREATE中 由于switch结构case中定义变量会报错  需要在外面加{}
	SOCKET socketSever = internetFunction();
	if (INVALID_SOCKET == socketSever)
	{
		//打开网络库到监听过程中有错误
		return 1;
	}
	//放进数组
	socketArray.fd_array[socketArray.fd_count] = socketSever;
	socketArray.fd_count++;
	//将socket与消息绑定
	if (SOCKET_ERROR == WSAAsyncSelect(socketSever, hwnd, UM_AsyncSelectMessage, FD_ACCEPT))
	{
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}

	//重绘窗口
	UpdateWindow(hwnd);

	//消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))    //一次取一个
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
		//MessageBox(NULL, L"客户端接入", L"提示", MB_OK | MB_TOPMOST);
		//获取socket  通过回调函数的参数3
		SOCKET reactSocket = (SOCKET)wParam;
		//获取消息	通过回调函数的参数4
		//LOWORD(lParam);		//消息
		if(NULL != HIWORD(lParam))		//错误码
		{
			if (WSAECONNABORTED == HIWORD(lParam))
			{
				//强制下线走这里
				//关闭该socket 的 消息
				WSAAsyncSelect(reactSocket, hwnd, NULL, NULL);
				closesocket(reactSocket);
				//数组中删除
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
		//具体消息
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
			//记录进数组
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
			TextOut(hDc, 0, y, str, strlen(str)); //这里最简单的方式是改成多字节字符集  不然就得用函数转换
			y += 20;
			break;
		case FD_CLOSE:
			//关闭该socket 的 消息
			WSAAsyncSelect(reactSocket, hwnd, NULL, NULL);
			closesocket(reactSocket);
			//数组中删除
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
		PostQuitMessage(0);  //传递了quit消息 程序才是真正结束		使用这个时要让getmassage 第二个参数为0 不然会成死循环
		break;
	}
	ReleaseDC(hwnd, hDc);
	return DefWindowProc(hwnd, uIDmessage, wParam, lParam);
}

SOCKET internetFunction()
{
	//打开网络库
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRes = WSAStartup(wVersion, &wsaData);
	if (0 != nRes)
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("环境异常,重启电脑 或 检查网络库");
			break;
		case WSAVERNOTSUPPORTED:
			printf("版本过低");
			break;
		case WSAEPROCLIM:
			printf("运行程序过多,端口不足");
			break;
		case WSAEINPROGRESS:
			printf("加载过程中阻塞 等待或重新启动程序");
			break;
		case WSAEFAULT:
			//参数2写错了
			break;
		}
	}
	//校验版本
	if (2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))
	{
		WSACleanup();
		return -1;
	}

	//创建socket
	SOCKET socketSever = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketSever)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return -1;
	}

	//绑定
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

	//监听
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
参数1 socket  参数2 窗口句柄	
参数3 自定义绑定的消息数值（注意不能与系统内定消息冲突）一般定义宏  在WM_USER 上+1 +2 +3等 (#define UM_ACCEPT  WM_USER+1)
参数4 绑定具体操作
************************/