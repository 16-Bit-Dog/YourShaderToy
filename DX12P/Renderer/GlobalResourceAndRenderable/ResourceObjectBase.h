#pragma once

#include <unordered_map>
#include <cinttypes>
#include <set>
#include "Type_Enum.h"

struct VertexShaderPipeline;
struct PipelineMain;
struct BuiltImage_c;
struct BuiltModel_c;
struct BuiltConstant_c;
struct BuiltPredefined_c;
struct DX11M3DR;


static bool HasCodeCompileError = false;
static std::vector<std::string > CodeCompileErrorNames;

void resetCodeCompile() {
	HasCodeCompileError = false;
	CodeCompileErrorNames.clear();
}


static std::set<std::string> usedNameCont{ "ComputeShaderInput", "BLOCK_X", "BLOCK_Y", "PROGRAM_CONSTANTS", "", "Vertex",

"ProjectionMatrixS", "ViewMatrixS", "WorldMatrixS",
"ProjectionMatrix", "ViewMatrix", "WorldMatrix",

"DefaultProjectionMatrixS", "DefaultViewMatrixS", "DefaultWorldMatrixS",
"DefaultProjectionMatrix", "DefaultViewMatrix", "DefaultWorldMatrix",

"ModelInfoBuf",
"AMBIENT_TEX", "EMISSIVE_TEX",  "DIFFUSE_TEX", "SPECULAR_TEX", "SPECULAR_POW_TEX", "NORMAL_TEX", "REFLECTANCE_TEX", "OPACITY_TEX",
"SAMPLER",
};

/*
struct MatrixTypeAndName_c {
	XMFLOAT4X4 val;
	std::string n;
	std::string nRW;
	MatrixTypeAndName_c(std::string* s, std::string* sRW, float* MatV[4][]) {
		n = *s;
		nRW = *sRW;
		val = *MatV;
	}
	~MatrixTypeAndName_c() {
		usedNameCont.erase(n);
		usedNameCont.erase(nRW);
	}
};*/
struct IntTypeAndName_c {
	int32_t val = 0;
	std::string n;
	std::string nRW;
	IntTypeAndName_c(std::string* s, std::string* sRW, const int32_t& intV) {
		n = *s;
		nRW = *sRW;
		val = intV;
	}
	~IntTypeAndName_c() {
		usedNameCont.erase(n);
		usedNameCont.erase(nRW);
	}
};
struct UintTypeAndName_c {
	uint32_t val = 0;
	std::string n;
	std::string nRW;
	UintTypeAndName_c(std::string* s, std::string* sRW, const uint32_t& uintV) {
		n = *s;
		nRW = *sRW;
		val = uintV;
	}
	~UintTypeAndName_c() {
		usedNameCont.erase(n);
		usedNameCont.erase(nRW);
	}
};
struct FloatTypeAndName_c {
	float val = 0.0f;
	std::string n;
	std::string nRW;
	FloatTypeAndName_c(std::string* s, std::string* sRW, const float& floatV) {
		n = *s;
		nRW = *sRW;
		val = floatV;
	}
	~FloatTypeAndName_c() {
		usedNameCont.erase(n);
		usedNameCont.erase(nRW);
	}
};



struct d4 {
	void* dataV = nullptr; //random accsess ptr

	int sizeX_c = 0;
	int sizeY_c = 0;
	int bpp_c = 0;
	int channels_c = 4;

	void SetUntil(int sizeX,int sizeY) {
		
		delete[] dataV;

		dataV = (void*)malloc(sizeY_c * sizeX_c * bpp_c * channels_c);

		sizeY_c = sizeY;
		sizeX_c = sizeX;

	}
	d4() {

	}

	d4(uint8_t* data8t, const int& sizeX, const int& sizeY, int channels) {
		sizeX_c = sizeX;
		sizeY_c = sizeY;
		this->channels_c = channels;
		bpp_c = 8;
		dataV = (void*)data8t;
	}
	d4(const int& sizeX, const int& sizeY, const int& bpp, int channels) {
		if (bpp == 8 || bpp == 16 || bpp == 32 || bpp == 64) {
			sizeX_c = sizeX;
			sizeY_c = sizeY;
			bpp_c = bpp;
			this->channels_c = channels;
			dataV = (void*)malloc(sizeY*sizeX*bpp*channels);
		}
		else {
			throw("wrong format");
		}
	}
	uint64_t size() {
		return sizeX_c * sizeY_c * channels_c * bpp_c;
	}
	~d4() {
		//		if(dataV != nullptr) delete[] dataV;
		delete[] dataV;
	}
};

struct TypeStorageMass {
	std::vector< IntTypeAndName_c > IT;
	std::vector< UintTypeAndName_c > UT;
	std::vector< FloatTypeAndName_c > FT;
	//	std::vector< MatrixTypeAndName_c > MT;
};

struct RegisterMaps {
	inline static std::set<int> UAV_R{0,1,2,3,4,5,6,7};
	inline static std::set<int> SRV_R{0,1,2,3,4,5,6,7};
	inline static std::set<int> CB_R{ 0,1,2,3,4,5,6 }; //for default matrixes
	inline static std::set<int> S_R{0};

	int uav_num = -1; //TODO: set all uav and srv and cb nums with func
	int srv_num = -1; //TODO: set all uav and srv and cb nums with func
	int cb_num = -1;
	int sampler_num = -1;

	std::string CBName() {
		return std::move(std::to_string(cb_num));
	}
	std::string SRVName() {
		return std::move(std::to_string(srv_num));
	}
	std::string UAVName() {
		return std::move(std::to_string(uav_num));
	}
	std::string SamplerName() {
		return std::move(std::to_string(sampler_num));
	}


	int AddUAVNum() {
		for (int i = 0; i < UAV_R.size() + 1; i++) {
			if (UAV_R.count(i) == 0) {
				UAV_R.insert(i);
				uav_num = i;
				return i;
			}
		}
	}
	int AddSRVNum() {
		for (int i = 0; i < SRV_R.size() + 1; i++) {
			if (SRV_R.count(i) == 0) {
				SRV_R.insert(i);
				srv_num = i;
				return i;
			}
		}
	}
	int AddCBNum() {
		for (int i = 0; i < CB_R.size() + 1; i++) {
			if (CB_R.count(i) == 0) {
				CB_R.insert(i);
				cb_num = i;
				return i;
			}
		}
	}

	int AddSNum() {
		for (int i = 0; i < S_R.size() + 1; i++) {
			if (S_R.count(i) == 0) {
				S_R.insert(i);
				sampler_num = i;
				return i;
			}
		}
	}
	void RemoveUAVNum() {
		UAV_R.erase(uav_num);
	}
	void RemoveSRVNum() {
		SRV_R.erase(srv_num);
	}
	void RemoveCBNum() {
		CB_R.erase(cb_num);
	}
	void RemoveSNum() {
		S_R.erase(sampler_num);
	}
};

struct RTVData_Base : RegisterMaps{
	std::string name;

	virtual void ReleaseBuffers() = 0;
	virtual void MakeBuffers() = 0;

	RTVData_Base() {
		AddUAVNum();
	}

	~RTVData_Base() {
		RemoveUAVNum();
	}
};
struct DEPTHData_Base : RegisterMaps {
	std::string name;

	virtual void ReleaseBuffers() = 0;
	virtual void MakeBuffers() = 0;

	DEPTHData_Base() {
		AddSRVNum();
	}

	~DEPTHData_Base() {
		RemoveSRVNum();
	}
};

struct ImageObjectToRenderer_Base : RegisterMaps {
	bool HasRW = false;
	bool HasRWinPixelShader = false;

	bool LinkSizeToRTV = false;

	std::string formatString = "";

	std::string name;

	std::string nameRW;

	std::string samplerName;

	ImageObjectToRenderer_Base() {
		AddUAVNum();
		AddSRVNum();
		AddSNum();
	}
	~ImageObjectToRenderer_Base() {
		RemoveSRVNum();
		RemoveUAVNum();
		RemoveSNum();
	}
};

struct StructObjectToRenderer_Base : RegisterMaps {

	std::string Name;
	std::string NameRW;
	std::string NameRW_SRV;
	std::string StructName;
	std::string StructElementName = "s";
	std::string StructElementNameRW = "s";

	std::vector<int> typesInOrder = { };
	std::vector<std::string> typesInOrderName = { }; //TODO: remove and change to fill with constructor

	TypeStorageMass ReferToData; //copy of type storage mass since you have compiled data here with names

	bool HasRW = false;

	StructObjectToRenderer_Base() {

		AddUAVNum();
		AddCBNum();
		AddSRVNum();
	}

	~StructObjectToRenderer_Base() {
		RemoveUAVNum();
		RemoveSRVNum();
		RemoveCBNum();
	}

};

struct ModelToRenderer_Base : RegisterMaps {

	std::string Name;
	std::string NameRW;

	ModelToRenderer_Base() {
		AddUAVNum();
		AddSRVNum();
	}
	~ModelToRenderer_Base() {
		RemoveSRVNum();
		RemoveUAVNum();
	}
};

struct PredefinedToRenderer_Base : RegisterMaps {
	std::vector<int> typesInOrder = { UINT_OBJ, UINT_OBJ, UINT_OBJ, UINT_OBJ,
	UINT_OBJ, UINT_OBJ, UINT_OBJ,
	FLOAT_OBJ, FLOAT_OBJ }; //PRE DEFINED TYPES
	std::vector<std::string> typesInOrderName = { "WINDOW_SIZE_X", "WINDOW_SIZE_Y", "MOUSE_POS_X", "MOUSE_POS_Y",
		"LEFT_CLICK_STATE", "RIGHT_CLICK_STATE", "MIDDLE_CLICK_STATE",
		"NET_TIME", "DELTA_LAST_KEY" };
	std::string Name = "PROGRAM_CONSTANTS";

	virtual void update(BuiltPredefined_c* bI) = 0;

	PredefinedToRenderer_Base() {
		AddCBNum();


	}
	~PredefinedToRenderer_Base() {
		RemoveCBNum();
	}

};


struct ResourceObjectBase {
	
	virtual void CompileCodeLogic(PipelineMain* OrderedPipelineState) = 0;
	virtual void AddItemTextDefault(std::vector<std::string>* v) = 0;
	virtual void AddItemTextImages(std::vector<std::string>* v) = 0;
	virtual void AddItemTextModels(std::vector<std::string>* v) = 0;
	virtual void AddItemTextConstants(std::vector<std::string>* v) = 0;
	virtual void AddItemTextRTV(std::vector<std::string>* v) = 0;
	virtual void AddItemTextDEPTH(std::vector<std::string>* v) = 0; //THESE ADD CODE IN XXResourceObjects.h files

	virtual void PreBindAllResources() = 0;

	virtual void ClearAllPredefined() = 0;
	virtual void ClearAllImages() = 0;
	virtual void ClearAllModels() = 0;
	virtual void ClearAllConstants() = 0;
	virtual void ClearAllObjects(bool fullClean) = 0;


	void wrapClearAll(bool clearPredefined) {
		ClearAllObjects(clearPredefined);
	}


	virtual void LoadImageFromData(BuiltImage_c* bI) = 0;
	virtual void LoadModelFromData(BuiltModel_c* bI) = 0;
	virtual void LoadConstantFromData(BuiltConstant_c* bI) = 0;
	virtual void LoadPredefinedFromData(BuiltPredefined_c* bI) = 0;
	virtual void UpdatePredefinedFromData(BuiltPredefined_c* bI) = 0;

	virtual void SetDataToPipelineVertex(BuiltModel_c* data, VertexShaderPipeline& vp) = 0;
	
	~ResourceObjectBase() {
		//wrapClearAll(true);
	}


};

