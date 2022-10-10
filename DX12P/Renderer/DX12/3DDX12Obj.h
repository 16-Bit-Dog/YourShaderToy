//this lib should work as a lone pair with 3DCommon and DXSafeInclude for anyone coding
#ifndef OFF_DX12OBJ

#ifndef DX12OBJ_LOADER
#define DX12OBJ_LOADER
#define WIN32_LEAN_AND_MEAN
using namespace Microsoft::WRL;
// DirectX 11 & windows specific headers/lib.
#pragma comment(lib, "Dwmapi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

#include <Windows.h>
#include <dwmapi.h>
#include <d3d12.h>
#include "d3dx12.h" //very good helper struct for DX12 - MSDN just refuses to teach raw stuff like D3D12_CPU_DESCRIPTOR_HANDLE prob due to it being a bad idea
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <AtlBase.h>
#include <atlconv.h>
#include <objidl.h>
#include <wincodec.h>
#include <algorithm>
#include <string>
#include "DXSafeInclude.h"


#include "3DCommons/3DCommon.h"
//#include "RenderableManager.h"

//TODO: load texture with WCI loader

struct DX12M3DR : M3DR{


	static void CreateCBufDepth1(ComPtr<ID3D12Resource>& resource, D3D12_CPU_DESCRIPTOR_HANDLE& resourceCpuDesc, int size, ComPtr<ID3D12Device>& dxDevice) {

		D3D12_CLEAR_VALUE CV = {};
		CV.Format = DXGI_FORMAT_UNKNOWN;
		CV.Color[0] = 0.0f;
		CV.Color[1] = 0.0f;
		CV.Color[2] = 0.0f;
		CV.Color[3] = 1.0f;

		D3D12_RESOURCE_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(D3D12_RESOURCE_DESC));
		gpuTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		gpuTexDesc.Format = DXGI_FORMAT_UNKNOWN;

		gpuTexDesc.Width = size;
		gpuTexDesc.Height = 1;

		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.DepthOrArraySize = 1;
		//gpuTexDesc.SampleDesc = 
		gpuTexDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;

		D3D12_HEAP_PROPERTIES heapP;
		heapP.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
		heapP.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
		heapP.CreationNodeMask = 0;
		heapP.VisibleNodeMask = 0;

		D3D12_HEAP_FLAGS heapF = {};

		D3D12_RESOURCE_STATES Rstate = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER
			| D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		dxDevice->CreateCommittedResource(
			&heapP,
			heapF,
			&gpuTexDesc,
			Rstate,
			&CV,
			//&defaultResourceData,
			__uuidof(resource), &resource);

		D3D12_CONSTANT_BUFFER_VIEW_DESC CbufDesc{};
		CbufDesc.SizeInBytes = size;
		CbufDesc.BufferLocation = resource->GetGPUVirtualAddress();


		dxDevice->CreateConstantBufferView(&CbufDesc, resourceCpuDesc);
	}

	inline static D3D12_INPUT_ELEMENT_DESC dxVertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDID", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	struct TextureObject {
		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, TEX_COUNT> TexSRV;
		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, TEX_COUNT > TexUAV;
		std::array <ComPtr<ID3D12Resource>, TEX_COUNT> TexR;
	
		void Load_Texture(int num, MaterialDataNamePair& mdnp) {
			
			D3D12_SUBRESOURCE_DATA defaultResourceData;
			defaultResourceData.pData = mdnp.d;
			defaultResourceData.RowPitch = mdnp.width * mdnp.channels;
			defaultResourceData.SlicePitch = mdnp.height * mdnp.width * mdnp.channels;

			D3D12_RESOURCE_DESC gpuTexDesc;	
			ZeroMemory(&gpuTexDesc, sizeof(D3D12_RESOURCE_DESC));
			gpuTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			gpuTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		
			gpuTexDesc.Width = mdnp.width;
			gpuTexDesc.Height = mdnp.height;
		
			gpuTexDesc.MipLevels = 1;
			gpuTexDesc.DepthOrArraySize = 1;

			gpuTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			gpuTexDesc.SampleDesc.Count = 1;
			gpuTexDesc.SampleDesc.Quality = 0;


			D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
			//UAVDesc.Texture2D
			ZeroMemory(&UAVDesc, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
			UAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //
			UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			
			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory(&SRVDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
			SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

			D3D12_HEAP_PROPERTIES heapP;
			heapP.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
			heapP.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
			heapP.CreationNodeMask = 0;
			heapP.VisibleNodeMask = 0;
			

			D3D12_HEAP_FLAGS heapF = {};

			D3D12_RESOURCE_STATES Rstate = D3D12_RESOURCE_STATE_UNORDERED_ACCESS 
				| D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

			D3D12_CLEAR_VALUE CV = {};
			CV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			CV.Color[0] = 0.0f;
			CV.Color[1] = 0.0f;
			CV.Color[2] = 0.0f;
			CV.Color[3] = 1.0f;
			

			dxDevice->CreateCommittedResource(
				&heapP,
				heapF,
				&gpuTexDesc,
				Rstate,
				&CV,
				__uuidof(TexR[num]), &TexR[num]);
			
			
			D3D12_BOX box = {};
			box.top = 0;
			box.bottom = gpuTexDesc.Height;
			box.left = 0;
			box.right = gpuTexDesc.Width;

			TexR[num]->WriteToSubresource(
				0,
				&box,
				defaultResourceData.pData,
				defaultResourceData.RowPitch,
				defaultResourceData.SlicePitch);

			dxDevice->CreateUnorderedAccessView(TexR[num].Get(), NULL, &UAVDesc, TexUAV[num]);

			dxDevice->CreateShaderResourceView(TexR[num].Get(), &SRVDesc, TexSRV[num]);
		}
	
	};

	std::vector<TextureObject> TexObj;
	
	void DefaultSamp() {

	}

	void LoadTextures() {
		DefaultSamp();
		TexObj.resize(MatData.size());

		for(int i = 0; i < MatData.size(); i++) {
			for (int ii = 0; ii < TEX_COUNT; ii++) {
				if (MatData[i].TexOn[ii]){
					TexObj[i].Load_Texture(ii, MatDataName[i].Tex[ii]);
				}
			}
		}
	}

	struct MaterialDX12 {

		bool UpdateMat = false;
		D3D12_CPU_DESCRIPTOR_HANDLE CpuDesc;
		ComPtr<ID3D12Resource> MatDataBuf;
		
	};

	inline static ComPtr<ID3D12Device> dxDevice;
	inline static UINT m_RTV_IS;
	inline static UINT m_SRV_IS;
	inline static UINT m_DSV_IS;
	inline static UINT m_SAMP_IS;

	inline static void SET_DX_CONTENT(ComPtr<ID3D12Device> dxDevice_tmp) {
		DX12M3DR::dxDevice = dxDevice_tmp.Get();
		m_RTV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_SRV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_DSV_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		m_SAMP_IS = dxDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	}
	D3D12_RENDER_TARGET_BLEND_DESC blendVal{};

	std::vector<MaterialDX12> Mat = {};
	
	ComPtr<ID3D12DescriptorHeap> SamplerHeap;

	D3D12_BLEND_DESC BlendState = {};

	ComPtr<ID3D12Resource> CBuf;
	D3D12_CPU_DESCRIPTOR_HANDLE CBufCpuDesc;

	ComPtr<ID3D12Resource> ArmatureCBuf;
	D3D12_CPU_DESCRIPTOR_HANDLE ArmatureCBufCpuDesc;

	std::vector<ComPtr<ID3D12Resource>> VBuf;
	std::vector<D3D12_VERTEX_BUFFER_VIEW> VBufView;
	std::vector < D3D12_CPU_DESCRIPTOR_HANDLE > VBufUAV;
	std::vector < ComPtr<ID3D12Resource> > IBuf;
	std::vector < D3D12_INDEX_BUFFER_VIEW > IBufView;
	 



	void CreateArmatureCBuf() {

		if (BoneDataTLMA.size() == 0) {
			XMFLOAT4X4 tmpForFill = { 1.0f,0.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f };

			BoneDataTLMA.push_back(tmpForFill);
		}

		int size = sizeof(XMFLOAT4X4) * BoneDataTLMA.size();

		CreateCBufDepth1(ArmatureCBuf, ArmatureCBufCpuDesc, size, dxDevice);
		
		
		if (BoneDataTLMA.size() != 0) {
			//dxDeviceContext->UpdateSubresource(ArmatureCBuf.Get(), 0, nullptr, &BoneDataTLMA[0], 0, 0);
			D3D12_BOX box = {};
			box.top = 0;
			box.bottom = 1;
			box.left = 0;
			box.right = sizeof(XMFLOAT4X4) * BoneDataTLMA.size();

			ArmatureCBuf->WriteToSubresource(
				0,
				&box,
				&BoneDataTLMA[0],
				size,
				1);
		}
		else {

		}
	}
	void DefaultMatBuf(int i) {

		CreateCBufDepth1(Mat[i].MatDataBuf, Mat[i].CpuDesc, sizeof(MaterialData), dxDevice);

		D3D12_BOX box = {};
		box.top = 0;
		box.bottom = 1;
		box.left = 0;
		box.right = sizeof(MaterialData);


		Mat[i].MatDataBuf->WriteToSubresource(
			0,
			&box,
			&MatData[0],
			sizeof(MaterialData),
			1);
	}

	void DefaultAllMatBuf() {
		Mat.resize(MatData.size());
		for (int i = 0; i < Mat.size(); i++) {
			DefaultMatBuf(i);
		}
	}
	
	void LoadVertexIndiceData() //also reloads vertex data
	{

		VBuf.resize(modelDat.size());
		VBufView.resize(modelDat.size());
		VBufUAV.resize(modelDat.size());

		IBuf.resize(modelDat.size());
		IBufView.resize(modelDat.size());

		for (int i = 0; i < VBuf.size(); i++) {
			SafeRelease(VBuf[i]);
			SafeRelease(IBuf[i]);


			D3D12_HEAP_PROPERTIES props;
			memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
			props.Type = D3D12_HEAP_TYPE_UPLOAD;
			props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			D3D12_RESOURCE_DESC desc;
			memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Width = modelDat[i].size() * sizeof(VNT);
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			dxDevice->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&VBuf[i]));

			D3D12_BOX box = {};
			box.top = 0;
			box.bottom = 1;
			box.left = 0;
			box.right = desc.Width;


			VBuf[i]->WriteToSubresource(0, &box, &modelDat[i], desc.Width, 1);

			D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
			//UAVDesc.Texture2D
			ZeroMemory(&UAVDesc, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
			UAVDesc.Format = DXGI_FORMAT_UNKNOWN; //
			UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;

			dxDevice->CreateUnorderedAccessView(VBuf[i].Get(), nullptr, &UAVDesc, VBufUAV[i]);
	
			VBufView[i].BufferLocation = VBuf[i]->GetGPUVirtualAddress();
			VBufView[i].SizeInBytes = desc.Width;
			VBufView[i].StrideInBytes = sizeof(VNT);

			//Create Index Buffer
			//D3D12_HEAP_PROPERTIES props;
			memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
			props.Type = D3D12_HEAP_TYPE_UPLOAD;
			props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			//D3D12_RESOURCE_DESC desc;
			memset(&desc, 0, sizeof(D3D12_RESOURCE_DESC));
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Width = sizeof(UINT) * (Indice[i].size());
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;
			dxDevice->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&IBuf[i]));

			box.top = 0;
			box.bottom = 1;
			box.left = 0;
			box.right = desc.Width;


			IBuf[i]->WriteToSubresource(0, &box, &modelDat[i], desc.Width, 1);

			IBufView[i].BufferLocation = IBuf[i]->GetGPUVirtualAddress();
			IBufView[i].Format = DXGI_FORMAT_UNKNOWN;
			IBufView[i].SizeInBytes = desc.Width;

		}
	}
	void DefaultCBuf() {
		CreateCBufDepth1(CBuf, CBufCpuDesc, sizeof(MaterialData), dxDevice); 
		
		D3D12_BOX box = {};
		box.top = 0;
		box.bottom = 1;
		box.left = 0;
		box.right = sizeof(MaterialData);


		CBuf->WriteToSubresource(
			0,
			&box,
			&ObjTune,
			sizeof(ObjTuneStatReg),
			1);
	}
	void DefaultSampler() {

		
		D3D12_DESCRIPTOR_HEAP_DESC samplerHDesc;
		samplerHDesc.NumDescriptors = 1;
		samplerHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		samplerHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		
		dxDevice->CreateDescriptorHeap(&samplerHDesc, IID_PPV_ARGS(&SamplerHeap));
		D3D12_SAMPLER_DESC samplerDesc;

		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_WRAP };
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_WRAP };
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_WRAP };
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 8;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC{ D3D12_COMPARISON_FUNC_NEVER }; //never remove model for now - always pass render pass
		samplerDesc.MinLOD = 1;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.Filter = D3D12_FILTER{ D3D12_FILTER_MIN_MAG_MIP_LINEAR };
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_MIRROR };
		
		
		dxDevice->CreateSampler(&samplerDesc, SamplerHeap->GetCPUDescriptorHandleForHeapStart());
	}

	void SetupBlendStateDefault() {
		//remember, independent blend should prob be toggleable or off

		blendVal.BlendEnable = true;
		blendVal.BlendOp = D3D12_BLEND_OP_ADD;
		blendVal.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blendVal.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blendVal.DestBlendAlpha = D3D12_BLEND_ZERO;
		blendVal.LogicOp = {};
		blendVal.LogicOpEnable = false;
		blendVal.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blendVal.SrcBlend = D3D12_BLEND_SRC_ALPHA ;
		blendVal.SrcBlendAlpha = D3D12_BLEND_ONE;

		
	}
	void AlwaysRunPostM3DR() {
		CreateArmatureCBuf();
		LoadVertexIndiceData();
		DefaultCBuf();
		SetupBlendStateDefault();
		DefaultSampler();
		DefaultAllMatBuf();
		LoadTextures();
	}

	DX12M3DR(std::vector<VNT>* V, bool ClearPtr = true, float ScaleVertex_t = 1.0f) {
		LoadVertexVectorBased(V, ClearPtr, ScaleVertex_t);

		//DX11 Specific

		AlwaysRunPostM3DR();

	}
	DX12M3DR(const std::string& path = "", float ScaleVertex_t = 1.0f) {
		LoadPathBased(path, ScaleVertex_t);

		//DX11 Specific

		AlwaysRunPostM3DR();

	}
	DX12M3DR() {

	}

	DX12M3DR& operator= (M3DR& tmp) {
		this->VertexStride = tmp.VertexStride;

		this->FilePath = tmp.FilePath;
		this->DirPath = tmp.DirPath;
		this->MatData = tmp.MatData;
		this->MatDataName = tmp.MatDataName;
		this->ScaleVertex = tmp.ScaleVertex;
		this->UpdateVDat = tmp.UpdateVDat;
		this->UpdateAnimDat = tmp.UpdateAnimDat;
		this->ObjTune = tmp.ObjTune;
		this->ObjTune = tmp.ObjTune;
		this->modelDat = tmp.modelDat;

		this->modelDatCheck = tmp.modelDatCheck;
		this->Bones = tmp.Bones;
		this->rootObj = tmp.rootObj;
		this->BoneDataTLMA = tmp.BoneDataTLMA;
		this->animNameS = tmp.animNameS;
		this->animNameI = tmp.animNameI;
		this->animStack = tmp.animStack;

		this->animStackMaxTime = tmp.animStackMaxTime;
		this->AnimVCacheMade = tmp.AnimVCacheMade;
		this->AnimVCacheData = tmp.AnimVCacheData;
		this->animDat = tmp.animDat;
		this->ClusterObject = tmp.ClusterObject;
		this->VboneDat = tmp.VboneDat;
		this->Indice = tmp.Indice;
		this->globalInverseTransform = tmp.globalInverseTransform;
		this->globalTransform = tmp.globalTransform;

		return *this;
	}
	

	DX12M3DR(M3DR& tmp) {
		*this = tmp;
		/*
		TODO, load tmp into d3d11dxr, and immedeatly load m3dr into for file manager --> change it so:
		- file manager can show data right away. - then this specific dx11 data is only in compile mode
			- delete dx11 is seperate from m3dr regular - every compile reloads and copies
			*/
		AlwaysRunPostM3DR();
	}
	~DX12M3DR() {
		//TODO: make deconstructor
	}
};


#endif

#endif