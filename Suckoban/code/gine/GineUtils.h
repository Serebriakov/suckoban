#pragma once
#include <string>
#include "Gine.h"
using namespace std;

namespace Gine
{
  // namespace: Dir
  //
  // summary:	Managing 2D tile directions

  namespace Dir 
  { 
    enum Enum
    {
      UP = 0,
      RIGHT,
      DOWN,
      LEFT,
      NONE_DIR
    };

    Enum OppositeDir(Enum dir);   
  }

  /// <summary>
  /// A node for a 2D tile schema
  /// </summary>

	struct Node
	{
    Node() : x(0), y(0) {};
    Node(int aX, int aY) : x(aX), y(aY) {};
	  int x;
	  int y;

    /// <summary>
    /// Returns a node with x/y inc/decremented
    /// </summary>
    /// <example> We want to get an upper node (y-1 on the map)
    /// <code>
    /// Node n(3, 5);
    /// Node m = n.GetAt(Dir::UP);
    /// // m.x = 3; m.y = 4;
    /// </code>
    /// </example>
    ///
    /// <param name="aDir"> Enum from Gine::Dir namespace </param>
    ///
    /// <returns> A neighbour node </returns>

    Node GetAt(Dir::Enum aDir);
    
    bool operator== (const Node& node) const { return x==node.x && y==node.y; };
	};

  // namespace: Utils
  //
  // summary:	Utilities functions

  namespace Utils 
  { 
    // Code utils
    float RandFrom(float a, float b);
    float Length(XMFLOAT3 a);
    float Distance(XMFLOAT3 a, XMFLOAT3 b);
    XMFLOAT3 RotateToVector(const XMFLOAT3& v);
    XMFLOAT3 VectorToRotate(const XMFLOAT3& r);
    XMFLOAT3 RotatePoint(XMFLOAT3 aPoint, XMFLOAT3 aRotation);

    // Modulo down the angles
    void OptimizeRotation(float aRot, float& aRotTarget);

    // Simple 1/2 tween
    void Tween(float& aX, float aTarget, float aSpeed, float aDt);

    // Text
    wstring ToWString(const string* s);  
    string ToString(const int i);
    string Format(const char* text, ...);
    string VFormat(const char* text, va_list ap);
  }
}