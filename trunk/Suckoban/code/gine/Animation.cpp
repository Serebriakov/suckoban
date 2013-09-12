#pragma once

#include "Animation.h"
using namespace Gine;

const XMVECTOR ZERO = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
const float ANIMATION_SPEED_FACTOR = 50.0f;

void Bone::Print()
{
  Info::Log("Bone:\t%s", name.c_str());

  if(parent)
    Info::Log("Parent:\t%s", parent->name.c_str());
  else
    Info::Log("Parent:\tNULL");

  if(childs.size() > 0)
  {
    Info::Log("Childs:\t%d", childs.size());
    for(UINT i=0; i<childs.size(); i++)
      Info::Log("%d:\t%s", i, childs[i]->name.c_str());
  }
  else
    Info::Log("Childs:\t0", name.c_str());

    
  Info::Log("Offset:");
  Info::Print(offset);
    
  Info::Log("To parent:");
  Info::Print(toParent);

  Info::Log("To root:");
  Info::Print(toRoot);

  Info::Log("");
}

void Keyframe::Print()
{
  Info::Log("Trs:\t%f\t%f\t%f", translation.x, translation.y, translation.z);
  Info::Log("Scl:\t%f\t%f\t%f", scale.x, scale.y, scale.z);
  Info::Log("Rtt:\t%f\t%f\t%f\t%f", rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w);
}

void Frame::Print() 
{
  Info::Log("Frame duration:\t%d", time);
  map<Bone*, Keyframe>::iterator it;
  for(it = offsets.begin(); it != offsets.end(); it++)
  {
    Info::Log("Bone:\t%s",it->first->name.c_str());
    it->second.Print();
  }

  Info::Log("");
}

Animation::Animation() :
  mName("defaultAnimation"),
  mBones(0),
  mBoneRoot(0),
  mIsOn(false),
  mDuration(0.0f),
  mTime(0.0f),
  miFrame(0)
{
}

Animation::Animation(string aName, float aDuration, vector<Bone>* aBones,
                     vector<Frame> aFrames) :
  mName(aName),
  mBones(aBones),
  mIsOn(false),
  mDuration(aDuration),
  mTime(0.0f),
  miFrame(0),
  mFrames(aFrames)
{
  for(UINT i=0; i<mBones->size(); i++)
    if(!mBones->at(i).parent)
      mBoneRoot = &mBones->at(i);
}

void Animation::Reset()
{
  mTime = 0.0f;
  miFrame = 0;
}

int Animation::GetNumBone(Bone* aBone)
{
  for(UINT i=0; i<mBones->size(); i++)
  {
    if(aBone == &mBones->at(i))
      return i;
  }

  return -1;
}

XMMATRIX Animation::GetInterpolatedOffset(Bone* aBone)
{
  Keyframe* currKey = &mFrames[miFrame].offsets[aBone];
  Keyframe* nextKey = &mFrames[miFrame+1].offsets[aBone];
  float thisTime = mFrames[miFrame].time;
  float nextTime = mFrames[miFrame+1].time;

  float lerpPercent = (mTime - thisTime) / (nextTime - thisTime);

  XMVECTOR t0 = XMLoadFloat3(&currKey->translation);
  XMVECTOR t1 = XMLoadFloat3(&nextKey->translation);
  XMVECTOR s0 = XMLoadFloat3(&currKey->scale);
  XMVECTOR s1 = XMLoadFloat3(&nextKey->scale);
  XMVECTOR r0 = XMLoadFloat4(&currKey->rotationQuat);
  XMVECTOR r1 = XMLoadFloat4(&nextKey->rotationQuat);

  XMVECTOR T = XMVectorLerp(t0, t1, lerpPercent);
  XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
  XMVECTOR R = XMQuaternionSlerp(r0, r1, lerpPercent);

  return XMMatrixAffineTransformation(S, ZERO, R, T);
}

void Animation::UpdateToRoot(Bone* aBone)
{
  XMMATRIX toParentM = XMLoadFloat4x4(&aBone->toParent);
  XMMATRIX frameOffsetM = GetInterpolatedOffset(aBone);

  XMMATRIX toRootM;
  if(aBone->parent)
  {
    XMMATRIX toRootParentM = XMLoadFloat4x4(&aBone->parent->toRoot);
    toRootM = frameOffsetM * toRootParentM;
  }
  else
  {
    toRootM = frameOffsetM;
  }
    
  XMStoreFloat4x4(&aBone->toRoot, toRootM);
  
  for(UINT i=0; i<aBone->childs.size(); i++)
    UpdateToRoot(aBone->childs[i]);
}

void Animation::GetCurrentOffsets(XMFLOAT4X4* aOffsets)
{
  for(UINT i=0; i<GetNumBones(); i++)
  {
    XMMATRIX offsetM = XMLoadFloat4x4(&mBones->at(i).offset);
    XMMATRIX toRootM = XMLoadFloat4x4(&mBones->at(i).toRoot);

    XMStoreFloat4x4(&aOffsets[i], offsetM * toRootM);
  }
}

void Animation::Tick(float dt)
{
  if(mIsOn)
  {
    mTime += dt * ANIMATION_SPEED_FACTOR;
    
    if(mTime > mDuration)
    {
      mTime = 0.0f;
      miFrame = 0;
    }

    if(mTime > mFrames[miFrame+1].time)
      miFrame++;

    UpdateToRoot(mBoneRoot);
  }
}