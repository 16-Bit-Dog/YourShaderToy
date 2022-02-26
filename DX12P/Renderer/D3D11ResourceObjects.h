
#ifndef DX11_H_RESOURCE_OBJ
#define DX11_H_RESOURCE_OBJ

#include "ResourceObjectBase.h"
#include "DX11IncludeMain.h"
#include "FileManagerResourceStruct.h"
#include "3DCommons/3DDX11Obj.h"

//DX11M3DR

//TODO: texture loaded clamp, mirror, ect. and sampling options
struct SrvUavPairDX11 {
	ComPtr<ID3D11ShaderResourceView> srv;
	ComPtr<ID3D11UnorderedAccessView> uav;

	DXGI_FORMAT format;

	SrvUavPairDX11(BuiltImage_c* data) {
		ComPtr<ID3D11Texture2D> r;
		
		if (data->data->bpp_c == 8) {
			format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		else if (data->data->bpp_c == 16) {
			if(data->UNORM_ELSE_FLOAT) format = DXGI_FORMAT_R16G16B16A16_UNORM;
			else format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		}
		else if (data->data->bpp_c == 32) {
			if (data->UNORM_ELSE_FLOAT) format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			else format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}


		D3D11_SUBRESOURCE_DATA defaultResourceData; //default data
		defaultResourceData.pSysMem = data->data->dataReturn();

		D3D11_TEXTURE2D_DESC gpuTexDesc;
		ZeroMemory(&gpuTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
		gpuTexDesc.Format = format;
		gpuTexDesc.Width = data->data->sizeX_c;
		gpuTexDesc.Height = data->data->sizeY_c;
		gpuTexDesc.MipLevels = 1;
		gpuTexDesc.ArraySize = 1;
		gpuTexDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS |
			D3D11_BIND_SHADER_RESOURCE;
		gpuTexDesc.SampleDesc.Count = 1;
		gpuTexDesc.SampleDesc.Quality = 0;
		gpuTexDesc.MiscFlags = 0;
		gpuTexDesc.CPUAccessFlags = 0;
		gpuTexDesc.Usage = D3D11_USAGE_DEFAULT;
		
		DX11Obj.dxDevice->CreateTexture2D(
			&gpuTexDesc,
			&defaultResourceData,
			&r);
		
		D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
		//UAVDesc.Texture2D
		ZeroMemory(&UAVDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		UAVDesc.Format = format; //
		UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		
		DX11Obj.dxDevice->CreateUnorderedAccessView(r.Get(), &UAVDesc, &uav);
		DX11Obj.dxDevice->CreateShaderResourceView(r.Get(), nullptr, &srv);
		//send data to SRV and UAV
	}

	~SrvUavPairDX11() {
		//SafeRelease(srv);
		//SafeRelease(uav);
		//TODO make deconsturctor
	}
};

struct ImageObjectToRendererDX11 {
	
	SrvUavPairDX11* data;

	ImageObjectToRendererDX11(BuiltImage_c* data_tmp) {
		data = new SrvUavPairDX11(data_tmp);
	}
	~ImageObjectToRendererDX11() {
		delete[] data;
	}
};

struct ConUavPairDX11 {
	ComPtr<ID3D11Buffer> con;
	ComPtr<ID3D11UnorderedAccessView> uav;

	ConUavPairDX11() {
		//bI->Path
	}

	~ConUavPairDX11() {
		//TODO make deconsturctor
	}
};

struct StructObjectToRendererDX11 {

	ConUavPairDX11 data;
	TypeStorageMass ReferToData; //copy of type storage mass since you have compiled data here with names

	StructObjectToRendererDX11(BuiltConstant_c* data) {
		ReferToData = data->vars; //copy
	}
	
};

struct ModelToRendererDX11 {
	DX11M3DR Model;
	ModelToRendererDX11(BuiltModel_c* data) {
		Model = DX11M3DR(data->Path);
	}
};

struct ResourceObjectBaseDX11 : ResourceObjectBase{

	ResourceObjectBaseDX11() {
		DX11Obj.ROB = this;
	}

	std::unordered_map<std::string /*name to identify image*/, ImageObjectToRendererDX11*/*data*/> ImageData; //use d4 to get and filter data
	std::unordered_map<std::string /*name to identify model*/, ModelToRendererDX11*/*data*/> ModelData;
	std::unordered_map<std::string /*name to identify struct*/, StructObjectToRendererDX11*/*data*/> ConstantData;

	void ClearAllImages() {

		for (auto& i : ImageData) {
			delete[] i.second;
		}

		ImageData.clear();
	}
	void ClearAllModels() {
		
		for (auto& i : ModelData) {
			delete[] i.second;
		}

		ModelData.clear();
	}
	void ClearAllConstants() {
		
		for (auto& i : ConstantData) {
			delete[] i.second;
		}

		ConstantData.clear();
	}
	void ClearAllObjects() {
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

};


#endif