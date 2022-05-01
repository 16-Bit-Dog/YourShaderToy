#ifndef FILE_MANAGER_STRUCTS
#define FILE_MANAGER_STRUCTS

//TODO: save somewhere on close the windows opened and reopen on program start based on txt file or smthing

#define MAX_IMAGE_NAME 10
#include <set>
#include <vector>
#include <string>
#include "ResourceObjectBase.h"
#include "RenderableManager.h"
#include "Renderable.h"
#include "Window_Struct.h"

static std::set<std::string> usedNameCont{ "ComputeShaderInput", "BLOCK_X", "BLOCK_Y", "PROGRAM_CONSTANTS", "", "Vertex",

"ProjectionMatrixS", "ViewMatrixS", "WorldMatrixS",
"ProjectionMatrix", "ViewMatrix", "WorldMatrix",
 
"DefaultProjectionMatrixS", "DefaultViewMatrixS", "DefaultWorldMatrixS",
"DefaultProjectionMatrix", "DefaultViewMatrix", "DefaultWorldMatrix"
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

struct TypeStorageMass {
	std::vector< IntTypeAndName_c > IT;
	std::vector< UintTypeAndName_c > UT;
	std::vector< FloatTypeAndName_c > FT;
//	std::vector< MatrixTypeAndName_c > MT;
};

struct d4 {
	uint8_t* data8 = nullptr; //only 1 is non null
	uint16_t* data16 = nullptr;
	uint32_t* data32 = nullptr;
	uint64_t* data64 = nullptr; //64 not included yet
	void* dataV = nullptr; //random accsess ptr

	int sizeX_c;
	int sizeY_c;
	int bpp_c;

	d4() {

	}

	d4(uint8_t* data8t, const int& sizeX, const int& sizeY) {
		sizeX_c = sizeX;
		sizeY_c = sizeY;
		bpp_c = 8;
		data8 = data8t;
		dataV = (void*)data8t;
	}
	d4(const int& sizeX, const int& sizeY, const int& bpp) {
		sizeX_c = sizeX;
		sizeY_c = sizeY;
		bpp_c = bpp;
		if (bpp == 8) {
			data8 = new uint8_t[sizeY * sizeX];
			dataV = (void*)data8;
		}
		else if (bpp == 16) {
			data16 = new uint16_t[sizeY * sizeX];
			dataV = (void*)data16;
		}
		else if (bpp == 32) {
			data32 = new uint32_t[sizeY * sizeX];
			dataV = (void*)data32;
		}
		else if (bpp == 64) {
			data64 = new uint64_t[sizeY * sizeX];
			dataV = (void*)data64;
		}
		else {
			throw("wrong format");
		}
	}
	void* dataReturn() {
		return dataV;
	}
	~d4() {
//		if(dataV != nullptr) delete[] dataV;
		if (data8 != nullptr) delete[] data8;
		if (data16 != nullptr) delete[] data16;
		if (data32 != nullptr) delete[] data32;
		if (data64 != nullptr) delete[] data64;
	}
};

void DealWithNameConflict(std::set<std::string>* usedName, std::string* Name, const std::string& suffixConflict);


/*
void DupNameHandle(std::set<std::string>* usedName, std::string* Name) {
	if (usedName->count(*Name) != 0) {
		for (int i = 0; i < Name->size(); i++) {
			if (usedName->count(*Name + "_" + std::to_string(i)) == 0) *Name += "_" + std::to_string(i); //don't allow dup names
		}
	}
}
*/

struct ObjectBuilder {
	
	inline static bool UNORM_ELSE_FLOAT_Driver;
	inline static bool LinkSizeToRTV;

	virtual void BuildItem() = 0;

};

struct CONST_DATA_PASS_c { //just for sizeof()
	uint32_t WindowSizeX;
	uint32_t WindowSizeY;
	uint32_t MousePosX;
	uint32_t MousePosY;

	uint32_t LeftClickState;
	uint32_t RightClickState;
	uint32_t MiddleClickState;
	float time;
	
	float DeltaLastKeyPress;
	float PAD[3];

	CONST_DATA_PASS_c() {
		
		WindowSizeX = GLFW_Window_C::Width;
		WindowSizeY = GLFW_Window_C::Height;

		MousePosX = std::floor(GLFW_Window_C::MousePosX);
		MousePosY = std::floor(GLFW_Window_C::MousePosY);

		LeftClickState = GLFW_Window_C::MouseLeftState;
		RightClickState = GLFW_Window_C::MouseRightState;
		MiddleClickState = GLFW_Window_C::MouseMiddleState;
		time = GLFW_Window_C::time;
		DeltaLastKeyPress = GLFW_Window_C::DeltaOfLastPress;
		//TODO fill struct to do built item compile
	}
	void update() {
		WindowSizeX = GLFW_Window_C::Width;
		WindowSizeY = GLFW_Window_C::Height;

		MousePosX = std::floor(GLFW_Window_C::MousePosX);
		MousePosY = std::floor(GLFW_Window_C::MousePosY);

		LeftClickState = GLFW_Window_C::MouseLeftState;
		RightClickState = GLFW_Window_C::MouseRightState;
		MiddleClickState = GLFW_Window_C::MouseMiddleState;
		time = GLFW_Window_C::time;
		DeltaLastKeyPress = GLFW_Window_C::DeltaOfLastPress;
	}
};


struct DepthTarget_s {
	inline static uint64_t GLOBAL_ID_COUNTER = 0;

	std::string TYPE_N = "";

	std::string name;
	uint64_t ID;

	bool ClearEveryNewPass = true;
	bool ClearEveryNewPassOld = false;

	std::string Spacing() {
		return std::move(std::string("D", ID));
	}

	inline static uint64_t GetNextID() {
		return std::move(GLOBAL_ID_COUNTER+1);
	}
	DepthTarget_s() {
		ID = GLOBAL_ID_COUNTER;

		GLOBAL_ID_COUNTER += 1;

		TYPE_N = "DEPTH";
		name = TYPE_N + " " + std::to_string(ID);
		DealWithNameConflict(&usedNameCont, &name, TYPE_N);

	}
	~DepthTarget_s() {
		usedNameCont.erase(name);
	}
};
struct RenderTarget_s {
	
	inline static uint64_t GLOBAL_ID_COUNTER = 0;

	std::string TYPE_N = "";

	std::string name;
	uint64_t ID;

	bool ClearEveryNewPass = true;
	bool ClearEveryNewPassOld = false;

	std::string Spacing() {
		return std::move(std::string("R", ID));
	}

	inline static uint64_t GetNextID() {
		return std::move(GLOBAL_ID_COUNTER+1);
	}
	RenderTarget_s() {
		ID = GLOBAL_ID_COUNTER;

		GLOBAL_ID_COUNTER += 1;

		TYPE_N = "RTV";
		name = TYPE_N + " " + std::to_string(ID);
		DealWithNameConflict(&usedNameCont, &name, TYPE_N);

	}
	~RenderTarget_s() {
		usedNameCont.erase(name);
	}
};

struct BuiltPredefined_c : ObjectBuilder {
	CONST_DATA_PASS_c* data = new CONST_DATA_PASS_c();

	std::vector<std::string>  namesU = {
	"WINDOW_SIZE_X",
	"WINDOW_SIZE_Y",
	"MOUSE_POS_X",
	"MOUSE_POS_Y",
	"LEFT_CLICK_STATE",
	"RIGHT_CLICK_STATE",
	"MIDDLE_CLICK_STATE",
	};
	std::vector<std::string>  namesF = {
	"NET_TIME"
	"DELTA_LAST_KEY"
	};

	//UINT32 WindowSizeX; -> //MainWin.Width;
	//UINT32 WindowSizeY; -> //MainWin.Height;

	//UINT32 MousePoxX; -> //MainWin.CursorPosX -> glfwGetCursorPos(MainWin.window, &xpos, &ypos);
	//UINT32 MousePoxY; -> //MainWin.CursorPosY -> glfwGetCursorPos(MainWin.window, &xpos, &ypos);

	// UINT32 LeftClickState; -> // MainWin.LeftClickState -> mouse_button_callback
	//UINT32 RightClick; -> //MainWin.RightClickState -> mouse_button_callback
	// UINT32 MiddleClick; -> //MainWin.MiddleClickState -> mouse_button_callback

	//float time; -> time


	BuiltPredefined_c() {
		
		for (const auto& i : namesU) {
			usedNameCont.insert(i);
		}
		for (const auto& i : namesF) {
			usedNameCont.insert(i);
		}
	}

	void UpdateItem() {
		data->update();
		Renderable::ROB->UpdatePredefinedFromData(this);
	}

	void BuildItem() {
		data->update();
		Renderable::ROB->LoadPredefinedFromData(this);
	}

	~BuiltPredefined_c() {
		for (const auto& i : namesU) {
			usedNameCont.erase(i);
		}
		for (const auto& i : namesF) {
			usedNameCont.erase(i);
		}
		delete data;
	}

};


struct BuiltImage_c : ObjectBuilder {
	//I read data immedeatly since it is simple, fast, and I want to make a TODO: preview option of image once loaded

	std::string Path = "";
	
	std::string Name = "";
	std::string NameRW = "";
	
	std::string SamplerName = "";

	d4 data;

	int bpp;
	int sizeX;
	int sizeY;
	int channels; 
	bool ReadWrite = true;
	bool IsPath;
	bool UNORM_ELSE_FLOAT = true;
	//d4 data;
	bool LinkSizeToRTV = false;

	void BuildItem() {
		Renderable::ROB->LoadImageFromData(this);
	}


	BuiltImage_c(const std::string& p, std::string s, const bool& IsPath, const int& sizeX_tmp, const int& sizeY_tmp, const int& channels_tmp, const int& bpp_tmp, const bool& UNORM_ELSE_FLOAT_tmp, const bool& LinkSizeToRTV, const d4* data_tmp) {
		//set name
		
		this->LinkSizeToRTV = LinkSizeToRTV;

		data = *data_tmp;

		UNORM_ELSE_FLOAT = UNORM_ELSE_FLOAT_tmp;
		this->IsPath = IsPath;
		Path = p;
		bpp = bpp_tmp;

		if (s == "") s = "TMP_TEX";
		else Name = s;
		
		
		DealWithNameConflict(&usedNameCont, &Name, "TEX");
		NameRW = Name + "_RW";
		DealWithNameConflict(&usedNameCont, &NameRW, "TEX_RW");
		SamplerName = Name + "_SAMPLE";
		DealWithNameConflict(&usedNameCont, &SamplerName, "_SAMPLE");

		sizeX = sizeX_tmp;
		sizeY = sizeY_tmp;
		channels = channels_tmp;


	}
	~BuiltImage_c() {
		usedNameCont.erase(Name);
	}



};


struct BuiltModel_c : ObjectBuilder {

	std::string Path = "";
	std::string Name = ""; //same as Buffer <-- when loaded 
	std::string NameRW = ""; //same as UAV which is already made in 3DDXObj.h - when I fetch, 

	int Type;
	//TODO: preinclude cube and sphere file which includes as menu item if you click the option, and auto sets up in loader


	void BuildItem() {
		Renderable::ROB->LoadModelFromData(this);
	}


	BuiltModel_c(const std::string& p, std::string s, const int& t) {
		//TODO fill M3DR based on path in D3D11ResourceObjects.h, else stop trying to load and throw error saying "invalid" 
		
		Path = p;

		if (s == "") s = "TMP_MODEL";
		else Name = s;

		
		DealWithNameConflict(&usedNameCont, &Name, "MODEL")
			;
		NameRW = Name + "_RW";

		DealWithNameConflict(&usedNameCont, &NameRW, "MODEL_RW");

		Type = t;
	}

	~BuiltModel_c() {

		usedNameCont.erase(Name);
		usedNameCont.erase(NameRW);

	}

};


struct BuiltConstant_c : ObjectBuilder {
	std::string Name = "";
	std::string NameRW = "";
	std::string NameRW_SRV = "";
	std::string StructName = "";
	TypeStorageMass vars;

	std::string StructElementName = "";
	std::string StructElementNameRW = "";
	//bool ReadWrite = false;
	
	bool ReadWrite = true;

	BuiltConstant_c(std::string s) {
		
		Name = s;
		DealWithNameConflict(&usedNameCont, &Name, "STRUCT");
		NameRW = Name + "_RW";
		DealWithNameConflict(&usedNameCont, &NameRW, "STRUCT_RW");
		NameRW_SRV = NameRW + "_SRV";
		DealWithNameConflict(&usedNameCont, &NameRW_SRV, "STRUCT_RW_SRV");

		StructName = Name+"_S";
		DealWithNameConflict(&usedNameCont, &StructName, "STRUCT_S");
	
		StructElementName = Name + "_rw";
		StructElementNameRW = Name + "_s";
		
		DealWithNameConflict(&usedNameCont, &StructElementName, "VAR_S");
		DealWithNameConflict(&usedNameCont, &StructElementNameRW, "VAR_S");

	}

	void AddInt(std::string* s, const int32_t& intV) {
		std::string stmp = *s;
		DealWithNameConflict(&usedNameCont, &stmp, "INT");
		std::string sRW = stmp + "_RW";
		DealWithNameConflict(&usedNameCont, &sRW, "INT_RW");
		vars.IT.push_back(IntTypeAndName_c(&stmp, &sRW, intV));
	}
	void AddUint(std::string* s, uint32_t& uintV) {
		std::string stmp = *s;
		DealWithNameConflict(&usedNameCont, &stmp, "UINT");
		std::string sRW = stmp + "_RW";
		DealWithNameConflict(&usedNameCont, &sRW, "UINT_RW");
		vars.UT.push_back(UintTypeAndName_c(&stmp, &sRW, uintV));
	}
	void AddFloat(std::string* s, float& floatV) {
		std::string stmp = *s;
		DealWithNameConflict(&usedNameCont, &stmp, "FLOAT");
		std::string sRW = stmp + "_RW";
		DealWithNameConflict(&usedNameCont, &sRW, "FLOAT_RW");
		vars.FT.push_back(FloatTypeAndName_c(&stmp, &sRW, floatV));
	}
/*	void AddMatrix(std::string* s, float* matV[4][4]) {
		XMMATRIX tmp;
		for (int i = 0; i < 4; i++) {
			tmp.r[i] = XMVectorSet(*matV[i][0], *matV[i][1], *matV[i][2], *matV[i][3]);
		}
		std::string stmp = *s;
		DealWithNameConflict(&usedNameCont, &stmp, "MATRIX");
		std::string sRW = stmp + "_RW";
		DealWithNameConflict(&usedNameCont, &sRW, "MATRIX_RW");
		vars.MT.push_back(MatrixTypeAndName_c(&stmp, &sRW, &tmp));
	}
*/

	
	//TODO, make value setter in info tab - this is gonna be messyish

	void BuildItem() {
		Renderable::ROB->LoadConstantFromData(this);
	}


	~BuiltConstant_c() {
		usedNameCont.erase(Name);
		usedNameCont.erase(NameRW);
		usedNameCont.erase(StructElementName);
		usedNameCont.erase(StructElementNameRW);
	}
};

bool IsNotAlphaAndUnderScore(unsigned char ch) {
	return (!std::isalnum(ch) && ch != '_');
}

void DealWithNameConflict(std::set<std::string>* usedName, std::string* Name, const std::string& suffixConflict) {
	Name->erase(std::remove_if(Name->begin(), Name->end(), IsNotAlphaAndUnderScore), Name->end()); //replace escape chars and such
	
	if (usedName->count(*Name) != 0) {
		for (int i = 0; i < usedName->size(); i++) {
			if (usedName->count(*Name + "_" + suffixConflict + "_" + std::to_string(i)) == 0) {
				*Name += +"_" + suffixConflict + "_" + std::to_string(i); //don't allow dup names
				i = usedName->size();
				(*usedName).insert(*Name);
			}
		}
	}
	else {
		(*usedName).insert(*Name);
	}
}

struct RTV_DEPTH {
	inline static std::unordered_map<uint64_t, RenderTarget_s*> RTV = { {0, new RenderTarget_s()} };
	inline static std::unordered_map<uint64_t, DepthTarget_s*> DEPTH = { {0, new DepthTarget_s()} };
};

#endif