#pragma once
#include "Main_Window.h"
#include <functional>

struct MASTER_Pipeline : MASTER_Function_Inherit {
	std::vector<std::function<void()>> PipelineAddQueue;

	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI_obj.WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Pipeline() {
		settingWindowSettingsMaker();
	}

	void DrawPipelineDiagram() {
		/*
		START ->
			Pipeline_Name_That_Runs_1 ->
			Pipeline_Name_That_Runs_2 ->
			Pipeline_Name_That_Runs_3 ->
		END

		shows how program is running
		*/

		ImGui::Text("Start->");
		for (const auto& i : PipeM.P) {
			ImGui::Text((i.second->name + "->").c_str());
		}
		ImGui::Text("End");

	}



	void DrawPipelineAdd(int PosInsert, std::string s = "") {
		/*
		New Pipeline [+]  //button
		*/

		ImGui::Text("Add Pipeline");
		ImGui::SameLine();

		if (ImGui::Button(("+##"+s).c_str())) {
			PipelineAddQueue.push_back([=]() {
				PipeM.AddNewPipelineToPosition(PosInsert); }
			);
		}

	}
	void DrawPipelineSub(int i) {
		/*
		New Pipeline [-]  //button
		*/

		ImGui::Text("Remove Pipeline");
		ImGui::SameLine();
		if (ImGui::Button(("-##" + PipeM.P[i]->padN).c_str())) {
			PipeM.P[i]->killP = true;
		}
	}

	void DrawPipelineOrder(int i) {
		if (ImGui::BeginMenu(("<Hover To Swap>##" + PipeM.P[i]->padN).c_str())) {
			for (auto& x : PipeM.P) {
				if (ImGui::Button((x.second->name + " ").c_str())) {
					PipeM.Swap(x.first, i);
				}
			}
			ImGui::End();
		}
//		if (PipeM.P[i].Order) {
//			PipeM.Swap(PipeM.P[i].Order, PipeM.P[i].OrderO);
//		}
	}

	void DrawPipelineName(int i) {
		ImGui::Text("Name: ");
		ImGui::SameLine();

		ImGui::InputText(("##" + PipeM.P[i]->padN).c_str(), &PipeM.P[i]->name);

		if (PipeM.P[i]->name == "") PipeM.P[i]->name = " ";
	}
	std::string sPad(int num) {
		std::string n = " ";
		for (int i = 0; i < num; i++) n += " ";
		return n;
	}

	void PostAddPipelines() {
		for (int i = 0; i < PipelineAddQueue.size(); i++)
			PipelineAddQueue[i]();
		PipelineAddQueue.clear();
	}

	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "Pipeline:")) {

			DrawPipelineDiagram();
			ImGui::NewLine();
			
			if(PipeM.P.count(0)==0) DrawPipelineAdd(0);
			
			ImGui::Separator();
			

			
			for (auto& i : PipeM.P) {
				i.second->padN = sPad(i.second->Order);
				ImGui::Text((i.second->name).c_str());
				ImGui::SameLine();
				if (ImGui::CollapsingHeader(("##"+i.second->padN).c_str(), NULL))
				{

					DrawPipelineAdd(i.first, i.second->padN);
					DrawPipelineSub(i.first);
					DrawPipelineOrder(i.first);
					DrawPipelineName(i.first);
				
				}
				ImGui::Separator();
			}
			
			PostAddPipelines();
			PipeM.CheckToKillPipe();

			

			//Draw drop down menu looping around all pipeline state objects - drawing order var and such - use ordering of map to advantage here
			ImGui::Separator();
			


			//TODO: essentially it looks like this:
			/*
			* //draw this demo and update every frame
			START ->
			Pipeline_Name_That_Runs_1 ->
			Pipeline_Name_That_Runs_2 ->
			Pipeline_Name_That_Runs_3 ->
			END

			New Pipeline [+]  //button
			------------------------
			Drop down menu of All pipelines - click to open the item
			------------------------
			Delete This Pipeline [-] //button

			Order: [INT INPUT] //Hint, 0 is the first pipeline to run - if you put 2 zeros, it sets the other copies to 1 greater, and in order does this to all; shows current number

			Name: "What is shown in drop down pipeline menu as name" <-- allow changing

			Compute Shaders [Check Box To Enable]:
			[Drop Down Menu]
			[Choose CS] __NULL__, Order: __0__ //-hint, 0 is first
			[+] //click plus to add compute shader, works in order given, NULL means nothing runs, it is no shader


			[VS] [Check Box To Enable] //hint - Vertex Shader; if disabled only ComputeShader runs
			[Choose VS] _____ //Click on button to open drop down menu of all names of made vertex shaders from Editor. Click and then it puts the VS name on the ____ position
			[Choose Vertex Buffer] ___ //choose from list -- Full Screen Quad is default
			[Drop Down Menu]
			{

			}

			[HS] [Check Box To Enable] //hint - Hull Shader
			[Choose HS] _____
			[Drop Down Menu]
			{

			}

			[DS] [Check Box To Enable] //hint - Domain Shader
			[Choose DS] _____
			[Drop Down Menu]
			{

			}

			[GS] [Check Box To Enable] //hint - Geometry Shader
			[Choose GS] _____
			[Drop Down Menu]
			{

			}

			[PS] [Check Box To Enable] //hint - Pixel Shader
			[Choose PS] _____ //checks if input works with VS selected
			[Drop Down Menu]
			{
			[Copy_Depth_Texture_After]
			[Copy_Output_Texture_After]
			}

			*/



			ImGui::End();
		}
	}

}MASTER_Pipeline_m;
