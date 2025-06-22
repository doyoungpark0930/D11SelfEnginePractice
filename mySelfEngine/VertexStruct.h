#pragma once

#include <DirectXMath.h>

using namespace DirectX;

// ������ �����ϴ� ������ Ÿ�� ����
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Col;
	XMFLOAT2 Tex;   // �ؽ�ó ��ǥ (U, V)
};
