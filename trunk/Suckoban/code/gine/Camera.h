//***************************************************************************************
// Camera.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Simple first person style camera class that lets the viewer explore the 3D scene.
//   -It keeps track of the camera coordinate system relative to the world space
//    so that the view matrix can be constructed.  
//   -It keeps track of the viewing frustum of the camera so that the projection
//    matrix can be obtained.
//***************************************************************************************

#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "d3dUtil.h"
#include "xnacollision.h"
#include "Model.h"
using namespace std;
using namespace XNA;

namespace Gine
{
  class Model;

  class Camera
  {
  public:
	  Camera();
	  ~Camera();

	  // Get/Set world camera position.
	  XMVECTOR GetPositionXM()const;
	  XMFLOAT3 GetPosition()const;
	  void SetPosition(float x, float y, float z);
	  void SetPosition(const XMFLOAT3& v);
	
	  // Get camera basis vectors.
	  XMVECTOR GetRightXM()const;
	  XMFLOAT3 GetRight()const;
	  XMVECTOR GetUpXM()const;
	  XMFLOAT3 GetUp()const;
	  XMVECTOR GetLookXM()const;
	  XMFLOAT3 GetLook()const;

	  // Get frustum properties.
	  float GetNearZ()const;
	  float GetFarZ()const;
	  float GetAspect()const;
	  float GetFovY()const;
	  float GetFovX()const;
    Frustum* GetFrustum();

	  // Get near and far plane dimensions in view space coordinates.
	  float GetNearWindowWidth()const;
	  float GetNearWindowHeight()const;
	  float GetFarWindowWidth()const;
	  float GetFarWindowHeight()const;
	
	  // Set frustum.
	  void SetLens(float fovY, float aspect, float zn, float zf);
    void SetAspect(float aspect);

	  // Define camera space via LookAt parameters.
	  void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	  void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	  // Get View/Proj matrices.
	  XMMATRIX View()const;
	  XMMATRIX Proj()const;
	  XMMATRIX ViewProj()const;

	  // Strafe/Walk the camera a distance d.
	  void Strafe(float d);
	  void Walk(float d);

	  // Rotate the camera.
	  void Pitch(float angle);
	  void RotateY(float angle);
    void Rotate(int aDx, int aDy);

	  // After modifying camera position/orientation, call to rebuild the view matrix.
	  void UpdateViewMatrix();

    // Frustum culling of a model list
    void Cull(vector<Model*>* aModels);

  private:

	  // Camera coordinate system with coordinates relative to world space.
	  XMFLOAT3 mPosition;
	  XMFLOAT3 mRight;
	  XMFLOAT3 mUp;
	  XMFLOAT3 mLook;

	  // Cache frustum properties.
	  float mNearZ;
	  float mFarZ;
	  float mAspect;
	  float mFovY;
	  float mNearWindowHeight;
	  float mFarWindowHeight;

	  // Cache View/Proj matrices.
	  XMFLOAT4X4 mView;
	  XMFLOAT4X4 mProj;

    // Frustrum
    Frustum mFrustum;
  };
}

#endif // CAMERA_H