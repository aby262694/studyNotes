
#include<cstdio>
#include<cstdlib>
#include<winsock2.h>
#include <ws2tcpip.h> //inet_pton

#pragma comment(lib,"ws2_32.lib")

struct numberList
{
	unsigned short count;
	SOCKET socketarray[WSA_MAXIMUM_WAIT_EVENTS];  // 填宏或者64都行  默认每次处理64个 
	WSAEVENT eventarray[64];
}interfaceNumber;

BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		//释放事件对象
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
	//设置请求监管窗口信息
	SetConsoleCtrlHandler(fun, TRUE);

	//打开网络库
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA WSAData;
	int startRes = WSAStartup(wVersionRequested, &WSAData);
	if (0 != startRes)
	{//失败了这里应该  关闭网络库
		WSACleanup();
		switch (startRes)
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
	if ( 2 != HIBYTE(WSAData.wVersion) || 2 != LOBYTE(WSAData.wVersion))
	{
		WSACleanup();
		return 0;
	}
	
	//创建socket
	SOCKET socketSever = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketSever)
	{
		int a = WSAGetLastError();
		WSACleanup();
		return 0;
	}

	//绑定
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

	//监听
	int liRes = listen(socketSever, SOMAXCONN);
	if (SOCKET_ERROR == liRes)
	{
		int a = WSAGetLastError();
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}

	//创建事件对象
	WSAEVENT eventSever = WSACreateEvent();
	if (WSA_INVALID_EVENT == eventSever)
	{
		int a = WSAGetLastError();
		closesocket(socketSever);
		WSACleanup();
		return 0;
	}
	/*************************
	相关函数
	WSACloseEvent(eventSever); 释放事件对象函数
	WSAResetEvent(); 将有信号的 事件对象重置为没信号的
	WSASetEvent();   将无信号的 事件 置成有信号的（不能决定是哪种信号）
	*************************/
	
	//将服务器socket 和 事件 装入结构体中
	interfaceNumber.socketarray[interfaceNumber.count] = socketSever;
	interfaceNumber.eventarray[interfaceNumber.count] = eventSever;
	interfaceNumber.count++;

	//绑定并投递给系统
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
	功能 给事件绑定上socket与操作码，并投递给操作系统
	参数1 被绑定的socket   参数2 被绑定的事件对象
	参数3 具体事件操作			多个操作用 | 连接			尽量绑定一个信号  不然也不知道具体到底是哪个操作信号响应
		FD_ACCEPT   有客户端连接   用于绑定 服务器socket
		FD_READ		有客户端发来消息	与客户端socket绑定
		FD_CLOSE	客户端下线	用于绑定 客户端socket
		FD_WRITE	可以给客户端发消息		与客户端绑定				只有在客户端连接时产生一次消息
		FD_CONNECT		在客户端代码中 可以用这个绑定服务器
		0		取消事件监视
	FD_QOS套接字服务质量状态发生变化通知
	得到服务质量信息		WSAloctl();		这个函数可以监测网络质量的变化

	返回值  成功返回0    失败返回socket_error
	*************************/

	//循环
	while (1)
	{
		//查看事件是否有信号
		DWORD getEventNumber = WSAWaitForMultipleEvents(interfaceNumber.count, interfaceNumber.eventarray, FALSE, WSA_INFINITE, FALSE);
		if (WSA_WAIT_FAILED == getEventNumber)
		{
			int a = WSAGetLastError();
			printf("错误码：%d\n", a);
			break;
		}
		////参数4设置超时时间时使用
		//if (WSA_WAIT_TIMEOUT == getEventNumber)
		//{
		//	continue;
		//}
		DWORD nIndex = getEventNumber - WSA_WAIT_EVENT_0;
		/********************
		参数1 有效事件个数			参数2 事件列表
		参数3 事件等待方式		TRUE 等待所有事件都产生信号才返回
			FALSE任何一个事件产生信号立即返回		返回值减去WSA_WAIT_EVENT_0表示事件对象的索引，就是有反应的事件对象的下标
			多个事件发生信号		返回下标最小的
		参数4 超时间隔 与select那个参数一样 单位毫秒  超时无事件响应返回WSA_WAIT_TIMEOUT
			可以填0 不等待		填WSA_INFINITE	等待到事件发生
		参数5	TRUE	重叠IO中使用			FALSE	现在这个模型中使用

		返回值	数组的下标值		参数3为TRUE 时 所有事件均有信号	
								参数3为FALSE时返回值减去WSA_WAIT_EVENT_0 == 数组中事件下标
				参数5为	TRUE时	返回WSA_WAIT_IO_COMPLETION
				超时返回 WSA_WAIT_TIMEOUT
		********************/

		//有信号的分类处理
		WSANETWORKEVENTS NetworkEvents;
		int ENERes = WSAEnumNetworkEvents(interfaceNumber.socketarray[nIndex], interfaceNumber.eventarray[nIndex], &NetworkEvents);
		if (SOCKET_ERROR == ENERes)
		{
			break;
		}
		/*********************
		功能		获取事件类型并将事件上的信号重置
		参数1	对应的socket			参数2	对应的事件
		参数3	是一个结构体指针	触发的事件类型装在这个结构体中	成员1 具体操作事件
							成员2 错误码  FD_ACCEPT事件错误码装在FD_ACCEPT_BIT下标里		没有错误对应就是0
									得到的是  socket出现的错误信息  不是函数运行的错误
		返回值	执行成功返回0		失败返回socket_error
		*********************/

		//消息处理	用if结构		else if 和switch 都逻辑不合理（在一个信号返回两种或多种操作时）
		//判断FD_ACCEPT
		if (NetworkEvents.lNetworkEvents & FD_ACCEPT)
		{
			//判断是客户端正常连接 还是	 客户端出问题了
			if ( 0 == NetworkEvents.iErrorCode[FD_ACCEPT_BIT])
			{//等于0说明没有错误，是正常连接
				SOCKET socketClient = accept(socketSever, NULL, NULL);
				if (INVALID_SOCKET == socketClient)
				{
					continue;
				}
				//创建客户端的事件对象
				WSAEVENT eventClient = WSACreateEvent();
				if (WSA_INVALID_EVENT == eventClient)
				{
					closesocket(socketClient);
					continue;
				}
				//投递给系统
				int CnRes = WSAEventSelect(socketClient, eventClient, FD_READ | FD_CLOSE | FD_WRITE);
				if (SOCKET_ERROR == CnRes)
				{
					int a = WSAGetLastError();
					WSACloseEvent(eventClient);
					closesocket(socketClient);
					continue;
				}
				//将客户端放进结构体数组中
				interfaceNumber.socketarray[interfaceNumber.count] = socketClient;
				interfaceNumber.eventarray[interfaceNumber.count] = eventClient;
				interfaceNumber.count++;

				printf("client accept\n");
			}
			else
			{//客户端自身出的问题不能影响服务器的正常运行		直接进入下一次循环即可
				continue;
			}
		}

		//判断FD_WRITE		这里用if逻辑上更严谨		else if也可以只是在某些特殊情况会有偏差（书上采用的elseif）
		if (NetworkEvents.lNetworkEvents & FD_WRITE) //只在连接时产生一次	可以在此处做数据初始化
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
			{	//出错
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
			{	//出错
				printf("socket error code:%d", NetworkEvents.iErrorCode[FD_READ_BIT]);
				continue;
			}
		}
		if (NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			
			//客户端断开连接，删除这个客户端
			closesocket(interfaceNumber.socketarray[nIndex]);
			WSACloseEvent(interfaceNumber.eventarray[nIndex]);
			//将删除的这个位置放上数组最后的一个数据		并数量-1
			--interfaceNumber.count;
			interfaceNumber.socketarray[nIndex] = interfaceNumber.socketarray[interfaceNumber.count];
			interfaceNumber.eventarray[nIndex] = interfaceNumber.eventarray[interfaceNumber.count];
			printf("this client close\n");
		
		}
	}
	

	//释放事件对象
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
可进行有序优化处理			相对有序
	最好的方式是添加线程与线程池	网络库结合线程 才能成为真正有用的网络模型
	也可以用循环 一个一个的查看信号（WSAWaitForMultipleEvents）可以一定程度上避免连续点击问题

增加事件处理数量
	一组一组来 通过线程 线程池来进行优化最好 每个线程一组64个
	也可以一个一个询问（WSAWaitForMultipleEvents）	就可以超过数量限制了
	一组一组也可以用for循环来做  将事件申请结构体数组 放入多个数组就行了
***************************/