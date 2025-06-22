#pragma once
#include <WindowsX.h>
#include "basewin.h"
#include "Renderer.h"

//https://github.com/microsoft/DirectXTK/wiki/WICTextureLoader
#include <wrl/wrappers/corewrappers.h>
#pragma comment(lib, "runtimeobject.lib")



class MainWindow : public BaseWindow<MainWindow>
{
public :
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;
	IDXGISwapChain* m_SwapChain;
	ID3D11Texture2D* pBackBuffer;
	ID3D11RenderTargetView* m_renderTargetView;

	int m_width = 1920;
	int m_height = 1050;
	std::unique_ptr<Renderer> m_Renderer;

private:
	void CreateDevice();
	void DiscardGraphicsResources();
	void SetRtvAndBackBuffer();
	void Initialize();

public:
	PCWSTR ClassName() const override { return L"DoYeong's Engine"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void Update();
	void Render();

};

