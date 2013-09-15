#include "Texture.h"
#include "Info.h"
#include "Gine.h"
using namespace Gine;

map<string, Texture*> Texture::All;

const char* TEXTURES_PATH = "data/gfx/models/";

bool Texture::Load(const char* name)
{
  Texture* texture = new Texture();
  wchar_t fileName[1024];
  string path;
  HRESULT result;

  // Diffuse
  path = TEXTURES_PATH;
  path += name;
  mbstowcs(fileName, path.c_str(), 1024);
  
  result = D3DX11CreateShaderResourceViewFromFile(Gine::gDevice, fileName, 0, 0, &texture->Diffuse, 0);
  if(FAILED(result)) {
    Info::Fatal("Couldn't load diffuse texture: %s", path.c_str());
    return false;
  }

  // TODO: Need to standarize loading *.obj and *.x files from Blender to load bump and spec maps like a boss
  // Bump
  //path = TEXTURES_PATH;
  //path += "_bump";
  //mbstowcs(fileName, path.c_str(), 1024);
  //
  //result = D3DX11CreateShaderResourceViewFromFile(Gine::gDevice, fileName, 0, 0, &texture->Bump, 0);
  //if(FAILED(result)) {
  //  Info::Fatal("Couldn't load bump texture: %s", path.c_str());
  //  return false;
  //}

  //// Specular
  //path = TEXTURES_PATH;
  //path += name;
  //path += "_spec";
  //mbstowcs(fileName, path.c_str(), 1024);
  //
  //result = D3DX11CreateShaderResourceViewFromFile(Gine::gDevice, fileName, 0, 0, &texture->Specular, 0);
  //if(FAILED(result)) {
  //  Info::Fatal("Couldn't load texture: %s", path.c_str());
  //  return false;
  //}

  texture->mName = name;
  All[name] = texture;
  
  return true;
}

Texture* Texture::Get(const char* name)
{
  if(!All.count(name)) {
    Load(name);
  }
 
  return All[name];
}

Texture::Texture()
{
  Diffuse = 0;
  Bump = 0;
  Specular = 0;
  mName = "Texture";
}

Texture::~Texture()
{
  ReleaseCOM(Diffuse);
  ReleaseCOM(Bump);
  ReleaseCOM(Specular);
}
