#pragma once

#include <d3dx11.h>
#include <xnamath.h>
#include <vector>
#include <map>
#include "xnacollision.h"
#include "Animation.h"
using namespace std;
using namespace XNA;
using namespace Gine;

class Mesh
{
public:
	Mesh();
  void Init();

	~Mesh();

  void SetScale(float aScale);
  void Transform();

	ID3D11Buffer* mVertexBuffer;
  int mVertices;
  ID3D11Buffer* mIndexBuffer;
  int mIndices;

  XMFLOAT3 mScale;
  XMFLOAT3 mRotate;
  XMFLOAT3 mTranslate;
  XMFLOAT4X4 mWorld;

  int mSubsets;
  vector<int> mSubsetIndexStart;

  AxisAlignedBox* mAxisAlignedBoundingBox;
  Sphere* mBoundingSphere;

  // Animation
  vector<Bone> mBones;
  map<string, Animation> mAnimations;
  Animation* mCurrentAnimation;

  void StartAnimation(string aName);
  void StopAllAnimations();
  UINT GetCurrentAnimationPose(XMFLOAT4X4* aOffsets);

  int GetNumBone(string aName);
  Bone* GetBone(string aName);

  void Tick(float dt);
};

