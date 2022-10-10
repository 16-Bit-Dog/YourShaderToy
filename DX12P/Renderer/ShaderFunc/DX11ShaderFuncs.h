#pragma once
//finally I will try to use com_ptr's

#define UNNegative 0 //assume 10000 pixel video max TODO FIX: wincodec stops working with unnegative at larger values


#include "DX11IncludeMain.h"
#include "3DDX11Obj.h"
#include "DXSafeInclude.h"
#include <string>

using namespace DirectX;

struct ShaderCDX11 {
    static inline ShaderCDX11* obj;

    template< class ShaderClass >  //done here so you can do GetLatestProfile<ID3D11PixelShader>(); or related to make your own shaders and such - I implemented the hull shaders and such here so cohesiveness is kept to a maximum
    std::string GetLatestProfile(ID3D11Device5* dxDevice); //template to get shader level to compile with

    template<>
    std::string GetLatestProfile<ID3D11VertexShader>(ID3D11Device5* dxDevice)
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = dxDevice->GetFeatureLevel();

        switch (featureLevel) // later if needed I will add a dx12 feature level... 
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
        {
            return "vs_5_0";
        }
        break;
        case D3D_FEATURE_LEVEL_10_1:
        {
            return "vs_4_1";
        }
        break;
        case D3D_FEATURE_LEVEL_10_0:
        {
            return "vs_4_0";
        }
        break;
        case D3D_FEATURE_LEVEL_9_3:
        {
            return "vs_4_0_level_9_3";
        }
        break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
        {
            return "vs_4_0_level_9_1";
        }
        break;
        }
        return "";
    }
    ////////////////////
    template<>
    std::string GetLatestProfile<ID3D11PixelShader>(ID3D11Device5* dxDevice)
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = dxDevice->GetFeatureLevel(); //feature level to compile pixel shader 
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
        {
            return "ps_5_0";
        }
        break;
        case D3D_FEATURE_LEVEL_10_1:
        {
            return "ps_4_1";
        }
        break;
        case D3D_FEATURE_LEVEL_10_0:
        {
            return "ps_4_0";
        }
        break;
        case D3D_FEATURE_LEVEL_9_3:
        {
            return "ps_4_0_level_9_3";
        }
        break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
        {
            return "ps_4_0_level_9_1";
        }
        break;
        }
        return "";
    }

    //////////////

    template<> // I setup these despite not using because a good chance exists that to expand PGE this code will be more than helpful - plus these are kinda place holders for loading shaders (need acsess to device in dx11 - plus you won't do it any other way...
    std::string GetLatestProfile<ID3D11ComputeShader>(ID3D11Device5* dxDevice)
    {
        // Query the current feature level:
        D3D_FEATURE_LEVEL featureLevel = dxDevice->GetFeatureLevel(); //feature level to compile pixel shader 
        switch (featureLevel)
        {
        case D3D_FEATURE_LEVEL_11_1:
        case D3D_FEATURE_LEVEL_11_0:
        {
            return "cs_5_0";
        }
        break;
        case D3D_FEATURE_LEVEL_10_1:
        {
            return "cs_4_1";
        }
        break;
        case D3D_FEATURE_LEVEL_10_0:
        {
            return "cs_4_0";
        }
        break;
        case D3D_FEATURE_LEVEL_9_3:
        {
            return "cs_4_0_level_9_3";
        }
        break;
        case D3D_FEATURE_LEVEL_9_2:
        case D3D_FEATURE_LEVEL_9_1:
        {
            return "cs_4_0_level_9_1";
        }
        break;
        }
        return "";
    }


    template< class ShaderClass >
    ShaderClass* CreateShader(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage, ID3D11Device5* dxDevice, ID3D11InputLayout** dxIL); //generic shader creation

    template<>
    ID3D11VertexShader* CreateShader<ID3D11VertexShader>(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage, ID3D11Device5* dxDevice, ID3D11InputLayout** dxIL) //vertex shader - shader type
    {
        ID3D11VertexShader* pVertexShader = nullptr;

        auto hr = dxDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pVertexShader); //make a shader based on buffer, buffer size, classtype, and return to pshader object
        
        if (FAILED(hr)) {
            pVertexShader = nullptr;
        }

        //SafeRelease(dxIL); //stop mem leak if overwrite previously used layout

        hr = dxDevice->CreateInputLayout( //make input layout - global change to input Layout
            DX11M3DR::dxVertexLayoutDesc, //vertex shader - input assembler data
            _countof(DX11M3DR::dxVertexLayoutDesc), //number of elements
            pShaderBlob->GetBufferPointer(),  //vertex shader buffer
            pShaderBlob->GetBufferSize(), //vetex shader blob size 
            dxIL); //input layout to output to

        if (FAILED(hr))
        {
            OutputDebugStringW(L"failed input layout setup");
        }

        return pVertexShader;
    }
    ////////////////////////////////
    template<>
    ID3D11PixelShader* CreateShader<ID3D11PixelShader>(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage, ID3D11Device5* dxDevice, ID3D11InputLayout** dxIL)
    {
        ID3D11PixelShader* pPixelShader = nullptr;
        auto hr = dxDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pPixelShader); //pixel shader version of the vertex shader above

        if (FAILED(hr)) {
            pPixelShader = nullptr;
        }

        return pPixelShader;
    }

    //////////////////////////////// these are kinda place holders for loading shaders (need acsess to device in dx11 - plus you won't do it any other way...
    template<>
    ID3D11ComputeShader* CreateShader<ID3D11ComputeShader>(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage, ID3D11Device5* dxDevice, ID3D11InputLayout** dxIL)
    {
        ID3D11ComputeShader* pComputeShader = nullptr;
        auto hr = dxDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pComputeShader); //pixel shader version of the vertex shader above

        if (FAILED(hr)) {
            pComputeShader = nullptr;
        }

        return pComputeShader;
    }



    template< class ShaderClass >
    ShaderClass* LoadShader(const std::string* shaderInfo, const std::string& entryPoint, const std::string& _profile, std::string* Error, ID3D11Device5* dxDevice, ID3D11InputLayout** dxIL) //LoadShader class
    {
        ID3DBlob* pShaderBlob = nullptr;
        ID3DBlob* pErrorBlob = nullptr;
        ShaderClass* pShader = nullptr;

        std::string profile = _profile;
        if (profile == "latest")
        {
            profile = GetLatestProfile<ShaderClass>(dxDevice); //get shader profiles/settings
        }

        UINT flags = D3DCOMPILE_OPTIMIZATION_LEVEL3;

#if _DEBUG
        flags |= D3DCOMPILE_DEBUG;
#endif
        HRESULT hr = D3DCompile2(shaderInfo->c_str(), shaderInfo->length(), nullptr,
            nullptr, nullptr, entryPoint.c_str(),
            profile.c_str(), flags, 0, 0, 0, 0, &pShaderBlob, &pErrorBlob);
        
        if (pErrorBlob != nullptr) {
            OutputDebugStringA("\n");
            OutputDebugStringA((const char*)pErrorBlob->GetBufferPointer());
            (*Error) = (const char*)pErrorBlob->GetBufferPointer();
        }
        else {
            (*Error) = "";

            pShader = CreateShader<ShaderClass>(pShaderBlob, nullptr, dxDevice, dxIL);

        }

        
        SafeRelease(pShaderBlob); // no longer need shader mem
        SafeRelease(pErrorBlob); // no longer need shader mem of error

        return pShader;
    }
};