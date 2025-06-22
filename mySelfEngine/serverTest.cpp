/*
//��������
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <atomic>

#define MAXLINE 512
#define PORTNUM 5000

using namespace std;

// ������ �޽����� ó���� �ݹ� �Լ��� ���� (������Ÿ��). ������� ����ڿ��� ��ȣ�ۿ�
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct StateInfo {
	// ... (struct members not shown)
};

int openServer();
void err_quit(const char* msg);
void CreateConsole();
void DoWorkAndRepaint(HWND hwnd);
inline StateInfo* GetAppState(HWND hwnd);

std::atomic<bool> g_isProcessing = false;

// ���α׷��� ������ �Լ� (�����ڵ� ����). crt�� main�� �θ��� main�� wWinMain�� �θ�
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// handle�� �ü�� ������ ID��, ������ ������ �����;ƴ�. �ü���� ���������� �����ϴ� ������ ��ü Ű ��
	// hInstance�� �� ������� � ���� ���� ���ø����̼ǿ� ���ϴ°�?�� �˱� ���� �ĺ���.
	// handle�� windowâ�� ����Ű�� �ĺ���. �Ѵ� �ڵ��̱���

	StateInfo* pState = new (std::nothrow) StateInfo; //����ü ���� ����

	if (pState == NULL)
		return 0;

	//�ü�� ���ο��� ���Ǵ� ����ü. c++ Ŭ�����ʹٸ�.
	const wchar_t CLASS_NAME[] = L"DoYeong's Window Class";

	// WNDCLASS ����ü: â�� �⺻ ������ ��� ����ü���� ������ Ŭ������ ���ø� ���� ����
	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;     // �޽��� ó�� �Լ� (�����찡 �޽����� ���� �� ȣ��)
	wc.hInstance = hInstance;        // ���� �ν��Ͻ� �ڵ�
	wc.lpszClassName = CLASS_NAME;   // â Ŭ���� �̸�

	// â Ŭ������ �ü���� ���
	RegisterClass(&wc);

	//������ ����ü�� â ������ CLASS_NAME�� ����Ǿ�����

	// â ����. ������ Ŭ���� �ν��Ͻ� (������� �ν��Ͻ��� 1��1���� �Ǿ����)
	// �� �Լ��� ȣ�������ν�, WM_Create message�� winproc�� ����. ������ ����ڿ�, ������, ���� �����찡 ���̱� ����
	HWND hwnd = CreateWindowEx(
		0,                              // Ȯ�� ��Ÿ�� (��� ����)
		CLASS_NAME,                     // ��ϵ� â Ŭ���� �̸�
		L"DoYeong's Engine",    // â ����
		WS_OVERLAPPEDWINDOW,            // �Ϲ����� ������ ��Ÿ�� (����ǥ���� + �ּ�/�ִ� ��ư)

		// ��ġ �� ũ�� ���� (CW_USEDEFAULT�� OS�� ������ ������)
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // �θ� â �ڵ� (����)
		NULL,       // �޴� �ڵ� (����)
		hInstance,  // �ν��Ͻ� �ڵ�
		pState        // ���⼭ ����ü ������ ����
	);

	// â ���� ���� �� ����
	if (hwnd == NULL)
	{
		return 0;
	}

	CreateConsole(); // GUI���(windows���)�� �ܼ� ���� �ڵ� �ʿ�
	std::cout << "Hi I'm Doyeong's engine. Console was created" << std::endl;

	// â�� ȭ�鿡 ǥ��
	ShowWindow(hwnd, nCmdShow);

	// �޽��� ����: �ü���κ��� �޽����� �޾� ó��. �� �����Ӹ��ٰ� �ƴϰ� �ü������ �޽����� ������ ������ ��ٸ�
	// ���� ������ ����ٸ� �޽��� �����ȿ� ������ ���� �־����
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);   // Ű���� �Է� �� �߰� ��ȯ
		DispatchMessage(&msg);    // WindowProc���� �޽��� ����
	}

	return 0;
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	StateInfo* pState;

	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pState = reinterpret_cast<StateInfo*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
	}
	else
	{
		pState = GetAppState(hwnd);
	}


	switch (uMsg)
	{
	case WM_DESTROY:  // â�� ���� ��, ������ �ı�
		PostQuitMessage(0);  // �޽��� ���� ���� ��û. GetMessage�� 0�� ��ȯ
		return 0;
	case WM_SIZE:
	{
		int width = LOWORD(lParam);  // ���� 16��Ʈ���� �ʺ� ����
		int height = HIWORD(lParam); // ���� 16��Ʈ���� ���� ����
		return 0;
	}
	case WM_PAINT:    // ȭ���� �ٽ� �׷��� �� ��. �̰� ������ ȭ�� ����
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		if (!g_isProcessing)
		{
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			TextOut(hdc, 10, 30, _T("Ŭ���̾�Ʈ ���� ��ٸ��� ��.."), 17);

		}
		else
		{
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			TextOut(hdc, 10, 30, _T("�����..."), 6);
		}

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_KEYDOWN: {
		if (wParam == 'A') {
			// �۾� ����
			std::thread(DoWorkAndRepaint, hwnd).detach(); //�ٸ� �����忡�� �ޱ�
			//DoWorkAndRepaint(hwnd);
		}
		return 0;
	}

	case WM_CLOSE: {
		if (MessageBox(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
		{
			DestroyWindow(hwnd);
		}
		// Else: User canceled. Do nothing.
		return 0;
	}

	}


	// ó������ ���� �޽����� �⺻ ó����� ����. ������ ó���� �޽����� ���⼭ �� ó���ϸ� �ȵ�
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


//https://learn.microsoft.com/ko-kr/windows/console/creation-of-a-console
void CreateConsole()
{
	AllocConsole();  // �ܼ� â ����, ���� ������� ��� ����

	//����� ��� �ϵ��� �ܼ� ����� ���ۿ� stdout���� ��Ʈ���� �ܼ� ��ġ�� ����
	FILE* fp_out;
	FILE* fp_in;

	//"������ stdout ��Ʈ���� �ܼ� ��� ��ġ(CONOUT$)�� �����ϰ�, �� �����͸� fp_out�� �����ض�"
	freopen_s(&fp_out, "CONOUT$", "w", stdout);   // std::cout
	freopen_s(&fp_out, "CONOUT$", "w", stderr);   // std::cerr
	freopen_s(&fp_in, "CONIN$", "r", stdin);      // std::cin

	std::ios::sync_with_stdio(); // C++ ��Ʈ���� C ��Ʈ�� ����ȭ
}


void err_quit(const char* msg)
{
	LPVOID IpMsgBuf;
	FormatMessage( //�ش������ ���� �ý��ۿ��� �����ϴ� �� ���� �޽����� ������
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&IpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)IpMsgBuf);
	LocalFree(IpMsgBuf);
}
int openServer()
{
	WSADATA wsaData;
	SOCKET sockfd, clientfd;
	int addrlen;
	char buf[MAXLINE + 1];
	int len = 0;
	struct sockaddr_in sockaddr, clientaddr;
	printf("server open\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("dlload error\n");
		return 1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == INVALID_SOCKET) {
		printf("socket create error\n");
		return 1;
	}

	ZeroMemory(&sockaddr, sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(PORTNUM); //���� ��Ʈ��ȣ ����
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //��� ��Ʈ��ũ �������̽����� ����
	//inet_addr("172.30.1.56");

	if (::bind(sockfd, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) err_quit("Bind"); //�����ּҸ� ���ϰ� ����
	if (::listen(sockfd, 5) == SOCKET_ERROR) err_quit("Listen");//���� ������ "���� ��� ����"�� ��ȯ

	addrlen = sizeof(clientaddr);
	clientfd = ::accept(sockfd, (SOCKADDR*)&clientaddr, &addrlen); //Ŭ���̾�Ʈ�� ������ ������ ������ ���
	//���� ��û�� ���� �̸� �����ϰ� ���� ���� Ŭ���̾�Ʈ ������ ��ȯ

	printf("Ŭ���̾�Ʈ�� �����\n");
	if (clientfd == INVALID_SOCKET)
	{
		err_quit("Accept");
		closesocket(sockfd);
		WSACleanup();
		return 0;
	}
	//inet_ntoa �Լ��� (Ipv4) ���ͳ� ��Ʈ��ũ �ּҸ� ���ͳ� ǥ�� ���� �Ҽ��� ������ ASCII ���ڿ��� ��ȯ�մϴ�
	//ntohs�� ��Ʈ��ũ ����Ʈ���� ȣ��Ʈ ����Ʈ��
	printf("[TCP����] Ŭ���̾�Ʈ ���� IP : %s %d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	while (1)
	{
		//�����͹ޱ�(��������)
		int retVal = recv(clientfd, (char*)&len, sizeof(int), 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//�����͹ޱ�(��������)
		retVal = recv(clientfd, buf, len, 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//���� ������ ���
		buf[retVal] = '\0';
		printf("[TCP/%s : %d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
	}

	closesocket(sockfd);
	printf("���� ��� ����");
	WSACleanup();
	return 0;
}

void DoWorkAndRepaint(HWND hwnd)
{
	openServer();
	g_isProcessing = true;

	// �ٽ� WM_PAINT ��û
	InvalidateRect(hwnd, nullptr, false);
}

inline StateInfo* GetAppState(HWND hwnd)
{
	///SetWindowLongPtr�� �ѹ� �ϸ�, GetWindowLongPtr���� pointer�� ��������
	return reinterpret_cast<StateInfo*>(
		GetWindowLongPtr(hwnd, GWLP_USERDATA)
		);
}*/