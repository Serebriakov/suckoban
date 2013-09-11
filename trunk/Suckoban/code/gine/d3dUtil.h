#ifndef D3DUTIL_H
#define D3DUTIL_H

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
 
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <xnamath.h>
#include <dxerr.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "MathHelper.h"
#include "LightHelper.h"
#include "d3dx11Effect.h"

namespace Gine
{
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

	struct Node
	{
    Node() : x(0), y(0) {};
    Node(int aX, int aY) : x(aX), y(aY) {};
	  int x;
	  int y;
    
    Node GetAt(Dir::Enum aDir)
    {
      using namespace Dir;
      switch(aDir)
      {
      case UP    : return Node(x, y + 1);
      case RIGHT : return Node(x + 1, y);
      case DOWN  : return Node(x, y - 1);
      case LEFT  : return Node(x - 1, y);
      default    : return Node(x, y);
      }
    }
    
    bool operator== (const Node& node) const { return x==node.x && y==node.y; };
	};
}

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
}

XMFLOAT3 operator* (XMFLOAT3 a, float l);
XMFLOAT3 operator* (float l, XMFLOAT3 a);
XMFLOAT3 operator+ (XMFLOAT3 a, XMFLOAT3 b);
XMFLOAT3 operator/ (XMFLOAT3 a, float b);


//---------------------------------------------------------------------------------------
// Simple d3d error checker for book demos.
//---------------------------------------------------------------------------------------

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)                                              \
	{                                                          \
		HRESULT hr = (x);                                      \
		if(FAILED(hr))                                         \
		{                                                      \
			DXTrace(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
		}                                                      \
	}
	#endif

#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif 


//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }

// #define XMGLOBALCONST extern CONST __declspec(selectany)
//   1. extern so there is only one copy of the variable, and not a separate
//      private copy in each .obj.
//   2. __declspec(selectany) so that the compiler does not complain about
//      multiple definitions in a .cpp file (it can pick anyone and discard 
//      the rest because they are constant--all the same).

namespace Colors
{
	XMGLOBALCONST XMVECTORF32 White     = {1.0f, 1.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Black     = {0.0f, 0.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Red       = {1.0f, 0.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Green     = {0.0f, 1.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Blue      = {0.0f, 0.0f, 1.0f, 0.0f};
	XMGLOBALCONST XMVECTORF32 Yellow    = {1.0f, 1.0f, 0.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Cyan      = {0.0f, 1.0f, 1.0f, 1.0f};
	XMGLOBALCONST XMVECTORF32 Magenta   = {1.0f, 0.0f, 1.0f, 1.0f};
  XMGLOBALCONST XMVECTORF32 MPink     = {0.0f, 0.0f, 0.0f, 0.0f};

	XMGLOBALCONST XMVECTORF32 Silver    = {0.75f, 0.75f, 0.75f, 1.0f};
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = {0.69f, 0.77f, 0.87f, 1.0f};

  // paintjet
	XMGLOBALCONST XMVECTORF32 PJ_Black = {24 / 256.0f, 20 / 256.0f, 12 / 256.0f,
                                        1.0f};

  // tango
  XMGLOBALCONST XMVECTORF32 Blue1 = {0.447f, 0.624f, 0.812f, 1.0f};
}

#endif // D3DUTIL_H