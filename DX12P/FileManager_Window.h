#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Main_Window.h"
#include <algorithm>
#include <set>
//TODO: delete from set in deconstructor the name usage, also add to set
//TODO: make global name check for conflict struct, to make sure name is valid


#define MAX_IMAGE_NAME 10

void DealWithNameConflict(std::set<std::string>* usedName, std::string* Name, std::string suffixConflict);

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
	uint64_t* data64 = nullptr;
	d4(uint8_t* data8t) {
		data8 = data8t;
	}
	d4(int sizeX, int sizeY, int bpp) {
		if (bpp == 8) {
			data8 = new uint8_t[sizeY*sizeX];
		}
		else if (bpp == 16) {
			data16 = new uint16_t[sizeY*sizeX];
		}
		else if (bpp == 32) {
			data32 = new uint32_t[sizeY*sizeX];
		}
		else if (bpp == 64) {
			data64 = new uint64_t[sizeY*sizeX];
		}
		else {
			throw("wrong format");
		}
	}
};

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

	virtual void BuildItemGeneric() = 0;
	
};

struct CONST_DATA_PASS_c { //just for sizeof()
	UINT32 WindowSizeX;
	UINT32 WindowSizeY;
	UINT32 MousePosX;
	UINT32 MousePosY;

	UINT32 LeftClickState;
	UINT32 RightClickState;
	UINT32 MiddleClickState;
	FLOAT time;

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

	void BuildFromExistingData() {
		//TODO - build from pre set data in WinMain.XXX <-- using sizeof(CONST_DATA_PASS_c) Const Buffer 
	}
	void CleanPreviousBuild() {
		//TODO - build from pre set data
	}
	void BuildItemGeneric() {
		CleanPreviousBuild();
		BuildFromExistingData();
	}

	void UpdateFromExistingData() {
		//TODO: update objects
	}
};

std::set<std::string> usedNameCont;
struct BuiltImage_c : ObjectBuilder{
	//I read data immedeatly since it is simple, fast, and I want to make a TODO: preview option of image once loaded
	
	std::string Path = "";
	std::string Name = "";
	
	int bpp;
	int sizeX;
	int sizeY;
	int channels;
	bool ReadWrite = false;
	bool IsPath;

	uint8_t* data8 = nullptr; //only 1 is non null
	uint16_t* data16 = nullptr; 
	uint32_t* data32 = nullptr;
	uint64_t* data64 = nullptr;


	void BuildEmpty() {
		//TODO
	}
	void BuildFromPath() {
		//TODO
	}
	void CleanPreviousBuild() {
		//TODO
	}
	void BuildItemGeneric() {
		CleanPreviousBuild();
		if (IsPath) {
			BuildFromPath();
		}
		else {
			BuildEmpty();
		}
	}

	BuiltImage_c(std::string p, std::string s, bool IsPath, int sizeX_tmp, int sizeY_tmp, int channels_tmp, int bpp_tmp) {
		//set name
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
		if (data8 != nullptr) delete[] data8;
		if (data16 != nullptr) delete[] data16;
		if (data32 != nullptr) delete[] data32;
		if (data64 != nullptr) delete[] data64;
	}



};


struct BuiltModel_c {

	std::string Path = "";
	std::string Name = ""; //same as Buffer <-- when loaded 
	std::string NameRW = ""; //same as UAV which is already made in 3DDXObj.h - when I fetch, 

	M3DR* ModelDat = nullptr;

	//TODO: preinclude cube and sphere file which includes as menu item if you click the option, and auto sets up in loader

	void BuildFromPath() {
		//TODO
	}
	void CleanPreviousBuild() {
		//TODO
	}
	void BuildItemGeneric() {
		CleanPreviousBuild();
		BuildFromPath();
	}

	BuiltModel_c(std::string p, std::string s) {
		//TODO fill M3DR based on path, else stop trying to load and throw error saying "invalid" 
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
		
		if(ModelDat!=nullptr) delete[] ModelDat;
	}

};

std::set<std::string> usedName_Constant;

struct BuiltConstant_c {
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

	void BuildFromInfoTab() {
		 //use preexisting build data to build, defaults to 0
	}
	void BuildFromInfoPath() {
		//dunno how to make this yet- prob Renderdoc syntax
	}
	void CleanPreviousBuild() {

	}
	void BuildItemGeneric() {
		CleanPreviousBuild();
		BuildFromInfoTab();
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

struct MASTER_FileManager : MASTER_Function_Inherit {

	bool BuiltObjects = false; //TODO: built objects is seperate option than built code
	//TODO: build code BUTTON and built object BUTTON, compile code done only after build object is done - auto does this.

	ImGuiFileDialog fImage;
	ImGuiFileDialog fModel;
	ImGuiFileDialog fConstant; //to read a XML or something I guess?

	std::vector<BuiltImage_c*> ImageStore;
	std::vector<BuiltModel_c*> ModelStore;
	std::vector<BuiltConstant_c*> ConstantStore;

	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI_obj.WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_FileManager() {
		settingWindowSettingsMaker();
	}

	void BuildAllObjectsItem() {
		//iterate through all items, run "clear mem" - THEN - run object builder from path
	}//TODO
	void DrawBuildAllObjects() {
		//TODO: button draw for BUILD OBJECTS
		BuildAllObjectsItem();
	}

	void DrawPREDEFINED() {
		if (ImGui::CollapsingHeader("Predefined:", NULL))
		{

			ImGui::Text("Variables:");
			ImGui::Text("Width"); ImGui::SameLine(); ImGui::HelpMarker("Main Window Width");
			ImGui::Text("Height"); ImGui::SameLine(); ImGui::HelpMarker("Main Window Height");
			ImGui::Text("MousePosX"); ImGui::SameLine(); ImGui::HelpMarker("Mouse Position X"); 
			ImGui::Text("MousePosY"); ImGui::SameLine(); ImGui::HelpMarker("Mouse Position Y");

			ImGui::Text("MouseLeftState"); ImGui::SameLine(); ImGui::HelpMarker("Left Mouse click: \npress = 1\nclosed = 0");
			ImGui::Text("MouseRightState"); ImGui::SameLine(); ImGui::HelpMarker("Right Mouse click: \npress = 1\nclosed = 0"); 
			ImGui::Text("MouseMiddleState"); ImGui::SameLine(); ImGui::HelpMarker("Middle Mouse click: \npress = 1\nclosed = 0");
			ImGui::Text("Time"); ImGui::SameLine(); ImGui::HelpMarker("Time since window creation");

		}
		ImGui::Separator();
	}

	void FileSelector(std::string FileTypeToFind /*.exe, .png, .jpeg ## ect*/, ImGuiFileDialog* fd, std::string name) {
		fd->OpenDialog("ChooseFileDlgKey", ("Choose " + name), FileTypeToFind.c_str(), ".");
	}

	//Add custom vars
	std::string RGBA_nameOfString = "";
	int RGBA_sizeX = 1;
	int RGBA_sizeY = 1;
	int RGBA_bbp = 8; //rgba mandatory - 8 bpp, 16 bpp, 32 bpp, 64 bpp
	

	//TODO, RW toggle after creation

	void AddImageToList(std::string Path, std::string Name, bool IsPath, int sizeX, int sizeY, int channels, int bpp) {
		ImageStore.push_back(new BuiltImage_c(Path, Name, IsPath, sizeX, sizeY, channels, bpp));
		//TODO add image with string name, make new object, and make the show-er for it
	}
	void AddModelToList(std::string Path, std::string Name) {
		ModelStore.push_back(new BuiltModel_c(Path, Name));
	}

	void AddImage() {
		if (ImGui::BeginMenu("Add##Image")) {
			if (ImGui::BeginMenu("Find Image##Image")) {
				ImGui::InputText("Name: ##AddImageN", &ToAddImageName, ImGuiInputTextFlags_CharsNoBlank);
				if (ImGui::Button("Search For File##Image")) {
					FileSelector(".png,.jpeg,.jpg", &fImage, "Image");
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Custom Image ##Image")) {
				ImGui::Text("Name: ");
				ImGui::SameLine();
				
				ImGui::InputText("##RGBA Add image", &RGBA_nameOfString, ImGuiInputTextFlags_CharsNoBlank);
				
				ImGui::InputInt("SizeX: ##RGBA Add image", &RGBA_sizeX);
				ImGui::InputInt("SizeY: ##RGBA Add image", &RGBA_sizeY);

				ImGui::InputInt("Bit Per Channel: ##RGBA Add image", &RGBA_bbp, RGBA_bbp, RGBA_bbp);
				if (RGBA_bbp > 64) RGBA_bbp = 64;
				else if (RGBA_bbp < 0) RGBA_bbp = 8;
				
				for (int i = 3; i < 7; i++) {
					if (RGBA_bbp > pow(2, i) && (RGBA_bbp % int(pow(2, i))) != 0) RGBA_bbp = int(pow(2, i));
				}

				if (ImGui::Button("Create ## RGBA Add image")) {
					AddImageToList(RGBA_nameOfString, RGBA_nameOfString, false, RGBA_sizeX, RGBA_sizeY, 4, RGBA_bbp /*, d4(RGBA_sizeX, RGBA_sizeY, RGBA_bbp)*/);
				}
				
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
	}
	std::string ToAddImageName = "";//TODO: organize code and loose vars
	std::string ToAddModelName = "";//TODO: organize code and loose vars
	void CheckImageFileSelectors() { //these are seperate from the model one and constant one since I may want special behavior like pre-and post filter
		if (fImage.Display("ChooseFileDlgKey"))
		{
			if (fImage.IsOk() == true)
			{
				int width, height, channels;
				std::string path = fImage.GetFilePathName();
				/*uint8_t* data = */ stbi_load(path.c_str(), &width, &height, &channels, 4); //force assume 4 channels (TODO: check if this force assumes 4 and forces padding to 4)
				AddImageToList(path, ToAddImageName, true, width, height, channels, 8/*, d4(data)*/);
				// action
			}
			// close
			fImage.Close();
		}
		if(fModel.Display("ChooseFileDlgKey"))
		{
			if (fModel.IsOk() == true)
			{
				AddModelToList(fModel.GetFilePathName(), ToAddModelName);// action
			}
			// close
			fModel.Close();
		}

	}


	bool RemoveImage(int i) {
		ImGui::Text("Remove");
		ImGui::SameLine();
		if (ImGui::Button(("-##ImageRemove" + sPad(i)).c_str())) {
			ImageStore.erase(ImageStore.begin()+i);
			return false; //deleted
		}
		return true; // did notdelete
	}
	void ToggleReadWrite(int i) {
		ImGui::SameLine();
		ImGui::Checkbox(("R/W Toggle##ImageRemove" + sPad(i)).c_str(), &ImageStore[i]->ReadWrite);
	}
	void ShowImage() {
		for (int i = 0; i < ImageStore.size(); i++) {
			ImGui::Separator();
			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(ImageStore[i]->Name.c_str());
			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();
			if (!RemoveImage(i)) {
				continue;
				i -= 1;
			}
			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();
			ToggleReadWrite(i);
			
			ImGui::Bullet();
			if (ImGui::CollapsingHeader(("Image Info:##" + sPad(i)).c_str(), NULL)) { //TODO test
				ImGui::Text(("sizeX: " + std::to_string(ImageStore[i]->sizeX)).c_str());
				ImGui::SameLine();
				ImGui::Text("|");
				ImGui::SameLine();
				ImGui::Text(("sizeY: " + std::to_string(ImageStore[i]->sizeY)).c_str());
				ImGui::SameLine();
				ImGui::Text("|");
				ImGui::SameLine();
				ImGui::Text(("bpp: " + std::to_string(ImageStore[i]->bpp)).c_str());
				ImGui::SameLine();
				ImGui::Text("|");
				if (ImageStore[i]->IsPath) {
					ImGui::Text("Path:"); ImGui::SameLine(); 
					ImGui::Text(ImageStore[i]->Path.c_str());
				}
				//TODO: show info tab collapsing header

			}
			ImGui::Separator();
		}
	}

	void DrawIMAGE() {
		if (ImGui::CollapsingHeader("Images:", NULL))
		{
			AddImage();
			//TODO: draw ImageStore and allow info expansion to show dimentions, and such
			ShowImage();


		}
		ImGui::Separator();
	}


	void AddModel() {
		if (ImGui::BeginMenu("Add ##Model")) {
			if (ImGui::BeginMenu("Find .FBX Model##Model")) {
				ImGui::InputText("Name: ##AddModelName", &ToAddModelName, ImGuiInputTextFlags_CharsNoBlank);
				if (ImGui::Button("Search For File##Model")) {
					FileSelector(".fbx", &fModel, "Model");
				}
				ImGui::EndMenu();
			}
			//TODO: create basic square flag
			ImGui::EndMenu();
		}
	}
	bool RemoveModel(int i) {
		ImGui::Text("Remove");
		ImGui::SameLine();
		if (ImGui::Button(("-##ModelRemove" + sPad(i)).c_str())) {
			ModelStore.erase(ModelStore.begin() + i);
			return false; //deleted
		}
		return true; // did notdelete
	}
	void ShowModel() {
		for (int i = 0; i < ModelStore.size(); i++) {
			ImGui::Separator();
			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(ModelStore[i]->Name.c_str());
			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();
			if (!RemoveModel(i)) {
				continue;
				i -= 1;
			}
			if(true) {
				ImGui::Text("Path:"); ImGui::SameLine(); 
				ImGui::Text(ModelStore[i]->Path.c_str());
			}
		}
	}
	void DrawMODEL() {
		if (ImGui::CollapsingHeader("Models:", NULL))
		{

			AddModel();
			ShowModel();
			//Options: import animation
			//Options: Load As R/W Buffer Under Name: ____

		}
		ImGui::Separator();
	}

	std::string ToAddConstantMenuName = "";
	int32_t ToAddConstantInt = 0;
	uint32_t ToAddConstantUint = 0;
	float ToAddConstantFloat = 0.0f;
	void AddConstantInt(std::string* s, int32_t* intV, int i) {
		ConstantStore[i]->AddInt(s, intV);
	}
	void AddConstantUint(std::string* s, uint32_t* uintV, int i) {
		ConstantStore[i]->AddUint(s, uintV);
	}
	void AddConstantFloat(std::string* s,float* floatV, int i) {
		ConstantStore[i]->AddFloat(s, floatV);
	}

	void AddConstantStruct(std::string s) {
		ConstantStore.push_back(new BuiltConstant_c(s));
	}
	std::string StructCreateName = "";
	void AddConstant() {
		if (ImGui::BeginMenu("Add ##Const")) {
			if (ImGui::BeginMenu("Create Struct##CreatConstantMenu")) {
				
				ImGui::InputText("Create Struct##ConstantMaker", &StructCreateName, ImGuiInputTextFlags_CharsNoBlank);

				if(ImGui::Button("Create##CreatConstant")) AddConstantStruct(StructCreateName);

				ImGui::EndMenu();
			}
			//TODO: create basic square flag
			ImGui::EndMenu();
		}
	}
	bool RemoveConstant(int i) {
		ImGui::Text("Remove");
		ImGui::SameLine();
		if (ImGui::Button(("-##ImageRemove" + sPad(i)).c_str())) {
			ConstantStore.erase(ConstantStore.begin() + i);
			return false; //deleted
		}
		return true; // did notdelete
	}
	void ShowIntData(int i) {
		for (int ii = 0; ii < ConstantStore[i]->vars.IT.size(); ii++) {
			ImGui::Text(("Name: " + ConstantStore[i]->vars.IT[ii].n).c_str());
			ImGui::Text("Value: ");
			ImGui::SameLine();
			ImGui::InputInt(("##ConstantVarInputInt" + ConstantStore[i]->vars.IT[ii].n).c_str(), &ConstantStore[i]->vars.IT[ii].val);
		}
	}
	void ShowUintData(int i) {
		for (int ii = 0; ii < ConstantStore[i]->vars.UT.size(); ii++) {
			ImGui::Text(("Name: " + ConstantStore[i]->vars.UT[ii].n).c_str());
			ImGui::Text("Value: ");
			ImGui::SameLine();
			ImGui::InputScalar(("##ConstantVarInputInt" + ConstantStore[i]->vars.UT[ii].n).c_str(), ImGuiDataType_U32, &ConstantStore[i]->vars.UT[ii].val);
		}
	}
	void ShowFloatData(int i) {
		for (int ii = 0; ii < ConstantStore[i]->vars.UT.size(); ii++) {
			ImGui::Text(("Name: " + ConstantStore[i]->vars.FT[ii].n).c_str());
			ImGui::Text("Value: ");
			ImGui::SameLine();
			ImGui::InputFloat(("##ConstantVarInputInt" + ConstantStore[i]->vars.FT[ii].n).c_str(), &ConstantStore[i]->vars.FT[ii].val);
		}
	}
	void ShowUintIntFloat(int i) {
		ShowIntData(i);
		ShowUintData(i);
		ShowFloatData(i);
		//TODO: show all name, uint, float, int and value plus allow value change
	}
	void ShowConstant() {
		for (int i = 0; i < ConstantStore.size(); i++) {
			ImGui::Separator();
			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(ConstantStore[i]->Name.c_str());
			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();

			if (ImGui::CollapsingHeader(("Constant's Info:##" + sPad(i)).c_str(), NULL)) { 
				ShowUintIntFloat(i);
			}

			if (!RemoveConstant(i)) {
				continue;
				i -= 1;
			}
			if (ImGui::BeginMenu(("Add Constant##CreateConstantMenu   " + sPad(i)).c_str())) {

				ImGui::InputText(("Name: ##ConstantCreateVar" +sPad(i)).c_str(), &ToAddConstantMenuName, ImGuiInputTextFlags_CharsNoBlank);
				ImGui::InputInt(("Def. Int: ##ConstantCreateVar" + sPad(i)).c_str(), &ToAddConstantInt);
				ImGui::InputScalar(("Def. Uint: ##ConstantCreateVar" + sPad(i)).c_str(), ImGuiDataType_U32, &ToAddConstantUint);
				ImGui::InputFloat(("Def. Float: ##ConstantCreateVar" + sPad(i)).c_str(), &ToAddConstantFloat);
				
				ImGui::SameLine();
				if (ImGui::Button(("Add Int##IntConstantModelStore" + sPad(i)).c_str())) {
					AddConstantInt(&ToAddConstantMenuName, &ToAddConstantInt, i);
				}
				

				ImGui::SameLine();
				if (ImGui::Button(("Add Uint##UintConstantModelStore" + sPad(i)).c_str())) {
					AddConstantUint(&ToAddConstantMenuName, &ToAddConstantUint, i);
				}
				
				ImGui::SameLine();
				if (ImGui::Button(("Add Float##ConstantModelStore" + sPad(i)).c_str())) {
					AddConstantFloat(&ToAddConstantMenuName, &ToAddConstantFloat, i);
				}

				ImGui::EndMenu();
			}
		}
		//TODO allow modification of defaults here, and name
	}
	void DrawCONSTANTS() {
		if (ImGui::CollapsingHeader("Constants:", NULL))
		{
			AddConstant();
			ShowConstant();
		}
		ImGui::Separator();
	}

	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "File Manager:")) {

			DrawBuildAllObjects();
			ImGui::Separator();
			ImGui::Separator();
			CheckImageFileSelectors();
			ImGui::Separator(); ImGui::Separator();
			DrawPREDEFINED();
			ImGui::Separator(); ImGui::Separator();
			DrawIMAGE();
			ImGui::Separator(); ImGui::Separator();
			DrawMODEL();
			ImGui::Separator(); ImGui::Separator();
			DrawCONSTANTS();
			ImGui::Separator(); ImGui::Separator();
			




			ImGui::End();
		}
	}

}MASTER_FileManager_m;