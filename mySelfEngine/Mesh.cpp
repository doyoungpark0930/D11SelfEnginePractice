#include "Mesh.h"

std::vector<Vertex> SquareVertices()
{
	std::vector<Vertex> vertex;

	vertex = {
		{ XMFLOAT3(1.0f,  1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f)},  
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },  
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },  
		{ XMFLOAT3(-1.0f,  1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }  
	};
	return vertex;
}

std::vector<unsigned int> SquareIndices()
{
	std::vector<unsigned int> indices;

	indices = {
		0, 1, 2,
		0, 2, 3
	};

	return indices;
}