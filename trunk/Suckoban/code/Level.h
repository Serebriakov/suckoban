#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include "gine/Model.h"
#include "gine/Info.h"
#include "gine/Effects.h"
#include "gine/Input.h"
#include "gine/Sprite.h"
#include "Moveable.h"
using namespace std;
using Gine::Info;

namespace Tile { enum Enum
{
  EMPTY       = ' ',
  WALL        = '#',
  PLAYER      = '@',
  PLAYER_GOAL = '+',
  BOX         = '$',
  BOX_GOAL    = '*',
  GOAL        = '.',
  FLOOR       = '_'
};}

class Level
{
public:
  static bool Load(const char* aPath, vector<Level>& aLevels);

  int    GetNumber() { return mNumber; };
  string GetName()   { return mName; };
  float  GetTime()   { return mTime; };
  int    GetSteps()  { return mSteps; };
  int    GetPushes() { return mPushes; };
  bool   IsUndo()    { return mUndo; };
  
  XMFLOAT3 GetPlayerPosition() { return mPlayer.GetModel()->GetPosition(); };
  bool IsSolved();
  
  void Reset();
  void Draw();
  void Tick(float dt);
  
private:
  Level();

  int mNumber;
  string mName;
  float mTime;
  int mSteps;
  int mPushes;
  bool mUndo;
  
  UINT mWidth;
  UINT mHeight;
  
  vector<vector<Tile::Enum>> mMap;
  vector<Model> mObjects;
  vector<Moveable> mBoxes;
  Moveable mPlayer;
  
  void FillFloor();
  bool IsTileEmpty(Node aN);
  Moveable* GetBoxOnTile(Node aN);
};

bool Level::Load(const char* aPath, vector<Level>& aLevels)
{
  fstream file(aPath);
  
  if(file.is_open())
  {
    string line;
    int number = 0;
    while(!file.eof())
    {
      getline(file, line);
      if(line.length() == 0 || line[0] == ';')
        continue;
        
      Level level;
      vector<Tile::Enum> row;
      number++;
      
      while(line.length() != 0 && line[0] != ';')
      {
        row.clear();
        
        for(UINT i=0; i<line.length(); i++)
          row.push_back(Tile::Enum(line[i]));
          
        if(row.size() > level.mWidth)
          level.mWidth = row.size();
        level.mMap.push_back(row);
        getline(file, line);
      }
      
      level.mNumber = number;
      level.mName = "";
      level.mHeight = level.mMap.size();    
      level.FillFloor();
      aLevels.push_back(level);
    }

    file.close();
    return true;
  }
  else
  {
    Info::Fatal("Couldn't open data file %s", aPath);
    return false;
  }
}

Level::Level() : 
  mNumber(0),
  mName(""),
  mWidth(0),
  mHeight(0),
  mSteps(0),
  mPushes(0),
  mUndo(false),
  mTime(0.0f)
{
}

bool Level::IsSolved()
{
  for(UINT i=0; i<mBoxes.size(); i++)
  {
    if(mBoxes[i].IsMoving())
      return false;
    Node boxPos = mBoxes[i].GetPosition();
    Node mapPos(-(boxPos.y - mHeight/2), boxPos.x + mWidth/2);
    Tile::Enum tile = mMap[mapPos.x][mapPos.y];
    if(Tile::GOAL != tile &&
       Tile::PLAYER_GOAL != tile &&
       Tile::BOX_GOAL != tile)
      return false;
  }
  
  return true;
}

void Level::Reset()
{
  mSteps = 0;
  mPushes = 0;
  mUndo = false;
  mTime = 0.0f;

  mObjects.clear();
  mBoxes.clear();
  
  for(UINT i=0; i<mMap.size(); i++)
    for(UINT j=0; j<mMap[i].size(); j++)
    {
      Node realPos(j - mWidth/2, -i + mHeight/2);
      int nObjects = mObjects.size();
      switch(mMap[i][j])
      {
      case Tile::WALL:
        mObjects.push_back(Model::Get("wall"));
        mObjects.push_back(Model::Get("floor"));
        break;
      case Tile::PLAYER_GOAL:
        mPlayer = Moveable(realPos, "player");
        mObjects.push_back(Model::Get("goal"));
        break;
      case Tile::PLAYER:
        mPlayer = Moveable(realPos, "player");
        mObjects.push_back(Model::Get("floor"));
        break; 
      case Tile::BOX_GOAL:
        mBoxes.push_back(Moveable(realPos, "box"));
        mObjects.push_back(Model::Get("goal"));
        break;
      case Tile::BOX:
        mBoxes.push_back(Moveable(realPos, "box"));
        mObjects.push_back(Model::Get("floor"));
        break; 
      case Tile::GOAL:
        mObjects.push_back(Model::Get("goal"));
        break;
      case Tile::FLOOR:
        mObjects.push_back(Model::Get("floor"));
        break;
      }
      
      for(UINT i=nObjects; i<mObjects.size(); i++)
        mObjects[i].SetPosition(XMFLOAT3(realPos.x, 0.0f, realPos.y));
    }
    
  sort(mObjects.begin(), mObjects.end());

  mPlayer.GetModel()->AnimationStart("Idle");
}
  
void Level::Draw()
{
  vector<Model*> renderList;
  for(UINT i=0; i<mObjects.size(); i++)
    renderList.push_back(&mObjects[i]);
  
  for(UINT i=0; i<mBoxes.size(); i++)
    renderList.push_back(mBoxes[i].GetModel());
    
  renderList.push_back(mPlayer.GetModel());
  
  Model::Draw(&renderList, Effects::BasicFX->BasicTech,
                           Effects::BasicFX->InstancedTech); 
}
  
void Level::Tick(float dt)
{
  mTime += dt;

  for(UINT i=0; i<mBoxes.size(); i++)
    mBoxes[i].Tick(dt);

  mPlayer.Tick(dt);
  mPlayer.GetModel()->Tick(dt);
  
  // For undo
  static Dir::Enum lastDir;
  static Moveable* lastBox;

  Dir::Enum dir;
  if(!mPlayer.IsMoving())
  {
    if(Input::PressedDir(dir))
    {
      Node nodeAtDir = mPlayer.GetPosition().GetAt(dir);
      if(!mPlayer.IsRotating())
        mPlayer.Rotate(dir);

      if(IsTileEmpty(nodeAtDir))
      {
        mPlayer.Move(dir);
        mPlayer.GetModel()->AnimationStart("Walk");
        mSteps++;
        mUndo = false;
      }
      else
      {
        Moveable* boxAtDir = GetBoxOnTile(nodeAtDir);
        Node behindTheBox = nodeAtDir.GetAt(dir);
        if(boxAtDir && IsTileEmpty(behindTheBox))
        {
          mPlayer.Move(dir);
          mPlayer.GetModel()->AnimationStart("Push");
          boxAtDir->Move(dir);
          mSteps++;
          mPushes++;
          mUndo = true;
          lastDir = dir;
          lastBox = boxAtDir;
        }
      }
    }
    else if(Input::Released('Z') && mUndo)
    {
      lastBox->Move(Dir::OppositeDir(lastDir));
      mPlayer.Move(Dir::OppositeDir(lastDir));
      mPlayer.GetModel()->AnimationStart("Push");
      mUndo = false;
    }
    else
    {
      mPlayer.GetModel()->AnimationStart("Idle");
    }
  }
}

void Level::FillFloor()
{
  // find player position and make a virtual map with walls only
  Node node;
  vector<vector<Tile::Enum>> wallsAndFloor = mMap;
  for(UINT i=0; i<mMap.size(); i++)
    for(UINT j=0; j<mMap[i].size(); j++)
    {
      if(Tile::PLAYER == mMap[i][j] || Tile::PLAYER_GOAL == mMap[i][j])
        node = Node(i, j);
      
      wallsAndFloor[i][j] = Tile::WALL == mMap[i][j] ? Tile::WALL : Tile::EMPTY;
    }
  
  // start a flood fill from player position
  vector<Node> q;
  q.push_back(node);
  while(!q.empty())
  {
    node = q.back();
    q.pop_back();
    if(Tile::EMPTY == wallsAndFloor[node.x][node.y])
    {
      wallsAndFloor[node.x][node.y] = Tile::FLOOR;
      q.push_back(node.GetAt(Dir::UP));
      q.push_back(node.GetAt(Dir::RIGHT));
      q.push_back(node.GetAt(Dir::DOWN));
      q.push_back(node.GetAt(Dir::LEFT));
    }
  }
  
  // mask virual map with real one
  for(UINT i=0; i<mMap.size(); i++)
    for(UINT j=0; j<mMap[i].size(); j++)
    {
      if(Tile::FLOOR == wallsAndFloor[i][j] && Tile::EMPTY == mMap[i][j])
        mMap[i][j] = Tile::FLOOR;
    }
}

bool Level::IsTileEmpty(Node aN)
{
  Node mapPos(-(aN.y - mHeight/2), aN.x + mWidth/2);
  if('#' == mMap[mapPos.x][mapPos.y])
    return false;
    
  return !GetBoxOnTile(aN);
}

Moveable* Level::GetBoxOnTile(Node aN)
{
  for(UINT i=0; i<mBoxes.size(); i++)
    if(mBoxes[i].GetPosition() == aN)
      return &mBoxes[i];
      
  return 0;
}