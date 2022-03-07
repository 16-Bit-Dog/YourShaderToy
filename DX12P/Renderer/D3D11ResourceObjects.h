
#define GET_OBJECT_STATIC


#ifndef DX11_H_RESOURCE_OBJ
#define DX11_H_RESOURCE_OBJ

#include "ResourceObjectBase.h"
#include "DX11IncludeMain.h"
#include "FileManagerResourceStruct.h"
#include "3DCommons/3DDX11Obj.h"
#include "RenderableManager.h"
#include "DX11H.h"
#include "Renderable.h"
#include "Type_Enum.h"
#include "3DCommons/StaticObjects.h"
//DX11M3DR

/*
struct DX11_OBJ_RESOURCE_S {
	inline static Renderable* DX11Obj;

	inline static ComPtr<ID3D11Device5> dxDeviceR;
	inline static ComPtr<ID3D11DeviceContext4> dxDeviceContextR;

	inline static void SET_DX_CONTENT(ComPtr<ID3D11Device5> dxDevice_tmp, ComPtr<ID3D11DeviceContext4> dxDeviceContext_tmp, Renderable* dxmo) {
		dxDeviceR = dxDevice_tmp;
		dxDeviceContextR = dxDeviceContext_tmp;
		DX11Obj = dxmo;
	}

};
*/

//TODO: texture loaded clamp, mirror, ect. and sampling options



struct ImageObjectToRendererDX11 {

	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11UnorderedAccessView> uav;
	bool HasRW = false;

	DXGI_FORMAT format;

	std::string name;
	std::string nameRW;


	ImageObjectToRendererDX11(BuiltImage_c* data) {
		nameRW = data->NameRW;
		name = data->Name;

		ComPtr<ID3D11Texture2D> r;

		if (data->data.bpp_c == 8) {
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		else if (data->data.bpp_c == 16) {
			if (data->UNORM_ELSE_FLOAT) format = DXGI_FORMAT_R16G16B16A16_UNORM;
			else format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}
		else if (data->data.bpp_c == 32) {
			if (data->UNORM_ELSE_FLOAT) format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			else format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}


		D3D11_SUBRESOURCE_DATA defaultResourceData; //default data
		defaultResourceData.pSysMem = data->data.dataReturn();

		D3D11_TEXTURE2D_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		gpuTexDesc.Format = format;
		gpuTexDesc.Width = data->data.sizeX_c;
		gpuTexDesc.Height = data->data.sizeY_c;
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
			&r);

		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		//UAVDesc.Texture2D
		ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		UAVDesc.Format = format; //
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

		if (data->ReadWrite) {
			MainDX11Objects::obj->dxDevice->CreateUnorderedAccessView(r.Get(), &UAVDesc, &uav);
			HasRW = data->ReadWrite;
		}

		MainDX11Objects::obj->dxDevice->CreateShaderResourceView(r.Get(), nullptr, &srv);
		//send data to SRV and UAV
	}
	~ImageObjectToRendererDX11() {
		
	}
};


struct StructObjectToRendererDX11 {
	ComPtr<ID3D11Buffer> con;
	
	ComPtr<ID3D11Buffer> uavB; // sperate modifiable object due to limits of DX11
	ComPtr<ID3D11UnorderedAccessView> uav;

	std::string Name;
	std::string NameRW;
	std::string StructName;

	std::vector<int> typesInOrder = { };
	std::vector<std::string> typesInOrderName = { }; //TODO: remove and change to fill with constructor
	std::vector<std::string> typesInOrderNameRW = { }; //TODO: remove and change to fill with constructor


	TypeStorageMass ReferToData; //copy of type storage mass since you have compiled data here with names

	bool HasRW = true;

	StructObjectToRendererDX11(BuiltConstant_c* data) {
		Name = data->Name;
		NameRW = data->NameRW;
		StructName = data->StructName;

		ReferToData = data->vars; //copy

		int ElementCount = ReferToData.IT.size() + ReferToData.UT.size()  + ReferToData.FT.size();
		int memSize = ReferToData.IT.size() * sizeof(int32_t) + ReferToData.UT.size() * sizeof(uint32_t) + ReferToData.FT.size() * sizeof(float);
		int memSizePost = memSize;
		while (memSizePost % 16 != 0) memSizePost += 4;

		float* Data = (float*)malloc(memSize);

		typesInOrder.resize(ElementCount);
		typesInOrderName.resize(ElementCount);
		typesInOrderNameRW.resize(ElementCount);

		int offset = 0;
		for (int i = 0; i < ReferToData.IT.size(); i++) {
			typesInOrder[i + offset] = INT_OBJ;
			typesInOrderName[i + offset] = ReferToData.IT[i].n;
			typesInOrderNameRW[i + offset] = ReferToData.IT[i].nRW;
			Data[i + offset] = *reinterpret_cast<float*>(&ReferToData.IT[i].val); //stick raw bytes as float into Data
		}
		offset += ReferToData.IT.size();
		for (int i = 0; i < ReferToData.UT.size(); i++) {
			typesInOrder[i + offset] = UINT_OBJ;
			typesInOrderName[i + offset] = ReferToData.IT[i].n;
			typesInOrderNameRW[i + offset] = ReferToData.IT[i].nRW;
			Data[i + offset] = *reinterpret_cast<float*>(&ReferToData.UT[i].val); //stick raw bytes as float into Data
		}
		offset += ReferToData.UT.size();
		for (int i = 0; i < ReferToData.FT.size(); i++) {
			typesInOrder[i + offset] = FLOAT_OBJ;
			typesInOrderName[i + offset] = ReferToData.IT[i].n;
			typesInOrderNameRW[i + offset] = ReferToData.IT[i].nRW;
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

		ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.ByteWidth = memSize;
		bufDesc.StructureByteStride = sizeof(float);
		bufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		MainDX11Objects::obj->dxDevice->CreateBuffer(&bufDesc, &defaultResourceData, &uavB);

		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		//UAVDesc.Texture2D
		ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		UAVDesc.Format = DXGI_FORMAT_UNKNOWN; //
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		UAVDesc.Buffer.FirstElement = 0;
		UAVDesc.Buffer.NumElements = 1; //TODO: check if it loads the struct correctly

		if (data->ReadWrite) {
			MainDX11Objects::obj->dxDevice->CreateUnorderedAccessView(uavB.Get(), &UAVDesc, &uav);
			HasRW = data->ReadWrite;
		}

		delete Data;
	}

};

struct ModelToRendererDX11 {
	DX11M3DR Model;
	std::string Name;
	std::string NameRW;

	ModelToRendererDX11(BuiltModel_c* data) {
		Name = data->Name;
		NameRW = data->NameRW;

		Model = DX11M3DR(data->Path);
#ifdef GET_OBJECT_STATIC
		OutputStringToFileForCopyPata(&Model);
#endif
	}
};

struct PredefinedToRendererDX11 {
	std::vector<int> typesInOrder = { UINT_OBJ, UINT_OBJ, UINT_OBJ, UINT_OBJ,
		UINT_OBJ, UINT_OBJ, UINT_OBJ,
		FLOAT_OBJ };
	std::vector<std::string> typesInOrderName = { "WINDOW_SIZE_X", "WINDOW_SIZE_Y", "MOUSE_POS_X", "MOUSE_POS_Y",
		"LEFT_CLICK_STATE", "RIGHT_CLICK_STATE", "MIDDLE_CLICK_STATE",
		"NET_TIME" };
	
	ComPtr<ID3D11Buffer> Cdata;
	std::string Name = "PROGRAM_CONSTANTS";
	

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
//		SafeRelease(Cdata);
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

	void ClearAllPredefined() {
		if(PredefinedData != nullptr)
		delete PredefinedData; //TODO: deal with delete later
		PredefinedData = nullptr;
	}
	void ClearAllImages() {

		for (auto& i : ImageData) {
			delete i.second;
		}

		ImageData.clear();
	}
	void ClearAllModels() {

		for (auto& i : ModelData) {
			delete i.second;
		}

		ModelData.clear();
	}
	void ClearAllConstants() {

		for (auto& i : ConstantData) {
			delete i.second;
		}

		ConstantData.clear();
	}
	void ClearAllObjects() {
		ClearAllPredefined();
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

};


#endif