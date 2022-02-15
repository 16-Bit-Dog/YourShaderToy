#pragma once
//
#include "Main_Window.h"

struct BuiltImage_c {
	std::string Path = "";
	std::string Name = "";
	
	int sizeX;
	int sizeY;

	bool built = false;//toggle to state if file was loaded into renderer

	bool ReadWrite = false;
};

struct BuiltModel_c {
	std::string Path = "";
	std::string Name = "";

	bool built = false;//toggle to state if file was loaded into renderer

	bool ReadWrite = false;
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

	std::vector<BuiltImage_c> ImageStore;
	std::vector<BuiltModel_c> ModelStore;
	std::vector<BuiltImage_c> ConstantStore;

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

	void AddImage() {
		if (ImGui::BeginMenu("Add")) {
			if (ImGui::BeginMenu("Find Image")) {
				if (ImGui::Button("Search For File")) {
					FileSelector(".png,.jpeg,.jpg", &fImage, "Image");
				}
			}
			if (ImGui::BeginMenu("Custom Image")) {
				//TODO:
			}
		}
	}
	void AddImageToList(std::string fPath) {
		//TODO add image with string name, make new object, and make the show-er for it
	}
	void CheckImageFileSelector() { //these are seperate from the model one and constant one since I may want special behavior like pre-and post filter
		if (fImage.Display("ChooseFileDlgKey"))
		{
			if (fImage.IsOk() == true)
			{
				AddImageToList(fImage.GetFilePathName());
				// action
			}
			// close
			fImage.Close();
		}
	}
	void DrawIMAGE() {
		if (ImGui::CollapsingHeader("Images:", NULL))
		{
			AddImage();
			//TODO: draw ImageStore and allow info expansion to show dimentions, and such

		}
		ImGui::Separator();
	}

	void AddModel() {

	}
	void DrawMODEL() {
		if (ImGui::CollapsingHeader("Models:", NULL))
		{


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
			DrawPREDEFINED();
			DrawIMAGE();
			DrawMODEL();
			DrawCONSTANTS();
			ImGui::Separator();

			ImGui::End();
		}
	}

}MASTER_FileManager_m;