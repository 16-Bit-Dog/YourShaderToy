
#ifndef DX11_H_RESOURCE_OBJ
#define DX11_H_RESOURCE_OBJ

#include "HLSLBuilder.h"
#include "ResourceObjectBase.h"
#include "DX11IncludeMain.h"
#include "3DDX11Obj.h"
#include "RenderableManager.h"
#include "DX11H.h"
#include "DX11ShaderFuncs.h"
#include "Renderable.h"
#include "Editor_Window.h"
#include "FileManagerResourceStruct.h"
#include "StaticDX11Object.h"


struct RTVData_DX11 : RTVData_Base {
	ComPtr<ID3D11RenderTargetView> rtv = nullptr;
	ComPtr<ID3D11Texture2D> t = nullptr;
	ComPtr<ID3D11UnorderedAccessView> uav = nullptr;
	
	bool ClearEveryNewPass = true;

	void ReleaseBuffers() {
		t.Reset(); 
		rtv.Reset();
		uav.Reset();
	}

	void MakeBuffers() {
		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_UNORDERED_ACCESS;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		depthStencilBufferDesc.Width = MainDX11Objects::obj->MainWidth;
		depthStencilBufferDesc.Height = MainDX11Objects::obj->MainHeight;
		depthStencilBufferDesc.MipLevels = 1;

		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;

		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		ThrowFailed(MainDX11Objects::obj->dxDevice->CreateTexture2D(
			&depthStencilBufferDesc,
			nullptr,
			t.GetAddressOf()));


		ThrowFailed(MainDX11Objects::obj->dxDevice->CreateRenderTargetView(
			t.Get(),
			nullptr,
			rtv.GetAddressOf()));

		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		//UAVDesc.Texture2D
		ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		UAVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		MainDX11Objects::obj->dxDevice->CreateUnorderedAccessView(t.Get(), &UAVDesc, &uav);

	}
	RTVData_DX11(RenderTarget_s* r) {
		
		name = r->name;
		ClearEveryNewPass = r->ClearEveryNewPass;
		MakeBuffers();
	
	}
	~RTVData_DX11() {
		ReleaseBuffers();
	}
};

struct DEPTHData_DX11 : DEPTHData_Base {
	ComPtr<ID3D11DepthStencilView> dsv = nullptr;
	ComPtr<ID3D11Texture2D> t = nullptr;
	ComPtr<ID3D11ShaderResourceView> srv = nullptr;

	bool ClearEveryNewPass = true;

	void ReleaseBuffers() {
		t.Reset();
		dsv.Reset();
		srv.Reset();
		//SafeRelease(srv);
	}
	void MakeBuffers() {


		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		depthStencilBufferDesc.Width = MainDX11Objects::obj->MainWidth;
		depthStencilBufferDesc.Height = MainDX11Objects::obj->MainHeight;
		depthStencilBufferDesc.MipLevels = 1;

		depthStencilBufferDesc.SampleDesc.Count = 1;
		depthStencilBufferDesc.SampleDesc.Quality = 0;

		depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

		ThrowFailed(MainDX11Objects::obj->dxDevice->CreateTexture2D(
			&depthStencilBufferDesc,
			nullptr,
			t.GetAddressOf()));


		ThrowFailed(MainDX11Objects::obj->dxDevice->CreateDepthStencilView(
			t.Get(),
			&MainDX11Objects::obj->dxDepthStencilDesc,
			dsv.GetAddressOf()));

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		//UAVDesc.Texture2D
		ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = -1;
		SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;


		MainDX11Objects::obj->dxDevice->CreateShaderResourceView(t.Get(), &SRVDesc, &srv);

	}
	DEPTHData_DX11(DepthTarget_s* d) {
		AddSRVNum();
		name = d->name;
		ClearEveryNewPass = d->ClearEveryNewPass;
		MakeBuffers();
	}
	~DEPTHData_DX11() {
		RemoveSRVNum();
	}
};

struct ImageObjectToRendererDX11 : ImageObjectToRenderer_Base {

	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11UnorderedAccessView> uav;
	ComPtr<ID3D11SamplerState> Samp;
	D3D11_SUBRESOURCE_DATA defaultResourceData; //default data
	uint8_t* pSysMem = nullptr;

	DXGI_FORMAT format;

	int sizeY = 0; //source given sizeY for pSysMem
	int sizeX = 0; //source given sizeX for pSysMem
	int channels = 0;
	int bpp = 0;

	void ReleaseBuffersNonSamp() {
		SafeRelease(uav);

		SafeRelease(srv);
	}
	void MakeBuffers(BuiltImage_c* data) {
		ComPtr<ID3D11Texture2D> r = NULL;

		D3D11_TEXTURE2D_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		gpuTexDesc.Format = format;
		if (LinkSizeToRTV == false) {
			gpuTexDesc.Width = data->data.sizeX_c;
			gpuTexDesc.Height = data->data.sizeY_c;
			defaultResourceData.pSysMem = data->data.dataV;
		}
		else {
			pSysMem = ScaleBuffersToSizeCPU(pSysMem, sizeY, sizeX, channels, bpp, MainDX11Objects::obj->MainWidth, MainDX11Objects::obj->MainHeight);
			defaultResourceData.pSysMem = pSysMem;
			gpuTexDesc.Width = MainDX11Objects::obj->MainWidth;
			gpuTexDesc.Height = MainDX11Objects::obj->MainHeight;
		}

		defaultResourceData.SysMemPitch = gpuTexDesc.Width * channels * bpp;
		defaultResourceData.SysMemSlicePitch = gpuTexDesc.Height * gpuTexDesc.Width * channels * bpp;

		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.ArraySize = 1;
		gpuTexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |
			D3D11_BIND_SHADER_RESOURCE;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;
		gpuTexDesc.MiscFlags = 0;
		gpuTexDesc.CPUAccessFlags = 0;
		gpuTexDesc.Usage = D3D11_USAGE_DEFAULT;

		MainDX11Objects::obj->dxDevice->CreateTexture2D(
			&gpuTexDesc,
			&defaultResourceData,
			r.GetAddressOf());

		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		//UAVDesc.Texture2D
		ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		UAVDesc.Format = format; //
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		if (HasRW) {
			MainDX11Objects::obj->dxDevice->CreateUnorderedAccessView(r.Get(), &UAVDesc, &uav);
		}

		MainDX11Objects::obj->dxDevice->CreateShaderResourceView(r.Get(), nullptr, &srv);
		//send data to SRV and UAV
		
	}
	
	ImageObjectToRendererDX11(BuiltImage_c* data) {

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
		

		defaultResourceData.pSysMem = pSysMem;
		defaultResourceData.SysMemPitch = sizeX * channels * bpp;
		defaultResourceData.SysMemSlicePitch = sizeY * sizeX * channels * bpp;

		MakeBuffers(data);

		D3D11_SAMPLER_DESC samp_desc;

		samp_desc.Filter = D3D11_FILTER{ D3D11_FILTER_ANISOTROPIC };
		samp_desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		samp_desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		samp_desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE{ D3D11_TEXTURE_ADDRESS_WRAP };
		samp_desc.MipLODBias = 0;
		samp_desc.MaxAnisotropy = 8;
		samp_desc.ComparisonFunc = D3D11_COMPARISON_FUNC{ D3D11_COMPARISON_LESS };
		samp_desc.MinLOD = 1;
		samp_desc.MaxLOD = D3D11_FLOAT32_MAX;

		MainDX11Objects::obj->dxDevice->CreateSamplerState(&samp_desc, &Samp);

	}
	~ImageObjectToRendererDX11() {
		delete[] pSysMem;

		SafeRelease(srv);
		SafeRelease(uav);
		SafeRelease(Samp);
		//SafeRelease TODO: check if I need to clean up anything
	}
};


struct StructObjectToRendererDX11 : StructObjectToRenderer_Base{
	ComPtr<ID3D11Buffer> con;
	
	ComPtr<ID3D11Buffer> uavB; // sperate modifiable object data for a const pair and not const
	ComPtr<ID3D11UnorderedAccessView> uav;
	ComPtr<ID3D11ShaderResourceView> srv;

	StructObjectToRendererDX11(BuiltConstant_c* data) {
		
		Name = data->Name;
		NameRW = data->NameRW;
		NameRW_SRV = data->NameRW_SRV;
		StructName = data->StructName;
		StructElementName = data->StructElementName;
		StructElementNameRW = data->StructElementNameRW;

		ReferToData = data->vars; //copy

		int ElementCount = ReferToData.IT.size() + ReferToData.UT.size()  + ReferToData.FT.size();
		int memSize = ReferToData.IT.size() * sizeof(int32_t) + ReferToData.UT.size() * sizeof(uint32_t) + ReferToData.FT.size() * sizeof(float);
		int memSizePost = memSize;
		while (memSizePost % 16 != 0) memSizePost += 4;

		float* Data = (float*)malloc(memSize);

		typesInOrder.resize(ElementCount);
		typesInOrderName.resize(ElementCount);
		//typesInOrderNameRW.resize(ElementCount);

		int offset = 0;
		for (int i = 0; i < ReferToData.IT.size(); i++) {
			typesInOrder[i + offset] = INT_OBJ;
			typesInOrderName[i + offset] = ReferToData.IT[i].n;
			//typesInOrderNameRW[i + offset] = ReferToData.IT[i].nRW;
			Data[i + offset] = *reinterpret_cast<float*>(&ReferToData.IT[i].val); //stick raw bytes as float into Data
		}
		offset += ReferToData.IT.size();
		for (int i = 0; i < ReferToData.UT.size(); i++) {
			typesInOrder[i + offset] = UINT_OBJ;
			typesInOrderName[i + offset] = ReferToData.UT[i].n;
			//typesInOrderNameRW[i + offset] = ReferToData.IT[i].nRW;
			Data[i + offset] = *reinterpret_cast<float*>(&ReferToData.UT[i].val); //stick raw bytes as float into Data
		}
		offset += ReferToData.UT.size();
		for (int i = 0; i < ReferToData.FT.size(); i++) {
			typesInOrder[i + offset] = FLOAT_OBJ;
			typesInOrderName[i + offset] = ReferToData.FT[i].n;
			//typesInOrderNameRW[i + offset] = ReferToData.IT[i].nRW;
			Data[i + offset] = (ReferToData.FT[i].val);
		}




		D3D11_BUFFER_DESC bufDesc;
		ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.ByteWidth = memSizePost;
		
		D3D11_SUBRESOURCE_DATA defaultResourceData; //default data
		defaultResourceData.pSysMem = Data;

		MainDX11Objects::obj->dxDevice->CreateBuffer(&bufDesc, &defaultResourceData, &con);

		
		if (data->ReadWrite) {
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			SRVDesc.Buffer.FirstElement = 0;
			SRVDesc.Buffer.NumElements = 1; //TODO: check if it loads the struct correctly


			D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
			//UAVDesc.Texture2D
			ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
			UAVDesc.Format = DXGI_FORMAT_UNKNOWN; //
			UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			UAVDesc.Buffer.FirstElement = 0;
			UAVDesc.Buffer.NumElements = 1; //TODO: check if it loads the struct correctly

			ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));
			bufDesc.Usage = D3D11_USAGE_DEFAULT;
			bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
			bufDesc.CPUAccessFlags = 0;
			bufDesc.ByteWidth = memSize;
			bufDesc.StructureByteStride = sizeof(float);
			bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

			MainDX11Objects::obj->dxDevice->CreateBuffer(&bufDesc, &defaultResourceData, &uavB);

			MainDX11Objects::obj->dxDevice->CreateUnorderedAccessView(uavB.Get(), &UAVDesc, &uav);
			MainDX11Objects::obj->dxDevice->CreateShaderResourceView(uavB.Get(), &SRVDesc, &srv);

			HasRW = data->ReadWrite;
	
		}

		delete Data;
	}

	~StructObjectToRendererDX11() {
//		if(this->HasRW)
//			SafeRelease(uavB);

//		SafeRelease(con);

	}
	
};

struct ModelToRendererDX11 : ModelToRenderer_Base{
	DX11M3DR* Model;
	int Type = -1;

	ModelToRendererDX11(BuiltModel_c* data) {
		
		Name = data->Name;
		NameRW = data->NameRW;
		Type = data->Type;

		if (data->Type == - 1) {
			Model = new DX11M3DR(data->tmpM3DR);
		}
		else if (data->Type >-1 && data->Type<StaticObjectPass.size()) {
			Model = new DX11M3DR(data->tmpM3DR);
		}
#ifdef GET_OBJECT_STATIC
		OutputStringToFileForCopyPata(&Model);
#endif
	}
	~ModelToRendererDX11() {
		if (Type == -1) {
			delete Model;
		}
	}
};

struct PredefinedToRendererDX11 : PredefinedToRenderer_Base{

	ComPtr<ID3D11Buffer> Cdata;
	

	PredefinedToRendererDX11(BuiltPredefined_c* data) {
		
		D3D11_BUFFER_DESC bufDesc;
		ZeroMemory(&bufDesc, sizeof(bufDesc));
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.ByteWidth = sizeof(CONST_DATA_PASS_c);

		D3D11_SUBRESOURCE_DATA defaultResourceData; //default data
		defaultResourceData.pSysMem = data->data;

		MainDX11Objects::obj->dxDevice->CreateBuffer(&bufDesc, &defaultResourceData, &Cdata);

		//Create size based on data size thing, and then pass to const, and layout in vector here the var type layout with enums
		//TODO: load data into const buffer under names stated previously


	}

	~PredefinedToRendererDX11() {
		
	}

	void update(BuiltPredefined_c* bI) {
		MainDX11Objects::obj->dxDeviceContext->UpdateSubresource(Cdata.Get(), 0, nullptr, bI->data, 0, 0);
	}
};



struct ResourceObjectBaseDX11 : ResourceObjectBase {

	

	inline static ResourceObjectBaseDX11* obj;

	void SetResourceObjectBaseDX11() {
		MainDX11Objects::ROB = this;
	}

	PredefinedToRendererDX11* PredefinedData = nullptr;
	std::unordered_map<std::string /*name to identify image*/, ImageObjectToRendererDX11*/*data*/> ImageData; //use d4 to get and filter data
	std::unordered_map<std::string /*name to identify model*/, ModelToRendererDX11*/*data*/> ModelData;
	std::unordered_map<std::string /*name to identify struct*/, StructObjectToRendererDX11*/*data*/> ConstantData;

	std::unordered_map<uint64_t, RTVData_DX11*> RTVData;
	std::unordered_map<uint64_t, DEPTHData_DX11*> DEPTHData;


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


			ModelToRendererDX11* md_tmp = ModelData[data->Name];

			vp.MatInfo.resize(md_tmp->Model->MatData.size());
			vp.Vdata.resize(md_tmp->Model->VBuf.size());
			vp.Idata.resize(md_tmp->Model->IBuf.size());
			vp.Icount.resize(md_tmp->Model->Indice.size());
			
			DX11M3DR* tmpM3 = ModelData[data->Name]->Model;

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

	void PreBindImageData(ImageObjectToRendererDX11* ID) {
	//	ID->UAV_R
	//	ID->uav

		//MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers();
		MainDX11Objects::obj->dxDeviceContext->VSSetShaderResources(ID->srv_num, 1, &ID->srv);
		MainDX11Objects::obj->dxDeviceContext->VSSetSamplers(ID->sampler_num, 1, &ID->Samp);

		//MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers();
		MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources(ID->srv_num, 1, &ID->srv);
		MainDX11Objects::obj->dxDeviceContext->PSSetSamplers(ID->sampler_num, 1, &ID->Samp);
//		MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources();
//		MainDX11Objects::obj->dxDeviceContext->PSSetSamplers();
		
		MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(ID->uav_num, 1, &ID->uav, &NOneP);
		MainDX11Objects::obj->dxDeviceContext->CSSetSamplers(ID->sampler_num, 1, &ID->Samp);
	}
	void PreBindAllImageData() {
		for (const auto& i : ImageData) {
			PreBindImageData(i.second);
		}
	}
	void BindModelData(DX11M3DR* Model, int Obj) {
		
		ID3D11UnorderedAccessView* tmpF =  NULL;

		for (int i = 0; i < MaterialEnum::TEX_COUNT; i++) {
			if (Model->MatData[Obj].TexOn[i]) {
				MainDX11Objects::obj->dxDeviceContext->VSSetShaderResources(i, 1, Model->TexObj[Obj].TexSRV[i].GetAddressOf());
				MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources(i, 1, Model->TexObj[Obj].TexSRV[i].GetAddressOf());
				MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(i, 1, &tmpF, &NOneP);

			}
		}
		
	}

	void BindModelDataCS(DX11M3DR* Model, int Obj) {

		ID3D11ShaderResourceView* tmpF = NULL;

		for (int i = 0; i < MaterialEnum::TEX_COUNT; i++) {
			if (Model->MatData[Obj].TexOn[i]) {
				MainDX11Objects::obj->dxDeviceContext->VSSetShaderResources(i, 1, &tmpF);
				MainDX11Objects::obj->dxDeviceContext->PSSetShaderResources(i, 1, &tmpF);
				MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(i, 1, Model->TexObj[Obj].TexUAV[i].GetAddressOf(), &NOneP);
			}
		}
	}
	void PreBindConstantData(StructObjectToRendererDX11* ID) {
		
		MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers(ID->cb_num, 1, ID->con.GetAddressOf());

		MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers(ID->cb_num, 1, ID->con.GetAddressOf());

		MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(ID->uav_num, 1, ID->uav.GetAddressOf(), &NOneP);


	}
	void PreBindAllConstantData() {
		for (const auto& i : ConstantData) {
			PreBindConstantData(i.second);
		}
	}
	void PreBindDefault() {
		MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers(PredefinedData->cb_num, 1, PredefinedData->Cdata.GetAddressOf());

		MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers(PredefinedData->cb_num, 1, PredefinedData->Cdata.GetAddressOf());
	
		MainDX11Objects::obj->dxDeviceContext->CSSetConstantBuffers(PredefinedData->cb_num, 1, PredefinedData->Cdata.GetAddressOf());
	}
	//TODO: add individual compile buttons and checkmark for if up to date data
	void PreBindAllResources() {

		PreBindAllImageData();
		PreBindAllConstantData();
		PreBindDefault();

	}

	void ClearAllPredefined() {
		if(PredefinedData != nullptr)
		delete PredefinedData; //TODO: deal with delete later
		PredefinedData = nullptr;
	}
	void ClearAllImages() {

		for (const auto& i : ImageData) {
			delete i.second;
		}

		ImageData.clear();
	}
	void ClearAllModels() {

		for (const auto& i : ModelData) {
			delete i.second;
		}

		ModelData.clear();
	}
	void ClearAllConstants() {

		for (const auto& i : ConstantData) {
			delete i.second;
		}

		ConstantData.clear();
	}
	void ClearAllObjects() {
		//ClearAllPredefined();
		ClearAllImages();
		ClearAllModels();
		ClearAllConstants();
	}
	void LoadImageFromData(BuiltImage_c* bI) {
		ImageData[bI->Name] = new ImageObjectToRendererDX11(bI);
	}
	void LoadModelFromData(BuiltModel_c* bI) {
		ModelData[bI->Name] = new ModelToRendererDX11(bI);
	}
	void LoadConstantFromData(BuiltConstant_c* bI) {
		ConstantData[bI->Name] = new StructObjectToRendererDX11(bI);
	}
	void LoadPredefinedFromData(BuiltPredefined_c* bI) {
		PredefinedData = new PredefinedToRendererDX11(bI);
	}
	void UpdatePredefinedFromData(BuiltPredefined_c* bI) {
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

	template<class T> 
	void UnBindCSToUAV(T& data){
		ID3D11UnorderedAccessView* nullC = nullptr;

		for (auto& i : data) {
			MainDX11Objects::obj->dxDeviceContext->CSSetUnorderedAccessViews(i.second->uav_num, 1, &nullC, &OffsetDef);
		}
	}

	template<class T>
	void UnBindCSToSRV(T& data) {
		ID3D11ShaderResourceView* nullC = nullptr;

		for (auto& i : data) {
			MainDX11Objects::obj->dxDeviceContext->CSSetShaderResources(i.second->srv_num, 1, &nullC);
		}
	}

	void RunLogic(PipelineObjectIntermediateStateDX11** item) {
		if ((*item)->PObj->On) {
			//TODO, make run logic, then make code output in order

			MainDX11Objects::obj->dxDeviceContext->OMSetRenderTargets(1, RTVData[(*item)->RTV_Num]->rtv.GetAddressOf(), DEPTHData[(*item)->DEPTH_Num]->dsv.Get());

			if (!(*item)->PObj->ComputeOnlyToggle) {
				MainDX11Objects::obj->MakeBlend((*item)->PObj->Vertex.BlendToMake);
				ID3D11BlendState* bss = MainDX11Objects::obj->BlendObjects[(*item)->PObj->Vertex.BlendToMake].Get();
				if (MainDX11Objects::obj->TestForOptimize.BlendObject != bss || MainDX11Objects::obj->TestForOptimize.BlendFactor != (*item)->PObj->Vertex.BlendFactor) {
					MainDX11Objects::obj->dxDeviceContext->OMSetBlendState(bss, &(*item)->PObj->Vertex.BlendFactor[0], 0xffffffff/*Write all*/);
					MainDX11Objects::obj->TestForOptimize.BlendObject = bss;
					MainDX11Objects::obj->TestForOptimize.BlendFactor = (*item)->PObj->Vertex.BlendFactor;
				}

				MainDX11Objects::obj->MakeRaster((*item)->PObj->Vertex.RasterToMake);
				ID3D11RasterizerState* rss = MainDX11Objects::obj->RasterObjects[(*item)->PObj->Vertex.RasterToMake].Get();
				if (MainDX11Objects::obj->TestForOptimize.RasterObject != rss) {
					MainDX11Objects::obj->dxDeviceContext->RSSetState(rss);
					MainDX11Objects::obj->TestForOptimize.RasterObject = rss;
				}

				MainDX11Objects::obj->MakeDepthStencil((*item)->PObj->Vertex.StencilToMake); //try to make if not existing yet
				ID3D11DepthStencilState** dss = MainDX11Objects::obj->DepthStencilObjects[(*item)->PObj->Vertex.StencilToMake].GetAddressOf();
				if (MainDX11Objects::obj->TestForOptimize.DepthStencilObject != *dss) {
					MainDX11Objects::obj->dxDeviceContext->OMGetDepthStencilState(dss, &MainDX11Objects::obj->REF_FOR_DEPTH_STENCIL);
					MainDX11Objects::obj->TestForOptimize.DepthStencilObject = *dss;
				}

				if (MainDX11Objects::obj->TestForOptimize.VertexShader != (*item)->VDat) {
					MainDX11Objects::obj->dxDeviceContext->VSSetShader((*item)->VDat, NULL, NULL);
					MainDX11Objects::obj->TestForOptimize.VertexShader = (*item)->VDat;
				}

				if (MainDX11Objects::obj->TestForOptimize.PixelShader != (*item)->PDat) {
					MainDX11Objects::obj->dxDeviceContext->PSSetShader((*item)->PDat, NULL, NULL);
					MainDX11Objects::obj->TestForOptimize.PixelShader = (*item)->PDat;
				}

				if ((*item)->PObj->Vertex.Vdata.size() != 0) {

					DX11M3DR* Model = (DX11M3DR*)((DX11M3DR*)((*item)->PObj->Vertex.RawModel));
					MainDX11Objects::obj->dxDeviceContext->PSSetSamplers(0, 1, Model->Sampler.GetAddressOf());
					MainDX11Objects::obj->dxDeviceContext->VSSetSamplers(0, 1, Model->Sampler.GetAddressOf());

					for (int i = 0; i < (*item)->PObj->Vertex.Vdata.size(); i++) {

						if (MainDX11Objects::obj->TestForOptimize.Model != (*item)->PObj->Vertex.Vdata[i]) {
							MainDX11Objects::obj->dxDeviceContext->VSSetConstantBuffers(6, 1, ((ID3D11Buffer**)&(*item)->PObj->Vertex.MatInfo[i]));
							MainDX11Objects::obj->dxDeviceContext->PSSetConstantBuffers(6, 1, ((ID3D11Buffer**)&(*item)->PObj->Vertex.MatInfo[i]));
							BindModelData( Model, i );

							MainDX11Objects::obj->dxDeviceContext->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&(*item)->PObj->Vertex.Vdata[i], &(*item)->PObj->Vertex.VertexStride, &OffsetDef);
							MainDX11Objects::obj->dxDeviceContext->IASetIndexBuffer((ID3D11Buffer*)((*item)->PObj->Vertex.Idata[i]), DXGI_FORMAT_R32_UINT, OffsetDef);
							MainDX11Objects::obj->dxDeviceContext->DrawIndexed((*item)->PObj->Vertex.Icount[i], 0, 0);

							MainDX11Objects::obj->TestForOptimize.Model = (*item)->PObj->Vertex.Vdata[i];
						}
					}

				}
				else {
					//run if no vertex data load default cube
					DX11M3DR* tmpC = GetD3D11Cube();
					void* tmpAddress = (void*)tmpC->VBuf[0].Get();
					if (MainDX11Objects::obj->TestForOptimize.Model != tmpAddress) {
						MainDX11Objects::obj->dxDeviceContext->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&tmpAddress, &tmpC->VertexStride, &OffsetDef);
						MainDX11Objects::obj->dxDeviceContext->IASetIndexBuffer(tmpC->IBuf[0].Get(), DXGI_FORMAT_R32_UINT, OffsetDef);
						MainDX11Objects::obj->dxDeviceContext->DrawIndexed(tmpC->Indice[0].size(), 0, 0);

						MainDX11Objects::obj->TestForOptimize.Model = tmpAddress;
					}

				}
			}
			//
			MainDX11Objects::obj->SetNullRTVandDEPTH();

			if (!(*item)->PObj->TurnComputeOffToggle) {

				BindCSToUAV(RTVData);
				BindCSToSRV(DEPTHData);

				for (int i = 0; i < (*item)->Compute.size(); i++) {
					if (MainDX11Objects::obj->TestForOptimize.ComputeShader != (*item)->Compute[i].CDat) {
						MainDX11Objects::obj->dxDeviceContext->CSSetShader((*item)->Compute[i].CDat, NULL, NULL);
						MainDX11Objects::obj->TestForOptimize.ComputeShader = (*item)->Compute[i].CDat;
					}
					if ((*item)->Compute[i].AutoSetBlockToWindowSize == false) {
						MainDX11Objects::obj->dxDeviceContext->Dispatch((*item)->Compute[i].DimX, (*item)->Compute[i].DimY, (*item)->Compute[i].DimZ);
					}
					else {
						MainDX11Objects::obj->dxDeviceContext->Dispatch(int(MainDX11Objects::obj->MainWidth/ Renderable::BLOCK_X), int(MainDX11Objects::obj->MainHeight/Renderable::BLOCK_Y), 1);
					}
					
				}

				UnBindCSToUAV(RTVData);
				UnBindCSToSRV(DEPTHData);
			
			}

		}
	}
	int SizeOfRTV() {
		return RTVData.size();
	}
	void ClearRTVAndDepth() {
		for (auto& i : RTVData) {
			if (i.second->ClearEveryNewPass) {
				MainDX11Objects::obj->dxDeviceContext->ClearRenderTargetView(i.second->rtv.Get(), &MainDX11Objects::obj->CLEAR_COLOR[0]);
			}
		}
		for (auto& i : DEPTHData) {
			if (i.second->ClearEveryNewPass) {
				MainDX11Objects::obj->dxDeviceContext->ClearDepthStencilView(i.second->dsv.Get(), D3D11_CLEAR_DEPTH, 1, 0);
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
		
		PipelineObjectIntermediateStateDX11* itemP = new PipelineObjectIntermediateStateDX11();
		PipelineObjectIntermediateStateDX11** item = &itemP;
		
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

		//TODO: template do these 3 into 1 function
		//compile Pixel Shader if not exist
		if (PipelineObjectIntermediateStateDX11::PixelShaderMap.find((*item)->PName) == PipelineObjectIntermediateStateDX11::PixelShaderMap.end()){
			PipelineObjectIntermediateStateDX11::PixelShaderMap[(*item)->PName] = ShaderCDX11::obj->LoadShader<ID3D11PixelShader>(&BasePipeline::code[BasePipeline::PIXEL], Pobj.second->Pixel.name, "latest", &Pobj.second->Pixel.ErrorMessage_s, MainDX11Objects::obj->dxDevice.Get(), MainDX11Objects::obj->dxIL.GetAddressOf());
			SetErrorShowIfHasError(Pobj.second->Pixel, Pobj.second);
		}
		(*item)->PDat = PipelineObjectIntermediateStateDX11::PixelShaderMap[(*item)->PName].Get();
		if ((*item)->PDat == nullptr) { 
			delete itemP;
			return -1;
		}

		//compile Vertex Shader if not exist
		if (PipelineObjectIntermediateStateDX11::VertexShaderMap.find((*item)->VName) == PipelineObjectIntermediateStateDX11::VertexShaderMap.end()) {
			PipelineObjectIntermediateStateDX11::VertexShaderMap[(*item)->VName] = ShaderCDX11::obj->LoadShader<ID3D11VertexShader>(&BasePipeline::code[BasePipeline::VERTEX], Pobj.second->Vertex.name, "latest", &Pobj.second->Vertex.ErrorMessage_s, MainDX11Objects::obj->dxDevice.Get(), MainDX11Objects::obj->dxIL.GetAddressOf());
			SetErrorShowIfHasError(Pobj.second->Vertex, Pobj.second);
		}
		(*item)->VDat = PipelineObjectIntermediateStateDX11::VertexShaderMap[(*item)->VName].Get();

		if ((*item)->VDat == nullptr) { 
			delete itemP;
			return -1;
		}

		//compile Compute shader if not exist
		for (const auto& i : (*item)->PObj->Compute) {

			if (PipelineObjectIntermediateStateDX11::ComputeShaderMap.find((*item)->Compute[iter].CName) == PipelineObjectIntermediateStateDX11::ComputeShaderMap.end()) {
				PipelineObjectIntermediateStateDX11::ComputeShaderMap[(*item)->Compute[iter].CName] = ShaderCDX11::obj->LoadShader<ID3D11ComputeShader>(&BasePipeline::code[BasePipeline::COMPUTE], i.second->name, "latest", &i.second->ErrorMessage_s, MainDX11Objects::obj->dxDevice.Get(), MainDX11Objects::obj->dxIL.GetAddressOf());
				SetErrorShowIfHasError(*i.second, Pobj.second);
			}
			(*item)->Compute[iter].CDat = PipelineObjectIntermediateStateDX11::ComputeShaderMap[(*item)->Compute[iter].CName].Get();


			iter++;
		}

		if ((*item)->PObj->On == true) {//Not needed if statment
			(*item)->ToRunLogic = [&]() {
				RunLogic(&MainDX11Objects::obj->CompiledCode[MainDX11Objects::obj->CompiledCode.size()-1]);
			};
		}
		else {
			delete itemP;
			return -1;
		}
		MainDX11Objects::obj->CompiledCode.push_back(itemP);
	}

	template<class T> 
	void ClearShaderMap(std::unordered_map<std::string,T>& map) {
		for (const auto& i : map) {
			if (i.second != nullptr) i.second->Release();
		}
		map.clear();
	}
	void ClearShaderCache() {
		ClearShaderMap(PipelineObjectIntermediateStateDX11::PixelShaderMap);
		ClearShaderMap(PipelineObjectIntermediateStateDX11::VertexShaderMap);
		ClearShaderMap(PipelineObjectIntermediateStateDX11::ComputeShaderMap);
	}

	void GenRTVFromExistingMap() {
		for (const auto& i : RTVData) {
			delete i.second;
		}
		RTVData.clear();

		for (auto& i : RTV_DEPTH::RTV) {
			RTVData[i.first] = new RTVData_DX11(i.second);
		}
	}
	void GenDEPTHFromExistingMap() {
		for (const auto& i : DEPTHData) {
			delete i.second;
		}
		DEPTHData.clear();

		for (auto& i : RTV_DEPTH::DEPTH) {
			DEPTHData[i.first] = new DEPTHData_DX11(i.second);
		}
	}

	void ReleaseRTVAndDepthAndResizeTex() {
		for (auto& i : RTVData) {
			i.second->ReleaseBuffers();
		}
		for (auto& i : DEPTHData) {
			i.second->ReleaseBuffers();
		}
		for (auto& i : ImageData) {
			i.second->ReleaseBuffersNonSamp();
		}
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
			
		MainDX11Objects::obj->CompiledCode.clear();

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
		PipelineObjectIntermediateStateDX11::SelectedFinalRTV = RTVData[PipelineObj::SelectedFinalRTV]->t.Get();
		//This was done to not add to ref counter since I acctually don't want to unknowingly not delete the resource -- I delete at 2 specific points known in time

		for (const auto& i : OrderedPipelineState->P) { //loop through all ordered pipeline objects

			CompileNewPipelineObject(i);

		}


	}

};


#endif