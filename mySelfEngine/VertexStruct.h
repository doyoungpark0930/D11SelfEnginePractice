#pragma once

#include <DirectXMath.h>

using namespace DirectX;

// ������ �����ϴ� ������ Ÿ�� ����
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;   // �ؽ�ó ��ǥ (U, V)
};

struct MODEL_CONSTANT
{
	XMFLOAT4X4 Model;
};

struct VIEWPROJ_CONSTANT
{
	XMFLOAT4X4 ViewProj;
};