#include"common.h"




//������
//  WINAPI  ����Լ�� ��Ҫ�� ���ݽ���ջ���й�
//����1 ��ǰ���ڵľ��  ������һ����  ���ڵ�Ψһ��ʶ�� �൱�ڴ��ڵ�ID
//����2 ǰһ����� �������һֱ��NULL  ������˵ûʲô����
//����3 �����в���
//����4 ָ��������ʾ��ʽ  ���� �����С��ʾ 
//   ��Щ�����ǲ���ϵͳ���ݵ� 
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPreInstance, LPTSTR lpCmdLine, int nShowcmd)//һ����ԭ�������־��񣬲�������
{
	//��ʼ��������
	WNDCLASSEX  w;
	w.cbClsExtra = 0;                  //�����ڴ�����β����һ�����ռ䣬���õĻ���Ϊ0
	w.cbSize = sizeof(WNDCLASSEX);
	w.cbWndExtra = 0;                  //�Ժ�Ի�����ܻ��漰
	w.hbrBackground = (HBRUSH)COLOR_WINDOW;     //����
	w.hCursor = LoadCursor(NULL,IDC_HAND);            //���  ʹ��ϵͳ���͵�һ������Ҫ��NULL  
										//ʹ�ü��ص���Դ ��һ��������ʵ�����hinstance �ڶ�����������Դ����Ҫ����ת��
	w.hIcon = LoadIcon(NULL,IDI_ASTERISK);             //״̬��ͼ��
	w.hIconSm = NULL;            //���Ͻ�Сͼ��
	w.hInstance = hinstance;     //���ھ��
	w.lpfnWndProc = Pels;            //�ص�������ַ  
	w.lpszClassName = "elsfk";       //�����������   ���� Ψһ�ı�ʶ  ���ֲ����ظ�
	w.lpszMenuName = NULL;        //�˵�
	w.style = CS_HREDRAW | CS_VREDRAW; //���  �����������ֱˢ�º�ˮƽˢ��
	//һ��12����Ա

	//ע�ᴰ�� ����
	if (0 == RegisterClassEx(&w))
	{
		int a = GetLastError();  //�õ���������ʾ
		return 0;                   //ע��ʧ�����ⲽ
	}
	//��������
	HWND hwnd;
	hwnd = CreateWindowEx(WS_MINIMIZEBOX, "elsfk", "����˹����",WS_OVERLAPPEDWINDOW, 200, 100, 500, 650, NULL, NULL, hinstance, NULL);
	if (NULL == hwnd)    //���ھ��
	{
		return 0;
	}

	//��ʾ����
	ShowWindow(hwnd, SW_SHOWNORMAL);  //������ʾ����0   ������0
	//�ػ洰��
	UpdateWindow(hwnd);

	//��Ϣѭ��
	MSG msg;
	while (GetMessage(&msg,NULL, 0, 0))//����1 ָ��MSG�ṹ���ָ��   ����2 ���ڵľ�� NULLȫ������
	{
		//������Ϣ�������ź�ת��Ϊ�ַ���Ϣ
		TranslateMessage(&msg); 
		//�ַ���Ϣ   ���� ��׼��Ϣ��������Ϣ ��֪ͨ��Ϣ���Զ�����Ϣ
		DispatchMessage(&msg);   
	}
	

	return 0;
}
//�ص�����
	LRESULT CALLBACK Pels(HWND hwnd, UINT nMsg, WPARAM wparam, LPARAM lparam)//1���   2��Ϣ��ID  3,4��Ϣ����
	{
		PAINTSTRUCT paint;
		HDC hdc;
		switch (nMsg)
		{
		case WM_CREATE:           //���ڴ�������ֻ����һ��
			messageCreate();
			break;
		case WM_PAINT:			//���������ڻ򴰿ڱ����Ǻ����·ų�����������Ϣ  Ҳ���ػ�Ĺ���
			hdc = BeginPaint(hwnd,&paint);  //����һ�����ڿɲ�������ı�ʶ
			//��ͬ����Ŀ ���������Ϳ�ʼ��һ����
			//���ﲻͬ����Ϣ�����ò�ͬ�ĺ���  ����ߴ���������

			messagePaint(hdc);
			
			EndPaint(hwnd,&paint);
			break;

		case WM_KEYDOWN: //������Ϣ
			messageKey( hwnd, wparam);
			break;

		case WM_TIMER:			//settimer����
			messageTimer(hwnd);
			break;

		case WM_CLOSE:		//�ر�ʱϵͳ����
			
			break;
		case WM_DESTROY:
			PostQuitMessage(0);//�����˳���Ϣ    �˳���Ϣ������ WM_CLOSE ��WM_DESTROY��WM_QUIT ���β���
			break;
		}
		return DefWindowProc(hwnd, nMsg, wparam, lparam); //�Դ��ڵĲ�����Ϣ ��ͨ���ص����������в���
	}