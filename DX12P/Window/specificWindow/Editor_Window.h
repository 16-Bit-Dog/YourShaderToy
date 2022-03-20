#pragma once

#include "Main_Window.h"

struct MASTER_Editor : MASTER_Function_Inherit {
	inline static MASTER_Editor* obj;

	std::vector<std::string> AutoAddGlobalsPredefined;
	std::vector<std::string> AutoAddGlobalsImages;
	std::vector<std::string> AutoAddGlobalsModels;
	std::vector<std::string> AutoAddGlobalsConstants;

	std::string Globals = "";
	std::string VsString = "";
	std::string PsString = "";
	std::string HsString = "";
	std::string DsString = "";
	std::string GsString = "";
	std::string CsString = "";
	std::string MeshsString = "";
	std::string AmpsString = "";
	std::string RayGensString = "";
	std::string MisssString = "";
	std::string HitsString = "";


	virtual void settingWindowSettingsMaker() {
		SettingWindowFlag = MASTER_IM_GUI::obj->WindowDrawFlagBuilder(
			false, false,
			false, false, false, false,
			false, false, false, false,
			false
		);
	}

	MASTER_Editor() {
		settingWindowSettingsMaker();
	}

	std::string GetStringWithNoGlobals() {

		std::string s = std::string(
			"////Globals\n"
			"//Auto_Added_Globals\n"
			"struct Vertex{\n"
			"float3 position : POSITION;\n"
			"float3 normal : NORMAL;\n"
			"float3 binormal : BINORMAL;\n"
			"float3 tangent : TANGENT;\n"
			"float3 uv : TEXCOORD;\n"
			"};\n"
		);

		for (auto& i : AutoAddGlobalsPredefined) {
			s += i;
		}
		for (auto& i : AutoAddGlobalsImages) {
			s += i;
		}
		for (auto& i : AutoAddGlobalsConstants) {
			s += i;
		}

		return std::move(s);

	}

	std::string GetStringWithGlobalsText() {


		std::string s = GetStringWithNoGlobals();
		s += Globals;
		return std::move(s);

	}

	void DrawGlobalsText() {

		std::string s = GetStringWithNoGlobals();
		ImGui::Text(s.c_str());
		ImGui::Text("//");
		ImGui::InputTextMultilineQuick("T0", &Globals, &TextType);
		ImGui::Text("////");
	}

	void DrawVertexShaderText() {

		ImGui::Text("//VertexShaders"); //add - hint: use 'Vertex' as your input structure type. Auto_Added_Globals shows the variables features for ALL vertex buffers loaded
		HelpMarker("Input to Vertex Shader\nMUST be 'Vertex'\n\n");
		ImGui::InputTextMultilineQuick("T1", &VsString, &TextType);

	}

	void DrawPixelShaderText() {

		ImGui::Text("//PixelShaders");
		ImGui::InputTextMultilineQuick("T2", &PsString, &TextType);

	}

	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "Editor:")) {
			/*
						Register Inputs: // hint - auto adds to AutoAddGlobals and Globals compiled code
						[Drop Down Menu]
						[[All existing Register inputs are added here]] - by default has a full screen quad here - every item has a drop down menu to change features about them
						{
						[+] //- Hint: click to add item, edit properties in existing input menu  //clicking one of these items adds them to Existing inputs - I put funky [] to show I need to add option to change in existing inputs
						-- [Vertex Buffer] Full screen quad [VariableName:___] -- hint: x = X pos;  y = Y pos; z =  Z pos;
						-- [Vertex Buffer] Load .obj Model [VariableName:___]
						[SEPERATOR]
						-- [float] Time  //hint - time in seconds, is a float
						-- [float2] Viewport_Resolution
						-- [float] TimeDelta
						-- [float2] MousePosition;
						-- [float] FrameCount;
						-- [float2] MousePosition;
						-- [float2] MouseButton; //x==1 -> LB down, y==2 -> RB down
						-- [texture2d] Depth_Texture  // links regiser to depth texture [starts black] - but you flag when it copies in pipeline setup
						-- [texture2d] Current_Output // links regiser to output texture [starts black] - but you flag when it copies in pipeline setup
						-- [texture2d<float4>] Load_Texture_From_Computer [VariableName:___]
						-- [RWtexture2d<float4>] Load_Texture_From_Computer [VariableName:___]
						-- [texture2d<float4>] Blank_Texture [VariableName:___:, SIZE: X ___ Y ___]


						}
			*/

			ImGui::Text("//HELP");
			ImGui::SameLine();
			HelpMarker("HELP:\n"
				"Hold SHIFT or use mouse to select text.\n"
				"CTRL+Left/Right to word jump.\n"
				"CTRL+A or double-click to select all.\n"
				"CTRL+X,CTRL+C,CTRL+V clipboard.\n"
				"CTRL+Z,CTRL+Y undo/redo.\n"
				"ESCAPE to revert.\n\n");

			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();

			DrawGlobalsText();

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("////Mandatory for output Shaders");
			
			DrawVertexShaderText();
			DrawPixelShaderText();

			ImGui::Text("////");

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("////Tesslation Shader content");
			ImGui::Text("//HullShaders - TODO");
			ImGui::InputTextMultilineQuick("##T3", &HsString, &TextType);
			ImGui::Text("//DomainShaders - TODO");
			ImGui::InputTextMultilineQuick("##T4", &DsString, &TextType);
			ImGui::Text("////");

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("////GeometryShader Content");
			ImGui::Text("//GeometryShaders - TODO");
			ImGui::InputTextMultilineQuick("##T5", &GsString, &TextType);
			ImGui::Text("////");

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("////ComputeShader Content - TODO");
			ImGui::Text("//ComputeShaders - TODO");
			ImGui::InputTextMultilineQuick("##T6", &CsString, &TextType);
			ImGui::Text("////");

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("////MeshShader Content - TODO");
			ImGui::Text("//MeshShaders - TODO");
			ImGui::InputTextMultilineQuick("T7", &MeshsString, &TextType);
			ImGui::Text("//AmplificationShaders - TODO");
			ImGui::InputTextMultilineQuick("##T8", &AmpsString, &TextType);
			ImGui::Text("////");

			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("////Ray-tracing Shader Content - TODO");
			ImGui::Text("//RayGen Shaders - TODO");
			ImGui::InputTextMultilineQuick("##T9", &RayGensString, &TextType);
			ImGui::Text("//Miss Shaders - TODO");
			ImGui::InputTextMultilineQuick("##T10", &MisssString, &TextType);
			ImGui::Text("//Hit Shaders - TODO");
			ImGui::InputTextMultilineQuick("##T11", &HitsString, &TextType);
			ImGui::Text("////");

			ImGui::Separator();
			ImGui::Separator();
			ImGui::Separator();

			ImGui::End();
		}
	}

};
