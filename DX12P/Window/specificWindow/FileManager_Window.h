#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Main_Window.h"
#include <algorithm>
#include <set>
#include "FileManagerResourceStruct.h"
#include "3dCommons/StaticObjects.h"
//TODO: delete from set in deconstructor the name usage, also add to set
//TODO: make global name check for conflict struct, to make sure name is valid
//TODO: add rw toggle for vars


struct MASTER_FileManager : MASTER_Function_Inherit {
	inline static MASTER_FileManager* obj;

	bool BuiltPredefined = false; //TODO: built objects is seperate option than built code
	//TODO: build code BUTTON and built object BUTTON, compile code done only after build object is done - auto does this.

	ImGuiFileDialog fImage;
	ImGuiFileDialog fModel;
	ImGuiFileDialog fConstant; //to read a XML or something I guess?


	BuiltPredefined_c* PredefinedStore = new BuiltPredefined_c();
	std::vector<BuiltImage_c*> ImageStore;
	std::vector<BuiltModel_c*> ModelStore;
	std::vector<BuiltConstant_c*> ConstantStore;

	void AddNewRTV() {
		Renderable::DXM->CompiledData = false;
		RTV_DEPTH::RTV[RenderTarget_s::GetNextID()] = (new RenderTarget_s());
	}
	void AddNewDEPTH() {
		Renderable::DXM->CompiledData = false;
		RTV_DEPTH::DEPTH[DepthTarget_s::GetNextID()] = (new DepthTarget_s());
	}

	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI::obj->WindowDrawFlagBuilder(
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
	void UpdateDefaults() {
		PredefinedStore->UpdateItem();
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
		Renderable::DXM->ROB->ClearAllObjects();
	}
	void FillEditorTextRTV() {
		Renderable::DXM->ROB->AddItemTextRTV(&MASTER_Editor::obj->AutoAddGlobalsRTV);
	}
	void FillEditorTextDEPTH() {
		Renderable::DXM->ROB->AddItemTextDEPTH(&MASTER_Editor::obj->AutoAddGlobalsDEPTH);
	}
	void FillEditorTextDefaults() {
		Renderable::DXM->ROB->AddItemTextDefault(&MASTER_Editor::obj->AutoAddGlobalsPredefined);
	}
	void FillEditorTextImages() {
		Renderable::DXM->ROB->AddItemTextImages(&MASTER_Editor::obj->AutoAddGlobalsImages);
	}
	void FillEditorTextModels() {
		Renderable::DXM->ROB->AddItemTextModels(&MASTER_Editor::obj->AutoAddGlobalsModels);
	}
	void FillEditorTextConstants() {
		Renderable::DXM->ROB->AddItemTextConstants(&MASTER_Editor::obj->AutoAddGlobalsConstants);
	}

	void FillEditor() {
		MASTER_Editor::obj->AutoAddGlobalsPredefined.clear();
		MASTER_Editor::obj->AutoAddGlobalsImages.clear();
		MASTER_Editor::obj->AutoAddGlobalsModels.clear();
		MASTER_Editor::obj->AutoAddGlobalsConstants.clear();
		MASTER_Editor::obj->AutoAddGlobalsRTV.clear();
		MASTER_Editor::obj->AutoAddGlobalsDEPTH.clear();

		FillEditorTextRTV();
		FillEditorTextDEPTH();
		FillEditorTextDefaults();
		FillEditorTextImages();
		FillEditorTextModels();
		FillEditorTextConstants();
	}
	void BuildAllObjectsItem() {
		CleanAllBuild();
//		BuildAllDefaults();
		BuildAllImages();
		BuildAllModels();
		BuildAllConstants();
		FillEditor();
		Renderable::DXM->CompiledData = true;
		Renderable::DXM->ROB->PreBindAllResources();
		//iterate through all items, run "clear mem" - THEN - run object builder from path
	}//TODO
	void DrawBuildAllObjects() {
		//TODO: button draw for BUILD OBJECTS
		if(ImGui::Button("COMPILE ALL##main resource compile button")) BuildAllObjectsItem();
	}

	void DrawPREDEFINED() {
		if (ImGui::CollapsingHeaderOpenGreen("Predefined:"))
		{
			ImGui::Indent();
			ImGui::Text("Variables:");
			ImGui::Text("WINDOW_SIZE_X"); ImGui::SameLine(); ImGui::HelpMarker("Main Window Width");
			ImGui::Text("WINDOW_SIZE_Y"); ImGui::SameLine(); ImGui::HelpMarker("Main Window Height");
			ImGui::Text("MOUSE_POS_X"); ImGui::SameLine(); ImGui::HelpMarker("Mouse Position X"); 
			ImGui::Text("MOUSE_POS_Y"); ImGui::SameLine(); ImGui::HelpMarker("Mouse Position Y");

			ImGui::Text("LEFT_CLICK_STATE"); ImGui::SameLine(); ImGui::HelpMarker("Left Mouse click: \npress = 1\nclosed = 0");
			ImGui::Text("RIGHT_CLICK_STATE"); ImGui::SameLine(); ImGui::HelpMarker("Right Mouse click: \npress = 1\nclosed = 0"); 
			ImGui::Text("MIDDLE_CLICK_STATE"); ImGui::SameLine(); ImGui::HelpMarker("Middle Mouse click: \npress = 1\nclosed = 0");
			ImGui::Text("NET_TIME"); ImGui::SameLine(); ImGui::HelpMarker("Time since window creation");
			ImGui::Unindent();
		}
		ImGui::Separator();
	}

	void FileSelector(const std::string& FileTypeToFind /*.exe, .png, .jpeg ## ect*/, ImGuiFileDialog* fd, const std::string& name) {
		fd->OpenDialog("ChooseFileDlgKey", ("Choose " + name), &FileTypeToFind[0], ".");
	}

	//Add custom vars
	std::string RGBA_nameOfString = "";
	int RGBA_sizeX = 1;
	int RGBA_sizeY = 1;
	int RGBA_bbp = 8; //rgba mandatory - 8 bpp, 16 bpp, 32 bpp, 64 bpp
	

	//TODO, RW toggle after creation

	void AddImageToList(const std::string& Path, const std::string& Name, const bool& IsPath, const int& sizeX, const int& sizeY, const int& channels, const int& bpp, const bool& UNORM_ELSE_FLOAT, const bool& LinkSizeToRTV, const NoiseDat_Obj& ndo, d4* data) {
		ImageStore.push_back(new BuiltImage_c(Path, Name, IsPath, sizeX, sizeY, channels, bpp, UNORM_ELSE_FLOAT, LinkSizeToRTV, ndo, data));
		Renderable::DXM->CompiledData = false;
		//TODO add image with string name, make new object, and make the show-er for it
	}
	void AddModelToList(const std::string& Path, const std::string& Name, const int& Type) {
		ModelStore.push_back(new BuiltModel_c(Path, Name, Type));
		Renderable::DXM->CompiledData = false;
	}

	void DrawNoiseMiniPreview() {
		ObjectBuilder::MakeAndSetNoiseWithParamForPreview(std::floor(GLFW_Window_C::time));
	}

	void AddImage() {
		if (ImGui::BeginMenuGreen("Add##Image")) {
			if (ImGui::BeginMenuGreen("Find Image##Image")) {
				ImGui::InputText("Name: ##AddImageN", &ToAddImageName, ImGuiInputTextFlags_CharsNoBlank);
				if (ImGui::Button("Search For File##Image")) {
					FileSelector(".png,.jpeg,.jpg", &fImage, "Image");
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenuGreen("Custom Image ##Image")) {
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
				ImGui::Checkbox("Link texture size to RTV ##image texture setting selector", &ObjectBuilder::LinkSizeToRTV); ImGui::SameLine(); ImGui::HelpMarker("auto resize texture to RTV size when window size is adjusted");
				
				ImGui::Checkbox("Fill with Noise (opens up preview and menu to customize)", &ObjectBuilder::NoiseDat.UseNoise);
				if (ObjectBuilder::NoiseDat.UseNoise) {
					ObjectBuilder::DrawAndHandleMinAndMaxSize();

					DrawNoiseMiniPreview();

					RGBA_bbp = 8;
				}

				if (ImGui::Button("Create ## RGBA Add image")) {
					d4 idat(RGBA_sizeX, RGBA_sizeY, RGBA_bbp, 4);
					AddImageToList(RGBA_nameOfString, RGBA_nameOfString, false, RGBA_sizeX, RGBA_sizeY, 4, RGBA_bbp, ObjectBuilder::UNORM_ELSE_FLOAT_Driver, ObjectBuilder::LinkSizeToRTV, ObjectBuilder::NoiseDat, &idat);
				}
				
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
	}
	std::string ToAddImageName = "";//TODO: organize code and loose vars
	std::string ToAddModelName = "";//TODO: organize code and loose vars
	int ToAddModelType = -1;
	void CheckImageFileSelectors() { //these are seperate from the model one and constant one since I may want special behavior like pre-and post filter
		if (fImage.Display("ChooseFileDlgKey"))
		{
			if (fImage.IsOk() == true)
			{
				int width, height, channels;
				std::string path = fImage.GetFilePathName();
				uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 4); //force assume 4 channels (TODO: check if this force assumes 4 and forces padding to 4)
				d4 idat(data, width, height, 4);
				AddImageToList(path, ToAddImageName, true, width, height, channels, 8, true, false, NoiseDat_Obj(), &idat);
				// action
			}
			// close
			fImage.Close();
		}
		if(fModel.Display("ChooseFileDlgKey"))
		{
			if (fModel.IsOk() == true)
			{
				AddModelToList(fModel.GetFilePathName(), ToAddModelName, -1);// action
			}
			// close
			fModel.Close();
		}

	}


	bool RemoveImage(const int& i) {
		ImGui::Text("Remove");
		ImGui::SameLine();
		if (ImGui::Button(("-##ImageRemove" + sPad(i)).c_str())) {
			ImageStore.erase(ImageStore.begin()+i);
			return false; //deleted
		}
		return true; // did notdelete
	}
	void ToggleReadWrite(const int& i) {
		ImGui::SameLine();
		ImGui::Checkbox(("R/W Toggle##ImageRemove" + sPad(i)).c_str(), &ImageStore[i]->ReadWrite);
	}
	void ShowImage() {
		for (int i = 0; i < ImageStore.size(); i++) {
			ImGui::Separator();
			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(ImageStore[i]->Name.c_str());
			ImGui::Text("RWName: ");
			ImGui::SameLine();
			ImGui::Text(ImageStore[i]->NameRW.c_str());
			ImGui::Text("Sampler: ");
			ImGui::SameLine();
			ImGui::Text(ImageStore[i]->SamplerName.c_str());
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
			
			ImGui::Indent();
			if (ImGui::CollapsingHeaderOpenGreen(("Image Info:##" + sPad(i)).c_str())) { //TODO test
				
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
				}
				//TODO: show info tab collapsing header
				
			}
			ImGui::Unindent();
			ImGui::Separator();
		}
	}

	void ShowRTV() {
		std::vector<uint64_t> ToRemove;

		
		for (auto& i : RTV_DEPTH::RTV) {
			ImGui::Text("Name Of RWTexture2d: "); ImGui::SameLine();
			ImGui::InputText(("##RTV name input" + i.second->Spacing()).c_str(), &i.second->name);
			
			ImGui::Checkbox(("Clear RTV Every New Pass##Toggle clear RTV" + i.second->Spacing()).c_str(), &i.second->ClearEveryNewPass);
			if (i.second->ClearEveryNewPassOld != i.second->ClearEveryNewPass) {
				Renderable::DXM->BufferReset = true;
			}
			i.second->ClearEveryNewPassOld = i.second->ClearEveryNewPass;

			if (ImGui::Button(("-##Remove RTV" + i.second->Spacing()).c_str())) {
				if (RTV_DEPTH::RTV.size() > 1) {
					ToRemove.push_back(i.first);
				}
			}
			ImGui::Separator();
		}

		for (const auto& i : ToRemove) {
			RTV_DEPTH::RTV.erase(i);
		}

	}
	void AddRTV() {
		if (ImGui::Button("+##Add new RTV")) {

			AddNewRTV();
		}
	}
	void DrawRTV() {
		if (ImGui::CollapsingHeaderOpenGreen("RTV:"))
		{
			ImGui::BeginChildBorderFixedSize("RTV:##Child");

			AddRTV();

			ShowRTV();

			ImGui::EndChild();

		}
		ImGui::Separator();
	}


	void ShowDEPTH() {
		std::vector<uint64_t> ToRemove;
		
		for (auto& i : RTV_DEPTH::DEPTH) {
			
			ImGui::Text("Name Of RWTexture2d: "); ImGui::SameLine();
			ImGui::InputText(("##DEPTH name input" + i.second->Spacing()).c_str(), &i.second->name);
			
			ImGui::Checkbox(("Clear Depth Every New Pass##Toggle clear DEPTH" + i.second->Spacing()).c_str(), &i.second->ClearEveryNewPass);
			if (i.second->ClearEveryNewPassOld != i.second->ClearEveryNewPass) {
				Renderable::DXM->BufferReset = true;
			}
			i.second->ClearEveryNewPassOld = i.second->ClearEveryNewPass;
			
			if (ImGui::Button(("-##Remove DEPTH" + i.second->Spacing()).c_str())) {
				if (RTV_DEPTH::DEPTH.size() > 1) {
					ToRemove.push_back(i.first);
				}
			}
			ImGui::Separator();
		}

		for (const auto& i : ToRemove) {
			RTV_DEPTH::DEPTH.erase(i);
		}

	}
	void AddDEPTH() {
		if (ImGui::Button("+##Add new DEPTH")) {
			AddNewDEPTH();
		}
	}
	void DrawDEPTH() {
		if (ImGui::CollapsingHeaderOpenGreen("DEPTH:"))
		{
			ImGui::BeginChildBorderFixedSize("Depth:##Child");

			AddDEPTH();

			ShowDEPTH();

			ImGui::EndChild();
		}
		ImGui::Separator();
	}
	void DrawIMAGE() {
		
		

		if (ImGui::CollapsingHeaderOpenGreen("Images:"))
		{
			ImGui::BeginChildBorderFixedSize("Images:##Child");

			AddImage();
			//TODO: draw ImageStore and allow info expansion to show dimentions, and such
			ShowImage();


			ImGui::EndChild();
		}
		ImGui::Separator();
	}

	void AddModel() {
		if (ImGui::BeginMenuGreen("Add ##Model")) {
			if (ImGui::BeginMenuGreen("Find .FBX Model##Model")) {
				ImGui::InputText("Name: ##AddModelName", &ToAddModelName, ImGuiInputTextFlags_CharsNoBlank);
				ImGui::InputInt("Name: ##AddModelName", &ToAddModelType);
				/*
				StaticObjectPass
				*/
				bool addFR = ImGui::Button("Add File##Model");
				ImGui::SameLine();
				ImGui::HelpMarker(ToAddStaticObjectString.c_str());

				if (addFR) {
					if (ToAddModelType<0 || ToAddModelType>StaticObjectPass.size()-1) {
						FileSelector(".fbx", &fModel, "Model");
					}
					else {
						AddModelToList("", ToAddModelName, ToAddModelType);// action
					}
				}
				ImGui::EndMenu();
			}
			//TODO: create basic square flag
			ImGui::EndMenu();
		}
	}
	bool RemoveModel(const int& i) {
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
			ImGui::Text("RWName: ");
			ImGui::SameLine();
			ImGui::Text(ModelStore[i]->NameRW.c_str());
			ImGui::SameLine();
			ImGui::Text("|");
			ImGui::SameLine();
			if (!RemoveModel(i)) {
				continue;
				i -= 1;
			}
			if(true) {

				if (ModelStore[i]->Type == -1) {
					ImGui::Text("Path:"); ImGui::SameLine();
					ImGui::Text(ModelStore[i]->Path.c_str());
				}
				else {
					ImGui::Text("Static Object:"); ImGui::SameLine();
					ImGui::Text(std::get<0>(StaticObjectPass[ModelStore[i]->Type]).c_str());
				}
			}
		}
	}
	void DrawMODEL() {
		if (ImGui::CollapsingHeaderOpenGreen("Models:"))
		{
			ImGui::BeginChildBorderFixedSize("Models:##Child");

			AddModel();
			ShowModel();
			//Options: import animation
			//Options: Load As R/W Buffer Under Name: ____

			ImGui::EndChild();
		}
		ImGui::Separator();
	}

	std::string ToAddConstantMenuName = "";
	int32_t ToAddConstantInt = 0;
	uint32_t ToAddConstantUint = 0;
	float ToAddConstantFloat = 0.0f;
	void AddConstantInt(std::string* s, int32_t& intV, const int& i) {
		ConstantStore[i]->AddInt(s, intV);
		Renderable::DXM->CompiledData = false;
	}
	void AddConstantUint(std::string* s, uint32_t& uintV, const int& i) {
		ConstantStore[i]->AddUint(s, uintV);
		Renderable::DXM->CompiledData = false;
	}
	void AddConstantFloat(std::string* s,float& floatV, const int& i) {
		ConstantStore[i]->AddFloat(s, floatV);
		Renderable::DXM->CompiledData = false;
	}
/*	void AddConstantMatrix(std::string* s, float* matV[4][4], const int& i) {
		ConstantStore[i]->AddMatrix(s, matV);
	}*/

	

	void AddConstantStruct(std::string s) {
		ConstantStore.push_back(new BuiltConstant_c(s));
	}
	std::string StructCreateName = "";
	void AddConstant() {
		if (ImGui::BeginMenuGreen("Add ##Const")) {
			if (ImGui::BeginMenuGreen("Create Struct##CreatConstantMenu")) {
				
				ImGui::InputText("Create Struct##ConstantMaker", &StructCreateName, ImGuiInputTextFlags_CharsNoBlank);

				if(ImGui::Button("Create##CreatConstant")) AddConstantStruct(StructCreateName);

				ImGui::EndMenu();
			}
			//TODO: create basic square flag
			ImGui::EndMenu();
		}
	}
	bool RemoveConstant(const int& i) {
		ImGui::Text("Remove");
		ImGui::SameLine();
		if (ImGui::Button(("-##ImageRemove" + sPad(i)).c_str())) {
			ConstantStore.erase(ConstantStore.begin() + i);
			return false; //deleted
		}
		return true; // did notdelete
	}
	void ShowIntData(const int& i) {
		for (int ii = 0; ii < ConstantStore[i]->vars.IT.size(); ii++) {
			ImGui::Text("Name: "); ImGui::SameLine(); ImGui::Text(ConstantStore[i]->vars.IT[ii].n.c_str());
			ImGui::Text("RWName: "); ImGui::SameLine(); ImGui::Text(ConstantStore[i]->vars.IT[ii].nRW.c_str());
			ImGui::Text("Value: ");
			ImGui::SameLine();
			ImGui::InputInt(("##ConstantVarInputInt" + ConstantStore[i]->vars.IT[ii].n).c_str(), &ConstantStore[i]->vars.IT[ii].val);
		}
	}
	void ShowUintData(const int& i) {
		for (int ii = 0; ii < ConstantStore[i]->vars.UT.size(); ii++) {
			ImGui::Text("Name: "); ImGui::SameLine(); ImGui::Text(ConstantStore[i]->vars.UT[ii].n.c_str());
			ImGui::Text("RWName: "); ImGui::SameLine(); ImGui::Text(ConstantStore[i]->vars.UT[ii].nRW.c_str());
			ImGui::Text("Value: ");
			ImGui::SameLine();
			ImGui::InputScalar(("##ConstantVarInputInt" + ConstantStore[i]->vars.UT[ii].n).c_str(), ImGuiDataType_U32, &ConstantStore[i]->vars.UT[ii].val);
		}
	}
	void ShowFloatData(const int& i) {
		for (int ii = 0; ii < ConstantStore[i]->vars.FT.size(); ii++) {
			ImGui::Text("Name: "); ImGui::SameLine(); ImGui::Text(ConstantStore[i]->vars.FT[ii].n.c_str());
			ImGui::Text("RWName: "); ImGui::SameLine(); ImGui::Text(ConstantStore[i]->vars.FT[ii].nRW.c_str());
			ImGui::Text("Value: ");
			ImGui::SameLine();
			ImGui::InputFloat(("##ConstantVarInputInt" + ConstantStore[i]->vars.FT[ii].n).c_str(), &ConstantStore[i]->vars.FT[ii].val);
		}
	}
	void ShowUintIntFloat(const int& i) {
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
			ImGui::Text("RWName: ");
			ImGui::SameLine();
			ImGui::Text(ConstantStore[i]->NameRW.c_str());			
			ImGui::Text("RW_SRV Name: ");
			ImGui::SameLine();
			ImGui::Text(ConstantStore[i]->NameRW_SRV.c_str());

			ImGui::Text("Struct Name: ");
			ImGui::SameLine();
			ImGui::Text(ConstantStore[i]->StructName.c_str());
			
			if (ImGui::CollapsingHeaderOpenGreen(("Constant's Info:##" + sPad(i)).c_str())) {
				ImGui::Indent();
				ShowUintIntFloat(i);
				ImGui::Unindent();
			}

			if (!RemoveConstant(i)) {
				continue;
				i -= 1;
			}
			if (ImGui::BeginMenuGreen(("Add Constant##CreateConstantMenu   " + sPad(i)).c_str())) {

				ImGui::InputText(("Name: ##ConstantCreateVar" +sPad(i)).c_str(), &ToAddConstantMenuName, ImGuiInputTextFlags_CharsNoBlank);
				ImGui::InputInt(("Def. Int: ##ConstantCreateVar" + sPad(i)).c_str(), &ToAddConstantInt);
				ImGui::InputScalar(("Def. Uint: ##ConstantCreateVar" + sPad(i)).c_str(), ImGuiDataType_U32, &ToAddConstantUint);
				ImGui::InputFloat(("Def. Float: ##ConstantCreateVar" + sPad(i)).c_str(), &ToAddConstantFloat);
				
				ImGui::SameLine();
				if (ImGui::Button(("Add Int##IntConstantModelStore" + sPad(i)).c_str())) {
					AddConstantInt(&ToAddConstantMenuName, ToAddConstantInt, i);
				}

				ImGui::SameLine();
				if (ImGui::Button(("Add Uint##UintConstantModelStore" + sPad(i)).c_str())) {
					AddConstantUint(&ToAddConstantMenuName, ToAddConstantUint, i);
				}
				
				ImGui::SameLine();
				if (ImGui::Button(("Add Float##ConstantModelStore" + sPad(i)).c_str())) {
					AddConstantFloat(&ToAddConstantMenuName, ToAddConstantFloat, i);
				}

				ImGui::EndMenu();
			}
		}
		//TODO allow modification of defaults here, and name
	}
	void DrawCONSTANTS() {
		if (ImGui::CollapsingHeaderOpenGreen("Constants:"))
		{
			ImGui::BeginChildBorderFixedSize("Constants:##Child");

			AddConstant();
			ShowConstant();

			ImGui::EndChild();
		}
		ImGui::Separator();
	}

	void PredefinedUpdaterAuto() {
		if (BuiltPredefined == false) {
			BuildAllDefaults();
			BuiltPredefined = true;
		}
		else {
			UpdateDefaults();
		}
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
			DrawRTV();
			ImGui::Separator(); ImGui::Separator();
			DrawDEPTH();
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

};