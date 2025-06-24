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
	CreateConsole(); // GUI���(windows���)�� �ܼ� ���� �ڵ� �ʿ�


	MainWindow win;
	win.m_Renderer = std::make_unique<Renderer>(win.m_hwnd, win.m_Device, win.m_Context, win.m_renderTargetView);


	if (!win.Create(L"DoYeong's Engine", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, //ũ�� ���� ����
		0, 200, 50, win.m_width, win.m_height))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	int cnt = 0;
	// �޽��� ���� ����
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