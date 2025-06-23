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


	// ���� ���� �ۼ�
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VIEWPROJ_CONSTANT);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// ���긮�ҽ� ������ �ۼ�
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VsConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// ���� ����
	HRESULT hr = m_Device->CreateBuffer(&cbDesc, &InitData, &m_ConstantBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}


}

void Renderer::SetShaders()
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
		m_Context,        // ID3D11DeviceContext*, mipmap ���� ���ϸ� �ֱ�, �ʿ� ������ nullptr
		L"C:\\Users\\Lenovo\\source\\repos\\mySelfEngine\\FirstTest.jpg",    // ���� ��� (�����ڵ� ���ڿ�)
		nullptr,           // �ؽ�ó ��ü�� �ʿ� ���ٸ� nullptr
		&m_SRV,              // SRV ��ȯ
		0                  // maxsize (0�̸� ����̽� �ִ� ũ�⿡ �°� �ڵ�)
	);

	if (FAILED(hr)) {
		DebugBreak();
	}

}

void Renderer::Render()
{
	// ���� Ÿ�� �� ���ε�
	m_Context->OMSetRenderTargets(1, &m_RTV, NULL);

	// ����Ÿ���� �Ķ������� �����
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
	// ���� ���� ������ ����
	m_Vertices = Vertices;

	// ���� ���� ä���
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * m_Vertices.size();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// ���긮�ҽ� ������ ä���
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Vertices.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// ���� ����(Gpu �޸𸮿� �ø�)
	HRESULT hr = m_Device->CreateBuffer(&bufferDesc, &InitData, &m_VertexBuffer);
	if (FAILED(hr))
	{
		DebugBreak();
	}

}

void Model::CreateIndexBuffer(const std::vector<unsigned int> &Indices)
{

	// �ε��� ������ ����
	m_Indices = Indices;
	// ���� ���� �ۼ�
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * m_Indices.size();
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// ���ҽ� ������ ����
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_Indices.data();
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// ����̽��� ���� ����
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

	// ���� ���� �ۼ�
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VIEWPROJ_CONSTANT);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// ���긮�ҽ� ������ �ۼ�
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &VsConstData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// ���� ����
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

