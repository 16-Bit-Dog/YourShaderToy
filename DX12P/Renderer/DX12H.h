#pragma once
//finally I will try to use com_ptr's

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Windows Runtime Library. Needed for Microsoft::WRL::ComPtr<> template class.
#include <wrl.h>
using namespace Microsoft::WRL;

// DirectX 12 specific headers.
#include <d3d12.h>
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
    
    HWND hwnd;

	static const UINT FrameCount = 2;

    //Pipeline Objects
    ComPtr<IDXGIFactory4> factory;
 
#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugLayer;
#endif // DEBUG


    //for now heavily learned from MSDN docs
    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_rtvDSize;
    
    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    // Synchronization objects.
    UINT m_frameIndex;
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

        //create command queue
    }

	void RendererStartUpLogic() {
        SetupRendererDebugLayer();
        SetupPipelineStateAndObject();

	}
}DXM;