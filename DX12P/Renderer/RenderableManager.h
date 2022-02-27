#ifndef RENDERABLE_MANAGER
#define RENDERABLE_MANAGER

#include <DX12H.h>
#include <DX11H.h>
#include "FileManagerResourceStruct.h"
#include "D3D11ResourceObjects.h"

Renderable* DXM;
MainDX12Objects DX12Obj;
MainDX11Objects DX11Obj;

ResourceObjectBaseDX11* ROBDX11;

//#ifdef FILE_MANAGER_STRUCTS
//void SetFileManagerRenderable() {
//	RendererFileM::SET_RENDERABLE(DXM);
//}
//#endif

void CreateAndSetRendererBaseDX11() {
	ROBDX11 = new ResourceObjectBaseDX11();
	ROBDX11->SetResourceObjectBaseDX11();
}

void SetDX11Renderer() {
	DXM = &DX11Obj;
	//SetFileManagerRenderable();
	CreateAndSetRendererBaseDX11();
}

void SetDX12Renderer() {
	DXM = &DX12Obj;
	//SetFileManagerRenderable();

}


#endif