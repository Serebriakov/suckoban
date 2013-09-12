//***************************************************************************************
// d3dUtil.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "d3dUtil.h"

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