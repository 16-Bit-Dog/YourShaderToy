//this lib should work as a lone pair with 3DCommon and DXSafeInclude for anyone coding

#ifndef DX11OBJ_LOADER
#define DX11OBJ_LOADER
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
#include "DXSafeInclude.h"


#include "3DCommons/3DCommon.h"
//#include "RenderableManager.h"

//TODO: load texture with WCI loader

struct DX12M3DR : M3DR{

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

			/*
			std::vector<uint8_t> a;
			a.resize(defaultResourceData.SysMemSlicePitch);
			memcpy(&a[0], &mdnp.d[0], defaultResourceData.SysMemSlicePitch);
			int b = a[defaultResourceData.SysMemSlicePitch-1];
			*/

			D3D12_RESOURCE_DESC gpuTexDesc;
			ZeroMemory(&gpuTexDesc, sizeof(D3D12_RESOURCE_DESC));
			gpuTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		
			gpuTexDesc.Width = mdnp.width;
			gpuTexDesc.Height = mdnp.height;
		
			gpuTexDesc.MipLevels = 1;
			gpuTexDesc.DepthOrArraySize = 1;
			//gpuTexDesc.SampleDesc = 
			gpuTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			gpuTexDesc.SampleDesc.Count = 1;
			gpuTexDesc.SampleDesc.Quality = 0;

			D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
			//UAVDesc.Texture2D
			ZeroMemory(&UAVDesc, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
			UAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //
			UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			
			D3D12_HEAP_PROPERTIES heapP;
			heapP.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapP.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_NOT_AVAILABLE;
			heapP.MemoryPoolPreference = D3D12_MEMORY_POOL_L1;
			heapP.CreationNodeMask = 0;
			heapP.VisibleNodeMask = 0;

			D3D12_HEAP_FLAGS heapF = 


			dxDevice->CreateCommittedResource(
					&gpuTexDesc,
					&defaultResourceData,
					&TexR[num]);

			dxDevice->CreateUnorderedAccessView(TexR[num].Get(), nullptr, &UAVDesc, TexUAV[num]);

			dxDevice->CreateShaderResourceView(TexR[num].Get(), nullptr, TexSRV[num]);

			//SafeRelease(TexR[num]);
			//delete[] defaultResourceData.pSysMem;
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

		ComPtr<ID3D12Resource> MatDataBuf;

	};

	inline static ComPtr<ID3D12Device> dxDevice;

	inline static void SET_DX_CONTENT(ComPtr<ID3D12Device> dxDevice_tmp) {
		DX12M3DR::dxDevice = dxDevice_tmp.Get();
	}

	std::vector<MaterialDX12> Mat = {};
	
	std::pair< D3D12_SAMPLER_DESC, ComPtr<D3D12_CPU_DESCRIPTOR_HANDLE>> Sampler = {};
	D3D12_BLEND_DESC BlendState = {};

	ComPtr<ID3D12Resource> CBuf;
	ComPtr<ID3D12Resource> ArmatureCBuf;

	std::vector < ComPtr<ID3D12Resource> > VBuf;
	std::vector < ComPtr<D3D12_CPU_DESCRIPTOR_HANDLE> > VBufUAV;
	std::vector < ComPtr<ID3D12Resource> > IBuf;



	void CreateArmatureCBuf() {

		if (BoneDataTLMA.size() == 0) {
			XMFLOAT4X4 tmpForFill = { 1.0f,0.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f ,1.0f };

			BoneDataTLMA.push_back(tmpForFill);
		}

		D3D11_BUFFER_DESC bufDesc;
		ZeroMemory(&bufDesc, sizeof(bufDesc));
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.ByteWidth = sizeof(XMFLOAT4X4) * BoneDataTLMA.size(); //for now max 64 bones

		dxDevice->CreateBuffer(&bufDesc, nullptr, &ArmatureCBuf);

		if (BoneDataTLMA.size() != 0) {
			dxDeviceContext->UpdateSubresource(ArmatureCBuf.Get(), 0, nullptr, &BoneDataTLMA[0], 0, 0);
		}
		else {

		}
	}
	void DefaultMatBuf(int i) {
		D3D11_BUFFER_DESC bufDesc;
		ZeroMemory(&bufDesc, sizeof(bufDesc));
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.ByteWidth = sizeof(MaterialData);

		dxDevice->CreateBuffer(&bufDesc, nullptr, &Mat[i].MatDataBuf);

		dxDeviceContext->UpdateSubresource(Mat[i].MatDataBuf.Get(), 0, nullptr, &MatData[i], 0, 0);
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
		IBuf.resize(modelDat.size());
		VBufUAV.resize(modelDat.size());

		for (int i = 0; i < VBuf.size(); i++) {
			//	if (VBuf != nullptr) {
			SafeRelease(VBuf[i]);
			//	}
			//	if (IBuf != nullptr){
			SafeRelease(IBuf[i]);
			//	}

			D3D11_BUFFER_DESC vertexBufferDesc; //describe buffer we will make
			ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

			vertexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER; //how to bind buffer 

			vertexBufferDesc.ByteWidth = sizeof(VNT) * (modelDat[i].size()); //size of buffer --> make it the size of verticies*vertexPosColor [since vertex will have pos and color
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 0 means no CPU acsess

			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //resource flag - 0 means none
			vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

			D3D11_SUBRESOURCE_DATA resourceData; //data for buffer
			ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
			resourceData.pSysMem = &modelDat[i][0]; //Vertex data for sub source

			dxDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &VBuf[i]); //create buffer, using data settings struct, struct of data, and vertex buffer output - this is also used to create other buffer styles

			D3D11_BUFFER_DESC a;
			VBuf[i]->GetDesc(&a);

			ID3D11Buffer* tmpVertex;

			D3D11_BUFFER_DESC vertexBufferDescU; //describe buffer we will make
			ZeroMemory(&vertexBufferDescU, sizeof(D3D11_BUFFER_DESC));

			vertexBufferDescU.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE; //how to bind buffer 

			vertexBufferDescU.ByteWidth = sizeof(VNT) * (modelDat[i].size()); //size of buffer --> make it the size of verticies*vertexPosColor [since vertex will have pos and color
			vertexBufferDescU.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ; // 0 means no CPU acsess

			vertexBufferDescU.Usage = D3D11_USAGE_DEFAULT; //resource flag - 0 means none

			vertexBufferDescU.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			vertexBufferDescU.StructureByteStride = sizeof(VNT);

			resourceData.pSysMem = &modelDat[i][0]; //Vertex data pos for sub source - use Position?

			dxDevice->CreateBuffer(&vertexBufferDescU, &resourceData, &tmpVertex);

			D3D11_UNORDERED_ACCESS_VIEW_DESC UAVdesc;
			//DXGI_FORMAT_R32_TYPELESS
			UAVdesc.Format = DXGI_FORMAT_UNKNOWN;
			UAVdesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAVdesc.Buffer.FirstElement = 0;
			UAVdesc.Buffer.NumElements = modelDat[i].size();
			UAVdesc.Buffer.Flags = 0;

			dxDevice->CreateUnorderedAccessView(tmpVertex, &UAVdesc, &VBufUAV[i]);

			D3D11_BUFFER_DESC indexBufferDesc; //buffer obj
			ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC)); //alloc

			indexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER; //type of buffer m8 - same logic as vertex
			indexBufferDesc.ByteWidth = sizeof(UINT) * (Indice[i].size());
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

			resourceData.pSysMem = &Indice[i][0]; //indice data for sub source

			dxDevice->CreateBuffer(&indexBufferDesc, &resourceData, &IBuf[i]); //make buffer
		}
	}
	void DefaultCBuf() {
		D3D11_BUFFER_DESC bufDesc;
		ZeroMemory(&bufDesc, sizeof(bufDesc));
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.ByteWidth = sizeof(ObjTuneStatReg);

		dxDevice->CreateBuffer(&bufDesc, nullptr, &CBuf);

		dxDeviceContext->UpdateSubresource(CBuf.Get(), 0, nullptr, &ObjTune, 0, 0);
	}
	void DefaultSampler() {
		D3D11_SAMPLER_DESC tmpSampleDesc;

		tmpSampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		tmpSampleDesc.MipLODBias = 0;
		tmpSampleDesc.MaxAnisotropy = 8;
		tmpSampleDesc.ComparisonFunc = D3D11_COMPARISON_FUNC{ D3D11_COMPARISON_NEVER }; //never remove model for now - always pass render pass
		tmpSampleDesc.MinLOD = 1;
		tmpSampleDesc.MaxLOD = D3D11_FLOAT32_MAX;
		tmpSampleDesc.Filter = D3D11_FILTER{ D3D11_FILTER_MIN_MAG_MIP_LINEAR };
		tmpSampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_MIRROR };

		dxDevice->CreateSamplerState(&tmpSampleDesc, &Sampler);
	}

	void SetupBlendStateDefault() {
		D3D11_BLEND_DESC blendVal;

		blendVal.AlphaToCoverageEnable = false;
		blendVal.IndependentBlendEnable = false;
		blendVal.RenderTarget[0].BlendEnable = true;
		blendVal.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendVal.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendVal.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendVal.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendVal.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendVal.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendVal.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		dxDevice->CreateBlendState(&blendVal, &BlendState);
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

	DX11M3DR(std::vector<VNT>* V, bool ClearPtr = true, float ScaleVertex_t = 1.0f) {
		LoadVertexVectorBased(V, ClearPtr, ScaleVertex_t);

		//DX11 Specific

		AlwaysRunPostM3DR();

	}
	DX11M3DR(const std::string& path = "", float ScaleVertex_t = 1.0f) {
		LoadPathBased(path, ScaleVertex_t);

		//DX11 Specific

		AlwaysRunPostM3DR();

	}
	DX11M3DR() {

	}

	DX11M3DR& operator= (M3DR& tmp) {
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
	

	DX11M3DR(M3DR& tmp) {
		*this = tmp;
		/*
		TODO, load tmp into d3d11dxr, and immedeatly load m3dr into for file manager --> change it so:
		- file manager can show data right away. - then this specific dx11 data is only in compile mode
			- delete dx11 is seperate from m3dr regular - every compile reloads and copies
			*/
		AlwaysRunPostM3DR();
	}
	~DX11M3DR() {
		//TODO: make deconstructor
	}
};


#endif