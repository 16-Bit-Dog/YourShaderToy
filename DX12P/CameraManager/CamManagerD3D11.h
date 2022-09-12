#pragma once
#include "directxmath.h"
#include "CamManager.h"
#include "DX11/DX11IncludeMain.h"
using namespace DirectX;

struct CameraManagerD3D11 : CameraManager {
	/*
	abstracted cam class for dx11 renderer

	*/

	D3D11_VIEWPORT* Viewport;

	ID3D11Device5* dxDevice = 0;
	ID3D11DeviceContext4* dxDeviceContext = 0;
	
	ComPtr<ID3D11Buffer> WorldMatrix;
	ComPtr<ID3D11Buffer> ViewMatrix;
	ComPtr<ID3D11Buffer> ProjMatrix;

	ComPtr<ID3D11Buffer> DefWorldMatrix;
	ComPtr<ID3D11Buffer> DefViewMatrix;
	ComPtr<ID3D11Buffer> DefProjMatrix;
	~CameraManagerD3D11() {

		//SafeRelease(dxDevice);

	}
	//check if it is the first or not pass to this renderer to setup default buffer values to renderer start location
	bool SetFirst = false;

	CameraManagerD3D11(ID3D11Device5* d, ID3D11DeviceContext4* c, D3D11_VIEWPORT* V) {
		dxDevice = d;
		dxDeviceContext = c;
		Viewport = V;

		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
		constantBufferDesc.CPUAccessFlags = 0;
		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		dxDevice->CreateBuffer(&constantBufferDesc, nullptr, &ViewMatrix); //make const buffer for application
		dxDevice->CreateBuffer(&constantBufferDesc, nullptr, &ProjMatrix); //make const buffer for frame
		dxDevice->CreateBuffer(&constantBufferDesc, nullptr, &WorldMatrix); //make const buffer for frame
		dxDevice->CreateBuffer(&constantBufferDesc, nullptr, &DefViewMatrix); //make const buffer for object 
		dxDevice->CreateBuffer(&constantBufferDesc, nullptr, &DefProjMatrix); //make const buffer for object 
		dxDevice->CreateBuffer(&constantBufferDesc, nullptr, &DefWorldMatrix); //make const buffer for object 

		DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		DefaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		camTarget = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	}

	void UpdateCam() { //only needed if 3d is utilized - matrix math tutorials give me the math understanding to do stuff :smile:
		camXRotTmp = camXRot;
		camYRotTmp = camYRot;
		camZRotTmp = camZRot;
		//Set tmp to current cam implemented rotation 

		//update rotation of cam
		camRotationMatrix = XMMatrixRotationRollPitchYaw(camYRot, camXRot, camZRot);

		//update camrea position based on math
		camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
		camTarget = XMVector3Normalize(camTarget);

		//get cam transform along axis forward, vertical, horizontal, or up
		camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
		camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
		camVertical = XMVector3TransformCoord(DefaultUp, camRotationMatrix);
		camUp = XMVector3Cross(camForward, camRight);

		//set new cam position
		camPosition += XMVectorScale(camForward, moveBackForward);
		camPosition += XMVectorScale(camRight, moveLeftRight);
		camPosition += XMVectorScale(camVertical, moveUpDown);

		moveLeftRight = 0;
		moveBackForward = 0;
		moveUpDown = 0;

		//set cam target location now
		camTarget = camPosition + camTarget;

		//look at cam setup
		dxViewMatrix = XMMatrixLookAtLH(camPosition, camTarget, camUp);

		//update buffer to cam - NOT default
		dxDeviceContext->UpdateSubresource(ViewMatrix.Get(), 0, nullptr, &dxViewMatrix, 0, 0); //update subresource data of constant buffer

		//update buffer to cam - default
		if(SetFirst) dxDeviceContext->UpdateSubresource(DefViewMatrix.Get(), 0, nullptr, &dxViewMatrix, 0, 0); 
	}
	virtual void UpdateCheck() {
		if (UpdateCamForce || moveLeftRight != 0 || moveUpDown != 0 || moveBackForward != 0 || camXRotTmp != camXRot || camYRotTmp != camYRot || camZRotTmp != camZRot) {
			//if cam value changed update whole buffer --> faster than update every frame
			UpdateCam();
			UpdateCamForce = false;
		}


		if (RotAxisAngle != RotAxisAngleTmp) {
			//if world rot axis changed update world buffer

			RotAxisAngleTmp = RotAxisAngle;

			DirectX::XMVECTOR rotationAxis = XMVectorSet(0, 1, 0, 0);

			dxWorldMatrix = XMMatrixRotationAxis(rotationAxis, RotAxisAngle);

			dxDeviceContext->UpdateSubresource(WorldMatrix.Get(), 0, nullptr, &dxWorldMatrix, 0, 0);
			if (SetFirst) dxDeviceContext->UpdateSubresource(DefWorldMatrix.Get(), 0, nullptr, &dxWorldMatrix, 0, 0);
		}

		if (FOVtmp != FOV) {
			//if FOV changes update proj matrix

			FOVtmp = FOV;
			dxProjectionMatrix = XMMatrixPerspectiveFovLH(FOV, Viewport->Width / Viewport->Height, DISTANCE_TO_NEAR_PLANE, DISTANCE_TO_FAR_PLANE);

			dxDeviceContext->UpdateSubresource(
				ProjMatrix.Get(),
				0,
				nullptr,
				&dxProjectionMatrix,
				0,
				0);
			if (SetFirst) dxDeviceContext->UpdateSubresource(DefProjMatrix.Get(), 0, nullptr, &dxProjectionMatrix, 0, 0);
		}
	}

};