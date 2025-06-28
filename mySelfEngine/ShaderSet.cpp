#include "ShaderSet.h"
CComPtr<ID3D11PixelShader> m_PixelShader;
CComPtr<ID3D11VertexShader> m_VertexShader;
CComPtr<ID3D11InputLayout>  m_VertexInputLayout;

CComPtr<ID3D11PixelShader> m_NormalPixelShader;
CComPtr<ID3D11VertexShader> m_NormalVertexShader;
CComPtr<ID3D11InputLayout>  m_NormalInputLayout;

void ShaderSet::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout1[] =
	{
		// POSITION
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		// COLOR
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		//Texture
		{
			"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	UINT numElements = ARRAYSIZE(layout1);

	HRESULT hr = m_Device->CreateInputLayout(
		layout1,
		numElements,
		g_vsshader,
		sizeof(g_vsshader),
		&m_VertexInputLayout
	);

	if (FAILED(hr))
	{
		DebugBreak();
	}

	D3D11_INPUT_ELEMENT_DESC layout2[] =
	{
		// POSITION
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		// COLOR
		{
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		//Texture
		{
			"COLOR",0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	numElements = ARRAYSIZE(layout2);

	hr = m_Device->CreateInputLayout(
		layout2,
		numElements,
		g_normalVertexShader,
		sizeof(g_normalVertexShader),
		&m_NormalInputLayout
	);

	if (FAILED(hr))
	{
		DebugBreak();
	}
}

void ShaderSet::SetShaders()
{
	HRESULT hr = S_OK;

	// pPSBlob �ʿ� ���� �� ��� g_psshader �迭 ���
	hr = m_Device->CreatePixelShader(
		g_psshader,                  // ����Ʈ �ڵ� ������
		sizeof(g_psshader),          // ����Ʈ �ڵ� ũ��
		nullptr,
		&m_PixelShader
	); 

	if (FAILED(hr))
	{
		DebugBreak();
	}


	hr = m_Device->CreateVertexShader(
		g_vsshader,
		sizeof(g_vsshader),
		nullptr,
		&m_VertexShader
	);

	if (FAILED(hr))
	{
		DebugBreak();
	};


	hr = m_Device->CreatePixelShader(
		g_normalPixelShader,
		sizeof(g_normalPixelShader),
		nullptr,
		&m_NormalPixelShader
	);

	if (FAILED(hr))
	{
		DebugBreak();
	}


	hr = m_Device->CreateVertexShader(
		g_normalVertexShader,
		sizeof(g_normalVertexShader),
		nullptr,
		&m_NormalVertexShader
	);

	if (FAILED(hr))
	{
		DebugBreak();
	};

}