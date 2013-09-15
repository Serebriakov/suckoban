#include "Moveable.h"

const float MOVE_DURR = 0.3f;
const float ROTATE_DURR = 0.1f;

Moveable::Moveable(Node position, const char* modelName)
{
  mModel = Model::Get(modelName);
  mModel.SetPosition(XMFLOAT3((FLOAT)position.x, 0.0f, (FLOAT)position.y));
  mPos = position;
  mDir = Dir::UP;
}

void Moveable::Move(Dir::Enum dir)
{
  if(IsMoving())
    return;
  
  switch(dir)
  {
  case Dir::UP: 
    mPosY.Start(NONE, MOVE_DURR, mModel.GetPosition().z, mModel.GetPosition().z + 1.0f);
    mPos.y++;
    break;
  case Dir::RIGHT:
    mPosX.Start(NONE, MOVE_DURR, mModel.GetPosition().x, mModel.GetPosition().x + 1.0f);
    mPos.x++;
    break;
  case Dir::DOWN:
    mPosY.Start(NONE, MOVE_DURR, mModel.GetPosition().z, mModel.GetPosition().z - 1.0f);
    mPos.y--;
    break;
  case Dir::LEFT:
    mPosX.Start(NONE, MOVE_DURR, mModel.GetPosition().x, mModel.GetPosition().x - 1.0f);
    mPos.x--;
    break;
  }
}

void Moveable::Rotate(Dir::Enum dir)
{
  Easing easing = NONE;
  float rotBy = 0.0f;

  // How I freaking hate implementing 2D rotations
  switch(dir)
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
    mRotY.Start(easing, ROTATE_DURR, mModel.GetRotation().y, mModel.GetRotation().y + rotBy);
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