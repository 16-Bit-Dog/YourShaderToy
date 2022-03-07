

#ifndef DX11OBJ_LOADER
#define DX11OBJ_LOADER

#include "DX11IncludeMain.h"
#include "3DCommon.h"
//#include "RenderableManager.h"

//TODO: load texture with WCI loader

struct MaterialData { //seperate struct to send to resource

	/*

		&MatData.HasAmbientTexture 0
		&MatData.HasEmissiveTexture, 1
		&MatData.HasDiffuseTexture, 2
		&MatData.HasSpecularTexture, 3
		&MatData.HasSpecularPowerTexture, 4
		&MatData.HasNormalTexture, 5
		&MatData.HasBumpTexture, 6
		&MatData.HasOpacityTexture, 7


	*/

	XMFLOAT4 Emissive = { 0,0,0,1 };
	XMFLOAT4 BaseColor = { 1,1,1,1 }; //uses if no texture - emulates a texture of 1 color only
	XMFLOAT4 Ambient = { 1,1,1,1 };
	XMFLOAT4 Diffuse = { 0.1,0.1,0.1,1 };
	XMFLOAT4 Specular = { 0.1,0.1,0.1,1 };
	XMFLOAT4 Reflectance = { 1.0f,1.0f,1.0f,1.0f };

	float Opacity = 1.0f;
	float SpecularStr = { 128 };
	float IndexOfRefraction = 0;
	INT32 TexOn[8] = { 0,0,0,0,0,0,0,0 };
	float BumpIntensity = 0.0f;

	float SpecularScale = 1.0f;
	float AlphaThreshold = 0.0f;
	UINT Lit = false;
	float pad;
};
struct MaterialDX11 {
	
	bool UpdateMat = false;

	ID3D11ShaderResourceView* TexSRV[8] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, };

	ID3D11Texture2D* TexR[8] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, };

	MaterialData MatData;

	ID3D11Buffer* MatDataBuf;

};

struct DX11M3DR : M3DR{
	
	inline static ComPtr<ID3D11Device5> dxDevice;
	inline static ComPtr<ID3D11DeviceContext4> dxDeviceContext;

	inline static void SET_DX_CONTENT(ComPtr<ID3D11Device5> dxDevice_tmp, ComPtr<ID3D11DeviceContext4> dxDeviceContext_tmp) {
		DX11M3DR::dxDevice = dxDevice_tmp;
		DX11M3DR::dxDeviceContext = dxDeviceContext_tmp;
	}

	std::vector<MaterialDX11> Mat = {};
	ID3D11SamplerState* Sampler = NULL;
	ID3D11BlendState* BlendState = NULL;

	ID3D11Buffer* CBuf;
	ID3D11Buffer* ArmatureCBuf;

	std::vector < ID3D11Buffer* > VBuf;
	std::vector < ID3D11UnorderedAccessView* > VBufUAV;
	std::vector < ID3D11Buffer* > IBuf;


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
			dxDeviceContext->UpdateSubresource(ArmatureCBuf, 0, nullptr, &BoneDataTLMA[0], 0, 0);
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

		dxDeviceContext->UpdateSubresource(Mat[i].MatDataBuf, 0, nullptr, &Mat[i].MatData, 0, 0);
	}

	void DefaultAllMatBuf() {
		for (int i = 0; i < Mat.size(); i++) {
			DefaultMatBuf(i);
		}
	}
	
	void LoadVertexIndiceData() //also reloads vertex data
	{


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

		dxDeviceContext->UpdateSubresource(CBuf, 0, nullptr, &ObjTune, 0, 0);
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

		SafeRelease(BlendState);
		dxDevice->CreateBlendState(&blendVal, &BlendState);
	}


	DX11M3DR(std::vector<VNT>* V, bool ClearPtr = true) {

		RESIZE_VECTORS_OBJ_LOAD(1);

		CreateArmatureCBuf();

		modelDat[0] = *V;

		AutoFillIndice(0);

		VertexPostProcess();

		LoadVertexIndiceData();

		DefaultCBuf();
		SetupBlendStateDefault();
		DefaultSampler();
		DefaultAllMatBuf();

		if (ClearPtr) delete V;

	}
	DX11M3DR(std::string path = "") {
		//SetupTexLinkResource();
		SetupBlendStateDefault();
		if (path != "") {
			LoadFBXFileWithVertex(path);
		}
		else {
			std::cout << "no file at path";
		}

		DefaultCBuf();
		DefaultSampler();
		DefaultAllMatBuf();

	}
	~DX11M3DR() {
		//TODO: make deconstructor
	}
};


#endif