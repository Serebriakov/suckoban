#pragma once

#include <string>
#include <vector>
#include "Model.h"
#include "Moveable.h"
using namespace std;

/// <summary>
/// One level in game with function to load multiple of them.
/// </summary>

class Level
{
  /// <summary>
  /// Values that represent tiles.
  /// Using standard Sokoban tile naming conventions.
  /// Floor added for better visual representation.
  /// </summary>

  enum Tile
  {
    EMPTY       = ' ',
    WALL        = '#',
    PLAYER      = '@',
    PLAYER_GOAL = '+',
    BOX         = '$',
    BOX_GOAL    = '*',
    GOAL        = '.',
    FLOOR       = '_'
  };

public:

  /// <summary>
  /// Loads levels from Sokoban level standard file and stores them in levels vector.
  /// </summary>
  /// <param name="path">    Full pathname of the file. </param>
  /// <param name="levels">  [in,out] The levels vector. </param>
  /// <returns> true if it succeeds, false if it fails. </returns>

  static bool Load(const char* path, vector<Level>& levels);

  int    GetNumber() const { return mNumber; };
  string GetName()   const { return mName; };
  float  GetTime()   const { return mTime; };
  int    GetSteps()  const { return mSteps; };
  int    GetPushes() const { return mPushes; };
  bool   IsUndo()    const { return mUndo; };
  
  XMFLOAT3 GetPlayerPosition() { return mPlayer.GetModel()->GetPosition(); };
  bool IsSolved() const;
  
  void Reset();
  void Draw();
  void Tick(float dt);
  
private:
  Level();

  int    mNumber;
  string mName;
  float  mTime;
  int    mSteps;
  int    mPushes;
  bool   mUndo;
  
  // WxH tiles
  UINT mWidth;
  UINT mHeight;
  
  // Virtual char level representation, stored for level restarting
  vector<vector<Tile>> mMap;
  // Static objects in the level
  vector<Model> mObjects;
  // Boxes to move
  vector<Moveable> mBoxes;
  // Player data
  Moveable mPlayer;

  /// <summary>
  /// Flood fills a level map floor from player's position.
  /// </summary>
  void FillFloor();

  bool IsTileEmpty(Node aN);
  Moveable* GetBoxOnTile(Node aN);
};