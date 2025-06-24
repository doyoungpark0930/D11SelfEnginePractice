#ifndef UNICODE
#define UNICODE
#endif

#include "basewin.h"
#include "MainWindow.h"

#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

void CreateConsole();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	CreateConsole(); // GUI기반(windows모드)은 콘솔 생성 코드 필요


	MainWindow win;
	win.m_Renderer = std::make_unique<Renderer>(win.m_hwnd, win.m_Device, win.m_Context, win.m_renderTargetView);


	if (!win.Create(L"DoYeong's Engine", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, //크기 조절 막음
		0, 200, 50, win.m_width, win.m_height))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	int cnt = 0;
	// 메시지 루프 실행
	MSG msg = { };
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			win.Update();
			win.Render();
			win.m_SwapChain->Present(1, 0);
		}
	}

	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{		
		Initialize();
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}



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