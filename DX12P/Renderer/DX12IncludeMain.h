#pragma once
//finally I will try to use com_ptr's

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <d3d12.h>
#include <../Special_DX_Headers/d3dx12.h> //very good helper struct for DX12 - MSDN just refuses to teach raw stuff like D3D12_CPU_DESCRIPTOR_HANDLE prob due to it being a bad idea
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
