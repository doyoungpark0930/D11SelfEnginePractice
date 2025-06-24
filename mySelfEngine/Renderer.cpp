#include "Renderer.h"
#include <stdio.h>

void Renderer::CreateModels()
{
	std::shared_ptr<Model> Square = std::make_shared<Model>(m_Device, m_Context);
	Square->CreateVertexBuffer(SquareVertices());
	Square->CreateIndexBuffer(SquareIndices());
	Square->CreateModelConstantBuffer();

	m_Models.push_back(Square);

	std::shared_ptr<Model> Square2 = std::make_shared<Model>(m_Device, m_Context);
	Square2->CreateVertexBuffer(SquareVertices());
	Square2->CreateIndexBuffer(SquareIndices());
	Square2->CreateModelConstantBuffer();

	m_Models.push_back(Square2);
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
			XMVectorSet(eyePos.x, eyePos.y, eyePos.z, eyePos.w),  // eye position
			XMVectorSet(lookAt.x,lookAt.y,lookAt.z,lookAt.w),   // look at
			XMVectorSet(up.x, up.y, up.z, up.w)); // up

	XMMATRIX Proj = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);

	XMStoreFloat4x4(&VsConstData.ViewProj, XMMatrixTranspose(View * Proj));


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
	HRESULT hr = m_Device->CreateSamplerState(&sampDesc, &m_Sampler);

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

void Renderer::CreateDepthStencil()
{
	//DepthStencil State설정
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT hr = m_Device->CreateDepthStencilState(&dsDesc, &m_DSState);

	if (FAILED(hr)) {
		DebugBreak();
	}

	//DepthStencil View설정
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = 1920;
	descDepth.Height = 1050;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	m_Device->CreateTexture2D(&descDepth, nullptr, &m_DepthStencilTexture);

	if (FAILED(hr)) {
		DebugBreak();
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	hr = m_Device->CreateDepthStencilView(
		m_DepthStencilTexture, // Depth stencil texture
		&descDSV, // Depth stencil desc
		&m_DSV);  // [out] Depth stencil view

	if (FAILED(hr)) {
		DebugBreak();
	}

}

void Renderer::ConstantUpdate()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	XMMATRIX View =
		XMMatrixLookAtLH(
			XMVectorSet(eyePos.x,eyePos.y,eyePos.z,eyePos.w),  // eye position
			XMVectorSet(lookAt.x, lookAt.y, lookAt.z, lookAt.w),   // look at
			XMVectorSet(up.x, up.y, up.z, up.w)); // up


	XMMATRIX Proj = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);

	XMFLOAT4X4 Model4x4;

	XMStoreFloat4x4(&Model4x4, XMMatrixTranspose(View * Proj));

	m_Context->Map(m_ConstantBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, &Model4x4, sizeof(Model4x4));

	m_Context->Unmap(m_ConstantBuffer.p, 0);
}

void Renderer::Update()
{
	eyePos.x += 0.01f;
	//m_Models[0]->pos.x += 0.001f;
	//m_Models[0]->rotationY += 0.1f;

	m_Models[1]->pos.x = 1.0f;
	m_Models[1]->pos.z = 1.0f;

	ConstantUpdate();
	for (auto& iter : m_Models)
	{
		iter->Update();
	}
}

void Renderer::Render()
{

	// 렌더 타겟 뷰 바인딩
	m_Context->OMSetDepthStencilState(m_DSState.p, 0);
	m_Context->OMSetRenderTargets(1, &m_RTV, m_DSV);
	m_Context->ClearDepthStencilView(m_DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
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







