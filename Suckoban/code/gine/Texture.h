#pragma once
#include "d3dUtil.h"
#include <string>
#include <map>
using namespace std;

namespace Gine 
{
  /// <summary>
  /// Model's textures.
  /// Includes diffuse, bump and specular map.
  /// Singleton textures container.
  /// </summary>

  class Texture
  {
  public:
    static map<string, Texture*> All;
    static bool     Load(const char* name);
    static Texture* Get (const char* name);

    ID3D11ShaderResourceView* Diffuse;
    ID3D11ShaderResourceView* Bump;
    ID3D11ShaderResourceView* Specular;

    ~Texture();

  private:
    string mName;

    Texture();
  };

}

