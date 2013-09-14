#pragma once

#include "Mesh.h"

namespace Gine
{
  /// <summary>
  /// Singleton class for loading and displaying 2D images
  /// </summary>

  class Sprite
  {
  public:
    friend class map<string, Sprite>;

    static bool Init();
    static void Destroy();

    static void Draw(const char* name, UINT x, UINT y);
    static void Draw(const char* name, UINT x, UINT y, float scale);
    static bool Load(const char* name);

    ~Sprite();

    XMFLOAT2 Size;
    XMFLOAT3 Scale;
    XMFLOAT3 Rotate;
    XMFLOAT3 Translate;

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