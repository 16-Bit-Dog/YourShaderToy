#pragma once

#include "DX11ShaderFuncs.h"
#include "Renderable.h"
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include <../imGUI/imgui_impl_dx11.h>

int BLOCK_SIZE = 8;
    
    bool NewImGUIDat = false;


using namespace DirectX;


struct MainDX11Objects : Renderable{

    int BLOCK_SIZE = 8;

    bool ClearRTV = true;

    int CurrentFrameRate = 23;

    int TargetFrameRate = 69;

    int SampleSize = 1; 

    bool UseWarpDev = false;

    UINT Width;
    UINT Height;
    HWND hwnd;
    GLFWwindow* window;

    bool bFullScreen = false;

    DXGI_RATIONAL refreshRateStatic;

    //Pipeline Objects
    
    ComPtr<ID3D11Device5> dxDevice = 0;
    ComPtr<ID3D11DeviceContext4> dxDeviceContext = 0;
    IDXGISwapChain1* dxSwapChain = nullptr;

    D3D11_DEPTH_STENCIL_VIEW_DESC dxDepthStencilDesc{
    DXGI_FORMAT_D32_FLOAT,D3D11_DSV_DIMENSION_TEXTURE2D
    };

    ID3D11RenderTargetView* dxRenderTargetView = nullptr;
    ID3D11Texture2D* dxDepthStencilBuffer = nullptr;
    ID3D11DepthStencilView* dxDepthStencilView = nullptr;
    ID3D11DepthStencilState* dxDepthStencilStateDefault = nullptr;

    ID3D11RasterizerState* dxRasterizerStateF = nullptr;
    ID3D11RasterizerState* dxRasterizerStateW = nullptr;

    D3D11_VIEWPORT dxViewport;

    ComPtr<IDXGIFactory2> dxFactory;
    ComPtr<IDXGIAdapter> dxAdapter;
    ComPtr<IDXGIDevice> dxGIDevice;

    DXGI_SWAP_CHAIN_DESC1 swapChainDescW; //reuse for when recreating swap chain and parts to resize screen params
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainDescF;
    void ClearBuffer(XMFLOAT4 p, bool ClearDepth) {
        float ClearColor[4] = { float(p.x), float(p.y), float(p.z), float(p.w) };
        dxDeviceContext->ClearRenderTargetView(dxRenderTargetView, ClearColor);

        if (ClearDepth) dxDeviceContext->ClearDepthStencilView(dxDepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
    }
    void DrawLogic(bool sync = false) override {

        if (NewImGUIDat) {
            ImGui::Render();
        }
        dxDeviceContext->OMSetRenderTargets(1, &dxRenderTargetView, NULL);
        if (ClearRTV == true) {
            ClearBuffer({ 0.0f,0.5f,0.0f,1.0f }, true);
        }
        ClearRTV = true;

        if (NewImGUIDat) {
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        dxSwapChain->Present(sync, 0); // Present with vsync

        NewImGUIDat = false;
    }

    ID3D11Resource* GetResourceOfUAVSRV(ID3D11View* u) {
        ID3D11Resource* r = nullptr;
        u->GetResource(&r);
        return r;
    }

    void CreateSwapChainAndAssociate(DXGI_FORMAT format) {
        swapChainDescW.Width = Width;
        swapChainDescW.Height = Height;
        swapChainDescW.BufferCount = 2;
        swapChainDescW.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDescW.Format = format;
        swapChainDescW.SampleDesc.Count = 1;
        swapChainDescW.SampleDesc.Quality = 0;
        swapChainDescW.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDescW.Scaling = DXGI_SCALING_NONE;

        swapChainDescF.Scaling = DXGI_MODE_SCALING_STRETCHED;
        swapChainDescF.RefreshRate = refreshRateStatic;
        swapChainDescF.Windowed = !bFullScreen;
        
        ID3D11Texture2D* backBuffer = nullptr;
        if (dxSwapChain != nullptr) {
            dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
        }
        if (backBuffer != nullptr) {
            SafeRelease(backBuffer);
        }
        if (dxRenderTargetView != nullptr) {
            SafeRelease(dxRenderTargetView);
        }
        if ((dxDepthStencilBuffer) == nullptr) {
            SafeRelease(dxDepthStencilBuffer);
        }

        if (dxSwapChain != nullptr) {
            dxDeviceContext->ClearState();
            dxDeviceContext->Flush();

            dxSwapChain->ResizeBuffers(0, Width, Height, format, swapChainDescW.Flags);
        }
        else {
            ThrowFailed(dxFactory->CreateSwapChainForHwnd(dxDevice.Get(), hwnd, &swapChainDescW, &swapChainDescF, NULL, &dxSwapChain));
        }

        backBuffer;
        dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    
        dxDevice->CreateRenderTargetView(
            backBuffer,
            nullptr,
            &dxRenderTargetView);

        SafeRelease(backBuffer);

        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        depthStencilBufferDesc.CPUAccessFlags = 0;
        depthStencilBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthStencilBufferDesc.Width = Width;
        depthStencilBufferDesc.Height = Height;
        depthStencilBufferDesc.MipLevels = 1;

        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.SampleDesc.Quality = 0;

        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        dxDevice->CreateTexture2D(
            &depthStencilBufferDesc,
            nullptr,
            &dxDepthStencilBuffer);



        dxDevice->CreateDepthStencilView(
            dxDepthStencilBuffer,
            &dxDepthStencilDesc,
            &dxDepthStencilView);

        dxViewport.Width = Width;
        dxViewport.Height = Height;
        dxViewport.TopLeftX = 0.0f;
        dxViewport.TopLeftY = 0.0f;
        dxViewport.MinDepth = 0.0f;
        dxViewport.MaxDepth = 1.0f;
    }

    void MakeNewWindowSwapChainAndAssociate(GLFWwindow* windowW, HWND sHwnd, UINT sWidth, UINT sHeight) override{
        window = windowW;
        Width = sWidth;
        Height = sHeight;
        hwnd = sHwnd;

        DEVMODEA WinMonitorInfo;

        EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &WinMonitorInfo);

        refreshRateStatic.Numerator = WinMonitorInfo.dmDisplayFrequency;
        refreshRateStatic.Denominator = 1;

        CreateSwapChainAndAssociate(DXGI_FORMAT_R8G8B8A8_UNORM);

        D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
        ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));


        depthStencilStateDesc.DepthEnable = FALSE; //this is a bad idea...
        depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilStateDesc.StencilEnable = FALSE;


        dxDevice->CreateDepthStencilState(&depthStencilStateDesc, &dxDepthStencilStateDefault);

        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

        rasterizerDesc.AntialiasedLineEnable = FALSE;
        rasterizerDesc.CullMode = D3D11_CULL_FRONT;
        rasterizerDesc.DepthBias = 0;
        rasterizerDesc.DepthBiasClamp = 0.0f;
        rasterizerDesc.DepthClipEnable = false;
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.FrontCounterClockwise = FALSE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.ScissorEnable = FALSE;
        rasterizerDesc.SlopeScaledDepthBias = 0.0f;

        dxDevice->CreateRasterizerState(
            &rasterizerDesc,
            &dxRasterizerStateF);

        rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

        dxDevice->CreateRasterizerState(
            &rasterizerDesc,
            &dxRasterizerStateW);
    }

    void ImGUIInit() override{
        ImGui_ImplDX11_Init(dxDevice.Get(), dxDeviceContext.Get());
    }

    void ImGUINewFrameLogic() override{
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void NewDX11Object() {
        UINT createDeviceFlags = 0; //D3D11_CREATE_DEVICE_BGRA_SUPPORT should also be here if need be
#if _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        ComPtr<ID3D11Device> dev;
        ComPtr<ID3D11DeviceContext> devC;
        D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE, //you always can use the software accelerator  :)
            nullptr,
            createDeviceFlags,
            featureLevels,
            _countof(featureLevels),
            D3D11_SDK_VERSION,
            &dev,
            nullptr,
            &devC);

        ThrowFailed(dev.As(&dxDevice));
        ThrowFailed(devC.As(&dxDeviceContext));

        dxDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxGIDevice);



        dxGIDevice->GetAdapter(&dxAdapter);

        dxAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxFactory);
    }
    
   
    void RendererStartUpLogic() override{
        
        NewDX11Object();

    }


    void CleanRendererState() override {

    }

}DX11Obj;