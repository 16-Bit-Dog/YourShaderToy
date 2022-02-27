#ifndef RENDERABLE_MANAGER
#define RENDERABLE_MANAGER

#include <DX12H.h>
#include <DX11H.h>
#include "FileManagerResourceStruct.h"

Renderable* DXM;
MainDX12Objects DX12Obj;
MainDX11Objects DX11Obj;


#ifdef FILE_MANAGER_STRUCTS
void SetFileManagerRenderable() {
	RendererFileM::SET_RENDERABLE(DXM);
}
#endif

void SetDX11Renderer() {
	DXM = &DX11Obj;
	SetFileManagerRenderable();
}

void SetDX12Renderer() {
	DXM = &DX12Obj;
	SetFileManagerRenderable();
}


#endif