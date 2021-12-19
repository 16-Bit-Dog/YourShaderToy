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
using namespace DirectX;

struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT4 col;
};

void ThrowFailed(HRESULT v) {
    if (v != 0x00000000) {
        throw("");
    }
}


struct MainDX12Objects {

    bool UseWarpDev = false;
   
    std::vector<UINT> Width;
    std::vector<UINT> Height;
    std::vector<HWND> hwnd;

	static const UINT FrameCount = 2;

    struct RenderTargetObj {
        ComPtr<ID3D12Resource> m[FrameCount];
    };

    //Pipeline Objects
    ComPtr<IDXGIFactory4> factory;
 
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugLayer;
#endif // DEBUG

    //for now heavily learned from MSDN docs
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    ComPtr<ID3D12Device> m_device;
    std::vector<RenderTargetObj> m_renderTargets;
    std::vector< ComPtr<IDXGISwapChain3> > m_swapChain; //index 0 is main
    std::vector< ComPtr<ID3D12CommandAllocator> > m_commandAllocator;
    std::vector< ComPtr<ID3D12CommandQueue> > m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    std::vector<ComPtr<ID3D12DescriptorHeap>> m_rtvHeap;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    std::vector<UINT> m_rtvDS;
    
    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    // Synchronization objects.
    std::vector <UINT> m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    void SetupRendererDebugLayer() {

#ifdef _DEBUG
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
            debugLayer->EnableDebugLayer();
        }
#endif

    }
    void SetupPipelineStateAndObject() {
        ThrowFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

        if (UseWarpDev) {
            ComPtr<IDXGIAdapter> adapter;

            ThrowFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter)));
            ThrowFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));
        }
        else {
            ComPtr<IDXGIAdapter1> Hadapter;

            ThrowFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&Hadapter)));
            ThrowFailed(D3D12CreateDevice(Hadapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));
        }

    }

    void MakeNewWindowSwapChainAndAssociate(HWND sHwnd, UINT sWidth, UINT sHeight) {

        Width.push_back(sWidth);
        Height.push_back(sHeight);
        hwnd.push_back(sHwnd);

        //create command queue
        m_commandQueue.push_back(ComPtr<ID3D12CommandQueue>());

        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ThrowFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue[m_commandQueue.size()-1])));

        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.BufferDesc.Width = sWidth;
        swapChainDesc.BufferDesc.Height = sHeight;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.OutputWindow = sHwnd;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.Windowed = TRUE;


        ComPtr<IDXGISwapChain> swapChain;
        ThrowFailed(factory->CreateSwapChain(
            m_commandQueue[m_commandQueue.size() - 1].Get(),
            &swapChainDesc,
            &swapChain
        ));
        
        m_swapChain.push_back(ComPtr<IDXGISwapChain3>());
        ThrowFailed(swapChain.As(&m_swapChain[m_swapChain.size() - 1]));

        //ThrowFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER)); I want to allow toggle of fullscreen and windowed mode

        m_frameIndex.push_back(UINT());
        m_frameIndex[m_frameIndex.size() - 1] = m_swapChain[m_swapChain.size() - 1]->GetCurrentBackBufferIndex();

        { //this is just an automatic scope*
            // Describe and create a render target view (RTV) descriptor heap.
            D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
            rtvHeapDesc.NumDescriptors = FrameCount;
            rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            m_rtvHeap.push_back(ComPtr<ID3D12DescriptorHeap>());
            ThrowFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap[m_rtvHeap.size() - 1])));

            m_rtvDS.push_back(UINT());
            m_rtvDS[m_rtvDS.size() - 1] = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        }

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap[m_rtvHeap.size() - 1]->GetCPUDescriptorHandleForHeapStart());

            // Create a RTV for each frame.
            m_renderTargets.push_back(RenderTargetObj());
            for (UINT n = 0; n < FrameCount; n++)
            {
                ThrowFailed(m_swapChain[m_swapChain.size() - 1]->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[m_renderTargets.size()-1].m[n])));
                m_device->CreateRenderTargetView(m_renderTargets[m_renderTargets.size()-1].m[n].Get(), nullptr, rtvHandle);
                rtvHandle.Offset(1, m_rtvDS[m_rtvDS.size()-1]);
            }
        }
        m_commandAllocator.push_back(ComPtr<ID3D12CommandAllocator>());
        ThrowFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[m_commandAllocator.size()-1])));

    }

    void RemoveWindowSwapChainAndAssociate() {

    }

    void PreloadAssets() {

    }

	void RendererStartUpLogic() {
        SetupRendererDebugLayer();
        SetupPipelineStateAndObject();
	}
}DXM;