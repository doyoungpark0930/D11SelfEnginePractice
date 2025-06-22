/*
//서버관련
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <atomic>

#define MAXLINE 512
#define PORTNUM 5000

using namespace std;

// 윈도우 메시지를 처리할 콜백 함수의 선언 (프로토타입). 예를들어 사용자와의 상호작용
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

// 프로그램의 진입점 함수 (유니코드 버전). crt가 main을 부르고 main이 wWinMain을 부름
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// handle은 운영체제 내부의 ID값, 포인터 같지만 포인터아님. 운영체제가 내부적으로 관리하는 윈도우 객체 키 값
	// hInstance는 이 윈도우는 어떤 실행 중인 애플리케이션에 속하는가?를 알기 위한 식별자.
	// handle은 window창을 가리키는 식별자. 둘다 핸들이긴함

	StateInfo* pState = new (std::nothrow) StateInfo; //구조체 동적 생성

	if (pState == NULL)
		return 0;

	//운영체제 내부에서 사용되는 구조체. c++ 클래스와다름.
	const wchar_t CLASS_NAME[] = L"DoYeong's Window Class";

	// WNDCLASS 구조체: 창의 기본 정보를 담는 구조체이자 윈도우 클래스의 템플릿 같은 개념
	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;     // 메시지 처리 함수 (윈도우가 메시지를 받을 때 호출)
	wc.hInstance = hInstance;        // 현재 인스턴스 핸들
	wc.lpszClassName = CLASS_NAME;   // 창 클래스 이름

	// 창 클래스를 운영체제에 등록
	RegisterClass(&wc);

	//윈도우 구조체와 창 생성은 CLASS_NAME로 연결되어있음

	// 창 생성. 윈도우 클래스 인스턴스 (윈도우와 인스턴스는 1대1대응 되어야함)
	// 이 함수를 호출함으로써, WM_Create message를 winproc가 받음. 윈도우 생산뒤에, 하지만, 아직 윈도우가 보이기 전에
	HWND hwnd = CreateWindowEx(
		0,                              // 확장 스타일 (사용 안함)
		CLASS_NAME,                     // 등록된 창 클래스 이름
		L"DoYeong's Engine",    // 창 제목
		WS_OVERLAPPEDWINDOW,            // 일반적인 윈도우 스타일 (제목표시줄 + 최소/최대 버튼)

		// 위치 및 크기 설정 (CW_USEDEFAULT는 OS가 적당히 지정함)
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // 부모 창 핸들 (없음)
		NULL,       // 메뉴 핸들 (없음)
		hInstance,  // 인스턴스 핸들
		pState        // 여기서 구조체 포인터 전달
	);

	// 창 생성 실패 시 종료
	if (hwnd == NULL)
	{
		return 0;
	}

	CreateConsole(); // GUI기반(windows모드)은 콘솔 생성 코드 필요
	std::cout << "Hi I'm Doyeong's engine. Console was created" << std::endl;

	// 창을 화면에 표시
	ShowWindow(hwnd, nCmdShow);

	// 메시지 루프: 운영체제로부터 메시지를 받아 처리. 매 프레임마다가 아니고 운영체제에서 메시지가 도착할 때까지 기다림
	// 게임 렌더링 만든다면 메시지 루프안에 프레임 루프 넣어야함
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);   // 키보드 입력 등 추가 변환
		DispatchMessage(&msg);    // WindowProc으로 메시지 전달
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
	case WM_DESTROY:  // 창이 닫힐 때, 윈도우 파괴
		PostQuitMessage(0);  // 메시지 루프 종료 요청. GetMessage는 0을 반환
		return 0;
	case WM_SIZE:
	{
		int width = LOWORD(lParam);  // 하위 16비트에서 너비 추출
		int height = HIWORD(lParam); // 상위 16비트에서 높이 추출
		return 0;
	}
	case WM_PAINT:    // 화면을 다시 그려야 할 때. 이거 없으면 화면 깨짐
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		if (!g_isProcessing)
		{
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			TextOut(hdc, 10, 30, _T("클라이언트 연결 기다리는 중.."), 17);

		}
		else
		{
			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
			TextOut(hdc, 10, 30, _T("연결됨..."), 6);
		}

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_KEYDOWN: {
		if (wParam == 'A') {
			// 작업 시작
			std::thread(DoWorkAndRepaint, hwnd).detach(); //다른 스레드에서 받기
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


	// 처리되지 않은 메시지는 기본 처리기로 전달. 하지만 처리한 메시지를 여기서 또 처리하면 안됨
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


//https://learn.microsoft.com/ko-kr/windows/console/creation-of-a-console
void CreateConsole()
{
	AllocConsole();  // 콘솔 창 생성, 아직 입출력은 사용 못함

	//입출력 사용 하도록 콘솔 입출력 버퍼와 stdout같은 스트림을 콘솔 장치와 연결
	FILE* fp_out;
	FILE* fp_in;

	//"기존의 stdout 스트림을 콘솔 출력 장치(CONOUT$)와 연결하고, 그 포인터를 fp_out에 저장해라"
	freopen_s(&fp_out, "CONOUT$", "w", stdout);   // std::cout
	freopen_s(&fp_out, "CONOUT$", "w", stderr);   // std::cerr
	freopen_s(&fp_in, "CONIN$", "r", stdin);      // std::cin

	std::ios::sync_with_stdio(); // C++ 스트림과 C 스트림 동기화
}


void err_quit(const char* msg)
{
	LPVOID IpMsgBuf;
	FormatMessage( //해당오류에 대해 시스템에서 제공하는 상세 오류 메시지를 가져옴
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
	sockaddr.sin_port = htons(PORTNUM); //서버 포트번호 지정
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY); //모든 네트워크 인터페이스에서 수신
	//inet_addr("172.30.1.56");

	if (::bind(sockfd, (SOCKADDR*)&sockaddr, sizeof(sockaddr)) == SOCKET_ERROR) err_quit("Bind"); //로컬주소를 소켓과 연결
	if (::listen(sockfd, 5) == SOCKET_ERROR) err_quit("Listen");//서버 소켓을 "수신 대기 상태"로 전환

	addrlen = sizeof(clientaddr);
	clientfd = ::accept(sockfd, (SOCKADDR*)&clientaddr, &addrlen); //클라이언트가 서버에 연결할 때까지 대기
	//연결 요청이 오면 이를 수락하고 새로 열린 클라이언트 소켓을 반환

	printf("클라이언트와 연결됨\n");
	if (clientfd == INVALID_SOCKET)
	{
		err_quit("Accept");
		closesocket(sockfd);
		WSACleanup();
		return 0;
	}
	//inet_ntoa 함수는 (Ipv4) 인터넷 네트워크 주소를 인터넷 표준 점선 소수점 형식의 ASCII 문자열로 변환합니다
	//ntohs는 네트워크 바이트에서 호스트 바이트로
	printf("[TCP서버] 클라이언트 접속 IP : %s %d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	while (1)
	{
		//데이터받기(고정길이)
		int retVal = recv(clientfd, (char*)&len, sizeof(int), 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//데이터받기(가변길이)
		retVal = recv(clientfd, buf, len, 0);
		if (retVal == SOCKET_ERROR)
		{
			err_quit("Recv");
			break;
		}
		else if (retVal == 0) break;

		//받은 데이터 출력
		buf[retVal] = '\0';
		printf("[TCP/%s : %d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
	}

	closesocket(sockfd);
	printf("소켓 통신 종료");
	WSACleanup();
	return 0;
}

void DoWorkAndRepaint(HWND hwnd)
{
	openServer();
	g_isProcessing = true;

	// 다시 WM_PAINT 요청
	InvalidateRect(hwnd, nullptr, false);
}

inline StateInfo* GetAppState(HWND hwnd)
{
	///SetWindowLongPtr를 한번 하면, GetWindowLongPtr으로 pointer를 얻어낼수잇음
	return reinterpret_cast<StateInfo*>(
		GetWindowLongPtr(hwnd, GWLP_USERDATA)
		);
}*/