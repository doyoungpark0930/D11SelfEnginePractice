#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 Tex;   // 텍스처 좌표 (U, V)
};

struct NormalVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT3 StartPos;  //vertex시작 지점
};



struct MODEL_CONSTANT
{
	XMFLOAT4X4 Model;
	XMFLOAT4X4 NormalModel;
};

struct VIEWPROJ_CONSTANT
{
	XMFLOAT4X4 ViewProj;
};