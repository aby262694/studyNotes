
#include<cstdio>
#include<cstdlib>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<MSWSock.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

//事件socket 数组 大小
#define SOCKET_EVENT_COUNT		1024

//输入输出数组大小
#define WSA_IO_COUNT			1024
char strRecv[WSA_IO_COUNT];
char strSend[WSA_IO_COUNT] = "join success\n";

//事件socket 数组
SOCKET g_socket[SOCKET_EVENT_COUNT];
OVERLAPPED g_overlappde[SOCKET_EVENT_COUNT];
int g_count;

//投递异步函数
int postAccept();
int postRecv(int index);
int postSend(int index);

void clear();

//cpu核数
int nProcessors;
HANDLE hPort;
//线程函数
DWORD WINAPI ThreadProc(LPVOID lpThreadParameter);
//线程数组
HANDLE* g_handle;

//支线线程关闭 控制开关		也可以用退出线程函数 TerminateThread();需要占用一部分cpu资源  最好运行完自己结束
BOOL g_flag = TRUE;

BOOL WINAPI fun(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		//退出 支线线程
		g_flag = FALSE;
		//释放线程句柄
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

	//打开网络库
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsadata;
	int nRes = WSAStartup(wVersion, &wsadata);
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
	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
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

	//放进数组里
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

	//创建完成端口
	hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (0 == hPort)
	{
		int a = GetLastError();   //注意这里没有WSA
		printf("%d\n", a);
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	/************
	创建完成端口  第一个是宏  填这个的时候第二个参数必须是null  参数三是忽略的 填啥都行 建议0
	参数4填0自动获取CPU核数
	************/

	//绑定
	hPort = CreateIoCompletionPort((HANDLE)socketServer, hPort, 0, 0);
	if (0 == hPort)
	{
		int a = GetLastError();   //注意这里没有WSA
		printf("%d\n", a);
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	/**********************************
	绑定   参数1	socket		参数2	完成端口变量
		参数3	将完成端口 绑定的信息 填在这里  得到完成端口响应时 通过此参数确定（与系统接收到的数据关联在一起）
		参数4	忽略	填0
		返回值		执行成功返回  hPort自己
					失败  返回0
	**********************************/

	//listen 
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}


	//投递accept
	int nResAccept = postAccept();
	if (0 != nResAccept)
	{
		printf("%d\n", nResAccept);
	}

	//得到操作系统信息
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	nProcessors = SystemInfo.dwNumberOfProcessors;

	//线程数组
	g_handle = new HANDLE[nProcessors];

	//创建线程
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
		功能		创建一根线程
		参数1	线程句柄是否能被继承	指定线程执行权限		NULL 不继承
		参数2	线程大小		填0 默认大小  就是1M		单位是字节
				项目属性系统中 可以修改主线程的大小		咱们自己创建的默认是子线程
		参数3	线程函数地址		DWORD WINAPI *PTHREAD_START_ROUTINE( LPVOID lpThreadParameter );
				线程函数的参数是 createthread的参数4传递进来的
		参数4	将外部数据传递给线程内部
		参数5	线程执行状态  立即执行 挂起等
				0	立即执行
				CREATE_SUSPENDED	挂起状态	调用ResumeThread时启动线程
				STACK_SIZE_PARAM_IS_A_RESERVATION	与参数2 关联  
					设置了参数2 为 栈保留大小（虚拟内存）默认1M		没设置栈提交大小（物理内存）默认4kb
		参数6	线程ID	可以填NULL
		返回值	成功返回线程句柄	最后需要释放CloseHandle			失败NULL
		**********************/
	}

	//主线程不能结束   要阻塞
	while (1)
	{
		Sleep(1000);  //停一秒
	}


	//退出 支线线程
	g_flag = FALSE;
	//释放线程句柄
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
	//	//立即完成
	//
	//	printf("client connect\n");
	//	//先绑定
	//	hPort = CreateIoCompletionPort((HANDLE)g_socket[g_count], hPort, g_count, 0);
	//	if (NULL == hPort)
	//	{
	//		int a = WSAGetLastError();
	//		printf("%d\n", a);
	//		//出错了  删除这个客户端
	//		closesocket(g_socket[g_count]);
	//		WSACloseEvent(g_overlappde[g_count].hEvent);
	//	}
	//
	//	//投递 异步 recv 和 send
	//	postRecv(g_count);
	//	postSend(g_count);
	//	//再次投递accept
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
	//		//延迟完成
	//
	//		return 0;
	//	}
	//	else
	//	{
	//		//出错了
	//		return a;
	//	}
	//}
	if (FALSE == nResAccept)
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING != a)
		{
			//函数执行出错
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
	//	//立即完成
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
	//		//延迟完成
	//
	//		return 0;
	//	}
	//	else
	//	{
	//		//出错了
	//		return a;
	//	}
	//}
	if (0 != nResRecv)
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING != a)
		{
			//函数执行出错
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
	//		//延迟完成
	//		return 0;
	//	}
	//	else
	//	{
	//		//执行失败  出错了
	//		int a = WSAGetLastError();
	//		return a;
	//	}
	//}
	if (0 != nResSend)
	{
		int a = WSAGetLastError();
		if (WSA_IO_PENDING != a || 0 != a)
		{
			//函数执行出错
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
			if (64 == a)		//不是网络特有	 所以 不是10054
			{//强制下线
				//客户端退出
				printf("client close\n");
				closesocket(g_socket[CompletionKey]);
				WSACloseEvent(g_overlappde[CompletionKey].hEvent);
				//g_count--;
				//g_socket[CompletionKey] = g_socket[g_count];
				//g_overlappde[CompletionKey].hEvent = g_overlappde[CompletionKey].hEvent;
				//由于 绑定下标 是指定的  所以不能通过这种方式 删除客户端
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
		无通知时线程挂起 不占用CPU
		功能  尝试尝试从端口 获取 有反应的数据包
		参数1	完成端口
		参数2	接受或者发送的字节数
		参数3	通过 完成端口绑定  参数3传递进来的
		参数4	重叠结构
		参数5	等待时间				INFINITE 一直等
		返回值	成功返回	TRUE	失败 FALSE
		**********************************/

		//分类处理
		if (0 == CompletionKey)			//此处也可以通过重叠结构判断
		{
			//accept
			//将socket 绑定到 完成端口上
			hPort = CreateIoCompletionPort((HANDLE)g_socket[g_count], hPort, g_count, 0);
			if (NULL == hPort)
			{
				int a = WSAGetLastError();
				printf("%d\n", a);
				//出错了  删除这个客户端
				closesocket(g_socket[g_count]);
				WSACloseEvent(g_overlappde[g_count].hEvent);
				continue;
			}
			//新客户端投递 recv 和send
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
				//客户端退出
				printf("client close\n");
				closesocket(g_socket[CompletionKey]);
				WSACloseEvent(g_overlappde[CompletionKey].hEvent);
				//g_count--;
				//g_socket[CompletionKey] = g_socket[g_count];
				//g_overlappde[CompletionKey].hEvent = g_overlappde[CompletionKey].hEvent;
				//由于 绑定下标 是指定的  所以不能通过这种方式 删除客户端
				g_socket[CompletionKey] = NULL;
				g_overlappde[CompletionKey].hEvent = NULL;
				continue;
			}
			if (0 != dword)
			{
				//接收或发送
				if (0 != strRecv[0] && 0 != strRecv[1])
				{
					//接收
					printf("%s\n", strRecv);
					memset(strRecv, 0, sizeof(strRecv));
					postRecv(CompletionKey);
				}
				else
				{
					//发送
				}
			}
		}
	}
	return 0;
}

/***************************
在完成端口中  不管是 立即执行 还是 延迟完成  都会有  完成通知
accept  recv和send 可以不分 立即 还是延迟了
***************************/