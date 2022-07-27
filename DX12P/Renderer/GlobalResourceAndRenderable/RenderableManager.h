#ifndef RENDERABLE_MANAGER
#define RENDERABLE_MANAGER

#include <DX12H.h>
#include <DX11H.h>
#include "FileManagerResourceStruct.h"
#include "D3D11ResourceObjects.h"

//#ifdef FILE_MANAGER_STRUCTS
//void SetFileManagerRenderable() {
//	RendererFileM::SET_RENDERABLE(DXM);
//}
//#endif

void SetDX11Renderer() {
	//if (Renderable::DXM != nullptr) Renderable::DXM = nullptr;

	Renderable::DXM = MainDX11Objects::obj;
	
	ResourceObjectBaseDX11::obj = new ResourceObjectBaseDX11();
	ResourceObjectBaseDX11::obj->SetResourceObjectBaseDX11();

	Renderable::DXM->RendererStartUpLogic();

}

void CleanRenderer() {
	if (Renderable::DXM != nullptr) {
		Renderable::DXM->CleanRendererState();
	}
}
void SetDX12Renderer() {
	//if (Renderable::DXM != nullptr) delete Renderable::DXM;
	
	Renderable::DXM = MainDX12Objects::obj;
	
	ResourceObjectBaseDX11::obj = new ResourceObjectBaseDX11();
	ResourceObjectBaseDX11::obj->SetResourceObjectBaseDX11();

	Renderable::DXM->RendererStartUpLogic();
	//SetFileManagerRenderable();

}


#endif