#pragma once
#include <Windows.h> 
#include "VertexStruct.h"
#include "Shaders.h"  // 빌드 시 생성된 헤더 포함
#include <atlbase.h>
#include "Mesh.h"
#include "WICTextureLoader11.h"
#include <memory>
#include "Model.h"

#include <d3d11.h>
#include <dxgi.h>          // DXGI_SWAP_CHAIN_DESC
#include <d3dcompiler.h>   // 셰이더 컴파일할 예정이면 필요

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")



class Renderer
{
public:
	Renderer(HWND hwnd, ID3D11Device* Device,
		ID3D11DeviceContext* Context, ID3D11RenderTargetView* RenderTargetView)
		: m_hwnd(hwnd), m_Device(Device), m_Context(Context), m_RTV(RenderTargetView)
	{
	}
	~Renderer()
	{
		if (m_VertexBuffer) m_VertexBuffer->Release();
		if (m_IndexBuffer) m_IndexBuffer->Release();
		if (m_Device) m_Device->Release();
		if (m_Context) m_Context->Release();
	}
public:
	CComPtr<ID3D11PixelShader> m_PixelShader = nullptr;
	CComPtr<ID3D11VertexShader> m_VertexShader = nullptr;
	CComPtr<ID3D11InputLayout> m_InputLayout = nullptr;
	CComPtr<ID3D11ShaderResourceView> m_SRV = nullptr;
	CComPtr<ID3D11SamplerState> m_Sampler = nullptr;
	CComPtr<ID3D11Buffer> m_ConstantBuffer = nullptr;
	CComPtr<ID3D11RasterizerState> m_RasterState = nullptr;

	CComPtr<ID3D11Texture2D> m_DepthStencilTexture = nullptr;
	CComPtr<ID3D11DepthStencilState> m_DSState = nullptr;
	CComPtr<ID3D11DepthStencilView> m_DSV = nullptr;

	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	std::vector<std::shared_ptr<Model>> m_Models;


private:
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_Context = nullptr;
	ID3D11RenderTargetView* m_RTV = nullptr;

	HWND m_hwnd;

	float fovY = XM_PIDIV4;
	float aspect = (float)1920 / (float)1050;
	float nearZ = 0.1f;
	float farZ = 100.0f;
	XMFLOAT4 eyePos = XMFLOAT4(0.0f, 0.0f, -5.0f, 1.0f);
	XMFLOAT4 lookAt= XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 up = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

public:
	void CreateModels();
	void CreateInputLayout();
	void CreateConstantBuffer();
	void CreateTexture();
	void CreateSampler();
	void CreateRs();
	void SetShaders();
	void CreateDepthStencil();
	void ConstantUpdate();
	void Update();
	void Render();

};




