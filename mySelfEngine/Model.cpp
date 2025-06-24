#include "Model.h"


void Model::CreateVertexBuffer(const std::vector<Vertex>& Vertices) {
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

void Model::CreateIndexBuffer(const std::vector<unsigned int>& Indices)
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
	XMMATRIX ModelMatrix =
		XMMatrixScaling(1.0f, 1.0f, 1.0f) *
		XMMatrixRotationY(XMConvertToRadians(0.0f)) *
		XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	MODEL_CONSTANT VsConstData;

	XMStoreFloat4x4(&VsConstData.Model, XMMatrixTranspose(ModelMatrix));

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

void Model::Update()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	XMMATRIX ModelMatrix =
		XMMatrixScaling(scale.x, scale.y, scale.z) *
		XMMatrixRotationY(XMConvertToRadians(rotationY)) *
		XMMatrixTranslation(pos.x, pos.y, pos.z);

	XMFLOAT4X4 Model4x4;

	XMStoreFloat4x4(&Model4x4, XMMatrixTranspose(ModelMatrix));
	m_Context->Map(m_ModelConstantBuffer.p, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, &Model4x4, sizeof(Model4x4));

	m_Context->Unmap(m_ModelConstantBuffer.p, 0);
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

