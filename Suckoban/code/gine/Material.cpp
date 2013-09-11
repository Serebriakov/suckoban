#include "Material.h"

map<string, Material> Material::All;

Material* Material::New(const char* aName)
{
  string name = aName;

  if(All.count(name) == 0)
  {
    Material material;
    material.mName = name;
    All[name] = material;
  }
 
  return &All[name];
}

Material* Material::New(const char* aName, XMFLOAT4 aAmbient, XMFLOAT4 aDiffuse,
                        XMFLOAT4 aSpecular, const char* aTextureName)
{
  string name = aName;
  if(All.count(name) != 0)
  {
    return &All[name];
  }

  Material* newMat = Material::New(aName);
  MaterialColors matCols(aAmbient, aDiffuse, aSpecular);
  newMat->materialColors = matCols;
  if(aDiffuse.w > 0.0f)
  {
    newMat->transparent = true;
  }

  if(aTextureName)
    newMat->texture = Texture::Get(aTextureName);
  else
    newMat->texture = Texture::Get("white.png");

  return newMat;
}

Material* Material::Get(const char* aName)
{
  string name = aName;

  if(All.count(name) == 0)
  {
    return 0;
  }
 
  return &All[name];
}

Material::Material()
{
  Init();
}

void Material::Init()
{
  transparent = false;
  hasTexture = false;
  texture = 0;
}

Material::~Material()
{
}
