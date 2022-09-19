#pragma once
#include "3DDX11Obj.h"
#include "3DCommons/StaticObjects.h"

DX11M3DR* D3D11Cube = nullptr;

DX11M3DR* GetD3D11Cube () {
	if (D3D11Cube == nullptr) {
		M3DR tmp = M3DR(std::get<1>(StaticObjectPass[0])(), std::get<3>(StaticObjectPass[0]));
		D3D11Cube = new DX11M3DR(tmp);
	}
	return D3D11Cube;
}