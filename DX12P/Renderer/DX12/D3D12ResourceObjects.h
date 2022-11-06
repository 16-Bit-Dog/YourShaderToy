#ifndef D3D12_OFF

#ifndef DX12_H_RESOURCE_OBJ
#define DX12_H_RESOURCE_OBJ

#include "HLSLBuilder.h"
#include "ResourceObjectBase.h"
#include "DX12IncludeMain.h"
#include "3DDX12Obj.h"
#include "RenderableManager.h"
#include "DX12H.h"
#include "DX12ShaderFuncs.h"
#include "Renderable.h"
#include "Editor_Window.h"
#include "FileManagerResourceStruct.h"
#include "StaticDX12Object.h"



struct RTVData_DX12 : RTVData_Base {
	D3D12_CPU_DESCRIPTOR_HANDLE rtv = {};
	ComPtr<ID3D12Resource> t = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE uav = {};
	
	bool ClearEveryNewPass = true;

	void ReleaseBuffers() {
		t.Reset(); 
		rtv = {};
		uav = {};
	}

	void MakeBuffers() {
		D3D12_RESOURCE_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(D3D12_RESOURCE_DESC));
		gpuTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		gpuTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		gpuTexDesc.Width = MainDX12Objects::obj->MainWidth;
		gpuTexDesc.Height = MainDX12Objects::obj->MainHeight;

		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.DepthOrArraySize = 1;
		//gpuTexDesc.SampleDesc = 
		gpuTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
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


		D3D12_HEAP_FLAGS heapF = {};

		D3D12_RESOURCE_STATES Rstate = D3D12_RESOURCE_STATE_UNORDERED_ACCESS
			| D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		D3D12_CLEAR_VALUE CV = {};
		CV.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		CV.Color[0] = 0.0f;
		CV.Color[1] = 0.0f;
		CV.Color[2] = 0.0f;
		CV.Color[3] = 1.0f;


		MainDX12Objects::obj->dxDevice->CreateCommittedResource(
			&heapP,
			heapF,
			&gpuTexDesc,
			Rstate,
			&CV,
			//&defaultResourceData,
			__uuidof(t), &t);

		D3D12_RENDER_TARGET_VIEW_DESC rtv_desc;
		rtv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

		MainDX12Objects::obj->dxDevice->CreateRenderTargetView(t.Get(), &rtv_desc, rtv);
		MainDX12Objects::obj->dxDevice->CreateUnorderedAccessView(t.Get(), nullptr, &UAVDesc, MainDX12Objects::obj->ResolveUavBufLocation(this->uav_num));

	}
	RTVData_DX12(RenderTarget_s* r) {
		
		name = r->name;
		ClearEveryNewPass = r->ClearEveryNewPass;
		MakeBuffers();
	
	}
	~RTVData_DX12() {
		ReleaseBuffers();
	}
};

struct DEPTHData_DX12 : DEPTHData_Base {
	D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
	ComPtr<ID3D12Resource> t = nullptr;
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};
	
	bool ClearEveryNewPass = true;

	void ReleaseBuffers() {
		t.Reset();
		dsv = {};
		srv = {};
		//SafeRelease(srv);
	}
	void MakeBuffers() {
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		dsv_desc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH | D3D12_DSV_FLAG_READ_ONLY_STENCIL;
		dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		D3D12_RESOURCE_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(D3D12_RESOURCE_DESC));
		gpuTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		gpuTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		gpuTexDesc.Width = MainDX12Objects::obj->MainWidth;
		gpuTexDesc.Height = MainDX12Objects::obj->MainHeight;

		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.DepthOrArraySize = 1;
		//gpuTexDesc.SampleDesc = 
		gpuTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;


		
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
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
		CV.Format = DXGI_FORMAT_R32_TYPELESS;
		CV.Color[0] = 0.0f;
		CV.Color[1] = 0.0f;
		CV.Color[2] = 0.0f;
		CV.Color[3] = 1.0f;


		MainDX12Objects::obj->dxDevice->CreateCommittedResource(
			&heapP,
			heapF,
			&gpuTexDesc,
			Rstate,
			&CV,
			//&defaultResourceData,
			__uuidof(t), &t);


		MainDX12Objects::obj->dxDevice->CreateDepthStencilView(t.Get(), &dsv_desc, dsv);
		MainDX12Objects::obj->dxDevice->CreateShaderResourceView(t.Get(), &SRVDesc, MainDX12Objects::obj->ResolveSrvBufLocation(this->srv_num));

	}
	DEPTHData_DX12(DepthTarget_s* d) {
		AddSRVNum();
		name = d->name;
		ClearEveryNewPass = d->ClearEveryNewPass;
		MakeBuffers();
	}
	~DEPTHData_DX12() {
		RemoveSRVNum();
	}
};

struct ImageObjectToRendererDX12 : ImageObjectToRenderer_Base {

	D3D12_CPU_DESCRIPTOR_HANDLE srv;
	D3D12_CPU_DESCRIPTOR_HANDLE uav;
	ComPtr<ID3D12Resource> buf;
	ComPtr<ID3D12DescriptorHeap> Samp;

	D3D12_SUBRESOURCE_DATA defaultResourceData; //default data
	uint8_t* pSysMem = nullptr;

	DXGI_FORMAT format;

	int sizeY = 0; //source given sizeY for pSysMem
	int sizeX = 0; //source given sizeX for pSysMem
	int channels = 0;
	int bpp = 0;

	void ReleaseBuffersNonSamp() {
		SafeRelease(buf);
		SafeRelease(Samp);
	}
	void MakeBuffers(BuiltImage_c* data) {
		
		D3D12_RESOURCE_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(D3D12_RESOURCE_DESC));
		gpuTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		gpuTexDesc.Format = format;

		//gpuTexDesc.Width = MainDX12Objects::obj->MainWidth;
		//gpuTexDesc.Height = MainDX11Objects::obj->MainHeight;

		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.DepthOrArraySize = 1;
		//gpuTexDesc.SampleDesc = 
		gpuTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;

		
		if (LinkSizeToRTV == false) {
			gpuTexDesc.Width = data->data.sizeX_c;
			gpuTexDesc.Height = data->data.sizeY_c;
			defaultResourceData.pData = data->data.dataV;
		}
		else {
			pSysMem = ScaleBuffersToSizeCPU(pSysMem, sizeY, sizeX, channels, bpp, MainDX12Objects::obj->MainWidth, MainDX11Objects::obj->MainHeight);
			defaultResourceData.pData = pSysMem;
			gpuTexDesc.Width = MainDX12Objects::obj->MainWidth;
			gpuTexDesc.Height = MainDX12Objects::obj->MainHeight;
		}		

		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
		SRVDesc.Format = format;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		ZeroMemory(&UAVDesc, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
		SRVDesc.Format = format; //
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
		CV.Format = format;
		CV.Color[0] = 0.0f;
		CV.Color[1] = 0.0f;
		CV.Color[2] = 0.0f;
		CV.Color[3] = 1.0f;


		MainDX12Objects::obj->dxDevice->CreateCommittedResource(
			&heapP,
			heapF,
			&gpuTexDesc,
			Rstate,
			&CV,
			//&defaultResourceData,
			__uuidof(buf), &buf);


		D3D12_BOX box = {};
		box.top = 0;
		box.bottom = gpuTexDesc.Height;
		box.left = 0;
		box.right = gpuTexDesc.Width;

		buf->WriteToSubresource(
			0,
			&box,
			defaultResourceData.pData,
			defaultResourceData.RowPitch,
			defaultResourceData.SlicePitch);

		MainDX12Objects::obj->dxDevice->CreateShaderResourceView(buf.Get(), &SRVDesc, MainDX12Objects::obj->ResolveSrvBufLocation(this->srv_num));

		MainDX12Objects::obj->dxDevice->CreateUnorderedAccessView(buf.Get(), NULL, &UAVDesc, MainDX12Objects::obj->ResolveUavBufLocation(this->uav_num));



		D3D12_DESCRIPTOR_HEAP_DESC samplerHDesc;
		samplerHDesc.NumDescriptors = 1;
		samplerHDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		samplerHDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		MainDX12Objects::obj->dxDevice->CreateDescriptorHeap(&samplerHDesc, IID_PPV_ARGS(&Samp));
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


		MainDX12Objects::obj->dxDevice->CreateSampler(&samplerDesc, MainDX12Objects::obj->ResolveSampBufLocation(this->sampler_num));

		
	}
	
	ImageObjectToRendererDX12(BuiltImage_c* data) {

		//TODO: sampler options
		LinkSizeToRTV = data->LinkSizeToRTV;
		nameRW = data->NameRW;
		samplerName = data->SamplerName;
		name = data->Name;
		HasRW = data->ReadWrite;
		
		if (data->data.bpp_c == 8) {
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			formatString = "unorm float4";
		}
		else if (data->data.bpp_c == 16) {
			if (data->UNORM_ELSE_FLOAT){
				format = DXGI_FORMAT_R16G16B16A16_UNORM;
				formatString = "unorm float4";
			}
			else{
				format = DXGI_FORMAT_R16G16B16A16_FLOAT;
				formatString = "float4";
			}
		}
		else if (data->data.bpp_c == 32) {
			if (data->UNORM_ELSE_FLOAT){ 
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				formatString = "unorm float4";
			}
			else{
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				formatString = "float4";
			}
		}
		this->sizeY = data->sizeY;
		this->sizeX = data->sizeX;
		this->channels = data->channels;
		this->bpp = data->bpp;

		int dataS = data->data.size();
		pSysMem = (uint8_t*)malloc(dataS);
		(*pSysMem) = (*((uint8_t*)data->data.dataV));
		
		defaultResourceData.pData = (void*)pSysMem;
		defaultResourceData.RowPitch = sizeX * channels * bpp;
		defaultResourceData.SlicePitch = sizeY * sizeY * channels * bpp;

		
		MakeBuffers(data);

	
		D3D12_SAMPLER_DESC samplerDesc = {};

		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_WRAP };
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_WRAP };
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_WRAP };
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 8;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC{ D3D12_COMPARISON_FUNC_LESS };
		samplerDesc.MinLOD = 1;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.Filter = D3D12_FILTER{ D3D12_FILTER_MIN_MAG_MIP_LINEAR };
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE{ D3D12_TEXTURE_ADDRESS_MODE_MIRROR };

		MainDX12Objects::obj->dxDevice->CreateSampler(&samplerDesc, Samp->GetCPUDescriptorHandleForHeapStart());

	}
	~ImageObjectToRendererDX12() {
		delete[] pSysMem;

		SafeRelease(buf);
		SafeRelease(Samp);
		//SafeRelease TODO: check if I need to clean up anything
	}
};


struct StructObjectToRendererDX12 : StructObjectToRenderer_Base{
	ComPtr<ID3D12Resource> con;
	D3D12_CPU_DESCRIPTOR_HANDLE conDesc = {};


	ComPtr<ID3D12Resource> uavB; // sperate modifiable object data for a const pair and not const
	D3D12_CPU_DESCRIPTOR_HANDLE uav = {};
	D3D12_CPU_DESCRIPTOR_HANDLE srv = {};

	StructObjectToRendererDX12(BuiltConstant_c* data) {
		
		Name = data->Name;
		NameRW = data->NameRW;
		NameRW_SRV = data->NameRW_SRV;
		StructName = data->StructName;
		StructElementName = data->StructElementName;
		StructElementNameRW = data->StructElementNameRW;

		ReferToData = data->vars; //copy

		int memSizePost;
		int memSize;
		float* Data;

		CalculateMemoryOrganization(&Data, memSize, memSizePost);


		HasRW = data->ReadWrite;

		
		if (data->ReadWrite) {


			/*
			MainDX11Objects::obj->dxDevice->CreateBuffer(&bufDesc, &defaultResourceData, &uavB);

			MainDX11Objects::obj->dxDevice->CreateUnorderedAccessView(uavB.Get(), &UAVDesc, &uav);
			MainDX11Objects::obj->dxDevice->CreateShaderResourceView(uavB.Get(), &SRVDesc, &srv);
			*/
	
			D3D12_SUBRESOURCE_DATA defaultResourceData;
			defaultResourceData.pData = Data;
			defaultResourceData.RowPitch = memSize;
			defaultResourceData.SlicePitch = 1;

			D3D12_RESOURCE_DESC gpuTexDesc;
			ZeroMemory(&gpuTexDesc, sizeof(D3D12_RESOURCE_DESC));
			gpuTexDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			gpuTexDesc.Format = DXGI_FORMAT_UNKNOWN;

			gpuTexDesc.Width = memSize;
			gpuTexDesc.Height = 1;

			gpuTexDesc.MipLevels = 1;
			gpuTexDesc.DepthOrArraySize = 1;

			gpuTexDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
			gpuTexDesc.SampleDesc.Count = 1;
			gpuTexDesc.SampleDesc.Quality = 0;


			D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
			//UAVDesc.Texture2D
			ZeroMemory(&UAVDesc, sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC));
			UAVDesc.Format = DXGI_FORMAT_UNKNOWN; //
			UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;

			D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory(&SRVDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
			SRVDesc.Format = DXGI_FORMAT_UNKNOWN; //
			SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;


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


			MainDX12Objects::obj->dxDevice->CreateCommittedResource(
				&heapP,
				heapF,
				&gpuTexDesc,
				Rstate,
				&CV,
				__uuidof(uavB), &uavB);


			D3D12_BOX box = {};
			box.top = 0;
			box.bottom = gpuTexDesc.Height;
			box.left = 0;
			box.right = gpuTexDesc.Width;

			uavB->WriteToSubresource(
				0,
				&box,
				defaultResourceData.pData,
				defaultResourceData.RowPitch,
				defaultResourceData.SlicePitch);

			MainDX12Objects::obj->dxDevice->CreateUnorderedAccessView(uavB.Get(), nullptr, &UAVDesc, uav);

			MainDX12Objects::obj->dxDevice->CreateShaderResourceView(uavB.Get(), &SRVDesc, srv);


		}
		else {
			
			DX12M3DR::CreateCBufDepth1(con, conDesc, memSizePost, MainDX12Objects::obj->dxDevice);

			D3D12_BOX box = {};
			box.top = 0;
			box.bottom = 1;
			box.left = 0;
			box.right = memSize;

			con->WriteToSubresource(
				0,
				&box,
				&Data,
				memSize,
				1);
		
		}

		delete Data;
	}

	~StructObjectToRendererDX12() {
//		if(this->HasRW)
//			SafeRelease(uavB);

//		SafeRelease(con);

	}
	
};

struct ModelToRendererDX12 : ModelToRenderer_Base{
	DX12M3DR* Model;
	int Type = -1;

	ModelToRendererDX12(BuiltModel_c* data) {
		
		Name = data->Name;
		NameRW = data->NameRW;
		Type = data->Type;

		if (data->Type == - 1) {
			Model = new DX12M3DR(data->tmpM3DR);
		}
		else if (data->Type >-1 && data->Type<StaticObjectPass.size()) {
			Model = new DX12M3DR(data->tmpM3DR);
		}
		//Model->SET_MODEL_BUFFER_LOCATIONS(MainDX12Objects::obj->ResolveConBufLocation(6), MainDX12Objects::obj->ResolveSrvBufLocation(0), MainDX12Objects::obj->ResolveUavBufLocation(0), MainDX12Objects::obj->ResolveSampBufLocation(0));
#ifdef GET_OBJECT_STATIC
		OutputStringToFileForCopyPata(&Model);
#endif
	}
	~ModelToRendererDX12() {
		if (Type == -1) {
			delete Model;
		}
	}
};

struct PredefinedToRendererDX12 : PredefinedToRenderer_Base{

	ComPtr<ID3D12Resource> CData;	
	D3D12_CPU_DESCRIPTOR_HANDLE CDataDesc = {};

	PredefinedToRendererDX12(BuiltPredefined_c* data) {
		CDataDesc = MainDX12Objects::obj->ResolveConBufLocation(cb_num);
		DX12M3DR::CreateCBufDepth1(CData, CDataDesc, sizeof(CONST_DATA_PASS_c), MainDX12Objects::obj->dxDevice);

		D3D12_BOX box = {};
		box.top = 0;
		box.bottom = 1;
		box.left = 0;
		box.right = sizeof(CONST_DATA_PASS_c);

		CData->WriteToSubresource(
			0,
			&box,
			&data->data,
			sizeof(CONST_DATA_PASS_c),
			1);

	}

	~PredefinedToRendererDX12() {
		
	}

	void update(BuiltPredefined_c* bI) {
		
		D3D12_BOX box = {};
		box.top = 0;
		box.bottom = 1;
		box.left = 0;
		box.right = sizeof(CONST_DATA_PASS_c);

		CData->WriteToSubresource(
			0,
			&box,
			bI->data,
			sizeof(CONST_DATA_PASS_c),
			1);

	}
};



struct ResourceObjectBaseDX12 : ResourceObjectBase {

	

	inline static ResourceObjectBaseDX12* obj;

	void SetResourceObjectBaseDX12() {
		MainDX12Objects::ROB = this;
	}

	PredefinedToRendererDX12* PredefinedData = nullptr;
	std::unordered_map<std::string /*name to identify image*/, ImageObjectToRendererDX12*/*data*/> ImageData; //use d4 to get and filter data
	std::unordered_map<std::string /*name to identify model*/, ModelToRendererDX12*/*data*/> ModelData;
	std::unordered_map<std::string /*name to identify struct*/, StructObjectToRendererDX12*/*data*/> ConstantData;

	std::unordered_map<uint64_t, RTVData_DX12*> RTVData;
	std::unordered_map<uint64_t, DEPTHData_DX12*> DEPTHData;

	template <typename T, class T2>
	void cleanOutMap(std::unordered_map<T, T2*>& r) {
		for (const auto& i : r) delete i.second;
		r.clear();

	}
	~ResourceObjectBaseDX12() {
		delete PredefinedData;
		cleanOutMap(ImageData);
		cleanOutMap(ModelData);
		cleanOutMap(ConstantData);
		cleanOutMap(RTVData);
		cleanOutMap(DEPTHData);
	}

	void SetDataToPipelineVertex(BuiltModel_c* data, VertexShaderPipeline& vp) {
		if (data == nullptr) {
			vp.modelData = nullptr;
			vp.Vdata.clear(); //run default cube if invalid data
			vp.MatInfo.clear();
			return;
		}
		if (ModelData.count(data->Name) > 0) { //check if valid model exists

			vp.modelData = data;

			vp.LoadedModelName = data->Name;
			vp.ModelType = data->Type;


			ModelToRendererDX12* md_tmp = ModelData[data->Name];

			vp.MatInfo.resize(md_tmp->Model->MatData.size());
			vp.Vdata.resize(md_tmp->Model->VBuf.size());
			vp.Idata.resize(md_tmp->Model->IBuf.size());
			vp.Icount.resize(md_tmp->Model->Indice.size());
			
			DX12M3DR* tmpM3 = ModelData[data->Name]->Model;

			vp.RawModel = tmpM3;

			for (int x = 0; x < tmpM3->VBuf.size(); x++) {
				vp.Vdata[x] = (void*)tmpM3->VBuf[x].Get();
			}
			for (int x = 0; x < tmpM3->MatData.size(); x++) {
				vp.MatInfo[x] = (void*)tmpM3->Mat[x].MatDataBuf.Get();
			}
			for (int x = 0; x < tmpM3->IBuf.size(); x++) {
				vp.Idata[x] = (void*)tmpM3->IBuf[x].Get();
				vp.Icount[x] = tmpM3->Indice[x].size();
			}
			vp.VertexStride = tmpM3->VertexStride;

		}
	}

	inline static const UINT NOneP = 0;

	void PreBindImageData(ImageObjectToRendererDX12* ID) {
		//MainDX12Objects::obj->Heaps.SRVHeapRef[ID->srv_num] = &ID->srv;
		//MainDX12Objects::obj->Heaps.UAVHeapRef[ID->uav_num] = &ID->uav;
		//MainDX12Objects::obj->Heaps.SAMPHeapRef[ID->sampler_num] = &ID->Samp->GetCPUDescriptorHandleForHeapStart();

		//MainDX12Objects::obj->m_rootSig
		//MainDX12Objects::obj->m_commandList->SetDescriptorHeaps
		
		/*
		//MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers();
		MainDX12Objects::obj->dxDeviceContext->VSSetShaderResources(ID->srv_num, 1, &ID->srv);
		MainDX11Objects::obj->dxDeviceContext->VSSetSamplers(ID->sampler_num, 1, &ID->Samp);

		//MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers();
		MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources(ID->srv_num, 1, &ID->srv);
		MainDX11Objects::obj->dxDeviceContext->PSSetSamplers(ID->sampler_num, 1, &ID->Samp);
//		MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources();
//		MainDX11Objects::obj->dxDeviceContext->PSSetSamplers();
		
		MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(ID->uav_num, 1, &ID->uav, &NOneP);
		MainDX11Objects::obj->dxDeviceContext->CSSetSamplers(ID->sampler_num, 1, &ID->Samp);
		*/
	}
	void PreBindAllImageData() {
		for (const auto& i : ImageData) {
			PreBindImageData(i.second);
		}
	}
	void BindModelData(DX12M3DR* Model, int Obj) {
		
		ID3D11UnorderedAccessView* tmpF =  NULL;

		for (int i = 0; i < MaterialEnum::TEX_COUNT; i++) {
			if (Model->MatData[Obj].TexOn[i]) {
	//			MainDX12Objects::obj->Heaps.SRVHeapRef[i] = &Model->TexObj[Obj].TexSRV;
	//			MainDX12Objects::obj->Heaps.UAVHeapRef[i] = &Model->TexObj[Obj].TexUAV;

				//MainDX11Objects::obj->dxDeviceContext->VSSetShaderResources(i, 1, Model->TexObj[Obj].TexSRV[i].GetAddressOf());
				//MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources(i, 1, Model->TexObj[Obj].TexSRV[i].GetAddressOf());
				//MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(i, 1, &tmpF, &NOneP);

			}
		}
		
	}

	void BindModelDataCS(DX12M3DR* Model, int Obj) {

		ID3D11ShaderResourceView* tmpF = NULL;

		for (int i = 0; i < MaterialEnum::TEX_COUNT; i++) {
			if (Model->MatData[Obj].TexOn[i]) {
				//MainDX12Objects::obj->Heaps.SRVHeapRef[i] = &Model->TexObj[Obj].TexSRV;
				//MainDX12Objects::obj->Heaps.UAVHeapRef[i] = &Model->TexObj[Obj].TexUAV;
				
				//MainDX11Objects::obj->dxDeviceContext->VSSetShaderResources(i, 1, &tmpF);
				//MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources(i, 1, &tmpF);
				//MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(i, 1, Model->TexObj[Obj].TexUAV[i].GetAddressOf(), &NOneP);
			}
		}
	}
	void PreBindConstantData(StructObjectToRendererDX12* ID) {
		//MainDX12Objects::obj->Heaps.CONHeapRef[ID->cb_num] = &ID->conDesc;
		
		//MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers(ID->cb_num, 1, ID->con.GetAddressOf());

		//MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers(ID->cb_num, 1, ID->con.GetAddressOf());

		//MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(ID->uav_num, 1, ID->uav.GetAddressOf(), &NOneP);


	}
	void PreBindAllConstantData() {
		for (const auto& i : ConstantData) {
			PreBindConstantData(i.second);
		}
	}
	void PreBindDefault() {
		//MainDX12Objects::obj->Heaps.CONHeapRef[PredefinedData->cb_num] = &PredefinedData->CDataDesc;


		//MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers(PredefinedData->cb_num, 1, PredefinedData->GetAddressOf());

		//MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers(PredefinedData->cb_num, 1, PredefinedData->Cdata.GetAddressOf());
	
		//MainDX11Objects::obj->dxDeviceContext->CSSetConstantBuffers(PredefinedData->cb_num, 1, PredefinedData->Cdata.GetAddressOf());
	}
	//TODO: add individual compile buttons and checkmark for if up to date data



	void PreBindAllResources() {

		//PreBindAllImageData();
		//PreBindAllConstantData();
		//PreBindDefault();

	}

	void ClearAllPredefined() override {
		if(PredefinedData != nullptr)
		delete PredefinedData; //TODO: deal with delete later
		PredefinedData = nullptr;
	}
	void ClearAllImages()  override {

		for (const auto& i : ImageData) {
			delete i.second;
		}

		ImageData.clear();
	}
	void ClearAllModels()  override {

		for (const auto& i : ModelData) {
			delete i.second;
		}

		ModelData.clear();
	}
	void ClearAllConstants()  override {

		for (const auto& i : ConstantData) {
			delete i.second;
		}

		ConstantData.clear();
	}
	void ClearAllObjects(bool fullClean) override{
		//SafeRelease(MainDX12Objects::obj->descHeapConSrvUav);
		//SafeRelease(MainDX12Objects::obj->descHeapSamp);


		//MainDX12Objects::obj->Heaps.CONHeapRef.clear();
		//MainDX12Objects::obj->Heaps.SRVHeapRef.clear();
		//MainDX12Objects::obj->Heaps.UAVHeapRef.clear();
		//MainDX12Objects::obj->Heaps.SAMPHeapRef.clear();

		if (fullClean) {
		//	ClearAllPredefined();
		}
		//ClearAllImages();
		//ClearAllModels();
		//ClearAllConstants();
	}
	void LoadImageFromData(BuiltImage_c* bI) {
		ImageData[bI->Name] = new ImageObjectToRendererDX12(bI);
	}
	void LoadModelFromData(BuiltModel_c* bI) {
		ModelData[bI->Name] = new ModelToRendererDX12(bI);
	}
	void LoadConstantFromData(BuiltConstant_c* bI) {
		ConstantData[bI->Name] = new StructObjectToRendererDX12(bI);
	}
	void LoadPredefinedFromData(BuiltPredefined_c* bI) {
		PredefinedData = new PredefinedToRendererDX12(bI);
	}
	void UpdatePredefinedFromData(BuiltPredefined_c* bI) {
		if (PredefinedData == NULL) LoadPredefinedFromData(bI);
		PredefinedData->update(bI);
	}


	inline static UINT OffsetDef = 0;

	template<class T> 
	void BindCSToUAV(T& data) {
		for (auto& i : data) {
			MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(i.second->uav_num, 1, i.second->uav.GetAddressOf(), &OffsetDef);
		}
	}

	template<class T>
	void BindCSToSRV(T& data) {
		for (auto& i : data) {
			MainDX11Objects::obj->dxDeviceContext->CSSetShaderResources(i.second->srv_num, 1, i.second->srv.GetAddressOf());
		}
	}
	void PrefillCommandList(PipelineObjectIntermediateStateDX12** item) {
		
		if (!(*item)->PObj->ComputeOnlyToggle) {

			(*item)->cmdLists.push_back(ComPtr<ID3D12GraphicsCommandList>());
			ComPtr<ID3D12GraphicsCommandList>& tmpCmdList = (*item)->cmdLists[(*item)->cmdLists.size() - 1];

			tmpCmdList->SetGraphicsRootSignature(MainDX12Objects::obj->defaultRootSig.Get());
			tmpCmdList->OMSetRenderTargets(1, &RTVData[(*item)->RTV_Num]->rtv, true, &DEPTHData[(*item)->DEPTH_Num]->dsv);
		}
		if (!(*item)->PObj->TurnComputeOffToggle) {
			for (int i = 0; i < (*item)->Compute.size(); i++) {
				(*item)->cmdLists.push_back(ComPtr<ID3D12GraphicsCommandList>());
				ComPtr<ID3D12GraphicsCommandList>& tmpCmdList = (*item)->cmdLists[(*item)->cmdLists.size() - 1];

				tmpCmdList->SetComputeRootSignature(MainDX12Objects::obj->defaultRootSig.Get());

			}
		}

	}

	
	void RunLogic(PipelineObjectIntermediateStateDX12** item) {
		if ((*item)->PObj->On) {
			//TODO: run command lists
		}
	}
	
	int SizeOfRTV() {
		return RTVData.size();
	}
	void ClearRTVAndDepth() {
		for (auto& i : RTVData) {
			if (i.second->ClearEveryNewPass) {
		//		MainDX11Objects::obj->dxDeviceContext->ClearRenderTargetView(i.second->rtv.Get(), &MainDX11Objects::obj->CLEAR_COLOR[0]);
			}
		}
		for (auto& i : DEPTHData) {
			if (i.second->ClearEveryNewPass) {
			//	MainDX11Objects::obj->dxDeviceContext->ClearDepthStencilView(i.second->dsv.Get(), D3D11_CLEAR_DEPTH, 1, 0);
			}
		}
	}

	void SetErrorShowIfHasError(BasePipeline& t, PipelineObj* po) {
		if (t.CheckIfHasError()) {
			HasCodeCompileError = true;
			CodeCompileErrorNames.push_back(po->name+" - "+t.ShaderTypeName);
		}
	}

	int CompileNewPipelineObject(const std::pair<const int, PipelineObj*>& Pobj) {
		
		PreBindAllResources();
		
		PipelineObjectIntermediateStateDX12* itemP = new PipelineObjectIntermediateStateDX12();
		PipelineObjectIntermediateStateDX12** item = &itemP;
		
		(*item)->PObj = Pobj.second;
		(*item)->VName = Pobj.second->Vertex.name;
		(*item)->PName = Pobj.second->Pixel.name;
		(*item)->setCSize((*item)->PObj->Compute.size());

		MapTools::CheckIfSelectedExists(RTV_DEPTH::RTV, &Pobj.second->RTV_Selected);
		MapTools::CheckIfSelectedExists(RTV_DEPTH::DEPTH, &Pobj.second->DEPTH_Selected);
		(*item)->RTV_Num = Pobj.second->RTV_Selected;
		(*item)->DEPTH_Num = Pobj.second->DEPTH_Selected;
		

		int iter = 0;
		for (const auto& i : (*item)->PObj->Compute) {
			(*item)->Compute[iter].CName = i.second->name;
			(*item)->Compute[iter].AutoSetBlockToWindowSize = i.second->AutoSetBlockToWindowSize;
			(*item)->Compute[iter].DimX = i.second->BlockSizeX;
			(*item)->Compute[iter].DimY = i.second->BlockSizeY;
			(*item)->Compute[iter].DimZ = i.second->BlockSizeZ;

			iter++;
		}
		iter = 0;

		if (Pobj.second->On == false) {
			delete itemP;
			return -1; 
		}

		//            { vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize() };

		//TODO: template do these 3 into 1 function
		//compile Pixel Shader if not exist
		if (PipelineObjectIntermediateStateDX12::PixelShaderMap.find((*item)->PName) == PipelineObjectIntermediateStateDX12::PixelShaderMap.end()){
			PipelineObjectIntermediateStateDX12::PixelShaderMap[(*item)->PName] = ShaderCDX12::CompileShader(&BasePipeline::code[BasePipeline::PIXEL], Pobj.second->Pixel.name, SHADER_PREFIX_D3D12::PS_PREFIX_D3D12, &Pobj.second->Pixel.ErrorMessage_s);
			SetErrorShowIfHasError(Pobj.second->Pixel, Pobj.second);
		}
		(*item)->PDat = PipelineObjectIntermediateStateDX12::PixelShaderMap[(*item)->PName];
		if ((*item)->PDat == nullptr) { 
			delete itemP;
			return -1;
		}

		//compile Vertex Shader if not exist
		if (PipelineObjectIntermediateStateDX12::VertexShaderMap.find((*item)->VName) == PipelineObjectIntermediateStateDX12::VertexShaderMap.end()) {
			PipelineObjectIntermediateStateDX12::VertexShaderMap[(*item)->VName] = ShaderCDX12::CompileShader(&BasePipeline::code[BasePipeline::VERTEX], Pobj.second->Vertex.name, SHADER_PREFIX_D3D12::VS_PREFIX_D3D12, &Pobj.second->Vertex.ErrorMessage_s);
			SetErrorShowIfHasError(Pobj.second->Vertex, Pobj.second);
		}
		(*item)->VDat = PipelineObjectIntermediateStateDX12::VertexShaderMap[(*item)->VName];

		if ((*item)->VDat == nullptr) { 
			delete itemP;
			return -1;
		}

		//compile Compute shader if not exist
		for (const auto& i : (*item)->PObj->Compute) {

			if (PipelineObjectIntermediateStateDX12::ComputeShaderMap.find((*item)->Compute[iter].CName) == PipelineObjectIntermediateStateDX12::ComputeShaderMap.end()) {
				PipelineObjectIntermediateStateDX12::ComputeShaderMap[(*item)->Compute[iter].CName] = ShaderCDX12::CompileShader(&BasePipeline::code[BasePipeline::COMPUTE], i.second->name, SHADER_PREFIX_D3D12::CS_PREFIX_D3D12, &i.second->ErrorMessage_s);
				SetErrorShowIfHasError(*i.second, Pobj.second);
			}
			(*item)->Compute[iter].CDat = PipelineObjectIntermediateStateDX12::ComputeShaderMap[(*item)->Compute[iter].CName];


			iter++;
		}

		

		(*item)->defaultGraphicsPipelineStateDesc = {};
		(*item)->defaultGraphicsPipelineStateDesc.NodeMask = 0;
		(*item)->defaultGraphicsPipelineStateDesc.pRootSignature = MainDX12Objects::obj->defaultRootSig.Get();
		(*item)->defaultGraphicsPipelineStateDesc.InputLayout = MainDX12Objects::obj->dxIL;
		(*item)->defaultGraphicsPipelineStateDesc.BlendState = MainDX12Objects::obj->MakeBlend((*item)->PObj->Vertex.BlendToMake);;
		(*item)->defaultGraphicsPipelineStateDesc.DepthStencilState = MainDX12Objects::obj->MakeDepthStencil((*item)->PObj->Vertex.StencilToMake);
		(*item)->defaultGraphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_R32_TYPELESS;
		//MainDX12Objects::obj->defaultGraphicsPipelineStateDesc.DS = NULL;
		//MainDX12Objects::obj->defaultGraphicsPipelineStateDesc.HS = NULL;
		(*item)->defaultGraphicsPipelineStateDesc.NumRenderTargets = 0;
		(*item)->defaultGraphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		(*item)->defaultGraphicsPipelineStateDesc.PS = *(*item)->PDat;
		(*item)->defaultGraphicsPipelineStateDesc.RasterizerState = MainDX12Objects::obj->MakeRaster((*item)->PObj->Vertex.RasterToMake);
		(*item)->defaultGraphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		//MainDX12Objects::obj->defaultGraphicsPipelineStateDesc.StreamOutput = NULL;
		(*item)->defaultGraphicsPipelineStateDesc.VS = *(*item)->VDat;

		(*item)->PrepComputeShader();
		//MainDX12Objects::obj->defaultComputePipelineStateDesc.CachedPSO = NULL;		
#ifdef DEBUG
		(*item)->defaultGraphicsPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
		(*item)->PrepComputeShader();
#else
		MainDX12Objects::obj->defaultGraphicsPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
#endif 

		PrefillCommandList(item);

		if ((*item)->PObj->On == true) {//Not needed if statment
			(*item)->ToRunLogic = [&]() {
				RunLogic(&MainDX12Objects::obj->CompiledCodeV[MainDX12Objects::obj->CompiledCodeV.size()-1]);
			};
		}
		else {
			delete itemP;
			return -1;
		}
		MainDX12Objects::obj->CompiledCodeV.push_back(itemP);
	}

	template<class T> 
	void ClearShaderMap(std::unordered_map<std::string,T*>& map) {
		for (const auto& i : map) {
			if (i.second != nullptr) delete i.second;
		}
		map.clear();
	}
	void ClearShaderCache() {
		ClearShaderMap(PipelineObjectIntermediateStateDX12::PixelShaderMap);
		ClearShaderMap(PipelineObjectIntermediateStateDX12::VertexShaderMap);
		ClearShaderMap(PipelineObjectIntermediateStateDX12::ComputeShaderMap);
	}

	void GenRTVFromExistingMap() {
		for (const auto& i : RTVData) {
			delete i.second;
		}
		RTVData.clear();

		for (auto& i : RTV_DEPTH::RTV) {
			RTVData[i.first] = new RTVData_DX12(i.second);
		}
	}
	void GenDEPTHFromExistingMap() {
		for (const auto& i : DEPTHData) {
			delete i.second;
		}
		DEPTHData.clear();

		for (auto& i : RTV_DEPTH::DEPTH) {
			DEPTHData[i.first] = new DEPTHData_DX12(i.second);
		}
	}

	void ReleaseRTVAndDepthAndResizeTex() {
		for (auto& i : RTVData) {
			i.second->ReleaseBuffers();
		}
		RTVData.clear();
		for (auto& i : DEPTHData) {
			i.second->ReleaseBuffers();
		}
		DEPTHData.clear();
		for (auto& i : ImageData) {
			i.second->ReleaseBuffersNonSamp();
		}
		ImageData.clear();
	}
	void MakeRTVAndDepthAndResizeTex() {
		for (auto& i : RTVData) {
			i.second->MakeBuffers();
		}
		for (auto& i : DEPTHData) {
			i.second->MakeBuffers();
		}
		for (auto& i : ImageData) {
			i.second->MakeBuffers(nullptr);
		}
	}

	void AddItemTextDefault(std::vector<std::string>* v) {
		
		HLSLBuilder::AddItemTextDefault(v, PredefinedData);
	}
	void AddItemTextImages(std::vector<std::string>* v) {
		for (const auto& x : ImageData)
		HLSLBuilder::AddItemTextImages(v, x.second);
	}
	void AddItemTextModels(std::vector<std::string>* v) {
		for (const auto& x : ModelData)
		HLSLBuilder::AddItemTextModels(v, x.second);
	}
	void AddItemTextConstants(std::vector<std::string>* v) {
		for( const auto& x : ConstantData)
		HLSLBuilder::AddItemTextConstants(v,  x.second);
	}
	void AddItemTextRTV(std::vector<std::string>* v) {
		if (RTVData.size() == 0) { //means no compile to these happened yet
			GenRTVFromExistingMap();
		}
		for (const auto& x : RTVData)
		HLSLBuilder::AddItemTextRTV(v, x.second);
	}
	void AddItemTextDEPTH(std::vector<std::string>* v) {
		if (DEPTHData.size() == 0) { //means no compile to these happened yet
			GenDEPTHFromExistingMap();
		}
		for (const auto& x : DEPTHData)
		HLSLBuilder::AddItemTextDEPTH(v, x.second);
	} 

	void CompileCodeLogic(PipelineMain* OrderedPipelineState) {
		resetCodeCompile();

		RtvAndDepthBlock::ClearRTVAndDepth = [&]() {ClearRTVAndDepth(); };
		RtvAndDepthBlock::ReleaseAllRTVAndDepth = [&]() {ReleaseRTVAndDepthAndResizeTex(); };
		RtvAndDepthBlock::MakeRTVAndDepth = [&]() {MakeRTVAndDepthAndResizeTex(); };
		RtvAndDepthBlock::SizeOfRTV = [&]() {return SizeOfRTV();};
			
		MainDX11Objects::obj->CompiledCodeV.clear();

		ClearShaderCache();
		//TODO, save code into format useable by human, if failed to compile skip and load err message into err section

		std::string Globals = MASTER_Editor::obj->GetStringWithGlobalsText();
		std::string VGlobals = MASTER_Editor::obj->GetStringVandPLocked() + Globals + MASTER_Editor::obj->VsString;
		std::string PGlobals = MASTER_Editor::obj->GetStringVandPLocked() + Globals + MASTER_Editor::obj->PsString;
		std::string CGlobals = MASTER_Editor::obj->GetStringCLocked() + Globals + MASTER_Editor::obj->CsString;
		BasePipeline::code[BasePipeline::VERTEX] = VGlobals;
		BasePipeline::code[BasePipeline::PIXEL] = PGlobals;
		BasePipeline::code[BasePipeline::COMPUTE] = CGlobals;

		BasePipeline::GenCode(BasePipeline::VERTEX);
		BasePipeline::GenCode(BasePipeline::PIXEL);
		BasePipeline::GenCode(BasePipeline::COMPUTE);

		GenRTVFromExistingMap();
		GenDEPTHFromExistingMap();

		MapTools::CheckIfSelectedExists(RTV_DEPTH::RTV, &PipelineObj::SelectedFinalRTV);
		PipelineObjectIntermediateStateDX12::SelectedFinalRTV = RTVData[PipelineObj::SelectedFinalRTV]->t.Get();
		//This was done to not add to ref counter since I acctually don't want to unknowingly not delete the resource -- I delete at 2 specific points known in time

		for (const auto& i : OrderedPipelineState->P) { //loop through all ordered pipeline objects

			CompileNewPipelineObject(i);

		}
	}

	void PreBuildLogic(bool Startup) {
		

		MainDX12Objects::obj->descHeapSamp.NumDescriptors = RegisterMaps::S_R.size() - RegisterMaps::INITIAL_SAMP;

		MainDX12Objects::obj->descHeapConSrvUav.NumDescriptors =
		RegisterMaps::SRV_R.size() - RegisterMaps::INITIAL_SRV +
		RegisterMaps::UAV_R.size() - RegisterMaps::INITIAL_UAV +
		RegisterMaps::CB_R.size() - RegisterMaps::INITIAL_CB;
		MainDX12Objects::obj->BlockedOffSetForSrvHeapStart = 0;
		MainDX12Objects::obj->BlockedOffSetForConHeapStart = MainDX12Objects::obj->m_SRV_CON_UAV_IS * (RegisterMaps::SRV_R.size() - RegisterMaps::INITIAL_SRV);
		MainDX12Objects::obj->BlockedOffSetForUavHeapStart = MainDX12Objects::obj->BlockedOffSetForConHeapStart + MainDX12Objects::obj->m_SRV_CON_UAV_IS * (RegisterMaps::CB_R.size() - RegisterMaps::INITIAL_CB);

		MainDX12Objects::obj->dxDevice->CreateDescriptorHeap(&MainDX12Objects::obj->descHeapSamp, __uuidof(MainDX12Objects::obj->HeapSamp), (void**)MainDX12Objects::obj->HeapSamp.GetAddressOf());
		MainDX12Objects::obj->dxDevice->CreateDescriptorHeap(&MainDX12Objects::obj->descHeapConSrvUav, __uuidof(MainDX12Objects::obj->HeapConSrvUav), (void**)MainDX12Objects::obj->HeapConSrvUav.GetAddressOf());

		

	}

};


#endif


#endif