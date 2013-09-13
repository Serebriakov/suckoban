#include "Material.h"
using namespace Gine;

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

Material* Material::New(const char* aName, XMFLOAT4 aAmbient, XMFLOAT4 aDiffuse, XMFLOAT4 aSpecular, 
                        const char* aTextureName)
{
  string name = aName;
  if(All.count(name) != 0)
    return &All[name];

  Material* newMat = Material::New(aName);
  MaterialColors matCols(aAmbient, aDiffuse, aSpecular);
  newMat->materialColors = matCols;
  if(aDiffuse.w > 0.0f)
    newMat->transparent = true;

  newMat->texture = aTextureName ? Texture::Get(aTextureName) : Texture::Get("white.png");

  // FIXME: newMat isn't added to a All map
  //        Should it be like that?
  return newMat;
}

Material* Material::Get(const char* aName)
{
  string name = aName;

  if(All.count(name) == 0)
    return 0;
 
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
