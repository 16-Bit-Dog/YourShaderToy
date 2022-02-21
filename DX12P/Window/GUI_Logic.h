#pragma once

#ifdef _WIN32
#define IM_NEWLINE  "\r\n"
#else
#define IM_NEWLINE  "\n"
#endif

#include <vector>
#include <string>
#include <functional>
#include <map>
#include <cstdint>
#include "RenderableManager.h"
#include "Pipeline_Window.h"
#include "Scene_Window.h"
#include "Settings_Window.h"
#include "Editor_Window.h"
#include "FileManager_Window.h"

struct GroupData;

	void GroupData::LinkToSettings() {
		WindowType = W_SETTING;

		ToDraw = [this]() { MASTER_Setting_m.BasicViewDraw(this); };
	}
	void GroupData::LinkToScene() {
		WindowType = W_SCENE;

		ToDraw = [this]() { MASTER_Scene_m.BasicViewDraw(this); };
	}
	void GroupData::LinkToEditor() {
		WindowType = W_EDITOR;

		ToDraw = [this]() { MASTER_Editor_m.BasicViewDraw(this); };
	}
	void GroupData::LinkToPipeline() {
		WindowType = W_PIPELINE;

		ToDraw = [this]() { MASTER_Pipeline_m.BasicViewDraw(this); };
	}
	void GroupData::LinkToFileManager() {
		WindowType = W_FILEMANAGER;

		ToDraw = [this]() { MASTER_Pipeline_m.BasicViewDraw(this); };
	}
	int GroupData::LinkBasedOnInt(int Input) {
		if (Win_Type_ID_Vector.count(Input)) {
			WindowType = Input;
			ToDraw = [this,Input]() {
				Win_Type_Initialization_Vector_Of_Type[Input](this); 
			};
			return 1;
		}
		throw("Link Based On Int Fail");
		return 0; //won't return since this is a FATAL flaw if it does not work
	}

	GroupData* GroupData::MakeNewMainWindowCheckAndDo(int WindowType = 1) {
		//enter window type to check if needs to be made - this is individual
		if (Win_Type_ID_Vector.count(WindowType)) {

			if (NewWindowCreationHandle[WindowType] == true) {
				
				GroupData* TmpGUI_Win = new GroupData(WindowTopHandle);
				TmpGUI_Win->ID = GLOBAL_WINDOW_ID_I();
				TmpGUI_Win->LinkBasedOnInt(WindowType);

				NewWindowCreationHandle[WindowType] = false;

				return TmpGUI_Win;
			}

			return nullptr;
		}

		return nullptr;
	}

	void StartUpFillVecs() {
		Add_New_Win_Type([](GroupData* GDV) { MASTER_Setting_m.BasicViewDraw(GDV); }, "Settings");
		Add_New_Win_Type([](GroupData* GDV) { MASTER_Scene_m.BasicViewDraw(GDV); }, "Scene");
		Add_New_Win_Type([](GroupData* GDV) { MASTER_Editor_m.BasicViewDraw(GDV); }, "Editor");
		Add_New_Win_Type([&](GroupData* GDV) { MASTER_Pipeline_m.BasicViewDraw(GDV); }, "Pipeline"); 
		Add_New_Win_Type([&](GroupData* GDV) { MASTER_FileManager_m.BasicViewDraw(GDV); }, "File Manager");
	}
