#include "Sprite.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include "Vertex.h"
#include "RenderStates.h"
#include "Gine.h"
using namespace Gine;

ID3D11Buffer* Sprite::mQuadVB = 0;
ID3D11Buffer* Sprite::mQuadIB = 0;
map<string, Sprite> Sprite::mSprites;

//------------------------------------------------------------------------------
// PUBLIC STATIC
//------------------------------------------------------------------------------

bool Sprite::Init()
{
  return mQuadVB || BuildQuadGeometryBuffers();
}

void Sprite::Destroy()
{
  ReleaseCOM(mQuadVB);
  ReleaseCOM(mQuadIB);

  map<string, Sprite>::iterator it;
  for(it = mSprites.begin(); it != mSprites.end(); it++)
    ReleaseCOM(it->second.mSpriteSRV);
}

void Sprite::Draw(string aName, UINT aX, UINT aY)
{
  if(mSprites.count(aName) || Load(aName))
  {
    mSprites[aName].translate = XMFLOAT3(aX, aY, 0.0f);
    mSprites[aName].Transform();
    mSprites[aName].Draw();
  }
}

void Sprite::Draw(string aName, UINT aX, UINT aY, float aScale)
{
  if(mSprites.count(aName) || Load(aName))
  {
    mSprites[aName].translate = XMFLOAT3(aX, aY, 0.0f);
    mSprites[aName].scale = XMFLOAT3(aScale, aScale, 1.0f);
    mSprites[aName].Transform();
    mSprites[aName].Draw();
  }
}

bool Sprite::Load(string aName)
{
  if(mSprites.count(aName))
    return true;

  Sprite sprite;

  string path = "data/gfx/sprites/";
  path += aName;
  path += ".png";
  wchar_t fileName[1024];
  mbstowcs(fileName, path.c_str(), 1024);
  
  HRESULT result = D3DX11CreateShaderResourceViewFromFile(
    Gine::gDevice, fileName, 0, 0, &sprite.mSpriteSRV, 0);

  if(FAILED(result))
  {
    Info::Fatal("Couldn't load a %s sprite", aName.c_str());
    return false;
  }

  sprite.SetDimentions();
  mSprites[aName] = sprite;
  Info::Log("Sprite loaded: %s", path.c_str());

  return true;
}

//------------------------------------------------------------------------------
// PUBLIC
//------------------------------------------------------------------------------

Sprite::Sprite()
{
  mSpriteSRV = 0;
  
  scale = XMFLOAT3(1.0f,1.0f,1.0f);
  rotate = XMFLOAT3(0.0f,0.0f,0.0f);
  translate = XMFLOAT3(0.0f,0.0f,0.0f);
  XMStoreFloat4x4(&mWorld,XMMatrixIdentity());
}

Sprite::~Sprite()
{
}

void Sprite::Draw()
{
  float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};

  Gine::gContext->IASetInputLayout(InputLayouts::Basic32);
  Gine::gContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  UINT stride = sizeof(Vertex::Basic32);
  UINT offset = 0;

  XMMATRIX identity = XMMatrixIdentity();

  // World matrix does all the job 
  // to output pixel-precise sprites transformation
  XMMATRIX world = XMLoadFloat4x4(&mWorld);
  XMMATRIX screenScale = XMMatrixScaling(2.0f / Gine::gScreenW,
                                         2.0f / Gine::gScreenH, 1.0f);
  XMMATRIX screenTranslate = XMMatrixTranslation(-1.0f, 1.0f, 0.0f);
  world *= screenScale * screenTranslate;

  ID3DX11EffectTechnique* spriteTech = Effects::BasicFX->SpriteTech;
  D3DX11_TECHNIQUE_DESC techDesc;
  spriteTech->GetDesc(&techDesc);
  
  for(UINT p = 0; p < techDesc.Passes; ++p)
  {
    Gine::gContext->IASetVertexBuffers(0, 1, &mQuadVB, &stride, &offset);
    Gine::gContext->IASetIndexBuffer(mQuadIB, DXGI_FORMAT_R32_UINT, 0);

    Effects::BasicFX->SetWorld(world);
    Effects::BasicFX->SetDiffuseMap(mSpriteSRV);

    // No texture transforms for the moment
    Effects::BasicFX->SetTexTransform(identity);

    // Save blend states
    ID3D11BlendState* prevState = 0;
    FLOAT prevFactor[4];
    UINT prevMask = 0;
    Gine::gContext->OMGetBlendState(&prevState, prevFactor, &prevMask); 

    // Render
    Gine::gContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 
                                    0xffffffff);
    spriteTech->GetPassByIndex(p)->Apply(0, Gine::gContext);
    Gine::gContext->DrawIndexed(6, 0, 0);

    // Recover blend states
    Gine::gContext->OMSetBlendState(prevState, prevFactor, prevMask);
  }
}

//------------------------------------------------------------------------------
// PRIVATE STATIC
//------------------------------------------------------------------------------

bool Sprite::BuildQuadGeometryBuffers()
{
	GeometryGenerator::MeshData quad;
	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);

	std::vector<Vertex::Basic32> vertices(quad.Vertices.size());

	for(UINT i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i].Pos    = quad.Vertices[i].Position;
		vertices[i].Normal = quad.Vertices[i].Normal;
		vertices[i].Tex    = quad.Vertices[i].TexC;
	}

  HRESULT result;

  D3D11_BUFFER_DESC vbd;
  vbd.Usage = D3D11_USAGE_IMMUTABLE;
  vbd.ByteWidth = sizeof(Vertex::Basic32) * quad.Vertices.size();
  vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vbd.CPUAccessFlags = 0;
  vbd.MiscFlags = 0;
  D3D11_SUBRESOURCE_DATA vinitData;
  vinitData.pSysMem = &vertices[0];
  result = Gine::gDevice->CreateBuffer(&vbd, &vinitData, &mQuadVB);
  if(FAILED(result))
  {
    Info::Fatal("Couldn't create sprite's vertex buffer");
    return false;
  }

  D3D11_BUFFER_DESC ibd;
  ibd.Usage = D3D11_USAGE_IMMUTABLE;
  ibd.ByteWidth = sizeof(UINT) * quad.Indices.size();
  ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  ibd.CPUAccessFlags = 0;
  ibd.MiscFlags = 0;
  D3D11_SUBRESOURCE_DATA iinitData;
  iinitData.pSysMem = &quad.Indices[0];
  result = Gine::gDevice->CreateBuffer(&ibd, &iinitData, &mQuadIB);
  if(FAILED(result))
  {
    Info::Fatal("Couldn't create sprite's index buffer");
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
// PRIVATE
//------------------------------------------------------------------------------

void Sprite::Transform()
{
  // Resize quad to pixel=-precize screen size
  XMMATRIX screenTranslate = XMMatrixTranslation(1.0f, -1.0f, 0.0f);
  XMMATRIX screenScale = XMMatrixScaling(Gine::gScreenW / 2.0f,
                                         Gine::gScreenH / 2.0f, 1.0f);

  // Resize quad to original sprite size
  XMMATRIX spriteSize = XMMatrixScaling(size.x / Gine::gScreenW,
                                        size.y / Gine::gScreenH, 1.0f);
  XMMATRIX spriteScale = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

  XMMATRIX S = screenTranslate * screenScale * spriteSize * spriteScale;
  XMMATRIX RX = XMMatrixRotationX(rotate.x);
  XMMATRIX RY = XMMatrixRotationY(rotate.y);
  XMMATRIX RZ = XMMatrixRotationZ(rotate.z);
  XMMATRIX R = RX*RY*RZ;
  translate.y *= -1.0f;
  XMMATRIX T = XMMatrixTranslationFromVector(XMLoadFloat3(&translate));
  
  XMStoreFloat4x4(&mWorld, S*R*T);
}

void Sprite::SetDimentions()
{
  if(mSpriteSRV)
  {
    ID3D11Resource* resource;
    ID3D11Texture2D* texResource;
    D3D11_TEXTURE2D_DESC desc;

    mSpriteSRV->GetResource(&resource);
    texResource = reinterpret_cast<ID3D11Texture2D*>(resource);
    texResource->GetDesc(&desc);
    size = XMFLOAT2(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
  }
}