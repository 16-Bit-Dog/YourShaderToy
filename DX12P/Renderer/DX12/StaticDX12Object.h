#pragma once 

#include "3DDX12Obj.h"
#include "3DCommons/StaticObjects.h"

DX12M3DR* D3D12Cube = nullptr;

DX12M3DR* GetD3D11Cube () {
	if (D3D12Cube == nullptr) {
		M3DR tmp = M3DR(std::get<1>(StaticObjectPass[0])(), std::get<3>(StaticObjectPass[0]));
		D3D12Cube = new DX12M3DR(tmp);
	}
	return D3D12Cube;
}