#pragma once
#include "directxmath.h"

using namespace DirectX;

struct CamVirtualSetup {
	virtual void UpdateCheck() = 0;
};

struct CameraManager : CamVirtualSetup {
	/*
	* has cam mang generic here, then passed to renderer, which in specific renderer takes in this type and makes new one then casts and sets type
	* 
	* 
	todo: pass 2 constant buffers, projection and view matrix that are SceneMatrix

	also pass DefaultMatrix //how it looks without moving the scene around)
	*/

	float DISTANCE_TO_NEAR_PLANE = 0.1f; //TODO: add thing in settings tab for this
	float DISTANCE_TO_FAR_PLANE = 10000.0f; //TODO: add thing in settings tab for this


	XMMATRIX dxWorldMatrix;
	XMMATRIX dxViewMatrix;
	XMMATRIX dxProjectionMatrix;
	XMMATRIX camRotationMatrix;

	XMVECTOR DefaultForward = XMVectorZero();
	XMVECTOR DefaultRight = XMVectorZero();
	XMVECTOR DefaultUp = XMVectorZero();
	XMVECTOR camForward = XMVectorZero();
	XMVECTOR camRight = XMVectorZero();
	XMVECTOR camUp = XMVectorZero();
	XMVECTOR camVertical = XMVectorZero();
	XMVECTOR camTarget = XMVectorZero();
	XMVECTOR camPosition = XMVectorZero();

	bool UpdateCamForce = true;
	
	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
	float moveUpDown = 0.0f;

	float camXRot = 0.0f;
	float camYRot = 0.0f;
	float camZRot = 0.0f;

	float camXRotTmp = 0.0f;
	float camYRotTmp = 0.0f;
	float camZRotTmp = 0.0f;

	float FOV = 1.0f; //randians
	float FOVtmp = 0.0f; //radians

	float RotAxisAngle = 0.0f;//randians
	float RotAxisAngleTmp = 1.0f;//randians

	float ShiftForwardAmount = 0.050;
	float ShiftSideAmount = 0.050;

	virtual void UpdateCheck() {

	}
};