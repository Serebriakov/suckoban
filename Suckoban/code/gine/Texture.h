#pragma once
#include "d3dUtil.h"
#include <string>
#include <map>
using namespace std;

class Texture
{
public:
  static map<string, Texture*> All;
  static bool Load(const char* aName);
  static Texture* Get(const char* aName);

  ID3D11ShaderResourceView* diffuse;
  ID3D11ShaderResourceView* bump;
  ID3D11ShaderResourceView* specular;

  ~Texture();

private:
  string mName;

  Texture();
};

