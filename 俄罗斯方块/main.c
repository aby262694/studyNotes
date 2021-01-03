#include"common.h"




//主函数
//  WINAPI  调用约定 主要跟 数据进入栈区有关
//参数1 当前窗口的句柄  本质是一个数  窗口的唯一标识， 相当于窗口的ID
//参数2 前一个句柄 这个参数一直是NULL  现在来说没什么用了
//参数3 命令行参数
//参数4 指定窗口显示方式  隐藏 最大最小显示 
//   这些参数是操作系统传递的 
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPreInstance, LPTSTR lpCmdLine, int nShowcmd)//一般用原来的名字就像，不容易乱
{
	//初始化窗口类
	WNDCLASSEX  w;
	w.cbClsExtra = 0;                  //紧跟在窗口类尾部的一块额外空间，不用的话设为0
	w.cbSize = sizeof(WNDCLASSEX);
	w.cbWndExtra = 0;                  //以后对话框可能会涉及
	w.hbrBackground = (HBRUSH)COLOR_WINDOW;     //背景
	w.hCursor = LoadCursor(NULL,IDC_HAND);            //光标  使用系统类型第一个参数要用NULL  
										//使用加载的资源 第一个参数填实例句柄hinstance 第二个参数填资源号需要类型转换
	w.hIcon = LoadIcon(NULL,IDI_ASTERISK);             //状态栏图标
	w.hIconSm = NULL;            //左上角小图标
	w.hInstance = hinstance;     //窗口句柄
	w.lpfnWndProc = Pels;            //回调函数地址  
	w.lpszClassName = "elsfk";       //窗口类的名字   窗口 唯一的标识  名字不能重复
	w.lpszMenuName = NULL;        //菜单
	w.style = CS_HREDRAW | CS_VREDRAW; //风格  这两个宏代表垂直刷新和水平刷新
	//一共12个成员

	//注册窗口 对象
	if (0 == RegisterClassEx(&w))
	{
		int a = GetLastError();  //得到错误码提示
		return 0;                   //注册失败走这步
	}
	//创建窗口
	HWND hwnd;
	hwnd = CreateWindowEx(WS_MINIMIZEBOX, "elsfk", "俄罗斯方块",WS_OVERLAPPEDWINDOW, 200, 100, 500, 650, NULL, NULL, hinstance, NULL);
	if (NULL == hwnd)    //窗口句柄
	{
		return 0;
	}

	//显示窗口
	ShowWindow(hwnd, SW_SHOWNORMAL);  //隐藏显示返回0   正常非0
	//重绘窗口
	UpdateWindow(hwnd);

	//消息循环
	MSG msg;
	while (GetMessage(&msg,NULL, 0, 0))//参数1 指向MSG结构体的指针   参数2 窗口的句柄 NULL全部接收
	{
		//翻译消息，将电信号转换为字符消息
		TranslateMessage(&msg); 
		//分发消息   包括 标准消息，命令消息 ，通知消息，自定义消息
		DispatchMessage(&msg);   
	}
	

	return 0;
}
//回调函数
	LRESULT CALLBACK Pels(HWND hwnd, UINT nMsg, WPARAM wparam, LPARAM lparam)//1句柄   2消息的ID  3,4消息类型
	{
		PAINTSTRUCT paint;
		HDC hdc;
		switch (nMsg)
		{
		case WM_CREATE:           //窗口创建初期只产生一次
			messageCreate();
			break;
		case WM_PAINT:			//在拉动窗口或窗口被覆盖后重新放出来产生的消息  也是重绘的过程
			hdc = BeginPaint(hwnd,&paint);  //返回一个窗口可操作区域的标识
			//不同的项目 代码从这里就开始不一样了
			//这里不同的消息可以用不同的函数  来提高代码整洁性

			messagePaint(hdc);
			
			EndPaint(hwnd,&paint);
			break;

		case WM_KEYDOWN: //按键消息
			messageKey( hwnd, wparam);
			break;

		case WM_TIMER:			//settimer产生
			messageTimer(hwnd);
			break;

		case WM_CLOSE:		//关闭时系统产生
			
			break;
		case WM_DESTROY:
			PostQuitMessage(0);//传递退出消息    退出消息有三个 WM_CLOSE ，WM_DESTROY，WM_QUIT 依次产生
			break;
		}
		return DefWindowProc(hwnd, nMsg, wparam, lparam); //对窗口的操作消息 会通过回调函数来进行操作
	}