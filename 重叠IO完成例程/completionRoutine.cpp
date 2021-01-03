
#include<cstdio>
#include<cstdlib>
#include<WinSock2.h>
#include<MSWSock.h>
#include<WS2tcpip.h>

#define WSA_ARRAY_MAX	1024	//事件socket 数组容量
#define WSA_IO_MAX		1024	//输入输出数组容量

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

//创建事件socket的数组
SOCKET g_socket[WSA_ARRAY_MAX];
OVERLAPPED g_overlapped[WSA_ARRAY_MAX];
int g_count;

//接收消息数组
char strRecv[WSA_IO_MAX];
char strSend[WSA_IO_MAX] = "join success\n";

int postAccept();
int postRecv(int index);
int postSend(int index);

void clear();

//回调函数
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


	//打开网络库
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA	WSAData;
	int nRes = WSAStartup(wVersion, &WSAData);
	if (0 != nRes)   //成功返回0
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("环境异常,重启电脑 或 检查网络库\n");
			break;
		case WSAVERNOTSUPPORTED:
			printf("版本过低\n");
			break;
		case WSAEPROCLIM:
			printf("运行程序过多,端口不足\n");
			break;
		case WSAEINPROGRESS:
			printf("加载过程中阻塞 等待或重新启动程序\n");
			break;
		case WSAEFAULT:
			//参数2写错了
			break;
		}
	}
	//校验版本
	if (2 != HIBYTE(WSAData.wVersion) || 2 != LOBYTE(WSAData.wVersion))
	{
		WSACleanup();
		return 0;
	}

	//创建服务器socket
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
	
	//放进数组
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
		//完成例程  参数5填TRUE	获取到完成例程 执行完一次的信号  WSA_WAIT_IO_COMPLETION
		if (WSA_WAIT_FAILED == nRes || WSA_WAIT_IO_COMPLETION == nRes)
		{//失败  或执行完成一次完成例程
			continue;
		}
		//服务器信号
		//信号置空
		WSAResetEvent(g_overlapped[0].hEvent);
		
		printf("client connect\n");
		//投递异步recv
		postRecv(g_count);

		postSend(g_count);
		//再次投递accept
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
		//立即接收
		printf("client connect\n");
		
		//投递异步recv
		postRecv(g_count);

		postSend(g_count);
		
		//再次投递accept
		g_count++;
		postAccept();
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//延迟接收

			return 0;
		}
		else
		{
			//出错了
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
		//立即完成
		//输出一下
		printf("%s\n", buf.buf);
		//清空数组
		memset(buf.buf, 0, sizeof(buf.buf));
		//再次投递
		postRecv(index);
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING == a)
		{
			//延迟完成

			return 0;
		}
		else
		{
			//执行失败  出错了
			int a = WSAGetLastError();
			return a;
		}
	}
}

void CALLBACK backRecv(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	/**************************************
	参数1	错误码   参数2	发送或得到的字节数
	参数3	重叠结构	参数4	执行方式
	**************************************/
	//得到数组下标
	//int i = 0;
	//for (i = 0; i < g_count; i++)
	//{
	//	if (lpOverlapped->hEvent == g_overlapped[i].hEvent)
	//	{
	//		break;
	//	}
	//}
	int i = lpOverlapped - &g_overlapped[0];  //得到下标的高效率方式	还有更好的方法
	

	if (10054 == dwError || 0 == cbTransferred)
	{
		//客户端退出
		printf("client close\n");
		//关闭socket  事件
		closesocket(g_socket[i]);
		WSACloseEvent(g_overlapped[i].hEvent);
		//数组中删除
		g_count--;
		g_socket[i] = g_socket[g_count];
		g_overlapped[i] = g_overlapped[g_count];

	}
	if( 0 != cbTransferred)
	{
		//传递数据
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
			//延迟完成
			return 0;
		}
		else
		{
			//执行失败  出错了
			int a = WSAGetLastError();
			return a;
		}
	}
}

//void CALLBACK backSend(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
//{
//
//}