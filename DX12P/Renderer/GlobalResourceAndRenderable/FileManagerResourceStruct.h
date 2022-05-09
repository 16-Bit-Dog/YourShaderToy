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
#include <random>

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

struct NoiseDat_Obj {
	bool UseNoise = true;
	uint8_t MinCol = 0;
	uint8_t MaxCol = 255;
};

struct ObjectBuilder {
	
	inline static bool UNORM_ELSE_FLOAT_Driver = true;
	inline static bool LinkSizeToRTV = false;
	
	inline static NoiseDat_Obj NoiseDat = NoiseDat_Obj();
	inline static bool MadeTMP = false;
	inline static std::vector<std::vector<std::array<uint8_t,4>>>* noiseTMPmem = nullptr;

	inline static void MakeNewArrOfDat() {
		MadeTMP = false;
		noiseTMPmem->clear();
	}

	inline static void DrawAndHandleMinAndMaxSize() {
		if (ImGui::InputScalar("Min color (0-255):", ImGuiDataType_U8, &ObjectBuilder::NoiseDat.MinCol)) {
			MakeNewArrOfDat();
		}
		if (ImGui::InputScalar("Max color (0-255):", ImGuiDataType_U8, &ObjectBuilder::NoiseDat.MaxCol)) {
			MakeNewArrOfDat();
		}
		if (ObjectBuilder::NoiseDat.MinCol > ObjectBuilder::NoiseDat.MaxCol) NoiseDat.MinCol = NoiseDat.MaxCol;
	}

	inline static void fillVec(std::vector<uint8_t>& v) {
		
		int m = ObjectBuilder::NoiseDat.MinCol;
		int index = 0;
		
		for (int i = ObjectBuilder::NoiseDat.MinCol; true; i++) {
			if (i > ObjectBuilder::NoiseDat.MaxCol) i = NoiseDat.MinCol;

			if (index == v.size()) return;

			v[index] = i;
			index += 1;

		}
		
	}

	inline static std::vector<std::vector<std::array<uint8_t, 4>>>* GenerateNoiseWithParam(uint64_t seed, uint32_t sizeX, uint32_t sizeY) {
		std::vector<uint8_t> dataBase;
		std::vector<std::vector<std::array<uint8_t, 4>>>* VecIn = new std::vector<std::vector<std::array<uint8_t, 4>>>();
		VecIn->resize(sizeY);
		if (seed > 1000000000000000) seed = 1;
		for (int i = 0; i < sizeY; i++) {
			(*VecIn)[i].resize(sizeX);

			if (sizeX < ObjectBuilder::NoiseDat.MaxCol-ObjectBuilder::NoiseDat.MinCol) dataBase.resize(ObjectBuilder::NoiseDat.MaxCol - ObjectBuilder::NoiseDat.MinCol); //if not encapsulating all colors desiered add them into the fray
			else dataBase.resize(sizeX);


	
			fillVec(dataBase);

			std::default_random_engine randomGen(seed);

			std::shuffle(dataBase.begin(), dataBase.end(), randomGen);

			for (int z = 0; z < sizeX; z++) {
				(*VecIn)[i][z] = { dataBase[z], dataBase[z], dataBase[z], 255};
			}
			
			dataBase.clear();

			seed += 1;
		}

		return std::move(VecIn);

	}
	
	
	inline static void MakeAndSetNoiseWithParamForPreview(uint64_t seed) {
		if (ImGui::BeginMenuGreen("Preview Noise##preview noise menu")) {
			if (MadeTMP == false) {
				if (noiseTMPmem != nullptr) delete noiseTMPmem;
				noiseTMPmem = GenerateNoiseWithParam(seed, 50, 50);
				MadeTMP = true;
			}

			
			ImGui::PushFont(CustomFontSmall);
			float c = 0.0f;
			ImVec2 tmpv;
			for (int y = 0; y < (*noiseTMPmem).size(); y++) {

				for (int x = 0; x < (*noiseTMPmem)[y].size(); x++) {
					//		ImVec2 tmpv_t = { tmpv.x+x,tmpv.y+y};
					//		ImVec2 tmpvM_t = { tmpvM.x+x,tmpvM.y+y };

							//ImU32* t = (ImU32*)&noiseTMPmem[y][x][0];

					c = float((*noiseTMPmem)[y][x][0]) / 255.0f;
					ImGui::TextColored({ c,c,c,1.0f }, "#");
					ImGui::SameLine();
					tmpv = ImGui::GetCursorScreenPos();
					ImGui::SetCursorScreenPos({ tmpv.x - ImGui::GetFontSize()-1, tmpv.y });
				}
				ImGui::NewLine();
				tmpv = ImGui::GetCursorScreenPos();
				ImGui::SetCursorScreenPos({ tmpv.x ,tmpv.y-ImGui::GetFontSize()-1});

			}
			ImGui::PopFont();
			ImGui::EndMenu();
		}
	}

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

static uint8_t* ScaleBuffersToSizeCPU(uint8_t* oldMem, const int& currY, const int& currX, const int& channels, const int& bpp, const int& newW, const int& newH) {
	int bppl = int(bpp / 8); //take in new data regardless of size and fix to scale
	uint8_t* newMem = (uint8_t*)malloc(newH * newW * channels * bpp); //new uint8_t[newH][newW][channels][int(bpp/8)];
	
	float ratioX = float(currX)/float(newW);
	float ratioY = float(currY)/float(newH);

	for (float y = 0; y < newH; y++) {
		for (float x = 0; x < newW; x++) {
			for (int c = 0; c < channels; c++) {
				for (int b = 0; b < bpp; b++) {
					newMem[int(y) * int(x) * c * b] = oldMem[int(y * ratioY) * int(x * ratioX) * c * b]; //trunicate works as well as round down
				}
			}
		}
	}
	delete[] oldMem;
	return newMem;
	//memcpy(pSysMem, newMem, newH * newW * channels * bpp);
	
}

struct BuiltImage_c : ObjectBuilder {
	//I read data immedeatly since it is simple, fast, and I want to make a TODO: preview option of image once loaded

	std::string Path = "";
	
	std::string Name = "";
	std::string NameRW = "";
	
	std::string SamplerName = "";

	d4 data;

	int bpp = 8;
	int sizeX = 1;
	int sizeY = 1;
	int channels = 4; 
	bool ReadWrite = true;
	bool IsPath = false;
	bool UNORM_ELSE_FLOAT = true;
	//d4 data;
	bool LinkSizeToRTV = false;

	NoiseDat_Obj ndo;

	void BuildItem() {
		Renderable::ROB->LoadImageFromData(this);
	}


	BuiltImage_c(const std::string& p, std::string s, const bool& IsPath, const int& sizeX_tmp, const int& sizeY_tmp, const int& channels_tmp, const int& bpp_tmp, const bool& UNORM_ELSE_FLOAT_tmp, const bool& LinkSizeToRTV_tmp, const NoiseDat_Obj ndo_tmp, d4* data_tmp) {
		//set name

		sizeX = sizeX_tmp;
		sizeY = sizeY_tmp;
		
		channels = channels_tmp;

		LinkSizeToRTV = LinkSizeToRTV_tmp;

		ndo = ndo_tmp;

		if (ndo.UseNoise == false) {
			data = *data_tmp;
		}
		else {

			std::vector<std::vector<std::array<uint8_t,4>>>* vecTmp = ObjectBuilder::GenerateNoiseWithParam(std::floor(GLFW_Window_C::time), sizeX, sizeY); //moved so pointer cost only
			bpp = 8;
			channels = 4;

			data = d4((uint8_t*)vecTmp, sizeX, sizeY, channels);

		}



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