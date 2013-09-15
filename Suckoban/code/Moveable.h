#pragma once

#include "Model.h"
#include "Tween.h"
#include "GineUtils.h"

/// <summary>
/// Moveable object in game, such as a box or the player.
/// Object can be moved from one tile to another and rotated as well.
/// </summary>

class Moveable
{
public:
  Moveable() {}
  Moveable(Node position, const char* modelName);
  
  Model* GetModel() { return &mModel; }
  
  Node GetPosition() const { return mPos; }
  bool IsMoving()    const { return mPosX.IsOn() || mPosY.IsOn(); }
  bool IsRotating()  const { return mRotY.IsOn(); }

  void Move(Dir::Enum dir);
  void Rotate(Dir::Enum dir);
  void Tick(float dt);
  
private:
  /// <summary> Virtual position on level map. </summary>
  Node  mPos;
  /// <summary> Model data with real position. </summary>
  Model mModel;
  /// <summary> Direction at which a model if facing. </summary>
  Dir::Enum mDir;
  
  Tween mPosX;
  Tween mPosY;
  Tween mRotY;
};