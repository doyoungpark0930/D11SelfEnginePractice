#include "Model.h"


void Model::CreateVertexBuffer(const std::vector<Vertex>& Vertices) {
	// 실제 정점 데이터 제공
	m_Vertices = Vertices;
	m_NormalBuffers.resize(Vertices.size());
	for (int i = 0 ; i < Vertices.size(); i++)
	{
		CreateNormalBuffer(Vertices[i], i);
	}
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

void Model::CreateNormalBuffer(const Vertex& Vertice, int i)
{
	m_NormalVertex[0].Pos = Vertice.Pos;
	m_NormalVertex[1].Pos = XMFLOAT3(
		Vertice.Pos.x + Vertice.Normal.x,
		Vertice.Pos.y + Vertice.Normal.y,
		Vertice.Pos.z + Vertice.Normal.z
	);
	m_NormalVertex[0].Normal = Vertice.Normal;
	m_NormalVertex[1].Normal = Vertice.Normal;

	m_NormalVertex[0].StartPos = Vertice.Pos;
	m_NormalVertex[1].StartPos = Vertice.Pos;
	
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(m_NormalVertex);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// 서브리소스 데이터 채우기
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = m_NormalVertex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// 버퍼 생성(Gpu 메모리에 올림)
	HRESULT hr = m_Device->CreateBuffer(&bufferDesc, &InitData, &m_NormalBuffers[i]);
	if (FAILED(hr))
	{
		DebugBreak();
	}
}

void Model::CreateIndexBuffer(const std::vector<unsigned int>& Indices)
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
	XMMATRIX ModelMatrix =
		XMMatrixScaling(1.0f, 1.0f, 1.0f) *
		XMMatrixRotationX(XMConvertToRadians(0.0f)) *
		XMMatrixRotationY(XMConvertToRadians(0.0f)) *
		XMMatrixRotationZ(XMConvertToRadians(0.0f))*
		XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&VsConstData.Model, XMMatrixTranspose(ModelMatrix));

	// translation을 제거
	ModelMatrix.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, XMVectorGetW(ModelMatrix.r[3]));

	// 전치행렬 계산
	XMMATRIX NormalMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, ModelMatrix));

	XMStoreFloat4x4(&VsConstData.NormalModel, XMMatrixTranspose(NormalMatrix));


	// 버퍼 설명 작성
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(MODEL_CONSTANT);
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

void Model::Update()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	XMMATRIX ModelMatrix =
		XMMatrixScaling(scale.x, scale.y, scale.z) *
		XMMatrixRotationX(XMConvertToRadians(rotation.x)) *
		XMMatrixRotationY(XMConvertToRadians(rotation.y)) *
		XMMatrixRotationZ(XMConvertToRadians(rotation.z)) *
		XMMatrixTranslation(pos.x, pos.y, pos.z);

	XMFLOAT4X4 Model4x4;
	XMFLOAT4X4 Normal4x4;

	XMStoreFloat4x4(&Model4x4, XMMatrixTranspose(ModelMatrix));

	//gpu닫고 cpu에서 복사할준비
	m_Context->Map(m_ModelConstantBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//Model행렬 복사
	memcpy(mappedResource.pData, &Model4x4, sizeof(Model4x4));
	
	//NormalModel행렬 준비
	ModelMatrix.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, XMVectorGetW(ModelMatrix.r[3]));
	XMMATRIX NormalMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, ModelMatrix));
	XMStoreFloat4x4(&Normal4x4, XMMatrixTranspose(NormalMatrix));

	memcpy((uint8_t*)mappedResource.pData + sizeof(Model4x4), &Normal4x4, sizeof(Normal4x4));

	m_Context->Unmap(m_ModelConstantBuffer.p, 0);
	
}

void Model::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &m_VertexBuffer.p, &stride, &offset);
	m_Context->IASetIndexBuffer(m_IndexBuffer.p, DXGI_FORMAT_R32_UINT, 0);
	m_Context->VSSetConstantBuffers(1, 1, &m_ModelConstantBuffer.p);

	m_Context->IASetInputLayout(m_VertexInputLayout);

	m_Context->VSSetShader(m_VertexShader, nullptr, 0);
	m_Context->PSSetShader(m_PixelShader, nullptr, 0);

	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Context->DrawIndexed(m_Indices.size(), 0, 0);
	
	for (int i = 0; i < m_NormalBuffers.size(); i++)
	{
		NormalRender(i);
	}
}

void Model::NormalRender(const int i)
{
	UINT stride = sizeof(NormalVertex);
	UINT offset = 0;
	m_Context->IASetVertexBuffers(0, 1, &m_NormalBuffers[i].p, &stride, &offset);
	m_Context->IASetInputLayout(m_NormalInputLayout);

	m_Context->VSSetShader(m_NormalVertexShader, nullptr, 0);
	m_Context->PSSetShader(m_NormalPixelShader, nullptr, 0);
	m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_Context->Draw(2, 0);
}