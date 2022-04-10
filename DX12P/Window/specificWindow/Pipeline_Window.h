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
		if (Renderable::DXM->CompiledData == false) ImGui::TextColored({ 1.0f, 0.1f, 0.1f, 1.0f }, "WARNING - FILE-MANAGER DATA NOT COMPILED");
		
		bool button = ImGui::Button("Pipeline & Code Compile##StartCodeCompile");

		if (Renderable::DXM->BufferReset || button || Renderable::DXM->AutoCodeCompile && Renderable::DXM->AutoCodeCompile_Wait < GLFW_Window_C::DeltaOfLastPress_CompileReset) {
			Renderable::DXM->BufferReset = false;

			GLFW_Window_C::DeltaOfLastPress_CompileReset = -2.0f;

			if (Renderable::DXM->AutoFileManagerCompile && Renderable::DXM->CompiledData == false) MASTER_FileManager::obj->BuildAllObjectsItem();

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
		if (ImGui::Button(("-##Remove This Pipeline" + PipelineMain::obj->P[i]->Spacing()).c_str())) {
			PipelineMain::obj->P[i]->killP = true;
		}
	}

	void DrawPipelineOrder(const int& i) {
		if (ImGui::BeginMenu(("<Hover To Swap>##" + PipelineMain::obj->P[i]->Spacing()).c_str())) {
			for (const auto& x : PipelineMain::obj->P) {
				if (ImGui::Button((x.second->name + " ").c_str())) {
					MapTools::Swap(x.first, i, PipelineMain::obj->P);
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

		ImGui::InputText(("##" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->name);

		if (PipelineMain::obj->P[i]->name == "") PipelineMain::obj->P[i]->name = " ";
	}


	void PostAddPipelines() {
		for (int i = 0; i < PipelineAddQueue.size(); i++)
			PipelineAddQueue[i]();
		PipelineAddQueue.clear();
	}

	void DrawPipelineComputeOnlyToggle(const int& i) {
		ImGui::Checkbox(("Compute Only##bool" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->ComputeOnlyToggle);
		ImGui::Checkbox(("Turn Compute Off##bool" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->TurnComputeOffToggle);
	}

	void DrawPipelineIfTrueComputeOnlyToggle(const int& i) {
		//TODO: draw only compute shader stuff alone
	}

	void DrawSelectModel(const int& i) {
		ImGui::Text("     "); ImGui::SameLine();
		if (ImGui::BeginMenu(("Select Model:##" + PipelineMain::obj->P[i]->Spacing()).c_str())) {
			for (const auto& x : MASTER_FileManager::obj->ModelStore) {
				if (ImGui::Button((x->Name + "##" + PipelineMain::obj->P[i]->Spacing()).c_str())) {
					Renderable::DXM->ROB->SetDataToPipelineVertex(x, PipelineMain::obj->P[i]->Vertex);
				}
			}
			ImGui::EndMenu();
		}
		ImGui::Text(("Loaded Model:" + PipelineMain::obj->P[i]->Vertex.LoadedModelName).c_str());
	}
	void DrawFaceToRenderSelect(const int& i) {
		ImGui::Checkbox(("Use Fill" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.RasterToMake.ToFill);
		ImGui::Checkbox(("AA Line" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.RasterToMake.AAL);
		if (ImGui::BeginMenu(("Select Face To Render:##" + PipelineMain::obj->P[i]->Spacing()).c_str())) {
			bool AllFace = false;
			bool BackFace = false;
			bool FrontFace = false; //.cull means to cull out
			if (PipelineMain::obj->P[i]->Vertex.RasterToMake.cull == 1) { ImGui::Bullet(); ImGui::Text(""); }
			else AllFace = ImGui::Button(("Both Faces##Select Button" + PipelineMain::obj->P[i]->Spacing()).c_str());
			if (AllFace) {
				PipelineMain::obj->P[i]->Vertex.RasterToMake.cull = 1;
			}

			if (PipelineMain::obj->P[i]->Vertex.RasterToMake.cull == 2) { ImGui::Bullet(); ImGui::Text(""); }
			else BackFace = ImGui::Button(("Back Faces##Select Button" + PipelineMain::obj->P[i]->Spacing()).c_str());
			if (BackFace) {
				PipelineMain::obj->P[i]->Vertex.RasterToMake.cull = 2;
			}

			if (PipelineMain::obj->P[i]->Vertex.RasterToMake.cull == 3) { ImGui::Bullet(); ImGui::Text("");}
			else FrontFace = ImGui::Button(("Front Faces##Select Button C" + PipelineMain::obj->P[i]->Spacing()).c_str());
			if (FrontFace) {
				PipelineMain::obj->P[i]->Vertex.RasterToMake.cull = 3;
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
		ImGui::SameLine(); ImGui::HelpMarker("0 is don't write, 255 is write all"
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

	void BlendFactorControlInput(std::string name, float* Comp) {
		ImGui::InputFloat(name.c_str(), Comp);
		ImGui::SameLine(); ImGui::HelpMarker("1.0f = blend color fully\n0.0f = don't blend"
		);
		if (*Comp < 0.0f) {
			(*Comp) = 0.0f;
		}
		else if (*Comp > 1.0f) {
			(*Comp) = 1.0f;
		}
	}

	//TODO: maybe template... maybe not worth it for readability tho...

	void BlendControlInput(std::string name, int* Comp) {
		ImGui::InputInt(name.c_str(), Comp);
		ImGui::SameLine(); ImGui::HelpMarker("Interpretation of Blend Factor:"
			"\n1 = Blend Factor (BF) is set to (0,0,0,0)"
			"\n2 = BF is set to (1,1,1,1)"
			"\n3 = BF is Pixel Shader (PS) RGBA Output Color"
			"\n4 = BF is Inversed PS RGBA Output Color"
			"\n5 = Alpha of PS Result is BF of RGBA"
			"\n6 = Inversed Alpha of PS Result is BF of RGBA"
			"\n7 = Alpha of Current Render Target (RT) is BF of RGBA"
			"\n8 = Inversed Alpha of Current RT is BF of RGBA"
			"\n9 = RGBA data of Current RT is BF of RGBA"
			"\n10 = Inversed RGBA data of Current RT is BF of RGBA"
			"\n11 = Alpha BF is 1.0f, RGB BF is min of Inversed-Alpha/Alpha"
			"\n14 = Use Blend Factor Set by User"
			"\n15 = Use Inverse Blend Factor Set by User"
			"\n16 = "
			"\n17 = "
			"\n18 = "
			"\n19 = "
		);  
		if (*Comp < 1) {
			(*Comp) = 1;
		}
		if (*Comp > 11 && *Comp < 14) {
			(*Comp) = 11;
		}
		else if (*Comp > 19) {
			(*Comp) = 19;
		}
	}
	void BlendOpControlInput(std::string name, int* Comp) {
		ImGui::InputInt(name.c_str(), Comp);
		ImGui::SameLine(); ImGui::HelpMarker("Blend Op of Pixels:\n"
			"\n1 = Add Source 1 and Source 2"
			"\n2 = Subtract Source 1 from Source 2"
			"\n3 = Subtract Source 2 from Source 1"
			"\n4 = Use Minimum of Source 1 and 2"
			"\n5 = Use Maximum of Source 1 and 2"
		);
		if (*Comp < 1) {
			(*Comp) = 1;
		}
		else if (*Comp > 5) {
			(*Comp) = 5;
		}
	}

	void DrawBlendSelect(const int& i) {

		ImGui::Checkbox(("Enable Blend" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.BlendToMake.BlendEnable);
		if (PipelineMain::obj->P[i]->Vertex.BlendToMake.BlendEnable) {


			ImGui::Checkbox(("Use Color Blend" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.BlendToMake.IndependentBlendEnable);
			ImGui::SameLine(); ImGui::HelpMarker("Compare resultant depth & \nenable writing to depth buffer");

			ImGui::Checkbox(("Use Alpha Blend" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.BlendToMake.AlphaToCoverageEnable);
			ImGui::SameLine(); ImGui::HelpMarker("Compare resultant fragments");

			ImGui::Text("     "); ImGui::SameLine(); ImGui::HelpMarker("Only Blend Control 14-15 use these values"); ImGui::SameLine();
			if (ImGui::BeginMenu(("Set Blend Factor:##" + PipelineMain::obj->P[i]->Spacing()).c_str())) {

				BlendFactorControlInput(("Red Blend" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendFactor[0]);
				BlendFactorControlInput(("Green Blend" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendFactor[1]);
				BlendFactorControlInput(("Blue Blend" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendFactor[2]);
				BlendFactorControlInput(("Alpha Blend" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendFactor[3]);
				ImGui::EndMenu();
			}

			ImGui::Text("     "); ImGui::SameLine();
			if (ImGui::BeginMenu(("Select Blend Specifics:##" + PipelineMain::obj->P[i]->Spacing()).c_str())) {

				BlendControlInput(("[SrcBlend] How PS Blends RGB" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendToMake.SrcBlend);
				BlendControlInput(("[DestBlend] How RTV Blends" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendToMake.DestBlend);
				BlendOpControlInput(("How To Blend SrcBlend and DestBlend" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendToMake.BlendOp);
				ImGui::Text("");
				BlendControlInput(("[SrcBlendAlpha] How PS Blends Alpha" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendToMake.SrcBlendAlpha);
				BlendControlInput(("[DestBlendAlpha] How RTV Blends Alpha" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendToMake.DestBlendAlpha);
				BlendOpControlInput(("How To Blend SrcBlendAlpha and DestBlendAlpha" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendToMake.BlendOpAlpha);
				ImGui::Text("");
				MaskForStencilUINT8(("Render Target Write Mask: " + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.BlendToMake.RenderTargetWriteMask);

				ImGui::EndMenu();
			}

		}


	}

	void DrawCompFunctionSelect(const int& i) {
		ImGui::Checkbox(("Use Depth Test" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.StencilToMake.EnableDepth);
		ImGui::SameLine(); ImGui::HelpMarker("Compare resultant depth & \nenable writing to depth buffer");

		ImGui::Checkbox(("Use Stencil Test" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.StencilToMake.EnableStencil);
		ImGui::SameLine(); ImGui::HelpMarker("Compare resultant fragments");

		ImGui::Text("     "); ImGui::SameLine();
		if (ImGui::BeginMenu(("Select Depth-Stencil Specifics:##" + PipelineMain::obj->P[i]->Spacing()).c_str())) {
			
			ImGui::Checkbox(("Depth Write Mask"+PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->Vertex.StencilToMake.DepthWriteMask);
			ImGui::SameLine(); ImGui::HelpMarker("no check = don't write to depth buffer\ncheck = write to depth buffer");
			CompFuncControlInt(("Depth Comparison" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.DepthComp); // use this func to control tri fragment test
			
			MaskForStencilUINT8(("Stencil Read Mask" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.WhereToReadFromStencil);
			MaskForStencilUINT8(("Stencil Write Mask" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.WhereToWriteToStencil);

			PassFailOpStencil(("FRONT: If Tri Fails The Comp. Pass" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontFaceStencilFailOp);
			PassFailOpStencil(("FRONT: If Tri passes Comp. - Depth Fails" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontFaceStencilDepthFailOp);
			PassFailOpStencil(("FRONT: If Tri Passes - Depth Passes" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontFaceStencilPassOp);
			CompFuncControlInt(("FRONT: Tri Comparison" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.FrontTriComp); // use this func to control tri fragment test
			
			PassFailOpStencil(("BACK: If Tri Fails The Comp. Pass" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackFaceStencilFailOp);
			PassFailOpStencil(("BACK: If Tri passes Comp. - Depth Fails" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackFaceStencilDepthFailOp);
			PassFailOpStencil(("BACK: If Tri Passes - Depth Passes" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackFaceStencilPassOp);
			CompFuncControlInt(("BACK: Tri Comparison" + PipelineMain::obj->P[i]->Spacing()), &PipelineMain::obj->P[i]->Vertex.StencilToMake.BackTriComp); // use this func to control tri fragment test
			
			
			ImGui::EndMenu();
		}
	}
	
	void DrawPipelineIfFalseComputeOnlyToggle(const int& i) {
		PipelineMain::obj->P[i]->Vertex.Input();

		DrawSelectModel(i);
		ImGui::TextColored({ 0.1f,1.0f,0.1f,1.0f }, "Rasterizer Settings:");
		DrawFaceToRenderSelect(i);
		ImGui::TextColored({ 0.1f,1.0f,0.1f,1.0f }, "Depth-Stencil:");
		DrawCompFunctionSelect(i);
		ImGui::TextColored({ 0.1f,1.0f,0.1f,1.0f }, "Blend:");
		DrawBlendSelect(i);
		ImGui::Spacing();
		PipelineMain::obj->P[i]->Pixel.Input();

		PipelineMain::obj->P[i]->DrawCompute();


	}

	void DrawPipelineOnToggle(const int& i) {
		ImGui::Checkbox(("Use Pipeline##bool" + PipelineMain::obj->P[i]->Spacing()).c_str(), &PipelineMain::obj->P[i]->On);
	}

	void DrawRTV(PipelineObj* i) {
		i->CheckIfRTVExistsAndRebind(RTV_DEPTH::RTV);
		ImGui::Text(("RTV Bound: "+ RTV_DEPTH::RTV[i->RTV_Selected]->name).c_str());
		if (ImGui::BeginMenu(("Change RTV Bound For Stage: ##Change RTV for PObj"+i->Spacing()).c_str())) {
			for (auto& x : RTV_DEPTH::RTV) {
				if (ImGui::Button((x.second->name+"##ButtonToChangeRTVBound"+i->Spacing()+x.second->Spacing()).c_str())) {
					i->RTV_Selected = x.first;
				}
			}
			ImGui::EndMenu();
		}
	}
	void DrawDEPTH(PipelineObj* i) {
		i->CheckIfDEPTHExistsAndRebind(RTV_DEPTH::DEPTH);
		ImGui::Text(("DEPTH Bound: " + RTV_DEPTH::DEPTH[i->DEPTH_Selected]->name).c_str());
		if (ImGui::BeginMenu(("Change DEPTH Bound For Stage: ##Change DEPTH for PObj" + i->Spacing()).c_str())) {
			for (auto& x : RTV_DEPTH::DEPTH) {
				if (ImGui::Button((x.second->name + "##ButtonToChangeDEPTHBound" + i->Spacing() + x.second->Spacing()).c_str())) {
					i->DEPTH_Selected = x.first;
				}
			}
			ImGui::EndMenu();
		}
	}
	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "Pipeline:")) {

			DrawPipelineDiagram();
			ImGui::NewLine();
			
			/*if (PipelineMain::obj->P.count(0) == 0)*/
			DrawPipelineAdd(0);
			
			ImGui::Separator();
			

			
			for (const auto& i : PipelineMain::obj->P) {
				ImGui::Text((i.second->name).c_str());
				ImGui::SameLine();
				if (ImGui::CollapsingHeader(("##"+i.second->Spacing()).c_str(), NULL))
				{
					DrawRTV(i.second);
					ImGui::NewLine();
					DrawDEPTH(i.second);
					ImGui::NewLine();
					DrawPipelineAdd(i.first, i.second->Spacing()+ "B");
					ImGui::NewLine();
					DrawPipelineSub(i.first);
					DrawPipelineOrder(i.first);
					DrawPipelineName(i.first);
					DrawPipelineOnToggle(i.first);
					DrawPipelineComputeOnlyToggle(i.first);
					if(PipelineMain::obj->P[i.first]->ComputeOnlyToggle) DrawPipelineIfTrueComputeOnlyToggle(i.first);
					else DrawPipelineIfFalseComputeOnlyToggle(i.first);
					ImGui::NewLine();
					DrawPipelineAdd(i.first+1, i.second->Spacing() + "A", "Add Pipeline After");
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
