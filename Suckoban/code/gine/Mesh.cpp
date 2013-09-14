#include "Mesh.h"
#include "GineUtils.h"
using namespace Gine;

Mesh::Mesh()
{
  Init();
}

void Mesh::Init()
{
  VertexBuffer = 0;
  IndexBuffer = 0;
  Vertices = 0;
  Indices = 0;

  Scale = XMFLOAT3(1.0f,1.0f,1.0f);
  Rotate = XMFLOAT3(0.0f,0.0f,0.0f);
  Translate = XMFLOAT3(0.0f,0.0f,0.0f);
  XMStoreFloat4x4(&World,XMMatrixIdentity());

  Subsets = 0;
  CurrentAnimation = 0;
}

Mesh::~Mesh()
{
  ReleaseCOM(VertexBuffer);
  ReleaseCOM(IndexBuffer);
  SafeDelete(BoundingBox);
  SafeDelete(BoundingSphere);
}

void Mesh::Transform()
{
  XMMATRIX S = XMMatrixScalingFromVector(XMLoadFloat3(&Scale));
  XMMATRIX RX = XMMatrixRotationX(Rotate.x);
  XMMATRIX RY = XMMatrixRotationY(Rotate.y);
  XMMATRIX RZ = XMMatrixRotationZ(Rotate.z);
  XMMATRIX R = RX*RY*RZ;
  XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&Translate));
  
  XMStoreFloat4x4(&World, S*R*T);
}

void Mesh::StartAnimation(const string* name)
{
  CurrentAnimation = &Animations[*name];
  CurrentAnimation->Start();
}

void Mesh::StopAllAnimations()
{
  map<string, Animation>::iterator it;
  for(it = Animations.begin(); it != Animations.end(); it++)
    it->second.Stop();
}

UINT Mesh::GetCurrentAnimationPose(XMFLOAT4X4& offsets) const
{
  if(CurrentAnimation)
  {
    CurrentAnimation->GetCurrentOffsets(&offsets);
    return CurrentAnimation->GetNumBones();
  }

  return 0;
}

int Mesh::GetNumBone(const string* name) const
{
  for(UINT i=0; i<Bones.size(); i++)
  {
    if(*name == Bones[i].name)
      return i;
  }

  return -1;
}

Bone* Mesh::GetBone(const string* name)
{
  for(UINT i=0; i<Bones.size(); i++)
  {
    if(*name == Bones[i].name)
      return &(Bones[i]);
  }

  return 0;
}

void Mesh::Tick(float dt)
{
  if(CurrentAnimation)
    CurrentAnimation->Tick(dt);
}