#ifndef RENDERABLE_MANAGER
#define RENDERABLE_MANAGER

#include <DX12H.h>
#include <DX11H.h>
#include "FileManagerResourceStruct.h"
#include "D3D11ResourceObjects.h"
#include "D3D12ResourceObjects.h"


void SetDX11Renderer() {
	
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

#ifndef D3D12_OFF

void SetDX12Renderer() {
	
	Renderable::DXM = MainDX12Objects::obj;
	
	ResourceObjectBaseDX12::obj = new ResourceObjectBaseDX12();
	ResourceObjectBaseDX12::obj->SetResourceObjectBaseDX12();

	Renderable::DXM->RendererStartUpLogic();
	
}
#endif


#endif