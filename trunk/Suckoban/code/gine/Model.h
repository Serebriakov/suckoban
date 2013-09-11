#pragma once

#include <assimp/scene.h>
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Vertex.h"

namespace Gine
{
  class Camera;

  struct InstancedData
  {
	  XMFLOAT4X4 World;
  };

  class Model
  {
  public:
    static map<string, Model> All;

    // Global objects management
    static bool  Init ();
    static Model Get  (const char* aName);
    static bool  Load (const char* aName);

    // Drawing
    static void  Draw          (vector<Model*>* aRenderList, ID3DX11EffectTechnique* aBasicTech, ID3DX11EffectTechnique* aInstancedTech);
    static void  DrawSingle    (vector<Model*>* aRenderList, ID3DX11EffectTechnique* aTech);
    static void  DrawInstanced (vector<Model*>* aRenderList, ID3DX11EffectTechnique* aTech);

    bool isInFrustum;
    bool castSsao;

    Model();
    ~Model();

    // Direct access
    XMFLOAT3& Scale()    { mUpdateScale    = true; return mScale;    }
    XMFLOAT3& Rotation() { mUpdateRotation = true; return mRotation; }
    XMFLOAT3& Position() { mUpdatePosition = true; return mPosition; }

    // Getters
    string   GetName()     { return mName;     }
    XMFLOAT3 GetScale()    { return mScale;    }
    XMFLOAT3 GetRotation() { return mRotation; }
    XMFLOAT3 GetPosition() { return mPosition; }
    XMMATRIX GetScaleMatrix()    { return XMLoadFloat4x4(&mScaleMatrix);    }
    XMMATRIX GetRotationMatrix() { return XMLoadFloat4x4(&mRotationMatrix); }
    XMMATRIX GetPositionMatrix() { return XMLoadFloat4x4(&mPositionMatrix); }
    XMMATRIX GetWorldMatrix();
    XMMATRIX GetWorldInverseMatrix()         { return XMLoadFloat4x4(&mWorldInverseMatrix); }
    XMMATRIX GetWorldInverseTransposeMatrix() { return XMLoadFloat4x4(&mWorldInverseTransposeMatrix); }
    AxisAlignedBox* GetAxisAlignedBoundingBox() { return mMesh ? mMesh->mAxisAlignedBoundingBox : 0; }
    Sphere*         GetBoundingSphere()         { return mMesh ? mMesh->mBoundingSphere : 0; }

    // Setters
    void SetScale    (XMFLOAT3 aScale)    { mUpdateScale    = true; mScale = aScale;       }
    void SetRotation (XMFLOAT3 aRotation) { mUpdateRotation = true; mRotation = aRotation; }
    void SetPosition (XMFLOAT3 aPosition) { mUpdatePosition = true; mPosition = aPosition; }
    void SetParent   (Model* aParent)     { mParent = aParent; }

    // General factor for proportional scaling
    float GetGeneralScale ()             { return mScale.x; };
    void  SetGeneralScale (float aScale) { mUpdateScale = true; mScale = XMFLOAT3(aScale,aScale,aScale); }

    // Special methods
    void SwitchMaterial             (const char* aFrom, const char* aTo);
    void CopyWithoutTransformations (const Model& aModel);

    // Operators
    bool operator<(const Model& aModel) { return mMesh < aModel.mMesh; };

    // Animations
    void AnimationStart(string aName);
    void AnimationStop(string aName);
    void Tick(float dt);

  private:
    static ID3D11Buffer* mInstancedBuffer;

    string            mName;
    Mesh*             mMesh;
    vector<Material*> mMaterials;
    vector<int>       mSubsetMaterial;
    bool              mCastShadow;
    Model*            mParent;

    XMFLOAT3 mScale;
    XMFLOAT3 mRotation;
    XMFLOAT3 mPosition;

    bool mUpdateScale;
    bool mUpdateRotation;
    bool mUpdatePosition;
    bool mUpdateWorld;

    XMFLOAT4X4 mScaleMatrix;
    XMFLOAT4X4 mRotationMatrix;
    XMFLOAT4X4 mPositionMatrix;
    XMFLOAT4X4 mWorldMatrix;
    XMFLOAT4X4 mWorldInverseMatrix;
    XMFLOAT4X4 mWorldInverseTransposeMatrix;

    void Reset();

    // File loading utils
    bool LoadFile      (const char* aFileName);
    bool LoadBones     (const aiScene* aScene, aiMesh* aMesh,
                        vector<Vertex::Basic32>* aVertices, int aVertexOff);
    bool LoadAnimations(const aiScene* aScene);
  };
}