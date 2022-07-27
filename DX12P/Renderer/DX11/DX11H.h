#pragma once 

#include "Renderable.h"
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include <../imGUI/imgui_impl_dx11.h>
#include "3DDX11Obj.h"
#include "PipelineObj.h"
#include "CamManagerD3D11.h"
#include <functional>

using namespace DirectX;

struct ResourceObjectBaseDX11;

struct PipelineObjectIntermediateStateDX11 {
    
    struct ComputeDataHolder {
        std::string CName;
        ID3D11ComputeShader* CDat;
        bool AutoSetBlockToWindowSize = false;
        uint32_t DimX;
        uint32_t DimY;
        uint32_t DimZ;
    };

    inline static ID3D11Resource* SelectedFinalRTV = nullptr; 

    inline static std::unordered_map<std::string, ComPtr<ID3D11VertexShader>> VertexShaderMap;
    inline static std::unordered_map<std::string, ComPtr<ID3D11PixelShader>> PixelShaderMap;
    inline static std::unordered_map<std::string, ComPtr<ID3D11ComputeShader>> ComputeShaderMap;
    
    uint64_t RTV_Num = 0;
    uint64_t DEPTH_Num = 0;

    std::string VName = "";
    ID3D11VertexShader* VDat = nullptr;

    std::string PName = "";
    ID3D11PixelShader* PDat = nullptr;

    std::vector<ComputeDataHolder> Compute;

    void setCSize(const int& size) {
        Compute.resize(size);
    }

    PipelineObj* PObj;
    //TODO: add compute shader stuff -- std::vector 
    std::function<void()> ToRunLogic;

};


struct MainDX11Objects : Renderable{

    Renderable* GetR() {
        return this;
    }

    struct InUseTest {
        ID3D11RasterizerState* RasterObject = nullptr;
        ID3D11DepthStencilState* DepthStencilObject = nullptr;
        ID3D11BlendState* BlendObject = nullptr;
        std::array<float,4> BlendFactor = {-1.0f,-1.0f, -1.0f, -1.0f};

        ID3D11VertexShader* VertexShader = nullptr;
        ID3D11PixelShader* PixelShader = nullptr;
        ID3D11ComputeShader* ComputeShader = nullptr;

        void* Model = nullptr;

        void SetNull() {
            RasterObject = nullptr;
            VertexShader = nullptr;
            PixelShader = nullptr;
            ComputeShader = nullptr;
            Model = nullptr;
        }
    };
    InUseTest TestForOptimize;

    std::unordered_map<RasterTypeMapMadeData, ComPtr<ID3D11RasterizerState>, RasterTypeMapMadeData_hash> RasterObjects;
    std::unordered_map<StencilTypeMapMadeData, ComPtr<ID3D11DepthStencilState>, StencilTypeMapMadeData_hash> DepthStencilObjects;
    std::unordered_map<BlendTypeMapMadeData, ComPtr<ID3D11BlendState>, BlendTypeMapMadeData_hash> BlendObjects;

    CameraManagerD3D11* CAM_S;

    std::vector<PipelineObjectIntermediateStateDX11*> CompiledCode; //use this ordered to pass through code states

    inline static MainDX11Objects* obj;

    ComPtr<ID3D11InputLayout> dxIL;

    //Pipeline Objects
    
    ComPtr<ID3D11Device5> dxDevice = 0;

#ifdef DEBUG
    ComPtr<ID3D11Debug> dxDebug = 0;
#endif // DEBUG

    ComPtr<ID3D11DeviceContext4> dxDeviceContext = 0;
    ComPtr<IDXGISwapChain1> dxSwapChain;

    D3D11_DEPTH_STENCIL_VIEW_DESC dxDepthStencilDesc{
    DXGI_FORMAT_D32_FLOAT,D3D11_DSV_DIMENSION_TEXTURE2D
    };

    ComPtr<ID3D11RenderTargetView> dxRenderTargetView = nullptr; 
    ComPtr<ID3D11Texture2D> dxDepthStencilBuffer = nullptr;
    ComPtr<ID3D11DepthStencilView> dxDepthStencilView = nullptr;
    ComPtr<ID3D11DepthStencilState> dxDepthStencilStateDefault = nullptr;

    D3D11_VIEWPORT dxViewport; //TODO create view port per scene view and draw after for scene view

    ComPtr<IDXGIFactory2> dxFactory;
    ComPtr<IDXGIAdapter> dxAdapter;
    ComPtr<IDXGIDevice> dxGIDevice;

    DXGI_SWAP_CHAIN_DESC1 swapChainDescW; //reuse for when recreating swap chain and parts to resize screen params
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainDescF;

    void MakeDepthStencil(StencilTypeMapMadeData& StencilToMake) {
        if (DepthStencilObjects.find(StencilToMake) == DepthStencilObjects.end()) {
            D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
            ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

            depthStencilStateDesc.DepthEnable = StencilToMake.EnableDepth; //this is a bad idea
            depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK(StencilToMake.DepthWriteMask);
            depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_FUNC(StencilToMake.DepthComp);
            depthStencilStateDesc.StencilEnable = StencilToMake.EnableStencil; //for now don't want it TODO: add stencil enable for faces properly later

            depthStencilStateDesc.StencilReadMask = StencilToMake.WhereToReadFromStencil;
            depthStencilStateDesc.StencilWriteMask = StencilToMake.WhereToWriteToStencil;
            // Stencil operations if pixel is front tri-facing.

            depthStencilStateDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP(StencilToMake.FrontFaceStencilFailOp);
            depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP(StencilToMake.FrontFaceStencilDepthFailOp);
            depthStencilStateDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP(StencilToMake.FrontFaceStencilPassOp);
            depthStencilStateDesc.FrontFace.StencilFunc = D3D11_COMPARISON_FUNC(StencilToMake.FrontTriComp);

            // Stencil operations if pixel is back tri-facing.
            depthStencilStateDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP(StencilToMake.BackFaceStencilFailOp);
            depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP(StencilToMake.BackFaceStencilDepthFailOp);
            depthStencilStateDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP(StencilToMake.BackFaceStencilPassOp);
            depthStencilStateDesc.BackFace.StencilFunc = D3D11_COMPARISON_FUNC(StencilToMake.BackTriComp);


            dxDevice->CreateDepthStencilState(
                &depthStencilStateDesc, 
                &DepthStencilObjects[StencilToMake]);
        }
    }
    void MakeBlend(BlendTypeMapMadeData& BlendToMake) {
        D3D11_BLEND_DESC blend;
        ZeroMemory(&blend, sizeof(D3D11_BLEND_DESC));

        blend.AlphaToCoverageEnable = BlendToMake.AlphaToCoverageEnable;
        blend.IndependentBlendEnable = BlendToMake.IndependentBlendEnable;
        blend.RenderTarget[0].BlendEnable = BlendToMake.BlendEnable;
        blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP(BlendToMake.BlendOp);
        blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP(BlendToMake.BlendOpAlpha);
        blend.RenderTarget[0].DestBlend = D3D11_BLEND(BlendToMake.DestBlend);
        blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND(BlendToMake.DestBlendAlpha);
        blend.RenderTarget[0].RenderTargetWriteMask = BlendToMake.RenderTargetWriteMask;
        blend.RenderTarget[0].SrcBlend = D3D11_BLEND(BlendToMake.SrcBlend);
        blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND(BlendToMake.SrcBlendAlpha);

    }
    void MakeRaster(RasterTypeMapMadeData& RasterToMake) {
        if (RasterObjects.find(RasterToMake) == RasterObjects.end()) {

            D3D11_RASTERIZER_DESC rasterizerDesc;
            ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

            rasterizerDesc.AntialiasedLineEnable = RasterToMake.AAL;
            rasterizerDesc.CullMode = D3D11_CULL_MODE(RasterToMake.cull);
            rasterizerDesc.DepthBias = 0.0f;
            rasterizerDesc.DepthBiasClamp = 0.0f;
            rasterizerDesc.SlopeScaledDepthBias = 0.0f;
            rasterizerDesc.DepthClipEnable = false;
            rasterizerDesc.FillMode = D3D11_FILL_MODE(RasterToMake.ToFill+2);
            rasterizerDesc.FrontCounterClockwise = FALSE;
            rasterizerDesc.MultisampleEnable = FALSE;
            rasterizerDesc.ScissorEnable = FALSE;


            dxDevice->CreateRasterizerState(
                &rasterizerDesc,
                &RasterObjects[RasterToMake]);

        }
    }

    
    void ClearBuffer(bool clearColorT = true, XMFLOAT4 p = { 0.0f,0.5f,0.0f,1.0f }) {
        if (clearColorT) {
            float ClearColor[4] = { float(p.x), float(p.y), float(p.z), float(p.w) };
            dxDeviceContext->ClearRenderTargetView(dxRenderTargetView.Get(), ClearColor);
        }
    }

    void ClearBufferDepth(bool ClearDepth = true) {
        if (ClearDepth) dxDeviceContext->ClearDepthStencilView(dxDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
    }
    
    void CopyRTVFinalBuffer() {
        ID3D11Texture2D* backBuffer = nullptr;
        dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

        dxDeviceContext->CopyResource(backBuffer, (ID3D11Texture2D*) PipelineObjectIntermediateStateDX11::SelectedFinalRTV);    
       
        SafeRelease(backBuffer);
    }

    void DrawOnMainWindow() {
        dxDeviceContext->RSSetViewports(1, &dxViewport);
        
        //dxDeviceContext->OMSetDepthStencilState(dxDepthStencilStateDefault.Get(), 0); //TODO add dynamic control of depth stencil state per draw and UINT tracker to stop rebinding
        MainDX11Objects::obj->dxDeviceContext->IASetInputLayout(MainDX11Objects::obj->dxIL.Get());
        MainDX11Objects::obj->dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        ID3D11Buffer* tmpB[6] = {CAM_S->DefWorldMatrix.Get(), CAM_S->DefViewMatrix.Get(), CAM_S->DefProjMatrix.Get(), CAM_S->WorldMatrix.Get(), CAM_S->ViewMatrix.Get(), CAM_S->ProjMatrix.Get() };
        MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers(0, 6, tmpB);
        MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers(0, 6, tmpB);
        TestForOptimize.SetNull();

        RtvAndDepthBlock::ClearRTVAndDepth();
        for (const auto& i : CompiledCode) {
            i->ToRunLogic();
        }

        if (RtvAndDepthBlock::SizeOfRTV() > 0) {
            CopyRTVFinalBuffer();
        }

        dxDeviceContext->OMSetRenderTargets(1, dxRenderTargetView.GetAddressOf(), dxDepthStencilView.Get()); //TODO: allow setting own depth stencil [new Depth Stencil] to allow fun stuff
    }

    void DrawOnSideWindow() {
        //TODO: make scene draw view work
    }

    ID3D11Resource* GetResourceOfUAVSRV(ID3D11View* u) {
        ID3D11Resource* r = nullptr;
        u->GetResource(&r);
        return r;
    }

    void SetNullRTVandDEPTH() {
        ID3D11RenderTargetView* nullViews[] = { nullptr };
        dxDeviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    }

    void DestroySwapChainAssociate(bool makeRTVandDEPTH = true) {
        ID3D11Texture2D* backBuffer = nullptr;

        if (dxSwapChain != nullptr) {
            dxDeviceContext->ClearState();
            dxDeviceContext->Flush();

            dxRenderTargetView.Reset();
            // Clear the previous window size specific context.
            SetNullRTVandDEPTH();
            dxDeviceContext->Flush();

            dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
        }
        if (backBuffer != nullptr) {
            SafeReleaseAlt(backBuffer);
        }
        if (dxSwapChain != nullptr) {
            BufferReset = true;
            RtvAndDepthBlock::ReleaseAllRTVAndDepth();
            if(makeRTVandDEPTH) RtvAndDepthBlock::MakeRTVAndDepth();
        }
    }
    void CreateSwapChainAndAssociate(DXGI_FORMAT format) {
        swapChainDescW.Width = MainWidth;
        swapChainDescW.Height = MainHeight;
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

        DestroySwapChainAssociate();

        if(dxSwapChain != nullptr){
#ifdef _DEBUG
            //dxDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif // _DEBUG

            ThrowFailed(dxSwapChain->ResizeBuffers(0, MainWidth, MainHeight, format, swapChainDescW.Flags));
            
        }
        else {
            dxFactory->CreateSwapChainForHwnd(dxDevice.Get(), hwnd, &swapChainDescW, &swapChainDescF, NULL, dxSwapChain.GetAddressOf());
        }

        ThrowFailed(dxSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer));
        
        ThrowFailed(dxDevice->CreateRenderTargetView(
            backBuffer,
            nullptr,
            dxRenderTargetView.GetAddressOf()));
        
        SafeRelease(backBuffer);
        
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        depthStencilBufferDesc.CPUAccessFlags = 0;
        depthStencilBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthStencilBufferDesc.Width = MainWidth;
        depthStencilBufferDesc.Height = MainHeight;
        depthStencilBufferDesc.MipLevels = 1;

        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.SampleDesc.Quality = 0;

        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

        ThrowFailed(dxDevice->CreateTexture2D(
            &depthStencilBufferDesc,
            nullptr,
            dxDepthStencilBuffer.GetAddressOf()));

        ThrowFailed(dxDevice->CreateDepthStencilView(
            dxDepthStencilBuffer.Get(),
            &dxDepthStencilDesc,
            dxDepthStencilView.GetAddressOf()));
            
        dxViewport.Width = MainWidth;
        dxViewport.Height = MainHeight;
        dxViewport.TopLeftX = 0.0f;
        dxViewport.TopLeftY = 0.0f;
        dxViewport.MinDepth = 0.0f;
        dxViewport.MaxDepth = 1.0f;
    }

    void MakeNewWindowSwapChainAndAssociate(GLFWwindow* windowW, HWND sHwnd, int& sWidth, int& sHeight) override{
        window = windowW;
        MainWidth = sWidth;
        MainHeight = sHeight;
        hwnd = sHwnd;

        DEVMODEA WinMonitorInfo;

        EnumDisplaySettingsA(NULL, ENUM_CURRENT_SETTINGS, &WinMonitorInfo);

        refreshRateStatic.Numerator = WinMonitorInfo.dmDisplayFrequency;
        refreshRateStatic.Denominator = 1;

        MainWidthR = &sWidth;
        MainHeightR = &sHeight;

        CreateSwapChainAndAssociate(DXGI_FORMAT_R8G8B8A8_UNORM);

        D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
        ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));


        depthStencilStateDesc.DepthEnable = FALSE; //this is a bad idea...
        depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilStateDesc.StencilEnable = FALSE;


        dxDevice->CreateDepthStencilState(&depthStencilStateDesc, dxDepthStencilStateDefault.GetAddressOf());

#ifdef DX11OBJ_LOADER
        DX11M3DR::SET_DX_CONTENT(dxDevice, dxDeviceContext);
#endif
//#ifdef DX11_H_RESOURCE_OBJ
//        DX11_OBJ_RESOURCE_S::SET_DX_CONTENT(dxDevice, dxDeviceContext, this);
//#endif
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

    }
    
    void MakeAdapterAndFactory() {
        //TODO, maybe use IDD_PPV_ARGS ? I can, but its not verbose I guess :Shrug:
        dxDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxGIDevice);
#ifdef DEBUG
        dxDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&dxDebug);
#endif // DEBUG
        dxGIDevice->GetAdapter(&dxAdapter);

        dxAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)dxFactory.GetAddressOf());
    }
   
    void MakeAndSetCam() {
        CAM_S = new CameraManagerD3D11(dxDevice.Get(), dxDeviceContext.Get(), &dxViewport);
        CAM = CAM_S;
    }
    void RendererStartUpLogic() override{
        NewDX11Object();
        MakeAdapterAndFactory();
        MakeAndSetCam();
    }


    void CleanRendererState() override {
        DestroySwapChainAssociate(false);

        if (ROB != nullptr){
            ROB->wrapClearAll(true);
            delete ROB;
            Renderable::ROB = nullptr;
        }

        ImGui_ImplDX11_Shutdown();

        SafeReleaseAlt(dxFactory);
        SafeReleaseAlt(dxAdapter);
        SafeReleaseAlt(dxGIDevice);
        SafeReleaseAlt(dxDebug);

        dxDeviceContext->ClearState();
        dxDeviceContext->Flush();
        SafeReleaseAlt(dxDevice);
        SafeReleaseAlt(dxDeviceContext);



        delete CAM_S;
        CompiledCode.clear();
    
    }

    void CheckAndResizeMainBuffer() {
       
        if (*MainWidthR != MainWidth || *MainHeightR != MainHeight) {
            MainWidth = *MainWidthR;
            MainHeight = *MainHeightR;
            CreateSwapChainAndAssociate(DXGI_FORMAT_R8G8B8A8_UNORM);
        }
    }

    void DrawLogic(bool sync = true) override {

        CheckAndResizeMainBuffer();

        if (NewImGUIDat) {
            ImGui::Render();
        }

        DrawOnMainWindow();
        DrawOnSideWindow(); //TODO: draw on Scene view with special movment
        
        if (NewImGUIDat) {
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        dxSwapChain->Present(sync, 0); // Present with vsync

        NewImGUIDat = false;
    }

};
