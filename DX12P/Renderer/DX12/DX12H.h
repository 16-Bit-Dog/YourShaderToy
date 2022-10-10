#include "rendererDefines.h"
#ifndef D3D12_OFF

#pragma once
//finally I will try to use com_ptr's

#include "DX12ShaderFuncs.h"
#include "3DDX12Obj.h"
#include "DX11H.h"
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include <../imGUI/imgui_impl_dx12.h>
#include "PipelineObj.h"

using namespace DirectX;

struct ResourceObjectBaseDX12;

struct PipelineObjectIntermediateStateDX12 {

    struct ComputeDataHolder {
        std::string CName;
        D3D12_SHADER_BYTECODE* CDat;
        bool AutoSetBlockToWindowSize = false;
        uint32_t DimX;
        uint32_t DimY;
        uint32_t DimZ;
    };



    inline static ID3D12Resource* SelectedFinalRTV = nullptr;

    inline static std::unordered_map<std::string, D3D12_SHADER_BYTECODE*> VertexShaderMap;
    inline static std::unordered_map<std::string, D3D12_SHADER_BYTECODE*> PixelShaderMap;
    inline static std::unordered_map<std::string, D3D12_SHADER_BYTECODE*> ComputeShaderMap;


    D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultGraphicsPipelineStateDesc;
    std::vector< D3D12_COMPUTE_PIPELINE_STATE_DESC > defaultComputePipelineStateDescV;
    std::vector<ComPtr<ID3D12GraphicsCommandList>> cmdLists;

    uint64_t RTV_Num = 0;
    uint64_t DEPTH_Num = 0;

    std::string VName = "";
    D3D12_SHADER_BYTECODE* VDat = nullptr;

    std::string PName = "";
    D3D12_SHADER_BYTECODE* PDat = nullptr;

    std::vector<ComputeDataHolder> Compute;

    void setCSize(const int& size) {
        Compute.resize(size);
    }

    PipelineObj* PObj;
    //TODO: add compute shader stuff -- std::vector 
    std::function<void()> ToRunLogic; //loops through code logic to run


    void PrepComputeShader() {
        defaultComputePipelineStateDescV.resize(Compute.size());
        for (int i = 0; i < defaultComputePipelineStateDescV.size(); i++) {
            defaultComputePipelineStateDescV[i] = {};
            defaultComputePipelineStateDescV[i].NodeMask = 0;
            defaultComputePipelineStateDescV[i].pRootSignature = MainDX12Objects::obj->defaultRootSig.Get();
            defaultComputePipelineStateDescV[i].CS = *Compute[i].CDat;
            defaultComputePipelineStateDescV[i].Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        }
    }
    void SetToDebug() {

        for (int i = 0; i < defaultComputePipelineStateDescV.size(); i++) {
            defaultComputePipelineStateDescV[i].Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
        }

    }
};

struct MainDX12Objects : Renderable{

    D3D12_BLEND_DESC MakeBlend(BlendTypeMapMadeData& BlendToMake) {
        D3D12_BLEND_DESC blend = {};

        blend.AlphaToCoverageEnable = BlendToMake.AlphaToCoverageEnable;
        blend.IndependentBlendEnable = BlendToMake.IndependentBlendEnable;
        blend.RenderTarget[0].BlendEnable = BlendToMake.BlendEnable;
        blend.RenderTarget[0].BlendOp = D3D12_BLEND_OP(BlendToMake.BlendOp);
        blend.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP(BlendToMake.BlendOpAlpha);
        blend.RenderTarget[0].DestBlend = D3D12_BLEND(BlendToMake.DestBlend);
        blend.RenderTarget[0].DestBlendAlpha = D3D12_BLEND(BlendToMake.DestBlendAlpha);
        blend.RenderTarget[0].RenderTargetWriteMask = BlendToMake.RenderTargetWriteMask;
        blend.RenderTarget[0].SrcBlend = D3D12_BLEND(BlendToMake.SrcBlend);
        blend.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND(BlendToMake.SrcBlendAlpha);

        return blend;
    }

    D3D12_DEPTH_STENCIL_DESC MakeDepthStencil(StencilTypeMapMadeData& StencilToMake) {
            D3D12_DEPTH_STENCIL_DESC depthStencilStateDesc;
            ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

            depthStencilStateDesc.DepthEnable = StencilToMake.EnableDepth; //this is a bad idea
            depthStencilStateDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK(StencilToMake.DepthWriteMask);
            depthStencilStateDesc.DepthFunc = D3D12_COMPARISON_FUNC(StencilToMake.DepthComp);
            depthStencilStateDesc.StencilEnable = StencilToMake.EnableStencil; //for now don't want it TODO: add stencil enable for faces properly later

            depthStencilStateDesc.StencilReadMask = StencilToMake.WhereToReadFromStencil;
            depthStencilStateDesc.StencilWriteMask = StencilToMake.WhereToWriteToStencil;
            // Stencil operations if pixel is front tri-facing.

            depthStencilStateDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP(StencilToMake.FrontFaceStencilFailOp);
            depthStencilStateDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP(StencilToMake.FrontFaceStencilDepthFailOp);
            depthStencilStateDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP(StencilToMake.FrontFaceStencilPassOp);
            depthStencilStateDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC(StencilToMake.FrontTriComp);

            // Stencil operations if pixel is back tri-facing.
            depthStencilStateDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP(StencilToMake.BackFaceStencilFailOp);
            depthStencilStateDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP(StencilToMake.BackFaceStencilDepthFailOp);
            depthStencilStateDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP(StencilToMake.BackFaceStencilPassOp);
            depthStencilStateDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC(StencilToMake.BackTriComp);

        }

    D3D12_RASTERIZER_DESC MakeRaster(RasterTypeMapMadeData& RasterToMake) {

        D3D12_RASTERIZER_DESC rasterizerDesc = {};

            rasterizerDesc.AntialiasedLineEnable = RasterToMake.AAL;
            rasterizerDesc.CullMode = D3D12_CULL_MODE(RasterToMake.cull);
            rasterizerDesc.DepthBias = 0.0f;
            rasterizerDesc.DepthBiasClamp = 0.0f;
            rasterizerDesc.SlopeScaledDepthBias = 0.0f;
            rasterizerDesc.DepthClipEnable = false;
            rasterizerDesc.FillMode = D3D12_FILL_MODE(RasterToMake.ToFill + 2);
            rasterizerDesc.FrontCounterClockwise = FALSE;
            rasterizerDesc.MultisampleEnable = FALSE;

            return rasterizerDesc;
    }

    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    
    inline static MainDX12Objects* obj;

    //ComPtr<ID3D11On12Device2> dx11OnDx12Device;

    Renderable* GetR() {
        return MainDX12Objects::obj->GetR();
    }


    D3D12_INPUT_LAYOUT_DESC dxIL;

    bool UseWarpDev = false;

    HWND hwnd;
    GLFWwindow* window;

    ComPtr<ID3D12DescriptorHeap> ImGUIHeap;

	static const UINT FrameCount = 2;

    struct RenderTargetObj {
        ComPtr<ID3D12Resource> m[FrameCount];
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle[FrameCount];

    };
    struct FrameContext
    {
        ID3D12CommandAllocator* commandAllocator;
        UINT64                  FenceValue;
    };

    //Pipeline Objects
    ComPtr<IDXGIFactory4> factory;
 
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugLayer;
#endif // DEBUG

    UINT m_RTV_IS;
    UINT m_SRV_CON_UAV_IS;
    UINT m_DSV_IS;
    UINT m_SAMP_IS;


    //for now heavily learned from MSDN docs
    HANDLE m_swapChainWaitableObject = NULL;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    ComPtr<ID3D12Device> dxDevice;
    RenderTargetObj m_renderTargets;
    ComPtr<IDXGISwapChain3> m_swapChain; //index 0 is main
    FrameContext FrameC[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;

    

    D3D12_DESCRIPTOR_HEAP_DESC descHeapConSrvUav;
    ComPtr<ID3D12DescriptorHeap> HeapConSrvUav;
    int BlockedOffSetForConHeapStart = 0;
    int BlockedOffSetForSrvHeapStart = 0;
    int BlockedOffSetForUavHeapStart = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE ResolveConBufLocation(int registerNum) {
        D3D12_CPU_DESCRIPTOR_HANDLE tmp;
        tmp.ptr = HeapConSrvUav->GetCPUDescriptorHandleForHeapStart().ptr + BlockedOffSetForConHeapStart + m_SRV_CON_UAV_IS * (registerNum - RegisterMaps::INITIAL_CB);
        return tmp;
    }
    D3D12_CPU_DESCRIPTOR_HANDLE ResolveSrvBufLocation(int registerNum) {
        D3D12_CPU_DESCRIPTOR_HANDLE tmp;
        tmp.ptr = HeapConSrvUav->GetCPUDescriptorHandleForHeapStart().ptr + BlockedOffSetForSrvHeapStart + m_SRV_CON_UAV_IS * (registerNum - RegisterMaps::INITIAL_SRV);
        return tmp;
    }
    D3D12_CPU_DESCRIPTOR_HANDLE ResolveUavBufLocation(int registerNum) {
        D3D12_CPU_DESCRIPTOR_HANDLE tmp;
        tmp.ptr = HeapConSrvUav->GetCPUDescriptorHandleForHeapStart().ptr + BlockedOffSetForUavHeapStart + m_SRV_CON_UAV_IS * (registerNum - RegisterMaps::INITIAL_UAV);
        return tmp;
    }

    D3D12_DESCRIPTOR_HEAP_DESC descHeapSamp;
    int BlockedOffSetForSampHeapStart = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE ResolveSampBufLocation(int registerNum) {
        D3D12_CPU_DESCRIPTOR_HANDLE tmp;
        tmp.ptr = HeapConSrvUav->GetCPUDescriptorHandleForHeapStart().ptr + BlockedOffSetForSampHeapStart + m_SAMP_IS * (registerNum - RegisterMaps::INITIAL_SAMP);
        return tmp;
    }

    ComPtr<ID3D12DescriptorHeap> HeapSamp;
    
    struct Heaps {
        Heaps() {

        }
        std::map<int, void*> RTVHeapRef;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        /*
        std::map<int, void*> SRVHeapRef;
        ComPtr<ID3D12DescriptorHeap> m_srvHeap;
        std::map<int, void*> UAVHeapRef;
        ComPtr<ID3D12DescriptorHeap> m_uavHeap;
        std::map<int, void*> CONHeapRef;
        ComPtr<ID3D12DescriptorHeap> m_conHeap;
        std::map<int, void*> SAMPHeapRef;
        ComPtr<ID3D12DescriptorHeap> m_samHeap;
        */
    }Heaps;
    
    
    enum {

        SrvDescRange = 0,
        UavDescRange,
        ConDescRange,
        SampDescRange,

    };

    enum {

        SrvUavConRootParam = 0,
        SampRootParam = 0,

    };

    std::vector<D3D12_DESCRIPTOR_RANGE> defaultDescRangeBlock;
    std::vector<D3D12_ROOT_DESCRIPTOR_TABLE> defaultDescTable;
    std::vector<D3D12_ROOT_PARAMETER> defaultRootParam;
    D3D12_ROOT_SIGNATURE_DESC defaultRootSigDesc;
    ComPtr<ID3D12RootSignature> defaultRootSig;

    std::vector< ComPtr<ID3D12PipelineState> > defaultPipelineState;
    std::vector < ComPtr<ID3D12PipelineState> > defaultComputePipelineState;
    ComPtr<ID3D12GraphicsCommandList> defaultCommandList;//main command list
    


    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    // Synchronization objects. - other than frame context
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_lastFenceValue = 0;
    
    std::vector<PipelineObjectIntermediateStateDX12*> CompiledCodeV; //use this ordered to pass through code states



    void ImGUIInit() override{
        ImGui_ImplDX12_Init(dxDevice.Get(), FrameCount, DXGI_FORMAT_R8G8B8A8_UNORM, ImGUIHeap.Get(), ImGUIHeap->GetCPUDescriptorHandleForHeapStart(), ImGUIHeap->GetGPUDescriptorHandleForHeapStart());
    }
    void ImGUINewFrameLogic() {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void CreateRenderTarget() {
        for (int n = 0; n < FrameCount; n++)
        {
            
            ThrowFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets.m[n])));
            
            dxDevice->CreateRenderTargetView(m_renderTargets.m[n].Get(), nullptr, m_renderTargets.rtvHandle[n]);
        }
    }

    void MakeImGUIHeap() {
        { //local scope creation
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            ThrowFailed(dxDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&ImGUIHeap)));
                

        }
    }

    void SetupRendererDebugLayer() {

#ifdef _DEBUG
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
            debugLayer->EnableDebugLayer();
        }
#endif

    }
    void GetHardwareAdapter(IDXGIFactory4* pFactory, IDXGIAdapter1** ppAdapter)
    {
        *ppAdapter = nullptr;
        for (UINT adapterIndex = 0; ; ++adapterIndex)
        {
            IDXGIAdapter1* pAdapter = nullptr;
            if (DXGI_ERROR_NOT_FOUND == pFactory->EnumAdapters1(adapterIndex, &pAdapter))
            {
                // No more adapters to enumerate.
                break;
            }

            // Check to see if the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                *ppAdapter = pAdapter;
                return;
            }
            pAdapter->Release();
        }
    }
    void SetupDXAdapterAndQueue() {
        ThrowFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

        if (UseWarpDev) {
            ComPtr<IDXGIAdapter> adapter;

            ThrowFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
            ThrowFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dxDevice)));
        }
        else {
            ComPtr<IDXGIAdapter1> Hadapter;

            GetHardwareAdapter(factory.Get(), &Hadapter);
            ThrowFailed(D3D12CreateDevice(Hadapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dxDevice)));
        }
        //create command queue

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ThrowFailed(dxDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    }
    DXGI_SWAP_CHAIN_DESC1 swapChainDescW;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainDescF;

    void CreateCommandListWithCurrAllocator(ComPtr<ID3D12GraphicsCommandList>& gcl, int backBuffer /*use m_swapChain->GetCurrentBackBufferIndex(); but call once for no overhead*/) {
        
        ThrowFailed(dxDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, FrameC[backBuffer].commandAllocator, NULL, IID_PPV_ARGS(&gcl)));

        ThrowFailed(defaultCommandList->Close());
    }

    void MakeNewWindowSwapChainAndAssociate(GLFWwindow* windowW, HWND sHwnd, int& sWidth, int& sHeight) override {


        window = windowW;
        MainWidth = sWidth;
        MainHeight = sHeight;
        hwnd = sHwnd;

        MainWidthR = &sWidth;
        MainHeightR = &sHeight;

        
        swapChainDescW.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        swapChainDescW.Width = MainWidth;
        swapChainDescW.Height = MainHeight;
        swapChainDescW.BufferCount = FrameCount;
        swapChainDescW.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDescW.Format = format;
        swapChainDescW.SampleDesc.Count = 1;
        swapChainDescW.SampleDesc.Quality = 0;
        swapChainDescW.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDescW.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDescW.Scaling = DXGI_SCALING_NONE;

        swapChainDescF.Scaling = DXGI_MODE_SCALING_STRETCHED;
        swapChainDescF.RefreshRate = refreshRateStatic;
        swapChainDescF.Windowed = !bFullScreen;

        ComPtr<IDXGISwapChain1> swapChain;
        
        ThrowFailed(factory->CreateSwapChainForHwnd(m_commandQueue.Get(), hwnd, &swapChainDescW, &swapChainDescF, NULL, swapChain.GetAddressOf()));

        swapChain.As(&m_swapChain);

        ThrowFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)); //I want to allow toggle of fullscreen and windowed mode

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        { //this is just an automatic scope*
            // Describe and create a render target view (RTV) descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = FrameCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            ThrowFailed(dxDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&Heaps.m_rtvHeap)));
        }
        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(Heaps.m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

            // Create a RTV for each frame.
            for (UINT n = 0; n < FrameCount; n++)
            {
                m_renderTargets.rtvHandle[n] = rtvHandle;

                rtvHandle.ptr += m_RTV_IS;

            }
        }
        for (int i = 0; i < FrameCount; i++)
            ThrowFailed(dxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&FrameC[i].commandAllocator)));

        ThrowFailed(dxDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, FrameC[0].commandAllocator, NULL, IID_PPV_ARGS(&defaultCommandList)));

        ThrowFailed(defaultCommandList->Close());

        ThrowFailed(dxDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
           
        m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        if (m_fenceEvent == NULL)
            throw("fence event not made");

        m_swapChain->SetMaximumFrameLatency(FrameCount);
        m_swapChainWaitableObject = m_swapChain->GetFrameLatencyWaitableObject();

        MakeImGUIHeap();
        CreateRenderTarget();
    }

    void RemoveWindowSwapChainAndAssociate() {

    }

    void PreloadAssets() {

    }

    void GetDescHandleIncrements() {
        m_RTV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        m_SRV_CON_UAV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_DSV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_SAMP_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    }
   

    void CleanRendererState() override {

    }

    void defaultDescriptorTableRanges() {
        defaultDescRangeBlock.resize(4);
        defaultRootParam.resize(2);

        defaultDescRangeBlock[SrvDescRange].BaseShaderRegister = RegisterMaps::INITIAL_SRV;
        defaultDescRangeBlock[SrvDescRange].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
        defaultDescRangeBlock[SrvDescRange].NumDescriptors = UINT_MAX;// MainDX12Objects::obj->Heaps.SRVHeapRef.size();
        defaultDescRangeBlock[SrvDescRange].RegisterSpace = 0;
        defaultDescRangeBlock[SrvDescRange].OffsetInDescriptorsFromTableStart = 0;

        defaultDescRangeBlock[UavDescRange].BaseShaderRegister = RegisterMaps::INITIAL_UAV;
        defaultDescRangeBlock[UavDescRange].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        defaultDescRangeBlock[UavDescRange].NumDescriptors = UINT_MAX;// MainDX12Objects::obj->Heaps.UAVHeapRef.size();
        defaultDescRangeBlock[UavDescRange].RegisterSpace = 0;
        defaultDescRangeBlock[UavDescRange].OffsetInDescriptorsFromTableStart = 0;

        defaultDescRangeBlock[ConDescRange].BaseShaderRegister = RegisterMaps::INITIAL_CB;
        defaultDescRangeBlock[ConDescRange].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
        defaultDescRangeBlock[ConDescRange].NumDescriptors = UINT_MAX;;// MainDX12Objects::obj->Heaps.CONHeapRef.size();
        defaultDescRangeBlock[ConDescRange].RegisterSpace = 0;
        defaultDescRangeBlock[ConDescRange].OffsetInDescriptorsFromTableStart = 0;

        defaultDescRangeBlock[SampDescRange].BaseShaderRegister = RegisterMaps::INITIAL_SAMP;
        defaultDescRangeBlock[SampDescRange].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
        defaultDescRangeBlock[SampDescRange].NumDescriptors = UINT_MAX;;// MainDX12Objects::obj->Heaps.SAMPHeapRef.size();
        defaultDescRangeBlock[SampDescRange].RegisterSpace = 0;
        defaultDescRangeBlock[SampDescRange].OffsetInDescriptorsFromTableStart = 0;


        defaultDescTable[SrvUavConRootParam].NumDescriptorRanges = 3;
        defaultDescTable[SrvUavConRootParam].pDescriptorRanges = defaultDescRangeBlock.data();


        defaultDescTable[SampRootParam].NumDescriptorRanges = 1;
        defaultDescTable[SampRootParam].pDescriptorRanges = &defaultDescRangeBlock.data()[SampDescRange];

        defaultRootParam[SrvUavConRootParam].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        defaultRootParam[SrvUavConRootParam].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        defaultRootParam[SrvUavConRootParam].DescriptorTable = defaultDescTable[SrvUavConRootParam];

        defaultRootParam[SampRootParam].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        defaultRootParam[SampRootParam].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        defaultRootParam[SampRootParam].DescriptorTable = defaultDescTable[SampRootParam];


    }

    void defaultRootSigConstruction() {
        defaultRootSigDesc.NumStaticSamplers = 0;
        defaultRootSigDesc.NumParameters = 2;
        defaultRootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        defaultRootSigDesc.pParameters = defaultRootParam.data();

        ComPtr<ID3DBlob> pOutBlob;
        ComPtr<ID3DBlob> pErrorBlob;
        ThrowFailed(D3D12SerializeRootSignature(&defaultRootSigDesc,
            D3D_ROOT_SIGNATURE_VERSION_1_0, pOutBlob.GetAddressOf(),
            pErrorBlob.GetAddressOf()));

        ThrowFailed(dxDevice->CreateRootSignature(0, pOutBlob->GetBufferPointer(),
            pOutBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
            (void**)&defaultRootSig));

        if (pErrorBlob != nullptr) {
            OutputDebugStringA("\n");
            OutputDebugStringA((const char*)pErrorBlob->GetBufferPointer());
        }

        descHeapConSrvUav.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descHeapConSrvUav.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descHeapConSrvUav.NodeMask = 0;
        
        descHeapSamp.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        descHeapSamp.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        descHeapSamp.NodeMask = 0;

        defaultCommandList->SetComputeRootSignature(defaultRootSig.Get());
        defaultCommandList->SetGraphicsRootSignature(defaultRootSig.Get());
        
    }
    
	void RendererStartUpLogic() override{
        SetupRendererDebugLayer();
        SetupDXAdapterAndQueue();
        GetDescHandleIncrements();

        defaultDescriptorTableRanges();
        defaultRootSigConstruction();

    }

    FrameContext* WaitForNextFrameResources() {
        UINT nextFrameIndex = m_frameIndex + 1;
        m_frameIndex = nextFrameIndex;

        HANDLE waitableObjects[] = { m_swapChainWaitableObject, NULL };
        DWORD numWaitableObjects = 1;

        FrameContext* frameC = &FrameC[nextFrameIndex % FrameCount];
        UINT64 fenceValue = frameC->FenceValue;
        if (fenceValue != 0) // means no fence was signaled
        {
            frameC->FenceValue = 0;
            m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
            waitableObjects[1] = m_fenceEvent;
            numWaitableObjects = 2;
        }

        WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

        return frameC;
    }

    void DrawLogic(bool vsync = true) override{
        //TODO- make drawer use the d3d11 draw logic call and wrap with dx12

        FrameContext* frameC = WaitForNextFrameResources(); //TODO: this frame calc for backbuffer
        UINT backBufferIdx = m_swapChain->GetCurrentBackBufferIndex();
        frameC->commandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = m_renderTargets.m[backBufferIdx].Get();
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        defaultCommandList->Reset(frameC->commandAllocator, NULL);
        defaultCommandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        XMFLOAT4 clear_color = { 0.5,1,0,1 };
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        defaultCommandList->ClearRenderTargetView(m_renderTargets.rtvHandle[backBufferIdx], clear_color_with_alpha, 0, NULL);
        defaultCommandList->OMSetRenderTargets(1, &m_renderTargets.rtvHandle[backBufferIdx], FALSE, nullptr);
        defaultCommandList->SetDescriptorHeaps(1, ImGUIHeap.GetAddressOf());
        
        if(NewImGUIDat) ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), defaultCommandList.Get());

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        defaultCommandList->ResourceBarrier(1, &barrier);
        ThrowFailed(defaultCommandList->Close());

        ID3D12CommandList* ppCommandLists[] = { defaultCommandList.Get() };
        m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(NULL, defaultCommandList.Get());

        m_swapChain->Present(vsync, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        UINT64 fenceValue = m_lastFenceValue + 1;
        m_commandQueue->Signal(m_fence.Get(), fenceValue);
        m_lastFenceValue = fenceValue;
        FrameC->FenceValue = fenceValue;

        NewImGUIDat = false; 
    }


    
};

#endif