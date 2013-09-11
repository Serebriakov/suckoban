#include "Texture.h"
#include "Info.h"
#include "Gine.h"
using Gine::Info;

map<string, Texture*> Texture::All;

bool Texture::Load(const char* aName)
{
  Texture* texture = new Texture();
  wchar_t fileName[1024];
  string path;
  HRESULT result;

  // Diffuse
  path = "data/gfx/models/";
  path += aName;
  //path += ".png";
  mbstowcs(fileName, path.c_str(), 1024);
  
  result = D3DX11CreateShaderResourceViewFromFile(Gine::gDevice, fileName, 0, 0, 
                                                  &texture->diffuse, 0);
  if(FAILED(result))
  {
    Info::Fatal("Couldn't load texture: %s", path.c_str());
    return false;
  }

  //// Bump
  //path = "GFX/";
  //path += aName;
  //path += "_bump.png";
  //mbstowcs(fileName, path.c_str(), 1024);
  //
  //result = D3DX11CreateShaderResourceViewFromFile(mDevice, fileName, 0, 0,
  //                                                &texture->bump, 0);
  //if(FAILED(result))
  //{
  //  Info::Fatal("Couldn't load texture: %s", path.c_str());
  //  //return false;
  //}

  //// Specular
  //path = "GFX/";
  //path += aName;
  //path += "_spec.png";
  //mbstowcs(fileName, path.c_str(), 1024);
  //
  //result = D3DX11CreateShaderResourceViewFromFile(mDevice, fileName, 0, 0,
  //                                                &texture->specular, 0);
  //if(FAILED(result))
  //{
  //  Info::Fatal("Couldn't load texture: %s", path.c_str());
  //  //return false;
  //}

  texture->mName = aName;
  All[aName] = texture;
  
  return true;
}

Texture* Texture::Get(const char* aName)
{
  if(!All.count(aName))
  {
    Load(aName);
  }
 
  return All[aName];
}

Texture::Texture()
{
  diffuse = 0;
  bump = 0;
  specular = 0;
  mName = "Texture";
}

Texture::~Texture()
{
  if(diffuse)
    ReleaseCOM(diffuse);
  if(bump)
    ReleaseCOM(bump);
  if(specular)
    ReleaseCOM(specular);
}
