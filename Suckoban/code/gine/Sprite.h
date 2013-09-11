#pragma once

#include "Mesh.h"

namespace Gine
{
  class Sprite
  {
  public:
    friend class map<string, Sprite>;

    static bool Init();
    static void Destroy();

    static void Draw(string aName, UINT aX, UINT aY);
    static void Draw(string aName, UINT aX, UINT aY, float aScale);
    static bool Load(string aName);

    ~Sprite();

    XMFLOAT2 size;
    XMFLOAT3 scale;
    XMFLOAT3 rotate;
    XMFLOAT3 translate;

  private:
    static map<string, Sprite> mSprites;
    static ID3D11Buffer* mQuadVB;
	  static ID3D11Buffer* mQuadIB;

    Sprite();

    static bool BuildQuadGeometryBuffers();
  
    XMFLOAT4X4 mWorld;
  
    ID3D11ShaderResourceView* mSpriteSRV;
  
    void Draw();
    void SetDimentions();
    void Transform();
  };
}