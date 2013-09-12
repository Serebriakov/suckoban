#pragma once

#include <map>
#include "d3dUtil.h"
#include "Info.h"
using namespace std;
using Gine::Info;

namespace Gine {

  /// <summary>
  /// Simple tree of the bones. 
  /// A bone with null as a parent is a root of the skeleton. 
  /// </summary>

  struct Bone
  {
    string name;

    // Root -> Position
    XMFLOAT4X4 offset;

    // Position -> Parent bone
    XMFLOAT4X4 toParent;
    Bone* parent;

    // Position -> Root
    XMFLOAT4X4 toRoot;

    vector<Bone*> childs;

    void Print();
  };

  /// <summary>
  /// Transformation data for one bone at one frame
  /// </summary>

  struct Keyframe
  {
    XMFLOAT3 translation;
    XMFLOAT3 scale;
    XMFLOAT4 rotationQuat;

    void Print();
  };

  /// <summary>
  /// A key position of a skeleton in one frame with frame time
  /// </summary>

  struct Frame
  {
    // Absolute time of this frame in animation
    float time;
    map<Bone*, Keyframe> offsets;

    void Print();
  };

  /// <summary>
  /// Complete skeletal animation made of list of key frames
  /// </summary>

  class Animation
  {
  public:
    Animation();
    Animation(string aName, float aDuration, vector<Bone>* aBones, vector<Frame> aFrames);

    bool IsOn()  { return mIsOn; }

    void Start() { mIsOn = true; }
    void Stop()  { mIsOn = false; }
    void Reset();
  
    void GetCurrentOffsets(XMFLOAT4X4* aOffsets);
    UINT GetNumBones() { return mBones->size(); }
  
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

}