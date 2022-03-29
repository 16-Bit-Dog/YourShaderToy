#pragma once
#include "directxmath.h"

using namespace DirectX;

struct CamVirtualSetup { 
	/*
	this is a generic virtual class holder for functions abstracted `CameraManager` requires to implement (for updating the abstracted cam managers for specific renderers



	*/

	virtual void UpdateCheck() = 0;
};

struct CameraManager : CamVirtualSetup {
	/*
	* has cam manger is a generic abstract base level class, it gets passed to main abstracted renderer, 
	
	This renderer is passed to all sub renderers like CamManagerD3D11 and 12.
	*/

	float DISTANCE_TO_NEAR_PLANE = 0.1f; //TODO: add thing in settings tab for this
	float DISTANCE_TO_FAR_PLANE = 10000.0f; //TODO: add thing in settings tab for this
	float LERP_DROP = 0.05f;


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

	bool UpdateCamForce = true; //forces camrea to update all movment data even if not moved yet
	
	//current position offset
	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
	float moveUpDown = 0.0f;

	//cam rotation offset values --> not auto updated with tmp
protected:
	float camXRot = 0.0f;
	float camYRot = 0.0f;
	float camZRot = 0.0f;
	float FOV = 1.0f; //randians of FOV 
	float RotAxisAngle = 0.0f;//randians of rotation axis angle

public:
	//cam current moved rotation values, held tmp
	float camXRotTmp = 0.0f;
	float camYRotTmp = 0.0f;
	float camZRotTmp = 0.0f;

	float FOVtmp = 0.0f; //radians tmp which is public in how its changed
	float RotAxisAngleTmp = 1.0f;//randians

	//how much you are to be pushed in direction vars
	float ShiftForwardAmount = 0.050; 
	float ShiftSideAmount = 0.050;

	virtual void UpdateCheck() {

	}
};