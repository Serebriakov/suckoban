#pragma once
#include "gine/Model.h"
#include "gine/Tween.h"
using namespace Gine;

class Moveable
{
public:
  Moveable();
  Moveable(Node aN, const char* aModel);
  
  Model* GetModel() { return &mModel; };
  
  Node GetPosition() { return mPos; };
  bool IsMoving() { return mPosX.IsOn() || mPosY.IsOn(); };
  bool IsRotating() { return mRotY.IsOn(); };
  void Move(Dir::Enum aDir);
  void Rotate(Dir::Enum aDir);
  void Tick(float dt);
  
private:
  Model mModel;
  Node  mPos;
  Dir::Enum mDir;
  
  Tween mPosX;
  Tween mPosY;
  Tween mRotY;
};

Moveable::Moveable()
{
}

Moveable::Moveable(Node aN, const char* aModel)
{
  mModel = Model::Get(aModel);
  mModel.SetPosition(XMFLOAT3(aN.x, 0.0f, aN.y));
  mPos = aN;
  mDir = Dir::UP;
}

void Moveable::Move(Dir::Enum aDir)
{
  Easing::Enum easing = Easing::NONE;
  float time = 0.3f;
  
  switch(aDir)
  {
  case Dir::UP: 
    mPosY.Start(easing, time, mModel.GetPosition().z, 
                              mModel.GetPosition().z + 1.0f, true);
    mPos.y++;
    break;
  case Dir::RIGHT:
    mPosX.Start(easing, time, mModel.GetPosition().x, 
                              mModel.GetPosition().x + 1.0f, true);
    mPos.x++;
    break;
  case Dir::DOWN:
    mPosY.Start(easing, time, mModel.GetPosition().z, 
                              mModel.GetPosition().z - 1.0f, true);
    mPos.y--;
    break;
  case Dir::LEFT:
    mPosX.Start(easing, time, mModel.GetPosition().x, 
                              mModel.GetPosition().x - 1.0f, true);
    mPos.x--;
    break;
  }
}

void Moveable::Rotate(Dir::Enum aDir)
{
  Easing::Enum easing = Easing::NONE;
  float time = 0.1f;
  
  float rotBy = 0.0f;
  switch(aDir)
  {
  case Dir::UP: switch(mDir) {
    case Dir::RIGHT: rotBy = -MathHelper::Pi * 0.5f; break;
    case Dir::DOWN:  rotBy = -MathHelper::Pi * 1.0f; break;
    case Dir::LEFT:  rotBy = +MathHelper::Pi * 0.5f; break;
    }
    mDir = Dir::UP;
    break;
  case Dir::RIGHT: switch(mDir) {
    case Dir::DOWN:  rotBy = -MathHelper::Pi * 0.5f; break;
    case Dir::LEFT:  rotBy = -MathHelper::Pi * 1.0f; break;
    case Dir::UP:    rotBy = +MathHelper::Pi * 0.5f; break;
    }
    mDir = Dir::RIGHT;
    break;
  case Dir::DOWN: switch(mDir) {
    case Dir::LEFT:  rotBy = -MathHelper::Pi * 0.5f; break;
    case Dir::UP:    rotBy = -MathHelper::Pi * 1.0f; break;
    case Dir::RIGHT: rotBy = +MathHelper::Pi * 0.5f; break;
    }
    mDir = Dir::DOWN;
    break;
  case Dir::LEFT: switch(mDir) {
    case Dir::UP:    rotBy = -MathHelper::Pi * 0.5f; break;
    case Dir::RIGHT: rotBy = -MathHelper::Pi * 1.0f; break;
    case Dir::DOWN:  rotBy = +MathHelper::Pi * 0.5f; break;
    }
    mDir = Dir::LEFT;
    break;
  }
  
  if(rotBy != 0.0f)
    mRotY.Start(easing, time, mModel.GetRotation().y, 
                              mModel.GetRotation().y + rotBy, true);
}

void Moveable::Tick(float dt)
{
  mPosX.Tick(dt);
  mPosY.Tick(dt);
  mRotY.Tick(dt);
  
  if(mPosX.IsOn() || mPosX.JustFinished())
    mModel.Position().x = mPosX.GetValue();
  if(mPosY.IsOn() || mPosY.JustFinished())
    mModel.Position().z = mPosY.GetValue();
  if(mRotY.IsOn() || mRotY.JustFinished())
    mModel.Rotation().y = mRotY.GetValue();
}