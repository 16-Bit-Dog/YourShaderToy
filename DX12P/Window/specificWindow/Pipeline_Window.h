#pragma once
#include "Main_Window.h"
#include <functional>
#include "FileManager_Window.h"
//TODO: hit compile option to fetch textures, build objects, ect


/*
TODO: add default working vertex and pixel shader code

*/

struct MASTER_Pipeline : MASTER_Function_Inherit {
	inline static MASTER_Pipeline* obj;

	std::vector<std::function<void()>> PipelineAddQueue;

	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI::obj->WindowDrawFlagBuilder(
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

		if (ImGui::Button("Pipeline & Code Compile##StartCodeCompile")) {
			Renderable::DXM->ROB->CompileCodeLogic(PipelineMain::obj);
			Renderable::DXM->CompiledCode = true;
		}

		ImGui::Text("Start->");
		for (const auto& i : PipelineMain::obj->P) {
			ImGui::Text((i.second->name + "->").c_str());
		}
		ImGui::Text("End");

	}



	void DrawPipelineAdd(int PosInsert, std::string s = "", std::string Phrase = "Add Pipeline Before") {
		/*
		New Pipeline [+]  //button
		*/

		ImGui::Text(Phrase.c_str());
		ImGui::SameLine();

		if (ImGui::Button(("+##[NewPipeline]"+s).c_str())) {
			PipelineAddQueue.push_back([=]() {
				PipelineMain::obj->AddNewPipelineToPosition(PosInsert); }
			);
		}

	}
	void DrawPipelineSub(const int& i) {
		/*
		New Pipeline [-]  //button
		*/

		ImGui::Text("Remove This Pipeline");
		ImGui::SameLine();
		if (ImGui::Button(("-##Remove This Pipeline" + PipelineMain::obj->P[i]->padN).c_str())) {
			PipelineMain::obj->P[i]->killP = true;
		}
	}

	void DrawPipelineOrder(const int& i) {
		if (ImGui::BeginMenu(("<Hover To Swap>##" + PipelineMain::obj->P[i]->padN).c_str())) {
			for (auto& x : PipelineMain::obj->P) {
				if (ImGui::Button((x.second->name + " ").c_str())) {
					PipelineMain::obj->Swap(x.first, i);
				}
			}
			ImGui::End();
		}
//		if (PipeM.P[i].Order) {
//			PipeM.Swap(PipeM.P[i].Order, PipeM.P[i].OrderO);
//		}
	}

	void DrawPipelineName(const int& i) {
		ImGui::Text("Name: ");
		ImGui::SameLine();

		ImGui::InputText(("##" + PipelineMain::obj->P[i]->padN).c_str(), &PipelineMain::obj->P[i]->name);

		if (PipelineMain::obj->P[i]->name == "") PipelineMain::obj->P[i]->name = " ";
	}


	void PostAddPipelines() {
		for (int i = 0; i < PipelineAddQueue.size(); i++)
			PipelineAddQueue[i]();
		PipelineAddQueue.clear();
	}

	void DrawPipelineComputeOnlyToggle(const int& i) {
		ImGui::Checkbox(("Compute Only##bool" + PipelineMain::obj->P[i]->padN).c_str(), &PipelineMain::obj->P[i]->ComputeOnlyToggle);
	}

	void DrawPipelineIfTrueComputeOnlyToggle(const int& i) {
		//TODO: draw only compute shader stuff alone
	}

	void DrawSelectModel(const int& i) {
		ImGui::Text("     "); ImGui::SameLine();
		if (ImGui::BeginMenu(("Select Model:##" + PipelineMain::obj->P[i]->padN).c_str())) {
			for (auto& x : MASTER_FileManager::obj->ModelStore) {
				if (ImGui::Button((x->Name + "##" + PipelineMain::obj->P[i]->padN).c_str())) {
					Renderable::DXM->ROB->SetDataToPipelineVertex(x, PipelineMain::obj->P[i]->Vertex);
				}
			}
			ImGui::EndMenu();
		}
	}
	void DrawFaceToRenderSelect(const int& i) {
		ImGui::Text("     "); ImGui::SameLine();
		ImGui::Checkbox(("Use Wireframe" + PipelineMain::obj->P[i]->padN).c_str(), &PipelineMain::obj->P[i]->Vertex.Wireframe);
		if (ImGui::BeginMenu(("Select Face To Render:##" + PipelineMain::obj->P[i]->padN).c_str())) {
			bool AllFace = false;
			bool BackFace = false;
			bool FrontFace = false;
			if (PipelineMain::obj->P[i]->Vertex.FaceToRender == RASTER_TYPE::ALL_SOLID) { ImGui::Bullet(); ImGui::Text(""); }
			else AllFace = ImGui::Button(("Both Faces##Select Button" + PipelineMain::obj->P[i]->padN).c_str());
			if (AllFace) {
				Renderable::DXM->ROB->SetPipelineFaceRender(PipelineMain::obj->P[i]->Vertex, RASTER_TYPE::ALL_SOLID);
			}

			if (PipelineMain::obj->P[i]->Vertex.FaceToRender == RASTER_TYPE::BACK_SOLID) { ImGui::Bullet(); ImGui::Text(""); }
			else BackFace = ImGui::Button(("Back Faces##Select Button" + PipelineMain::obj->P[i]->padN).c_str());
			if (BackFace) {
				Renderable::DXM->ROB->SetPipelineFaceRender(PipelineMain::obj->P[i]->Vertex, RASTER_TYPE::BACK_SOLID);
			}

			if (PipelineMain::obj->P[i]->Vertex.FaceToRender == RASTER_TYPE::FRONT_SOLID) { ImGui::Bullet(); ImGui::Text("");}
			else FrontFace = ImGui::Button(("Front Faces##Select Button C" + PipelineMain::obj->P[i]->padN).c_str());
			if (FrontFace) {
				Renderable::DXM->ROB->SetPipelineFaceRender(PipelineMain::obj->P[i]->Vertex, RASTER_TYPE::FRONT_SOLID);
			}
			ImGui::EndMenu();
		}
	}

	void CompFuncControlInt(std::string name, int* Comp) {
		ImGui::InputInt(name.c_str(), Comp);
		ImGui::SameLine(); ImGui::HelpMarker("Function to pass new pixel data:"
			"\n1 = never pass"
			"\n2 = pass if less"
			"\n3 = pass if equal"
			"\n4 = pass if less/equal"
			"\n5 = pass if greater"
			"\n6 = pass if not-equal"
			"\n7 = pass if greater/equal"
			"\n8 = pass if always"
		);
		if (*Comp < 1) {
			(*Comp) = 1;
		}
		else if (*Comp > 8) {
			(*Comp) = 8;
		}
	}

	void MaskForStencilUINT8(std::string name, int* Comp) {
		ImGui::InputInt(name.c_str(), Comp);
		ImGui::SameLine(); ImGui::HelpMarker("Range to sample:"
			"\n1 = never pass"
			"\n2 = pass if less"
			"\n3 = pass if equal"
			"\n4 = pass if less/equal"
			"\n5 = pass if greater"
			"\n6 = pass if not-equal"
			"\n7 = pass if greater/equal"
			"\n8 = pass if always"
		);
		if (*Comp < 0) {
			(*Comp) = 0;
		}
		else if (*Comp > 255) {
			(*Comp) = 255;
		}
	}

	void PassFailOpStencil(std::string name, int* Comp) {
		ImGui::InputInt(name.c_str(), Comp);
		ImGui::SameLine(); ImGui::HelpMarker("What To Do With Data:"
			"\n1 = Don't Change Stencil Data"
			"\n2 = Set Stencil Data to 0"
			"\n3 = Set Stencil Data to Reference"
			"\n4 = Add 1 to Stencil Data - CLAMP value"
			"\n5 = Sub 1 to Stencil Data - CLAMP value"
			"\n6 = Invert Stencil Data"
			"\n7 = Add 1 to Stencil Data - WRAP value"
			"\n8 = Sub 1 to Stencil Data - WRAP value"
		);
		if (*Comp < 1) {
			(*Comp) = 1;
		}
		else if (*Comp > 8) {
			(*Comp) = 8;
		}
	}

	void DrawCompFunctionSelect(const int& i) {
		ImGui::Text("     "); ImGui::SameLine();
		ImGui::Checkbox(("Use Depth Test" + PipelineMain::obj->P[i]->padN).c_str(), &PipelineMain::obj->P[i]->Vertex.StencilToMake.EnableDepth);
		ImGui::SameLine(); ImGui::HelpMarker("Compare resultant depth & \nenable writing to depth buffer");

		ImGui::Text("     "); ImGui::SameLine();
		ImGui::Checkbox(("Use Stencil Test" + PipelineMain::obj->P[i]->padN).c_str(), &PipelineMain::obj->P[i]->Vertex.StencilToMake.EnableStencil);
		ImGui::SameLine(); ImGui::HelpMarker("Compare resultant fragments");

		ImGui::Text("     "); ImGui::SameLine();
		if (ImGui::BeginMenu(("Select Values:##" + PipelineMain::obj->P[i]->padN).c_str())) {
			
			ImGui::Checkbox(("Depth Write Mask"+PipelineMain::obj->P[i]->padN).c_str(), &PipelineMain::obj->P[i]->Vertex.StencilToMake.DepthWriteMask);
			ImGui::SameLine(); ImGui::HelpMarker("no check = don't write to depth buffer\ncheck = write to depth buffer");
			CompFuncControlInt(("Depth Comparison" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.DepthComp); // use this func to control tri fragment test
			
			MaskForStencilUINT8(("Stencil Read Mask" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.WhereToReadFromStencil);
			MaskForStencilUINT8(("Stencil Write Mask" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.WhereToWriteToStencil);

			PassFailOpStencil(("FRONT: If Tri Fails The Comp. Pass" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontFaceStencilFailOp);
			PassFailOpStencil(("FRONT: If Tri passes Comp. - Depth Fails" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontFaceStencilDepthFailOp);
			PassFailOpStencil(("FRONT: If Tri Passes - Depth Passes" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontFaceStencilPassOp);
			CompFuncControlInt(("FRONT: Tri Comparison" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontTriComp); // use this func to control tri fragment test
			
			PassFailOpStencil(("BACK: If Tri Fails The Comp. Pass" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackFaceStencilFailOp);
			PassFailOpStencil(("BACK: If Tri passes Comp. - Depth Fails" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackFaceStencilDepthFailOp);
			PassFailOpStencil(("BACK: If Tri Passes - Depth Passes" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackFaceStencilPassOp);
			CompFuncControlInt(("BACK: Tri Comparison" + PipelineMain::obj->P[i]->padN), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackTriComp); // use this func to control tri fragment test
			
			
			ImGui::EndMenu();
		}
	}
	
	void DrawPipelineIfFalseComputeOnlyToggle(const int& i) {
		PipelineMain::obj->P[i]->Vertex.Input();

		DrawSelectModel(i);
		DrawFaceToRenderSelect(i);
		DrawCompFunctionSelect(i);
		ImGui::Spacing();
		PipelineMain::obj->P[i]->Pixel.Input();

	}

	void DrawPipelineOnToggle(const int& i) {
		ImGui::Checkbox(("Use Pipeline##bool" + PipelineMain::obj->P[i]->padN).c_str(), &PipelineMain::obj->P[i]->On);
	}

	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "Pipeline:")) {

			DrawPipelineDiagram();
			ImGui::NewLine();
			
			/*if (PipelineMain::obj->P.count(0) == 0)*/
			DrawPipelineAdd(0);
			
			ImGui::Separator();
			

			
			for (auto& i : PipelineMain::obj->P) {
				i.second->padN = sPad(i.second->Order);
				ImGui::Text((i.second->name).c_str());
				ImGui::SameLine();
				if (ImGui::CollapsingHeader(("##"+i.second->padN).c_str(), NULL))
				{

					DrawPipelineAdd(i.first, i.second->padN+"B");
					ImGui::NewLine();
					DrawPipelineSub(i.first);
					DrawPipelineOrder(i.first);
					DrawPipelineName(i.first);
					DrawPipelineOnToggle(i.first);
					DrawPipelineComputeOnlyToggle(i.first);
					if(PipelineMain::obj->P[i.first]->ComputeOnlyToggle) DrawPipelineIfTrueComputeOnlyToggle(i.first);
					else DrawPipelineIfFalseComputeOnlyToggle(i.first);
					ImGui::NewLine();
					DrawPipelineAdd(i.first+1, i.second->padN+"A", "Add Pipeline After");
				}
				ImGui::Separator();
			}
			
			PostAddPipelines();
			PipelineMain::obj->CheckToKillPipe();

			

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

};
