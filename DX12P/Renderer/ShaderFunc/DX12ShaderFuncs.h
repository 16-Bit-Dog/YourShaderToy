#pragma once
//finally I will try to use com_ptr's

#define UNNegative 0 //assume 10000 pixel video max TODO FIX: wincodec stops working with unnegative at larger values


#include "DX12IncludeMain.h"
#include "DXSafeInclude.h"
#include <string>


using namespace DirectX;

namespace SHADER_PREFIX_D3D12 {
    const static std::string VS_PREFIX_D3D12 = "vs_5_0";
    const static std::string PS_PREFIX_D3D12 = "ps_5_0";
    const static std::string CS_PREFIX_D3D12 = "cs_5_0";
};

struct ShaderCDX12 {

    static D3D12_SHADER_BYTECODE* CompileShader(const std::string* shaderInfo, const std::string& entryPoint, const std::string& _profile, std::string* Error) //LoadShader class
    {
        ID3DBlob* pShaderBlob = nullptr;
        ID3DBlob* pErrorBlob = nullptr;


        UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif
        HRESULT hr = D3DCompile2(shaderInfo->c_str(), shaderInfo->length(), nullptr,
            nullptr, nullptr, entryPoint.c_str(),
            _profile.c_str(), flags, 0, 0, 0, 0, &pShaderBlob, &pErrorBlob);
        


        if (pErrorBlob != nullptr) {
            OutputDebugStringA("\n");
            OutputDebugStringA((const char*)pErrorBlob->GetBufferPointer());
            (*Error) = (const char*)pErrorBlob->GetBufferPointer();
        }
        else {
            (*Error) = "";
            D3D12_SHADER_BYTECODE* tmp = new D3D12_SHADER_BYTECODE();
            tmp->pShaderBytecode = pShaderBlob->GetBufferPointer();
            tmp->BytecodeLength = pShaderBlob->GetBufferSize();
            return tmp;

        }


        SafeRelease(pErrorBlob); // no longer need shader mem of error
    }
};

