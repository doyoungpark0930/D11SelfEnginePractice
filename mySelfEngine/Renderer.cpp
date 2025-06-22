#include "Renderer.h"
#include <stdio.h>

void Renderer::CreateVertexBuffer() {
	// 실제 정점 데이터 제공
	std::vector<Vertex> verticesCombo = SquareVertices();

	// 버퍼 설명 채우기
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * verticesCombo.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// 서브리소스 데이터 채우기
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = verticesCombo.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// 버퍼 생성(Gpu 메모리에 올림)
	HRESULT hr = m_Device->CreateBuffer(&bufferDesc, &InitData, &m_VertexBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}


	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// POSITION
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		// COLOR
		{
			"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		},
		//Texture
		{
			"TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
			D3D11_INPUT_PER_VERTEX_DATA, 0
		}
	};
	UINT numElements = ARRAYSIZE(layout);

	hr = m_Device->CreateInputLayout(
		layout,
		numElements,
		g_vsshader,
		sizeof(g_vsshader),
		&m_InputLayout
	);

	if (FAILED(hr))
	{
		DebugBreak();
	}

}

void Renderer::CreateIndexBuffer()
{

	// 인덱스 데이터 생성
	std::vector<unsigned int> indices = SquareIndices();
	// 버퍼 설명 작성
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// 리소스 데이터 정의
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = indices.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// 디바이스로 버퍼 생성
	HRESULT hr = m_Device->CreateBuffer(&bufferDesc, &InitData, &m_IndexBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}


}

void Renderer::CreateConstantBuffer()
{
	ID3D11Buffer* g_pConstantBuffer11 = NULL;

	// 셰이더와 통신할 상수 데이터 정의
	struct VS_CONSTANT_BUFFER
	{
		XMFLOAT4X4 mWorldViewProj;
		XMFLOAT4 vSomeVectorThatMayBeNeededByASpecificShader;
		float fSomeFloatThatMayBeNeededByASpecificShader;
		float fTime;
		float fSomeFloatThatMayBeNeededByASpecificShader2;
		float fSomeFloatThatMayBeNeededByASpecificShader3;
	};

	// 상수 데이터 채우기
	VS_CONSTANT_BUFFER VsConstData;
	VsConstData.mWorldViewProj = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	VsConstData.vSomeVectorThatMayBeNeededByASpecificShader = XMFLOAT4(1, 2, 3, 4);
	VsConstData.fSomeFloatThatMayBeNeededByASpecificShader = 3.0f;
	VsConstData.fTime = 1.0f;
	VsConstData.fSomeFloatThatMayBeNeededByASpecificShader2 = 2.0f;
	VsConstData.fSomeFloatThatMayBeNeededByASpecificShader3 = 4.0f;

	// 버퍼 설명 작성
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// 서브리소스 데이터 작성
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VsConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// 버퍼 생성
	HRESULT hr = m_Device->CreateBuffer(&cbDesc, &InitData, &g_pConstantBuffer11);
	if (FAILED(hr))
	{
		DebugBreak();
	}

	// 버퍼 셋팅
	m_Context->VSSetConstantBuffers(0, 1, &g_pConstantBuffer11);

}

void Renderer::SetShaders()
{
	HRESULT hr = S_OK;

	// pPSBlob 필요 없음 → 대신 g_psshader 배열 사용
	hr = m_Device->CreatePixelShader(
		g_psshader,                  // 바이트 코드 데이터
		sizeof(g_psshader),          // 바이트 코드 크기
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

}


void Renderer::CreateTexture()
{
	HRESULT hr = CreateWICTextureFromFile(
		m_Device,         // ID3D11Device*
		m_Context,        // ID3D11DeviceContext*, mipmap 생성 원하면 넣기, 필요 없으면 nullptr
		L"C:\\Users\\Lenovo\\source\\repos\\mySelfEngine\\FirstTest.jpg",    // 파일 경로 (유니코드 문자열)
		nullptr,           // 텍스처 자체가 필요 없다면 nullptr
		&m_SRV,              // SRV 반환
		0                  // maxsize (0이면 디바이스 최대 크기에 맞게 자동)
	);

	if (FAILED(hr)) {
		DebugBreak();
	}

	CComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	m_Device->CreateSamplerState(&sampDesc, &sampler);

}

void Renderer::Render()
{
	// 렌더 타겟 뷰 바인딩
	m_Context->OMSetRenderTargets(1, &m_RTV, NULL);

	// 렌더타겟을 파란색으로 지우기
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // R,G,B,A
	m_Context->ClearRenderTargetView(m_RTV, clearColor);

	m_Context->IASetInputLayout(m_InputLayout);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	m_Context->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_Context->VSSetShader(m_VertexShader, nullptr, 0);
	m_Context->PSSetShader(m_PixelShader, nullptr, 0);

	m_Context->PSSetShaderResources(0, 1, &m_SRV.p);
	m_Context->PSSetSamplers(0, 1, &m_Sampler.p);

	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Context->DrawIndexed(6, 0, 0);
}

