#include "Renderer.h"
#include <stdio.h>

void Renderer::CreateModels()
{
	std::shared_ptr<Model> Square = std::make_shared<Model>(m_Device,m_Context);
	Square->CreateVertexBuffer(SquareVertices());
	Square->CreateIndexBuffer(SquareIndices());
	Square->CreateModelConstantBuffer();

	m_Models.push_back(Square);
}
void Renderer::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
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
	UINT numElements = ARRAYSIZE(layout);

	HRESULT hr = m_Device->CreateInputLayout(
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

void Renderer::CreateConstantBuffer()
{
	VIEWPROJ_CONSTANT VsConstData;

	XMMATRIX View = 
		XMMatrixLookAtLH(
			XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f),  // eye position
			XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),   // look at
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)); // up

	float fovY = XM_PIDIV4; 
	float aspect = (float)1920 / (float)1050;
	float nearZ = 0.1f;
	float farZ = 100.0f;

	XMMATRIX Proj = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);

	XMStoreFloat4x4(&VsConstData.ViewProj, XMMatrixTranspose(View*Proj));


	// 버퍼 설명 작성
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VIEWPROJ_CONSTANT);
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
	HRESULT hr = m_Device->CreateBuffer(&cbDesc, &InitData, &m_ConstantBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}


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

void Renderer::CreateSampler()
{
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr  = m_Device->CreateSamplerState(&sampDesc, &m_Sampler);

	if (FAILED(hr))
	{
		DebugBreak();
	}
}

void Renderer::CreateRs()
{
	D3D11_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FrontCounterClockwise = FALSE; 

	HRESULT hr = m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState);
	if (FAILED(hr))
	{
		DebugBreak();
	}
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

}

void Renderer::Render()
{
	// 렌더 타겟 뷰 바인딩
	m_Context->OMSetRenderTargets(1, &m_RTV, NULL);

	// 렌더타겟을 파란색으로 지우기
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; // R,G,B,A
	m_Context->ClearRenderTargetView(m_RTV, clearColor);
	m_Context->RSSetState(m_RasterState.p);
	m_Context->IASetInputLayout(m_InputLayout);

	m_Context->VSSetShader(m_VertexShader, nullptr, 0);
	m_Context->PSSetShader(m_PixelShader, nullptr, 0);

	m_Context->PSSetShaderResources(0, 1, &m_SRV.p);
	m_Context->PSSetSamplers(0, 1, &m_Sampler.p);

	m_Context->VSSetConstantBuffers(0, 1, &m_ConstantBuffer.p);

	for (auto& iter : m_Models)
	{
		iter->Render();
	}

}








//Model

void Model::CreateVertexBuffer(const std::vector<Vertex> &Vertices) {
	// 실제 정점 데이터 제공
	m_Vertices = Vertices;

	// 버퍼 설명 채우기
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * m_Vertices.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// 서브리소스 데이터 채우기
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Vertices.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// 버퍼 생성(Gpu 메모리에 올림)
	HRESULT hr = m_Device->CreateBuffer(&bufferDesc, &InitData, &m_VertexBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}

}

void Model::CreateIndexBuffer(const std::vector<unsigned int> &Indices)
{

	// 인덱스 데이터 생성
	m_Indices = Indices;
	// 버퍼 설명 작성
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * m_Indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// 리소스 데이터 정의
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Indices.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// 디바이스로 버퍼 생성
	HRESULT hr = m_Device->CreateBuffer(&bufferDesc, &InitData, &m_IndexBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}


}

void Model::CreateModelConstantBuffer()
{
	MODEL_CONSTANT VsConstData;
	XMMATRIX Model =
		XMMatrixScaling(1.0f, 1.0f, 1.0f) *
		XMMatrixRotationY(XMConvertToRadians(0.0f)) *
		XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&VsConstData.Model, XMMatrixTranspose(Model));

	// 버퍼 설명 작성
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VIEWPROJ_CONSTANT);
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
	HRESULT hr = m_Device->CreateBuffer(&cbDesc, &InitData, &m_ModelConstantBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}

}

void Model::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &m_VertexBuffer.p, &stride, &offset);
	m_Context->IASetIndexBuffer(m_IndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
	m_Context->VSSetConstantBuffers(1, 1, &m_ModelConstantBuffer.p);

	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Context->DrawIndexed(m_Indices.size(), 0, 0);
}

