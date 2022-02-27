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

struct RendererFileM {
	inline static Renderable* Render;

	inline static void SET_RENDERABLE(Renderable* r_tmp) {
		Render = r_tmp;
	}
};

struct IntTypeAndName_c {
	int32_t val = 0;
	std::string n;
	IntTypeAndName_c(std::string* s, int32_t* intV) {
		n = *s;
		n = *intV;
	}
};
struct UintTypeAndName_c {
	uint32_t val = 0;
	std::string n;
	UintTypeAndName_c(std::string* s, uint32_t* uintV) {
		n = *s;
		val = *uintV;
	}
};
struct FloatTypeAndName_c {
	float val = 0.0f;
	std::string n;
	FloatTypeAndName_c(std::string* s, float* floatV) {
		n = *s;
		val = *floatV;
	}
};

struct TypeStorageMass {
	std::vector< IntTypeAndName_c > IT;
	std::vector< UintTypeAndName_c > UT;
	std::vector< FloatTypeAndName_c > FT;
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

	d4(uint8_t* data8t, int sizeX, int sizeY) {
		sizeX_c = sizeX;
		sizeY_c = sizeY;
		bpp_c = 8;
		data8 = data8t;
		dataV = (void*)data8t;
	}
	d4(int sizeX, int sizeY, int bpp) {
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

void DealWithNameConflict(std::set<std::string>* usedName, std::string* Name, std::string suffixConflict);


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
	inline static std::set<std::string> usedNameCont;

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

};

struct BuiltPredefined_c : ObjectBuilder {
	//UINT32 WindowSizeX; -> //MainWin.Width;
	//UINT32 WindowSizeY; -> //MainWin.Height;

	//UINT32 MousePoxX; -> //MainWin.CursorPosX -> glfwGetCursorPos(MainWin.window, &xpos, &ypos);
	//UINT32 MousePoxY; -> //MainWin.CursorPosY -> glfwGetCursorPos(MainWin.window, &xpos, &ypos);

	// UINT32 LeftClickState; -> // MainWin.LeftClickState -> mouse_button_callback
	//UINT32 RightClick; -> //MainWin.RightClickState -> mouse_button_callback
	// UINT32 MiddleClick; -> //MainWin.MiddleClickState -> mouse_button_callback

	//float time; -> time

	void BuildItem() {

	}

};


struct BuiltImage_c : ObjectBuilder {
	//I read data immedeatly since it is simple, fast, and I want to make a TODO: preview option of image once loaded

	std::string Path = "";
	std::string Name = "";

	d4 data;

	int bpp;
	int sizeX;
	int sizeY;
	int channels; 
	bool ReadWrite = false;
	bool IsPath;
	bool UNORM_ELSE_FLOAT;
	//d4 data;

	void BuildItem() {
		RendererFileM::Render->ROB->LoadImageFromData(this);
	}

	BuiltImage_c(std::string p, std::string s, bool IsPath, int sizeX_tmp, int sizeY_tmp, int channels_tmp, int bpp_tmp, bool UNORM_ELSE_FLOAT_tmp, d4* data_tmp) {
		//set name
		data = *data_tmp;

		UNORM_ELSE_FLOAT = UNORM_ELSE_FLOAT_tmp;
		IsPath = IsPath;
		Path = p;
		bpp = bpp_tmp;

		if (s == "") s = "TMP_TEX";
		else Name = s;

		DealWithNameConflict(&usedNameCont, &Name, "TEX");

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

	//TODO: preinclude cube and sphere file which includes as menu item if you click the option, and auto sets up in loader


	void BuildItem() {
		RendererFileM::Render->ROB->LoadModelFromData(this);
	}

	BuiltModel_c(std::string p, std::string s) {
		//TODO fill M3DR based on path in D3D11ResourceObjects.h, else stop trying to load and throw error saying "invalid" 
		Path = p;

		if (s == "") s = "TMP_MODEL";
		else Name = s;

		NameRW = Name + "_RW";

		DealWithNameConflict(&usedNameCont, &Name, "MODEL");
		DealWithNameConflict(&usedNameCont, &NameRW, "MODEL_RW");

	}

	~BuiltModel_c() {

		usedNameCont.erase(Name);
		usedNameCont.erase(NameRW);

	}

};

static std::set<std::string> usedName_Constant;

struct BuiltConstant_c : ObjectBuilder {
	std::string Name = "";
	TypeStorageMass vars;

	BuiltConstant_c(std::string s) {
		Name = s;
		DealWithNameConflict(&usedNameCont, &Name, "STRUCT");
	}

	void AddInt(std::string* s, int32_t* intV) {
		std::string stmp = *s;
		DealWithNameConflict(&usedNameCont, &stmp, "INT");
		vars.IT.push_back(IntTypeAndName_c(s, intV));
	}
	void AddUint(std::string* s, uint32_t* uintV) {
		std::string stmp = *s;
		DealWithNameConflict(&usedNameCont, &stmp, "UINT");
		vars.UT.push_back(UintTypeAndName_c(s, uintV));
	}
	void AddFloat(std::string* s, float* floatV) {
		std::string stmp = *s;
		DealWithNameConflict(&usedNameCont, &stmp, "FLOAT");
		vars.FT.push_back(FloatTypeAndName_c(s, floatV));
	}



	bool ReadWrite = false;

	bool IsPath = false;
	//TODO, make value setter in info tab - this is gonna be messyish

	void BuildItem() {
		RendererFileM::Render->ROB->LoadConstantFromData(this);
	}

	~BuiltConstant_c() {
		usedNameCont.erase(Name);
		usedNameCont.erase(Name);
	}
};

void DealWithNameConflict(std::set<std::string>* usedName, std::string* Name, std::string suffixConflict) {
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


#endif