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

namespace Gine {

  /// <summary>
  /// Unique model's mesh with default transformation
  /// </summary>

  class Mesh
  {
  public:
	  Mesh();
	  ~Mesh();

    void Init();
    void Tick(float dt);

    void SetScale(const float scale) { Scale = XMFLOAT3(scale, scale, scale); }
    void Transform();

    // Vertices and indices data
	  ID3D11Buffer* VertexBuffer;
    int Vertices;
    ID3D11Buffer* IndexBuffer;
    int Indices;

    // Mesh orientation data
    XMFLOAT3 Scale;
    XMFLOAT3 Rotate;
    XMFLOAT3 Translate;
    XMFLOAT4X4 World;

    // Subsets for materials
    int Subsets;
    vector<int> SubsetIndexStart;

    // Bounding geometry for collisions
    AxisAlignedBox* BoundingBox;
    Sphere* BoundingSphere;

    // Animation structures
    vector<Bone> Bones;
    map<string, Animation> Animations;
    Animation* CurrentAnimation;

    void StartAnimation(const string* name);
    void StopAllAnimations();

    /// <summary>
    /// Stores current animaton pose in a matrix passed as a offsets param
    /// </summary>
    /// <param name="offsets">  [in,out] A matrix to store bones offsets data </param>
    /// <returns> A number of bones in current animation </returns>

    UINT GetCurrentAnimationPose(XMFLOAT4X4& offsets) const;

    int GetNumBone(const string* name) const;
    Bone* GetBone(const string* name);
  };

}