#pragma once

#include <DX12H.h>
#include <DX11H.h>

Renderable* DXM;


void SetDX11Renderer() {
	DXM = &DX11Obj;
}

void SetDX12Renderer() {
	DXM = &DX12Obj;
}