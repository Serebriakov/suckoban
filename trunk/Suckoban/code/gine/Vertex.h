//***************************************************************************************
// Vertex.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines vertex structures and input layouts.
//***************************************************************************************

#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

namespace Vertex
{
	// Basic 32-byte vertex structure.
	struct Basic32
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
    XMFLOAT3 TangentU;
    XMFLOAT3 Weights;
    BYTE     BoneIndices[4];
	};

  struct Particle
	{
		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};

  struct Billboard
	{
		XMFLOAT3 Pos;
		XMFLOAT2 Size;
	};
}

class InputLayoutDesc
{
public:
	// Init like const int A::a[4] = {0, 1, 2, 3}; in .cpp file.
	static const D3D11_INPUT_ELEMENT_DESC Basic32[6];
	static const D3D11_INPUT_ELEMENT_DESC InstancedBasic32[10];
	static const D3D11_INPUT_ELEMENT_DESC Particle[5];
	static const D3D11_INPUT_ELEMENT_DESC Billboard[2];
};

class InputLayouts
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11InputLayout* Basic32;
	static ID3D11InputLayout* InstancedBasic32;
	static ID3D11InputLayout* Particle;
	static ID3D11InputLayout* Billboard;
};

#endif // VERTEX_H
