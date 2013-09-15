#include "Level.h"
#include <algorithm>
#include <fstream>
#include "Effects.h"
#include "Input.h"

bool Level::Load(const char* path, vector<Level>& levels)
{
  fstream file(path);
  
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
      vector<Tile> row;
      number++;
      
      while(line.length() != 0 && line[0] != ';')
      {
        row.clear();
        
        for(UINT i=0; i<line.length(); i++)
          row.push_back((Tile)(line[i]));
          
        if(row.size() > level.mWidth)
          level.mWidth = row.size();

        level.mMap.push_back(row);
        getline(file, line);
      }
      
      level.mNumber = number;
      level.mName = "";
      level.mHeight = level.mMap.size();    
      level.FillFloor();
      levels.push_back(level);
    }

    file.close();
    return true;
  }
  else
  {
    Info::Fatal("Couldn't open data file %s", path);
    return false;
  }
}

Level::Level() : 
  mNumber(0),
  mName("unnamed"),
  mWidth(0),
  mHeight(0),
  mSteps(0),
  mPushes(0),
  mUndo(false),
  mTime(0.0f)
{
}

bool Level::IsSolved() const
{
  for(UINT i=0; i<mBoxes.size(); i++)
  {
    if(mBoxes[i].IsMoving())
      return false;

    Node boxPos = mBoxes[i].GetPosition();
    Node mapPos(-(boxPos.y - mHeight/2), boxPos.x + mWidth/2);
    Tile tile = mMap[mapPos.x][mapPos.y];
    if(GOAL != tile && PLAYER_GOAL != tile && BOX_GOAL != tile)
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
  
  // Parse char level map and instert objects to vectors
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
  
  Model::Draw(&renderList, Effects::BasicFX->BasicTech, Effects::BasicFX->InstancedTech); 
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
    else if(Input::Pressed('Z') && mUndo)
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
  // Find player position and make a virtual map with walls only
  Node node;
  vector<vector<Tile>> wallsAndFloor = mMap;
  for(UINT i=0; i<mMap.size(); i++)
    for(UINT j=0; j<mMap[i].size(); j++)
    {
      if(PLAYER == mMap[i][j] || PLAYER_GOAL == mMap[i][j])
        node = Node(i, j);
      
      wallsAndFloor[i][j] = (WALL == mMap[i][j]) ? WALL : EMPTY;
    }
  
  // Start a flood fill from player's position
  vector<Node> q;
  q.push_back(node);
  while(!q.empty())
  {
    node = q.back();
    q.pop_back();
    if(EMPTY == wallsAndFloor[node.x][node.y])
    {
      wallsAndFloor[node.x][node.y] = FLOOR;
      q.push_back(node.GetAt(Dir::UP));
      q.push_back(node.GetAt(Dir::RIGHT));
      q.push_back(node.GetAt(Dir::DOWN));
      q.push_back(node.GetAt(Dir::LEFT));
    }
  }
  
  // Mask virual map with real one
  for(UINT i=0; i<mMap.size(); i++)
    for(UINT j=0; j<mMap[i].size(); j++)
    {
      if(FLOOR == wallsAndFloor[i][j] && EMPTY == mMap[i][j])
        mMap[i][j] = FLOOR;
    }
}

bool Level::IsTileEmpty(Node aN)
{
  Node mapPos(-(aN.y - mHeight/2), aN.x + mWidth/2);
  if(WALL == mMap[mapPos.x][mapPos.y])
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