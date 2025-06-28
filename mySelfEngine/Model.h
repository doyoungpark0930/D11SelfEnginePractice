#pragma
#include "VertexStruct.h"
#include <vector>
#include <atlbase.h>
#include "ShaderSet.h"

#include <d3d11.h>
#include <dxgi.h>          // DXGI_SWAP_CHAIN_DESC
#include <d3dcompiler.h>   // 셰이더 컴파일할 예정이면 필요

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Model
{
public:
	Model(ID3D11Device* Device, ID3D11DeviceContext* Context)
		: m_Device(Device), m_Context(Context)
	{

	};
	~Model()
	{
		if (m_Device) m_Device->Release();
		if (m_Context) m_Context->Release();
	}
public:
	CComPtr<ID3D11Buffer> m_VertexBuffer;
	CComPtr<ID3D11Buffer> m_IndexBuffer;
	CComPtr<ID3D11Buffer> m_ModelConstantBuffer;

	std::vector<CComPtr<ID3D11Buffer>> m_NormalBuffers;

	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);

	void CreateVertexBuffer(const std::vector<Vertex>& Vertices);
	void CreateNormalBuffer(const Vertex& Vertices,int i);
	void CreateIndexBuffer(const std::vector<unsigned int>& Indices);
	void CreateModelConstantBuffer();
	void Render();
	void NormalRender(const int i);
	void Update();

private:
	std::vector<Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	NormalVertex m_NormalVertex[2];
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_Context;
};