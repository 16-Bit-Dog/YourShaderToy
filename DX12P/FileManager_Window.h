#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Main_Window.h"
#include <algorithm>
#include <set>
//TODO: delete from set in deconstructor the name usage, also add to set

#define MAX_IMAGE_NAME 10

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

void DupNameHandle(std::set<std::string>* usedName, std::string* Name) {
	if (usedName->count(*Name) != 0) {
		for (int i = 0; i < Name->size(); i++) {
			if (usedName->count(*Name + "_" + std::to_string(i)) == 0) *Name += "_" + std::to_string(i); //don't allow dup names
		}
	}
}

std::set<std::string> usedName_Image;
struct BuiltImage_c {
	//I read data immedeatly since it is simple, fast, and I want to make a TODO: preview option of image once loaded
	
	std::string Path = "";
	std::string Name = "";
	
	int bpp;
	int sizeX;
	int sizeY;
	int channels;
	bool built = false;//toggle to state if file was loaded into renderer
	bool ReadWrite = false;

	uint8_t* data8 = nullptr; //only 1 is non null
	uint16_t* data16 = nullptr; 
	uint32_t* data32 = nullptr;
	uint64_t* data64 = nullptr;


	BuiltImage_c(std::string s, bool IsPath, int sizeX_tmp, int sizeY_tmp, int channels_tmp, int bpp_tmp, d4 data /* maybe just use a byte */) {
		//set name
		Path = s;
		bpp = bpp_tmp;
		
		if (s == "") s = "TMP_TEX";
		int size = ((MAX_IMAGE_NAME < s.size()) ?  MAX_IMAGE_NAME : s.size());
		
		for (int i = 0; i < size; i++) Name[i] = s[i];
		
		DupNameHandle(&usedName_Image, &Name);
		usedName_Image.insert(Name);

		sizeX = sizeX_tmp;
		sizeY = sizeY_tmp;
		channels = channels_tmp;

		if (bpp_tmp == 8) {
			data8 = data.data8;
		}
		else if (bpp_tmp == 16) {
			data16 = data.data16;
		}
		else if (bpp_tmp == 32) {
			data32 = data.data32;
		}
		else if (bpp_tmp == 64) {
			data64 = data.data64;
		}

	}
	~BuiltImage_c() {
		usedName_Image.erase(Name);
		if (data8 != nullptr) delete[] data8;
		if (data16 != nullptr) delete[] data16;
		if (data32 != nullptr) delete[] data32;
		if (data64 != nullptr) delete[] data64;
	}



};


std::set<std::string> usedName_Model;
std::set<std::string> usedNameRW_Model;
struct BuiltModel_c {

	std::string Path = "";
	std::string Name = ""; //same as Buffer <-- when loaded 
	std::string NameRW = ""; //same as UAV which is already made in 3DDXObj.h - when I fetch, 

	bool built = false;//toggle to state if file was loaded into renderer

	M3DR* ModelDat = nullptr;

	BuiltModel_c(std::string s) {
		//TODO fill M3DR based on path, else stop trying to load and throw error saying "invalid" 
		Path = s;

		if (s == "") s = "TMP_MODEL";
		int size = ((MAX_IMAGE_NAME < s.size()) ? MAX_IMAGE_NAME : s.size());

		for (int i = 0; i < size; i++) Name[i] = s[i];
		NameRW = Name + "_RW";

		DupNameHandle(&usedName_Model, &Name);
		DupNameHandle(&usedNameRW_Model, &NameRW);
		usedName_Model.insert(Name);
		usedName_Model.insert(NameRW);

	}

	~BuiltModel_c() {

		usedName_Model.erase(Name);
		usedNameRW_Model.erase(NameRW);
		
		if(ModelDat!=nullptr) delete[] ModelDat;
	}

};

struct BuiltConstant_c {
	std::string Name = "";

	bool built = false;//toggle to state if file was loaded into renderer

	bool ReadWrite = false;
};

struct MASTER_FileManager : MASTER_Function_Inherit {

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

	void DrawPREDEFINED() {
		if (ImGui::CollapsingHeader("Predefined:", NULL))
		{


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

	void AddImageToList(std::string Name, bool IsPath, int sizeX, int sizeY, int channels, int bpp, d4 data) {
		ImageStore.push_back(new BuiltImage_c(Name, IsPath, sizeX, sizeY, channels, bpp, data));
		//TODO add image with string name, make new object, and make the show-er for it
	}

	void AddImage() {
		if (ImGui::BeginMenu("Add")) {
			if (ImGui::BeginMenu("Find Image")) {
				if (ImGui::Button("Search For File")) {
					FileSelector(".png,.jpeg,.jpg", &fImage, "Image");
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Custom Image")) {
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
					AddImageToList(RGBA_nameOfString, false, RGBA_sizeX, RGBA_sizeY, 4, RGBA_bbp, d4(RGBA_sizeX, RGBA_sizeY, RGBA_bbp));
				}
				
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
	}

	void CheckImageFileSelectors() { //these are seperate from the model one and constant one since I may want special behavior like pre-and post filter
		if (fImage.Display("ChooseFileDlgKey"))
		{
			if (fImage.IsOk() == true)
			{
				int width, height, channels;
				std::string path = fImage.GetFilePathName();
				uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 4); //force assume 4 channels (TODO: check if this force assumes 4 and forces padding to 4)
				AddImageToList(path, true, width, height, channels, 8, d4(data));
				// action
			}
			// close
			fImage.Close();
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
				ImGui::SameLine();
				ImGui::SameLine();
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

	}
	void DrawMODEL() {
		if (ImGui::CollapsingHeader("Models:", NULL))
		{
			//Options: import animation
			//Options: Load As R/W Buffer Under Name: ____

		}
		ImGui::Separator();
	}

	void AddConstant() {

	}
	void DrawCONSTANTS() {
		if (ImGui::CollapsingHeader("Constants:", NULL))
		{

		}
		ImGui::Separator();
	}

	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "File Manager:")) {

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