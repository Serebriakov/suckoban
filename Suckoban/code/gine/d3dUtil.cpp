//***************************************************************************************
// d3dUtil.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "d3dUtil.h"

const float EPSILON = 0.001f;

namespace Gine { namespace Dir {
  Enum OppositeDir(Enum dir)
  {
    switch(dir)
    {
    case UP    : return DOWN;
    case RIGHT : return LEFT;
    case DOWN  : return UP;
    case LEFT  : return RIGHT;
    default    : return NONE_DIR;
    }
  }
}}

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

XMFLOAT3 operator* (XMFLOAT3 a, float l) {
  return XMFLOAT3(a.x * l, a.y * l, a.z * l);
}

XMFLOAT3 operator* (float l, XMFLOAT3 a) {
  return XMFLOAT3(a.x * l, a.y * l, a.z * l);
}

XMFLOAT3 operator/ (XMFLOAT3 a, float l) {
  return XMFLOAT3(a.x / l, a.y / l, a.z / l);
}

XMFLOAT3 operator+ (XMFLOAT3 a, XMFLOAT3 b) {
  return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}