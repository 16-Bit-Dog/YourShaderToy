#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Main_Window.h"
#include <algorithm>
#include <set>
#include "FileManagerResourceStruct.h"
//TODO: delete from set in deconstructor the name usage, also add to set
//TODO: make global name check for conflict struct, to make sure name is valid



struct MASTER_FileManager : MASTER_Function_Inherit {

	bool BuiltObjects = false; //TODO: built objects is seperate option than built code
	//TODO: build code BUTTON and built object BUTTON, compile code done only after build object is done - auto does this.

	ImGuiFileDialog fImage;
	ImGuiFileDialog fModel;
	ImGuiFileDialog fConstant; //to read a XML or something I guess?


	BuiltPredefined_c* PredefinedStore = new BuiltPredefined_c();
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

	void BuildAllDefaults() {
		PredefinedStore->BuildItem();
	}
	void BuildAllImages() {
		for (int i = 0; i < ImageStore.size(); i++) {
			ImageStore[i]->BuildItem();
		}
	}
	void BuildAllModels() {
		for (int i = 0; i < ModelStore.size(); i++) {
			ModelStore[i]->BuildItem();
		}
	}
	void BuildAllConstants() {
		for (int i = 0; i < ConstantStore.size(); i++) {
			ConstantStore[i]->BuildItem();
		}
	}
	void CleanAllBuild() {
		DXM->ROB->ClearAllObjects();
	}
	void BuildAllObjectsItem() {
		CleanAllBuild();
		BuildAllDefaults();
		BuildAllImages();
		BuildAllModels();
		BuildAllConstants();
		//iterate through all items, run "clear mem" - THEN - run object builder from path
	}//TODO
	void DrawBuildAllObjects() {
		//TODO: button draw for BUILD OBJECTS
		if(ImGui::Button("COMPILE ALL##main resource compile button")) BuildAllObjectsItem();
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

	void AddImageToList(std::string Path, std::string Name, bool IsPath, int sizeX, int sizeY, int channels, int bpp, bool UNORM_ELSE_FLOAT, d4* data) {
		ImageStore.push_back(new BuiltImage_c(Path, Name, IsPath, sizeX, sizeY, channels, bpp, UNORM_ELSE_FLOAT, data));
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
				if (RGBA_bbp > 32) RGBA_bbp = 32;
				else if (RGBA_bbp < 0) RGBA_bbp = 8;
				
				for (int i = 3; i < 7; i++) {
					if (RGBA_bbp > pow(2, i) && (RGBA_bbp % int(pow(2, i))) != 0) RGBA_bbp = int(pow(2, i));
				}

				ImGui::Checkbox("UNORM/FLOAT ##image texture setting selector", &ObjectBuilder::UNORM_ELSE_FLOAT_Driver); ImGui::SameLine(); ImGui::HelpMarker("checkmark = unorm texture -- else it is float");
				if (ImGui::Button("Create ## RGBA Add image")) {
					d4 idat(RGBA_sizeX, RGBA_sizeY, RGBA_bbp);
					AddImageToList(RGBA_nameOfString, RGBA_nameOfString, false, RGBA_sizeX, RGBA_sizeY, 4, RGBA_bbp, ObjectBuilder::UNORM_ELSE_FLOAT_Driver, &idat);
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
				uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 4); //force assume 4 channels (TODO: check if this force assumes 4 and forces padding to 4)
				d4 idat(data, width, height);
				AddImageToList(path, ToAddImageName, true, width, height, channels, 8, true, &idat);
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