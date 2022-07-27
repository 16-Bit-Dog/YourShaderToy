#pragma once
//finally I will try to use com_ptr's

#include "DX12ShaderFuncs.h"
#include "DX11H.h"
#include <../imGUI/imgui.h>
#include <../imGUI/imgui_impl_glfw.h>
#include <../imGUI/imgui_impl_dx12.h>
#include "PipelineObj.h"

using namespace DirectX;

struct MainDX12Objects : Renderable{

    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    
    inline static MainDX12Objects* obj;

    ComPtr<ID3D11On12Device2> dx11OnDx12Device;

    MainDX11Objects* dx11obj;

    Renderable* GetR() {
        return MainDX11Objects::obj->GetR();
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

    //for now heavily learned from MSDN docs
    HANDLE m_swapChainWaitableObject = NULL;
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    ComPtr<ID3D12Device> dxDevice;
    RenderTargetObj m_renderTargets;
    ComPtr<IDXGISwapChain3> m_swapChain; //index 0 is main
    FrameContext FrameC[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_RTV_IS;
    UINT m_SRV_IS;
    UINT m_DSV_IS;
    UINT m_SAMP_IS;


    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    // Synchronization objects. - other than frame context
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_lastFenceValue = 0;
    
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

    void MakeNewWindowSwapChainAndAssociate(GLFWwindow* windowW, HWND sHwnd, int& sWidth, int& sHeight) override{


        window = windowW;
        MainWidth = sWidth;
        MainHeight = sHeight;
        hwnd = sHwnd;

        MainWidthR = &sWidth;
        MainHeightR = &sHeight;

        

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
        ThrowFailed(factory->CreateSwapChainForHwnd(m_commandQueue.Get(), hwnd, &swapChainDescW, &swapChainDescF, NULL, dx11obj->dxSwapChain.GetAddressOf()));

        //dx11obj->dxSwapChain = swapChain.Get();

        dx11obj->dxSwapChain = m_swapChain;
        
        
        dx11obj->MakeNewWindowSwapChainAndAssociate(window, glfwGetWin32Window(window), sWidth, sHeight);

        //ThrowFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)); I want to allow toggle of fullscreen and windowed mode

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        { //this is just an automatic scope*
            // Describe and create a render target view (RTV) descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = FrameCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            ThrowFailed(dxDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
        }

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

            // Create a RTV for each frame.
            for (UINT n = 0; n < FrameCount; n++)
            {
                m_renderTargets.rtvHandle[n] = rtvHandle;

                rtvHandle.ptr += m_RTV_IS;

            }
        }
        for (int i = 0; i < FrameCount; i++)
        ThrowFailed(dxDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&FrameC[i].commandAllocator)));

        ThrowFailed(dxDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, FrameC[0].commandAllocator, NULL, IID_PPV_ARGS(&m_commandList)));

        ThrowFailed(m_commandList->Close());

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
        m_SRV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        m_DSV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_SAMP_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    }

    void SetupDX11DX12() {

        dx11obj = MainDX11Objects::obj;

        UINT createDeviceFlags = 0; //D3D11_CREATE_DEVICE_BGRA_SUPPORT should also be here if need be
#if _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        ComPtr<ID3D11Device> tmpD;
        ComPtr<ID3D11DeviceContext> tmpDC;

        ThrowFailed(D3D11On12CreateDevice(
        dxDevice.Get(),
        createDeviceFlags,
        nullptr,
        0,
        reinterpret_cast<IUnknown**>(m_commandQueue.GetAddressOf()),
        1,
        0,
        tmpD.GetAddressOf(),//dx11obj->dxDevice.GetAddressOf(),
        tmpDC.GetAddressOf(),//dx11obj->dxDeviceContext.GetAddressOf(),
        nullptr
        ));

    // Query the 11On12 device from the 11 device.
        ThrowFailed(tmpD.As(&dx11obj->dxDevice));
        ThrowFailed(tmpDC.As(&dx11obj->dxDeviceContext));
        ThrowFailed(tmpD.As(&dx11OnDx12Device));

        dx11obj->MakeAdapterAndFactory();
        dx11obj->MakeAndSetCam();
    }

	void RendererStartUpLogic() override{
        SetupRendererDebugLayer();
        SetupDXAdapterAndQueue();
        GetDescHandleIncrements();
        SetupDX11DX12();

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
        m_commandList->Reset(frameC->commandAllocator, NULL);
        m_commandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        XMFLOAT4 clear_color = { 0.5,1,0,1 };
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        m_commandList->ClearRenderTargetView(m_renderTargets.rtvHandle[backBufferIdx], clear_color_with_alpha, 0, NULL);
        m_commandList->OMSetRenderTargets(1, &m_renderTargets.rtvHandle[backBufferIdx], FALSE, nullptr);
        m_commandList->SetDescriptorHeaps(1, ImGUIHeap.GetAddressOf());
        
        if(NewImGUIDat) ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList.Get());

        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        m_commandList->ResourceBarrier(1, &barrier);
        ThrowFailed(m_commandList->Close());

        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(1, ppCommandLists);

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(NULL, m_commandList.Get());

        m_swapChain->Present(vsync, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        UINT64 fenceValue = m_lastFenceValue + 1;
        m_commandQueue->Signal(m_fence.Get(), fenceValue);
        m_lastFenceValue = fenceValue;
        FrameC->FenceValue = fenceValue;

        NewImGUIDat = false; 
    }

    void CleanRendererState() override {

    }
};