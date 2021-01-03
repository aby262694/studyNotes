
#include<cstdio>
#include<cstdlib>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<MSWSock.h>		//acceptex

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

#define MAX_COUNT 1024

SOCKET g_socket[MAX_COUNT];  //这里数量没要求
OVERLAPPED g_allOverLapped[MAX_COUNT];
int g_count;
//接受数组
char strRecv[1024];
char strSend[1024] = "join success\n";
//清理数组函数
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


	//打开网络库
	WORD wVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	int nRes = WSAStartup(wVersion, &wsaData);
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
	if (2 != HIBYTE(wsaData.wVersion) || 2 != LOBYTE(wsaData.wVersion))
	{
		WSACleanup();
		return -1;
	}

	//创建socket
	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return -1;
	}
	/**********************
	前三个参数与socket 一样		返回值与socket 一样
	参数4 设置详细的套接字属性   如 发送数据是否需要连接 是否需要完整送达  套接字权限等   不使用填null
	参数5 一组socket的组ID   保留   暂时 没作用 填 null
	参数6 指定套接字属性WSA_FLAG_OVERLAPPED  用于重叠IO的宏

			WSA_FLAG_MULTIPOINT_C_ROOT
			WSA_FLAG_MULTIPOINT_C_LEAF 
			WSA_FLAG_MULTIPOINT_D_ROOT
			WSA_FLAG_MULTIPOINT_D_LEAF		用于多播协议

			WSA_FLAG_ACCESS_SYSTEM_SECURITY	用于网络权限  需要配合参数4 使用

			WSA_FLAG_NO_HANDLE_INHERIT		套接字不可被继承
	**********************/

	//绑定 端口和地址
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
	//监听
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		int a = WSAGetLastError();
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	//放数组里
	g_socket[g_count] = socketServer;
	g_allOverLapped[g_count].hEvent = WSACreateEvent();
	g_count++;

	//accept的改良版AcceptEx		由于调用复杂封装成一个函数
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
			//有信号了
			DWORD transfer;
			DWORD flag;
			BOOL getResult = WSAGetOverlappedResult(g_socket[i], &g_allOverLapped[i], &transfer, TRUE, &flag);
			if (FALSE == getResult)
			{
				//失败			//直接强制关闭客户端也走这里
				int a = WSAGetLastError();
				if (10054 == a)
				{
					printf("client close\n");
					//关闭socket和事件
					closesocket(g_socket[i]);
					WSACloseEvent(g_allOverLapped[i].hEvent);
					//数组中删除
					g_count--;
					g_socket[i] = g_socket[g_count];
					g_allOverLapped[i].hEvent = g_allOverLapped[g_count].hEvent;
					//为了循环不会跳过最后拿过来的这个socket i需要减1
					i--;
				}
				continue;
			}
			/************************************
			参数1	对应socket			参数2 重叠结构地址
			参数3	传入传出数据大小		参数4 事件通知填true
			参数5	传输方式
			返回值
			************************************/
			
			//信号重置为空
			WSAResetEvent(g_allOverLapped[i].hEvent);

			//成功
			if (0 == i)
			{//接受连接完成
				printf("client connet\n");
				//对新客户端投递WSARecv
				int pR = postRecv(g_count);
				if ( 0 != pR )
				{
					//出错 
					printf("错误码：%d\n", pR);
				}
				postSend(g_count);

				g_count++;
				postAccept();
			}
			else if (0 == transfer)
			{//客户端下线
				printf("client close\n");
				//关闭socket和事件
				closesocket(g_socket[i]);
				WSACloseEvent(g_allOverLapped[i].hEvent);
				//数组中删除
				g_count--;
				g_socket[i] = g_socket[g_count];
				g_allOverLapped[i].hEvent = g_allOverLapped[g_count].hEvent;
				//为了循环不会跳过最后拿过来的这个socket i需要减1
				i--;
			}
			else if (0 != transfer)
			{
				//发送或接收成功
				if (0 != strRecv[0])
				{//接受消息
					//接受到消息
					//输出一下
					printf("%s\n", strRecv);
					//情况数组
					memset(strRecv, 0, sizeof(strRecv));
					//再次投递
					postRecv(i);
				}
				else
				{//发送消息 send
			
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
	//接受后再自加比较好g_count++;   接受失败就不用自加了
	char str[1024] = { 0 };
	BOOL nResAccept = AcceptEx(g_socket[0], g_socket[g_count], str, 0, sizeof(sockaddr_in) + 16, 
		sizeof(sockaddr_in) + 16, NULL, &g_allOverLapped[0]);
	if (TRUE == nResAccept)
	{
		//立即完成了
		//投递recv异步处理		最后需要再次投递acceptex
		postRecv(g_count);
		//postSend(g_count);
		//根据情况投递send
		//客户端数量++  g_count++
		g_count++;
		printf("client connet\n");
		//再次投递acceptex
		postAccept();		//调用自己（递归）
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//延迟处理

			return 0;
		}
		else
		{
			//出错了
			return a;
		}
	}
	/***************************
	功能：投递服务器socket 异步接受链接
		客户端发来的第一组数据有acceptex接受  后面的才由recv接受 
	参数1	服务器    参数2 新的客户端	   参数3		一个字符数组指针 不能设置成空
	参数4    参数3所填数组的大小	 可以填0，表示参数3作用失效		即使无效 参数3也必须正常填写 不能填NULL
	参数5	本地地址信息保留字节数+16个字节
	参数6	远程地址信息保留字节数+16个字节
	参数7	配合参数3，4使用的， 若立刻接受到消息  通过该参数得到接受到的字节个数（定义一个变量填地址）不用填NULL
	参数8	重叠结构		&g_allOverLapped[0]		这里是服务器的重叠结构
	返回值	TRUE 立即完成返回  刚执行就有客户端连接了
			FALSE	出错 int a = WSAGetLastError();
						a == ERROR_IO_PENDING	异步等待		等待客户端连接
						其他根据错误码解决
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

	/********************************
	参数1  客户端socket	参数2 接受到信息后的存储位置	结构体WSABUF 结构体包括字符数组的指针和大小
	参数3  有几个结构体	参数4  传递成功接受字节数的个数
	参数5	指定读取方式		参数6	重叠结构
	参数7	回调函数  不用填NULL		完成例程中使用
	返回值   0立即完成
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
		//立即完成

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