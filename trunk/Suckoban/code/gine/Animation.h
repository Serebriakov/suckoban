#pragma once

#include <string>
#include <vector>
#include <map>
#include "d3dUtil.h"
#include "Info.h"
using namespace std;
using Gine::Info;

struct Bone
{
  string name;
  XMFLOAT4X4 offset;
  XMFLOAT4X4 toParent;

  Bone* parent;
  vector<Bone*> childs;

  XMFLOAT4X4 toRoot;

  void Print()
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
};

struct Keyframe
{
  XMFLOAT3 translation;
  XMFLOAT3 scale;
  XMFLOAT4 rotationQuat;

  void Print()
  {
    Info::Log("Trs:\t%f\t%f\t%f", translation.x, translation.y, translation.z);
    Info::Log("Scl:\t%f\t%f\t%f", scale.x, scale.y, scale.z);
    Info::Log("Rtt:\t%f\t%f\t%f\t%f", rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w);
  }
};

struct Frame
{
  float time;
  map<Bone*, Keyframe> offsets;

  void Print()
  {
    Info::Log("Frame time:\t%d", time);
    map<Bone*, Keyframe>::iterator it;
    for(it = offsets.begin(); it != offsets.end(); it++)
    {
      Info::Log("Bone:\t%s",it->first->name.c_str());
      it->second.Print();
    }

    Info::Log("");
  }
};

class Animation
{
public:
  Animation();
  Animation(string aName, float aDuration, vector<Bone>* aBones,
            vector<Frame> aFrames);

  bool IsOn() { return mIsOn; };

  void Start();
  void Stop();
  void Reset();
  
  void GetCurrentOffsets(XMFLOAT4X4* aOffsets);
  UINT GetNumBones() { return mBones->size(); };
  
  void Tick(float dt);

private: 
  string mName;
  Bone* mBoneRoot;
  vector<Bone>* mBones;
  
  bool  mIsOn;
  float mDuration;
  float mTime;
  UINT  miFrame;
  vector<Frame> mFrames;

  int GetNumBone(Bone* aBone);
  void UpdateToRoot(Bone* aBone);
  XMMATRIX GetInterpolatedOffset(Bone* aBone);
};