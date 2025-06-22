#pragma once

#include <DirectXMath.h>

using namespace DirectX;

// 정점을 설명하는 데이터 타입 정의
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Col;
	XMFLOAT2 Tex;   // 텍스처 좌표 (U, V)
};
