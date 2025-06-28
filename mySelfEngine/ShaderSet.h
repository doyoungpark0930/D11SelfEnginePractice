#pragma once
#include <atlbase.h>
#include "Shaders.h"  // 빌드 시 생성된 헤더 포함
#include "basewin.h"

#include <d3d11.h>
#include <dxgi.h>          // DXGI_SWAP_CHAIN_DESC
#include <d3dcompiler.h>   // 셰이더 컴파일할 예정이면 필요

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

extern CComPtr<ID3D11PixelShader> m_PixelShader;
extern CComPtr<ID3D11VertexShader> m_VertexShader;
extern CComPtr<ID3D11InputLayout> m_VertexInputLayout;

extern CComPtr<ID3D11PixelShader> m_NormalPixelShader;
extern CComPtr<ID3D11VertexShader> m_NormalVertexShader;
extern CComPtr<ID3D11InputLayout> m_NormalInputLayout;

class ShaderSet
{
public:
	ShaderSet(ID3D11Device* Device,
		ID3D11DeviceContext* Context)
		: m_Device(Device), m_Context(Context)
	{
		m_Device->AddRef();
		m_Context->AddRef();
	}
	~ShaderSet()
	{
		if (m_Device) m_Device->Release();
		if (m_Context) m_Context->Release();
	}

public :
	void CreateInputLayout();
	void SetShaders();

private:
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_Context = nullptr;
};