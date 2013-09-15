#pragma once

#include <Windows.h>
#include <xnamath.h>
#include <string>
#include <map>
#include "Texture.h"
using namespace std;

namespace Gine
{
  /// <summary> 
  /// Storing ambient, diffuse and specular colors
  /// </summary>

  struct MaterialColors
  {
	  MaterialColors() { ZeroMemory(this, sizeof(this)); }
    MaterialColors(XMFLOAT4 aAmbient, XMFLOAT4 aDiffuse, XMFLOAT4 aSpecular)
      : Ambient(aAmbient), Diffuse(aDiffuse), Specular(aSpecular) {}

	  XMFLOAT4 Ambient;
	  XMFLOAT4 Diffuse;
	  XMFLOAT4 Specular;
  };

  /// <summary>
  /// Model's material.
  /// Singleton materials container.
  /// </summary>

  class Material
  {
  public:
    // A little trick for granting access from map [] operator to Material
    friend class map<string, Material>;

    static map<string, Material> All;
    static Material* New(const char* aName);
    static Material* New(const char* aName, XMFLOAT4 aAmbient, XMFLOAT4 aDiffuse, XMFLOAT4 aSpecular,
                         const char* aTextureName);
    static Material* Get(const char* aName);
  
    void Init();

    ~Material();

    string GetName() { return mName;};

    MaterialColors materialColors;
    bool transparent;
  
	  bool hasTexture;
	  Texture* texture;

  private:
    Material();

    string mName;
  };

}