#include "MainWindow.h"

void MainWindow::CreateDevice()
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = m_width;
	sd.BufferDesc.Height = m_height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	HRESULT hr = S_OK;

	IDXGIAdapter* pAdapter;
	IDXGIFactory* pFactory = NULL;

	// Create a DXGIFactory object.
	if (FAILED(hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
	{
		DebugBreak();
	}

	for (UINT i = 0; pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		DXGI_ADAPTER_DESC desc;
		pAdapter->GetDesc(&desc);

		if (wcsstr(desc.Description, L"RTXs"))
		{
			// NVIDIA RTX 어댑터 발견
			break;
		}

		SafeRelease(&pAdapter);
	}
	SafeRelease(&pFactory);

	D3D_FEATURE_LEVEL FeatureLevels[] = {
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1
	};

	D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = D3D_FEATURE_LEVEL_9_1;
	D3D_DRIVER_TYPE myDriverType = D3D_DRIVER_TYPE_HARDWARE;
	if (pAdapter)
	{
		myDriverType = D3D_DRIVER_TYPE_UNKNOWN;

		DXGI_ADAPTER_DESC desc = {};
		pAdapter->GetDesc(&desc);
		wprintf(L"GPU Name: %s\n", desc.Description);
	}


	if (FAILED(hr = D3D11CreateDeviceAndSwapChain(pAdapter,
		myDriverType,
		NULL,
		0,
		FeatureLevels,
		ARRAYSIZE(FeatureLevels),
		D3D11_SDK_VERSION,
		&sd,
		&m_SwapChain,
		&m_Device,
		&MaxSupportedFeatureLevel,
		&m_Context)))
	{
		DebugBreak();
	}
	SafeRelease(&pAdapter);


}

void MainWindow::SetRtvAndBackBuffer()
{
	HRESULT hr = S_OK;

	// 백버퍼 가져오기
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	if (SUCCEEDED(hr))
	{
		// 렌더 타겟 뷰 생성
		m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_renderTargetView);
	}
	else
	{
		DebugBreak();
	}


	D3D11_VIEWPORT vp;
	vp.Width = m_width;
	vp.Height = m_height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_Context->RSSetViewports(1, &vp);


}

void  MainWindow::Initialize()
{

//https://github.com/microsoft/DirectXTK/wiki/WICTextureLoader 
#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
	if (FAILED(initialize))
	{
		DebugBreak();
	}
#else
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		DebugBreak();
	}
#endif

	CreateDevice();
	SetRtvAndBackBuffer();
	m_Renderer = std::make_unique<Renderer>(m_hwnd, m_Device, m_Context, m_renderTargetView);
	m_ShaderSet = std::make_unique<ShaderSet>(m_Device, m_Context);
	m_Renderer->CreateModels();
	m_Renderer->CreateTexture();
	m_Renderer->CreateSampler();
	m_Renderer->CreateConstantBuffer();
	m_Renderer->CreateRs();
	m_Renderer->CreateDepthStencil();

	m_ShaderSet->SetShaders();
	m_ShaderSet->CreateInputLayout();
}

using Clock = std::chrono::high_resolution_clock;
Clock::time_point lastTime = Clock::now();
int frameCount = 0;
float fps = 0.0f;

void FrameCount()
{
	frameCount++;

	// 현재 시간
	auto now = Clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTime);

	// 1초 경과 시 FPS 계산
	if (duration.count() >= 1000)
	{
		fps = frameCount * 1000.0f / duration.count();  // 밀리초 단위 → FPS 계산
		frameCount = 0;
		lastTime = now;

		// 콘솔 출력
		printf("FPS: %.2f\n", fps);
	}
}

void MainWindow::Render()
{
	//FrameCount();
	m_Renderer->Render();
	
}
void MainWindow::Update()
{
	if (m_isMovingUp)
	{
		m_Renderer->eyePos.y += 0.01f;
		m_Renderer->lookAt.y += 0.01f;
	}
	if (m_isMovingDown)
	{
		m_Renderer->eyePos.y -= 0.01f;
		m_Renderer->lookAt.y -= 0.01f;
	}
	if (m_isMovingLeft)
	{
		m_Renderer->eyePos.x -= 0.01f;
		m_Renderer->lookAt.x -= 0.01f;
	}
	if (m_isMovingRight)
	{
		m_Renderer->eyePos.x += 0.01f;
		m_Renderer->lookAt.x += 0.01f;
	}
	m_Renderer->Update();
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pBackBuffer);
	SafeRelease(&m_Device);
	SafeRelease(&m_Context);
	SafeRelease(&m_SwapChain);
	SafeRelease(&m_renderTargetView);
}

