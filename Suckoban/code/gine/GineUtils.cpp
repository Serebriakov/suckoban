#include "GineUtils.h"
#include <vector>
#include <cstdarg>
using namespace Gine;

const float EPSILON = 0.001f;

Dir::Enum Dir::OppositeDir(Dir::Enum dir)
{
  using namespace Dir;
  switch(dir)
  {
  case UP    : return DOWN;
  case RIGHT : return LEFT;
  case DOWN  : return UP;
  case LEFT  : return RIGHT;
  default    : return NONE_DIR;
  }
}

Node Node::GetAt(Dir::Enum aDir)
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

float Utils::RandFrom(float a, float b)
{
  int random = rand() % 1001;
  return a + ((b - a) * (random / 1000.0f));
}

float Utils::Distance(XMFLOAT3 a, XMFLOAT3 b)
{
  return sqrt((a.x - b.x) * (a.x - b.x) +
              (a.y - b.y) * (a.y - b.y) +
              (a.z - b.z) * (a.z - b.z));
}

float Utils::Length(XMFLOAT3 a)
{
  return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

XMFLOAT3 Utils::RotateToVector(const XMFLOAT3& v)
{
  float r = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

  XMFLOAT3 rotation;
  rotation.x = acos(v.y / r) - MathHelper::Pi/2.0f;
  rotation.y = atan2(v.x, v.z);
  rotation.z = 0.0f;

  return rotation;
}

XMFLOAT3 Utils::VectorToRotate(const XMFLOAT3& r)
{
  float inc = r.x + MathHelper::Pi / 2.0f;
  float azi = r.y;

  XMFLOAT3 v;
  v.z = sin(inc) * cos(azi);
  v.x = sin(inc) * sin(azi);
  v.y = cos(inc);
  
  float len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
  return XMFLOAT3(v.x/len, v.y/len, v.z/len);
}

XMFLOAT3 Utils::RotatePoint(XMFLOAT3 aPoint, XMFLOAT3 aRotation)
{
  XMVECTOR pointV = XMLoadFloat3(&aPoint);
  XMMATRIX RX = XMMatrixRotationX(aRotation.x);
  XMMATRIX RY = XMMatrixRotationY(aRotation.y);
  XMMATRIX RZ = XMMatrixRotationZ(aRotation.z);
  XMMATRIX R = RX * RY * RZ;
  pointV = XMVector3TransformCoord(pointV, R);
  
  XMFLOAT3 position;
  XMStoreFloat3(&position, pointV);
  return position;
}

void Utils::Tween(float& aX, float aTarget, float aSpeed, float aDt)
{
  if(abs(aX-aTarget) > EPSILON)
  {
    aX += (aTarget-aX)*aSpeed*aDt;
  }
}

void Utils::OptimizeRotation(float aRot, float& aRotTarget)
{
  if(abs(aRotTarget - aRot) > MathHelper::Pi)
  {
    if(aRotTarget > aRot)
    {
      aRotTarget -= MathHelper::Pi*2.0f;
    }
    else
    {
      aRotTarget += MathHelper::Pi*2.0f;
    }
  }
}

wstring Utils::ToWString(const string* s)
{
  return wstring(s->begin(), s->end());
}

wstring Utils::ToWString(const char* s)
{
  string str = s;
  return wstring(str.begin(), str.end());
}

string Utils::ToString(const int i)
{
  return to_string((long double)i);
}

string Utils::Format(const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  std::string buf = Gine::Utils::VFormat(fmt, ap);
  va_end (ap);
  return buf;
}

string Utils::VFormat(const char *fmt, va_list ap)
{
  // Allocate a buffer on the stack that's big enough for us almost
  // all the time.  Be prepared to allocate dynamically if it doesn't fit.
  size_t size = 1024;
  char stackbuf[1024];
  vector<char> dynamicbuf;
  char *buf = &stackbuf[0];

  while (1) {
    // Try to vsnprintf into our buffer.
    int needed = vsnprintf (buf, size, fmt, ap);
    // NB. C99 (which modern Linux and OS X follow) says vsnprintf
    // failure returns the length it would have needed.  But older
    // glibc and current Windows return -1 for failure, i.e., not
    // telling us how much was needed.

    if (needed <= (int)size && needed >= 0) {
      // It fit fine so we're done.
      return string(buf, (size_t) needed);
    }

    // vsnprintf reported that it wanted to write more characters
    // than we allotted.  So try again using a dynamic buffer.  This
    // doesn't happen very often if we chose our initial size well.
    size = (needed > 0) ? (needed+1) : (size*2);
    dynamicbuf.resize (size);
    buf = &dynamicbuf[0];
  }
}

ID3D11Texture2D* Utils::CreateDefaultTexture2D()
{
	D3D11_TEXTURE2D_DESC texDesc;
  texDesc.Width              = gScreenW;
  texDesc.Height             = gScreenH;
	texDesc.MipLevels          = 1;
	texDesc.ArraySize          = 1;
	texDesc.SampleDesc.Count   = 1;
  texDesc.SampleDesc.Quality = 0;
	texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage              = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags     = 0; 
	texDesc.MiscFlags          = 0;

	ID3D11Texture2D* texture = 0;
	HR(gDevice->CreateTexture2D(&texDesc, 0, &texture));

  return texture;
}