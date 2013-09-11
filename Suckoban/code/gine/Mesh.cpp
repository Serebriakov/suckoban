#include "Mesh.h"

Mesh::Mesh()
{
  Init();
}

void Mesh::Init()
{
  mVertexBuffer = 0;
  mIndexBuffer = 0;
  mVertices = 0;
  mIndices = 0;

  mScale = XMFLOAT3(1.0f,1.0f,1.0f);
  mRotate = XMFLOAT3(0.0f,0.0f,0.0f);
  mTranslate = XMFLOAT3(0.0f,0.0f,0.0f);
  XMStoreFloat4x4(&mWorld,XMMatrixIdentity());

  mSubsets = 0;
  mCurrentAnimation = 0;
}

Mesh::~Mesh()
{
  ReleaseCOM(mVertexBuffer);
  ReleaseCOM(mIndexBuffer);
  delete mAxisAlignedBoundingBox;
  delete mBoundingSphere;
}

void Mesh::SetScale(float aScale)
{
  mScale = XMFLOAT3(aScale,aScale,aScale);
}

void Mesh::Transform()
{
  XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat3(&mScale));
  XMMATRIX RX = XMMatrixRotationX(mRotate.x);
  XMMATRIX RY = XMMatrixRotationY(mRotate.y);
  XMMATRIX RZ = XMMatrixRotationZ(mRotate.z);
  XMMATRIX R = RX*RY*RZ;
  XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&mTranslate));
  
  XMStoreFloat4x4(&mWorld, S*R*T);
}

void Mesh::StartAnimation(string aName)
{
  mCurrentAnimation = &mAnimations[aName];
  mCurrentAnimation->Start();
}

void Mesh::StopAllAnimations()
{
  map<string, Animation>::iterator it;
  for(it = mAnimations.begin(); it != mAnimations.end(); it++)
    it->second.Stop();
}

UINT Mesh::GetCurrentAnimationPose(XMFLOAT4X4* aOffsets)
{
  if(mCurrentAnimation)
  {
    mCurrentAnimation->GetCurrentOffsets(aOffsets);
    return mCurrentAnimation->GetNumBones();
  }

  return 0;
}

int Mesh::GetNumBone(string aName)
{
  for(UINT i=0; i<mBones.size(); i++)
  {
    if(aName == mBones[i].name)
      return i;
  }

  return -1;
}

Bone* Mesh::GetBone(string aName)
{
  for(UINT i=0; i<mBones.size(); i++)
  {
    if(aName == mBones[i].name)
      return &mBones[i];
  }

  return 0;
}

void Mesh::Tick(float dt)
{
  if(mCurrentAnimation)
    mCurrentAnimation->Tick(dt);
}