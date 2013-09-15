#include "Model.h"
#include <fstream>
#include <sstream>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include "Vertex.h"
#include "Info.h"
#include "Effects.h"
#include "RenderStates.h"
#include "Gine.h"
using namespace std;
using namespace Gine;

map<string, Model> Model::All;
ID3D11Buffer*      Model::mInstancedBuffer = 0;

const int MAX_INSTANCES = 200;
const BYTE MAX_BONES = 96;
const char* MODELS_PATH = "data/gfx/models/";
const char* FILE_FORMATS[2] = {".obj", ".x"};
const int N_FILE_FORMATS = sizeof(FILE_FORMATS) / sizeof(const char*);

// For SSAO mapping
XMMATRIX toTexSpace(
  0.5f,  0.0f, 0.0f, 0.0f,
	0.0f, -0.5f, 0.0f, 0.0f,
	0.0f,  0.0f, 1.0f, 0.0f,
	0.5f,  0.5f, 0.0f, 1.0f);

bool Model::Init()
{
  // Instanced buffer
  D3D11_BUFFER_DESC vbd;
  vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(InstancedData) * MAX_INSTANCES;
  vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	
  HRESULT result = Gine::gDevice->CreateBuffer(&vbd, 0, &mInstancedBuffer);
  if(FAILED(result))
  {
    Info::Fatal("Couldn't create instanced buffer");
    return false;
  }

  return true;
}

Model Model::Get(const char* aName)
{
  if(!All.count(aName))
    Load(aName);
 
  return All[aName];
}

bool Model::Load(const char* aName)
{
  string path = MODELS_PATH;
  path += aName;

  Model model;
  for(UINT i=0; i<N_FILE_FORMATS; i++)
  {
    string pathExt = path + FILE_FORMATS[i];
    if(model.LoadFile(pathExt.c_str()))
    { 
      model.mName = aName;
      All[aName] = model;

      Info::Log("Model loaded: %s", pathExt.c_str());
      return true;
    }
    // If mesh was made, but model not loaded properly
    else if(model.mMesh)
      delete model.mMesh;
  }

  Info::Fatal("Couldn't load a %s model", aName);
  return false;
}

void Model::Draw(vector<Model*>* aRenderList, ID3DX11EffectTechnique* aBasicTech, ID3DX11EffectTechnique* aInstancedTech)
{
  if(aRenderList->size() == 0)
    return;
  if(aRenderList->size() == 1)
  {
    DrawSingle(aRenderList, aBasicTech);
    return;
  }

  // TODO: Sort renderlist by meshes
  
  vector<Model*>& rl = *aRenderList;
  Mesh* mesh = rl[0]->mMesh;

  vector<Model*> singlesList;
  vector<Model*> instancedList;

  int arrayStart = 0;
  Mesh* currMesh;
  for(UINT i=1; i<rl.size() + 1; i++)
  {
    if(i < rl.size())
      currMesh = rl[i]->mMesh;
    if(currMesh != mesh || i == rl.size())
    {
      if(i-arrayStart == 1)
        singlesList.push_back(rl[i-1]);
      else
      {
        instancedList.clear();
        for(UINT j=arrayStart; j<i; j++)
        {
          instancedList.push_back(rl[j]);
        }
        DrawInstanced(&instancedList, aInstancedTech);
      }
      
      arrayStart = i;
      mesh = currMesh;
    }
  }
  DrawSingle(&singlesList, aBasicTech);
}

void Model::DrawSingle(vector<Model*>* aRenderList, ID3DX11EffectTechnique* aTech)
{
  if(aRenderList->size() == 0)
    return;

  vector<Model*>& rl = *aRenderList;

  Gine::gContext->IASetInputLayout(InputLayouts::Basic32);
  Gine::gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

  UINT stride = sizeof(Vertex::Basic32);
  UINT offset = 0;

  D3DX11_TECHNIQUE_DESC techDesc;
  aTech->GetDesc(&techDesc);

  XMMATRIX view = gCamera->View();
  XMMATRIX viewProj = gCamera->ViewProj();
  bool isSsaoTech = Effects::SsaoNormalDepthFX->NormalDepthAlphaClipTech == aTech ||
                    Effects::SsaoNormalDepthFX->NormalDepthTech == aTech;
  for(UINT i=0; i<rl.size(); i++)
  {
    Model& model = *rl[i];

    if(isSsaoTech && !model.CastSsao)
      continue;

    for(UINT p=0; p<techDesc.Passes; p++)
    {
      ID3DX11EffectPass* pass = aTech->GetPassByIndex(p);
      Gine::gContext->IASetVertexBuffers(0, 1, &model.mMesh->VertexBuffer, &stride, &offset);
      Gine::gContext->IASetIndexBuffer(model.mMesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

      //XMMATRIX world = XMLoadFloat4x4(&model.mMesh->mWorld) * model.GetWorldMatrix();
      XMMATRIX world = model.GetWorldMatrix();
      XMMATRIX worldView = world * view;
      XMMATRIX worldViewProj = world * viewProj;
      XMMATRIX worldInvTranspose = model.GetWorldInverseTransposeMatrix();

      if(isSsaoTech)
      {
			  XMMATRIX worldInvTransposeView = worldInvTranspose * view;

			  Effects::SsaoNormalDepthFX->SetWorldView(worldView);
			  Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
			  Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
			  Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());
        if(model.mMaterials.size() > 1)
          Effects::SsaoNormalDepthFX->SetDiffuseMap(model.mMaterials[1]->texture->Diffuse);
      }
      else
      {
        Effects::BasicFX->SetWorld(world);
        Effects::BasicFX->SetWorldView(worldView);
        Effects::BasicFX->SetWorldViewProj(worldViewProj);
        Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
        Effects::BasicFX->SetWorldViewProjTex(worldViewProj * toTexSpace);

        XMFLOAT4X4 offsets[MAX_BONES];
        UINT numBones = model.mMesh->GetCurrentAnimationPose(*offsets);
        Effects::BasicFX->SetBoneTransforms(offsets, numBones);
      }

      for(int i=0; i<model.mMesh->Subsets; i++)
      {
        int indexStart = model.mMesh->SubsetIndexStart[i];
        int indexDrawAmount = model.mMesh->SubsetIndexStart[i+1] - model.mMesh->SubsetIndexStart[i];

        if(!isSsaoTech)
        {
          Effects::BasicFX->SetMaterialColors(model.mMaterials[model.mSubsetMaterial[i]]->materialColors);
          Effects::BasicFX->SetDiffuseMap(model.mMaterials[model.mSubsetMaterial[i]]->texture->Diffuse);
          Effects::BasicFX->SetBumpMap(model.mMaterials[model.mSubsetMaterial[i]]->texture->Bump);
          Effects::BasicFX->SetSpecularMap(model.mMaterials[model.mSubsetMaterial[i]]->texture->Specular);
		      Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
        }

        pass->Apply(0, Gine::gContext);
        Gine::gContext->DrawIndexed(indexDrawAmount, indexStart, 0);
      }
    }
  }
}

void Model::DrawInstanced(vector<Model*>* aRenderList, ID3DX11EffectTechnique* aTech)
{
  if(aRenderList->size() == 0)
    return;

  // Create instanced buffer
  D3D11_MAPPED_SUBRESOURCE mappedData; 
  Gine::gContext->Map(mInstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD,
                      0, &mappedData);

  InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);
  for(UINT i=0; i<aRenderList->size(); ++i)
  {
    InstancedData instancedData;
    XMStoreFloat4x4(&instancedData.World, aRenderList->at(i)->GetWorldMatrix());
	  dataView[i] = instancedData;
  }

  Gine::gContext->Unmap(mInstancedBuffer, 0);

  // Get model
  Model& model = *aRenderList->at(0);

  // Draw instanced
  Gine::gContext->IASetInputLayout(InputLayouts::InstancedBasic32);
  Gine::gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
  float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

  UINT stride[2] = {sizeof(Vertex::Basic32), sizeof(InstancedData)};
  UINT offset[2] = {0,0};

  ID3D11Buffer* vbs[2] = {model.mMesh->VertexBuffer, mInstancedBuffer};

  XMMATRIX view = gCamera->View();
  XMMATRIX viewProj = gCamera->ViewProj();

  D3DX11_TECHNIQUE_DESC techDesc;
  aTech->GetDesc(&techDesc);

  for(UINT p=0; p<techDesc.Passes; p++)
  {
    ID3DX11EffectPass* pass = aTech->GetPassByIndex(p);
    Gine::gContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
    Gine::gContext->IASetIndexBuffer(model.mMesh->IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    XMMATRIX world = XMLoadFloat4x4(&model.mMesh->World);
    XMMATRIX worldView = world * view;
    XMMATRIX worldViewProj = world * viewProj;
    XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);

    Effects::BasicFX->SetWorld(world);
    Effects::BasicFX->SetWorldView(worldView);
    Effects::BasicFX->SetWorldViewProj(worldViewProj);
    Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
    Effects::BasicFX->SetWorldViewProjTex(toTexSpace);

    for(int i=0; i<model.mMesh->Subsets; i++)
    {
      int indexStart = model.mMesh->SubsetIndexStart[i];
      int indexDrawAmount = model.mMesh->SubsetIndexStart[i+1] - model.mMesh->SubsetIndexStart[i];

      Effects::BasicFX->SetMaterialColors(model.mMaterials[model.mSubsetMaterial[i]]->materialColors);
      Effects::BasicFX->SetDiffuseMap( model.mMaterials[model.mSubsetMaterial[i]]->texture->Diffuse);
      Effects::BasicFX->SetBumpMap(model.mMaterials[model.mSubsetMaterial[i]]->texture->Bump);
      Effects::BasicFX->SetSpecularMap(model.mMaterials[model.mSubsetMaterial[i]]->texture->Specular);
		  Effects::BasicFX->SetTexTransform(XMMatrixIdentity());

      pass->Apply(0, Gine::gContext);
      Gine::gContext->DrawIndexedInstanced(indexDrawAmount, aRenderList->size(), indexStart, 0, 0);
    }
  }
}

Model::Model()
{
  Reset();
}

void Model::Reset()
{
  mName       = "default";
  mCastShadow = false;
  mMesh       = 0;
  mParent     = 0;
  InFrustum   = true;
  CastSsao    = true;

  mScale    = XMFLOAT3(1.0f,1.0f,1.0f);
  mRotation = XMFLOAT3(0.0f,0.0f,0.0f);
  mPosition = XMFLOAT3(0.0f,0.0f,0.0f);
  
  XMStoreFloat4x4(&mScaleMatrix,   XMMatrixIdentity());
  XMStoreFloat4x4(&mRotationMatrix,XMMatrixIdentity());
  XMStoreFloat4x4(&mPositionMatrix,XMMatrixIdentity());
  XMStoreFloat4x4(&mWorldMatrix,   XMMatrixIdentity());

  mUpdateScale    = true;
  mUpdateRotation = true;
  mUpdatePosition = true;
  mUpdateWorld    = true;
}

Model::~Model()
{
}

XMMATRIX Model::GetWorldMatrix()
{
  XMMATRIX S, R, T, W;
  if(mUpdateScale)
  {
    S = XMMatrixScalingFromVector(XMLoadFloat3(&mScale));
    XMStoreFloat4x4(&mScaleMatrix, S);
    mUpdateWorld = true;
  }
  if(mUpdateRotation)
  {
    XMMATRIX RX = XMMatrixRotationX(mRotation.x);
    XMMATRIX RY = XMMatrixRotationY(mRotation.y);
    XMMATRIX RZ = XMMatrixRotationZ(mRotation.z);
    R = RX * RY * RZ;
    XMStoreFloat4x4(&mRotationMatrix, R);
    mUpdateWorld = true;
  }
  if(mUpdatePosition)
  {
    T = XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
    XMStoreFloat4x4(&mPositionMatrix, T);
    mUpdateWorld = true;
  }
  if(mUpdateWorld)
  {
    if(!mUpdateScale)
      S = XMLoadFloat4x4(&mScaleMatrix);
    if(!mUpdateRotation)
      R = XMLoadFloat4x4(&mRotationMatrix);
    if(!mUpdatePosition)
      T = XMLoadFloat4x4(&mPositionMatrix);

    W = S * R * T;
    if(mParent)
      W *= mParent->GetWorldMatrix();
    XMStoreFloat4x4(&mWorldMatrix, W);
    XMStoreFloat4x4(&mWorldInverseMatrix, XMMatrixInverse(&XMMatrixDeterminant(W), W));
    XMStoreFloat4x4(&mWorldInverseTransposeMatrix, MathHelper::InverseTranspose(W));

    mUpdateScale    = false;
    mUpdateRotation = false;
    mUpdatePosition = false;
    mUpdateWorld    = false;

    return W;
  }

  return XMLoadFloat4x4(&mWorldMatrix);
}

XMFLOAT4 ToXMFLOAT4(aiColor4D aCol)
{
  return XMFLOAT4(aCol.r,aCol.g,aCol.b,aCol.a);
}

XMFLOAT4X4 ToXMFLOAT4X4(aiMatrix4x4 aMatrix)
{
  XMMATRIX M = XMLoadFloat4x4(&XMFLOAT4X4(&aMatrix.a1));
  XMFLOAT4X4 FM;
  XMStoreFloat4x4(&FM, XMMatrixTranspose(M));
  return FM;
}

XMFLOAT3X3 ToXMFLOAT3X3(aiMatrix3x3 aMatrix)
{
  XMMATRIX M = XMLoadFloat3x3(&XMFLOAT3X3(&aMatrix.a1));
  XMFLOAT3X3 FM;
  XMStoreFloat3x3(&FM, XMMatrixTranspose(M));
  return FM;
}

bool Model::LoadFile(const char* aFileName)
{
  const aiScene* scene = aiImportFile(aFileName, aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

  if(scene)
  {
    mMesh = new Mesh();
    vector<Vertex::Basic32> vertices;
    vector<int> indices;

    // Materials
    for(unsigned int i=0; i<scene->mNumMaterials; i++)
    {
      aiMaterial* mat = scene->mMaterials[i];

      aiColor4D ambient;
      aiColor4D diffuse;
    	aiColor4D specular;

      aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &ambient);
      aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
      aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &specular);
      mat->Get(AI_MATKEY_OPACITY, diffuse.a);

      aiString name;
      mat->Get(AI_MATKEY_NAME,name);

      bool hasTexture = false;
      aiString textureName;
      if(mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
      {
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);
        hasTexture = true;
      }

      Material* mtl = Material::New(name.C_Str(), ToXMFLOAT4(ambient), ToXMFLOAT4(diffuse), ToXMFLOAT4(specular),
                                    hasTexture ? textureName.C_Str() : 0);
      mMaterials.push_back(mtl);

      mtl->materialColors.Ambient = mtl->materialColors.Diffuse;
    }

    // Mesh subsets
    XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
    XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
    XMVECTOR vMin = XMLoadFloat3(&vMinf3);
    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
    for(unsigned int i=0; i<scene->mNumMeshes; i++)
    {
      aiMesh* mesh = scene->mMeshes[i];

      // Vertex
      int vertexOff = vertices.size();
      unsigned int numVertices = mesh->mNumVertices;
      mMesh->SubsetIndexStart.push_back(vertexOff);
      mMesh->Subsets++;

      for(unsigned int i_vert = 0; i_vert < numVertices; i_vert++)
      {
        aiVector3D vertex = mesh->mVertices[i_vert];
        aiVector3D normal = mesh->mNormals[i_vert];
        aiVector3D uv     = mesh->mTextureCoords[0][i_vert];
        aiVector3D tan    = mesh->mTangents[i_vert];

        Vertex::Basic32 v;
        v.Pos      = XMFLOAT3(vertex.x,vertex.y,vertex.z);
        v.Normal   = XMFLOAT3(normal.x,normal.y,normal.z);
        v.Tex      = XMFLOAT2(uv.x, uv.y);
        v.TangentU = XMFLOAT3(tan.x, tan.y, tan.z);
        v.Weights  = XMFLOAT3(0.0f, 0.0f, 0.0f);
        v.BoneIndices[0] = MAX_BONES;
        v.BoneIndices[1] = MAX_BONES;
        v.BoneIndices[2] = MAX_BONES;
        v.BoneIndices[3] = MAX_BONES;
        vertices.push_back(v);

        vMin = XMVectorMin(vMin, XMLoadFloat3(&v.Pos));
        vMax = XMVectorMax(vMax, XMLoadFloat3(&v.Pos));
      }

      // Index
      for(unsigned int i_face = 0; i_face < mesh->mNumFaces; i_face++)
      {
        aiFace face = mesh->mFaces[i_face];
        for(unsigned int i_ind = 0; i_ind < face.mNumIndices; i_ind++)
        {
          indices.push_back(face.mIndices[i_ind]+vertexOff);
        }
      }

      // Material
      mSubsetMaterial.push_back(mesh->mMaterialIndex);

      // Bones
      if(mesh->HasBones() && !LoadBones(scene, mesh, &vertices, vertexOff))
        return false;
    }
    mMesh->SubsetIndexStart.push_back(vertices.size());
    
    mMesh->Vertices = vertices.size();
    mMesh->Indices = indices.size();

    // Bounding volumes
    mMesh->BoundingBox = new AxisAlignedBox();
    mMesh->BoundingSphere = new Sphere();
    XMVECTOR center = 0.5f * (vMin + vMax);
    XMStoreFloat3(&mMesh->BoundingBox->Center, center);
    XMStoreFloat3(&mMesh->BoundingBox->Extents, 0.5f * (vMax - vMin));
    XMVECTOR maxDistance = XMVectorReplicate(0.0f);
    for(UINT i=0; i<vertices.size(); i++)
    {
      XMVECTOR distance = XMVector3Length(XMLoadFloat3(&vertices[i].Pos) - center);
      maxDistance = XMVectorGreater(maxDistance, distance);
    }
    mMesh->BoundingSphere->Center = mMesh->BoundingBox->Center;
    mMesh->BoundingSphere->Radius = XMVectorGetX(maxDistance);

    // Animations
    // 1. Every bone is keyed for every keyframe
    // 2. No scaling
    if(scene->HasAnimations() && !LoadAnimations(scene))
      return false;

    // Buffers
    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex::Basic32) * mMesh->Vertices;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(Gine::gDevice->CreateBuffer(&vbd, &vinitData, &mMesh->VertexBuffer));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * mMesh->Indices;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(Gine::gDevice->CreateBuffer(&ibd, &iinitData, &mMesh->IndexBuffer));

    return true;
  }
  else
  {
    return false;
  }
}

void Model::SwitchMaterial(const char* aFrom, const char* aTo)
{
  Material* from = Material::Get(aFrom);
  Material* to   = Material::Get(aTo);
  if(from && to)
    for(UINT i=0; i<mMaterials.size(); i++)
      if(mMaterials[i] == from)
        mMaterials[i] = to;
}

void Model::CopyWithoutTransformations(const Model& aModel)
{
  mMesh           = aModel.mMesh;
  mMaterials      = aModel.mMaterials;
  mSubsetMaterial = aModel.mSubsetMaterial;
  mParent         = aModel.mParent;
  mCastShadow     = aModel.mCastShadow;
  CastSsao        = aModel.CastSsao;
  InFrustum       = aModel.InFrustum;
}

void Model::AnimationStart(string aName)
{
  if(!mMesh->Animations.count(aName))
    Info::Fatal("Couldn't find animation \"%s\" for \"%s\" model", aName.c_str(), mName.c_str());
  else
  {
    mMesh->StopAllAnimations();
    mMesh->StartAnimation(&aName);
  }
}

void Model::AnimationStop(string aName)
{
  mMesh->Animations[aName].Stop();
}

void Model::Tick(float dt)
{
  mMesh->Tick(dt);
}

bool Model::LoadBones(const aiScene* aScene, aiMesh* aMesh, vector<Vertex::Basic32>* aVertices, int aVertexOff)
{
  UINT numBones = aMesh->mNumBones;
  vector<string> boneParentNames;
  vector<Bone>& bones = mMesh->Bones;

  for(UINT j=0; j<numBones; j++)
  {
    // Bone data
    aiBone& boneData = *aMesh->mBones[j];
    aiNode& boneNode = *aScene->mRootNode->FindNode(boneData.mName);

    string     boneName = boneData.mName.C_Str();
    XMFLOAT4X4 boneOffset = ToXMFLOAT4X4(boneData.mOffsetMatrix);
    XMFLOAT4X4 boneToParent = ToXMFLOAT4X4(boneNode.mTransformation);

    // Create bone and add to a skeleton
    Bone bone = { boneName, boneOffset, boneToParent, 0 };
    bones.push_back(bone);

    // Save parent bone name for hierarchy 
    aiNode& parentNode = *boneNode.mParent;
    boneParentNames.push_back(parentNode.mName.C_Str());

    // Assign vertex weights
    for(UINT k=0; k<boneData.mNumWeights; k++)
    {
      UINT iVert = boneData.mWeights[k].mVertexId + aVertexOff;
      Vertex::Basic32& vertex = aVertices->at(iVert);

      UINT offset;
      for(offset=0; offset<4; offset++)
        if(vertex.BoneIndices[offset] == MAX_BONES)
          break;
      if(offset > 3)
      {
        Info::Log("Too much weights assigned to one vertex - skipping");
        continue;
      }

      vertex.BoneIndices[offset] = (BYTE)j;
      if(offset == 0)
        vertex.Weights.x = boneData.mWeights[k].mWeight;
      if(offset == 1)
        vertex.Weights.y = boneData.mWeights[k].mWeight;
      if(offset == 2)
        vertex.Weights.z = boneData.mWeights[k].mWeight;
    }
  }

  // Set parent bone pointers from bone names
  for(UINT j=0; j<numBones; j++)
  {
    string parentName = boneParentNames[j];
    for(UINT k=0; k<numBones; k++)
    {
      if(bones[k].name == parentName)
      {
        bones[j].parent = &bones[k];
        bones[k].childs.push_back(&bones[j]);
      }
    }
  }

  return true;
}

bool Model::LoadAnimations(const aiScene* aScene)
{
  for(UINT i=0; i<aScene->mNumAnimations; i++)
  {
    aiAnimation& anim = *aScene->mAnimations[i];

    // For every bone, get offsets in next frames
    vector<vector<Keyframe>> boneFrameOffset;
    vector<string> boneNames;
    vector<float> frameTimes;

    if(anim.mNumChannels != mMesh->Bones.size())
    {
      Info::Fatal("Number of animation channels is different than bones number");
      return false;
    }

    int lastNumOfKeys = anim.mChannels[0]->mNumPositionKeys;
    for(UINT j=0; j<anim.mNumChannels; j++)
    {
      aiNodeAnim& nodeAnim = *anim.mChannels[j];
      boneNames.push_back(nodeAnim.mNodeName.C_Str());

      if(nodeAnim.mNumPositionKeys != nodeAnim.mNumRotationKeys)
      {
        Info::Fatal("Number of position keys is different than rotation keys");
        return false;
      }
      int numOfKeys = nodeAnim.mNumPositionKeys;
      if(numOfKeys != lastNumOfKeys)
      {
        Info::Fatal("Number of animation keys for different bones is not equal");
        return false;
      }
      lastNumOfKeys = numOfKeys;

      vector<Keyframe> frameOffset;
      for(UINT k=0; k<numOfKeys; k++)
      {
        Keyframe keyframe;

        aiVector3D pos = nodeAnim.mPositionKeys[k].mValue;
        keyframe.translation = XMFLOAT3(pos.x, pos.y, pos.z);
            
        aiQuaternion rot = nodeAnim.mRotationKeys[k].mValue;
        keyframe.rotationQuat = XMFLOAT4(rot.x, rot.y, rot.z, rot.w);

        keyframe.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

        frameOffset.push_back(keyframe);
      }
      boneFrameOffset.push_back(frameOffset);

      // Fill frame times only from the first channel
      if(frameTimes.size() == 0)
      {
        for(UINT k=0; k<numOfKeys; k++)
        {
          aiQuatKey key = nodeAnim.mRotationKeys[k];
          frameTimes.push_back(key.mTime);
        }
      }
    }

    // Fill frames vector
    vector<Frame> frames;
    for(UINT i=0; i<frameTimes.size(); i++)
    {
      Frame frame;
      frame.time = frameTimes[i];

      for(UINT k=0; k<boneFrameOffset.size(); k++)
      {
        string boneName = boneNames[k];
        Bone* bone = mMesh->GetBone(&boneName);

        frame.offsets[bone] = boneFrameOffset[k][i];
      }

      frames.push_back(frame);
    }

    // Create mesh animation
    string animName = anim.mName.C_Str();
    Animation animation(animName, anim.mDuration, &mMesh->Bones, frames);
    mMesh->Animations[animName] = animation;
  }

  return true;
}