#pragma once

#include "Main_Window.h"

struct MASTER_Editor : MASTER_Function_Inherit {
	inline static MASTER_Editor* obj;

	//global strings 
	std::vector<std::string> AutoAddGlobalsRTV;
	std::vector<std::string> AutoAddGlobalsDEPTH;

	std::vector<std::string> AutoAddGlobalsPredefined; 
	std::vector<std::string> AutoAddGlobalsImages;
	std::vector<std::string> AutoAddGlobalsModels;
	std::vector<std::string> AutoAddGlobalsConstants;

	std::string Globals = std::string(
		"float4 YPRToQuat(in float4 ypr){\n"
		"//radians of yaw pitch roll to quat. NOT angle\n"
		"return float4(sin(ypr.z / 2.0) * cos(ypr.y / 2.0) * cos(ypr.x / 2.0) - cos(ypr.z / 2.0) * sin(ypr.y / 2.0) * sin(ypr.x / 2.0),\n"
		"cos(ypr.z / 2.0) * sin(ypr.y / 2.0) * cos(ypr.x / 2) + sin(ypr.z / 2.0) * cos(ypr.y / 2.0) * sin(ypr.x / 2.0),\n"
		"cos(ypr.z / 2.0) * cos(ypr.y / 2.0) * sin(ypr.x / 2) - sin(ypr.z / 2.0) * sin(ypr.y / 2.0) * cos(ypr.x / 2.0),\n"
		"cos(ypr.z / 2.0) * cos(ypr.y / 2.0) * cos(ypr.x / 2) + sin(ypr.z / 2.0) * sin(ypr.y / 2.0) * sin(ypr.x / 2.0));\n"
		"}\n"
		"float4 QuatRotateAroundPoint(in float4 pos, in float4 quat){\n"
		"float3 t = 2.0 * cross(quat.xyz,pos.xyz);\n"
		"return float4(pos.xyz + quat.w * t+cross(quat.xyz,t) ,1);\n"
		"}\n"
		"float4 GetTransQuatScale(in float4 pos, in float4 trans, in float4 scale, in float4 quat){\n"
		"return QuatRotateAroundPoint(pos,quat)*scale+trans;\n"
		"}\n"
		"float4 ApplyTransQuatScale(in matrix mat, in float4 pos, in float4 trans, in float4 scale, in float4 quat){\n"
		"return mul(mat, GetTransQuatScale(pos,trans,scale,quat));\n"
		"}\n"
		"void SetNullToIdentityMatrix(inout matrix mat){\n"
		"mat[0][0] = 1;\n"
		"mat[1][1] = 1;\n"
		"mat[2][2] = 1;\n"
		"mat[3][3] = 1;\n"
		"}\n"

	);

	std::string VsString = std::string(
		
		

		"VertexOut SimpleVS(Vertex IN){\n"
		"VertexOut OUT;\n"
		"matrix mvp = mul(ProjectionMatrix, ViewMatrix);" //todo, maybe add specifier for world mat, since this is just world rot
		//"float2 track = float2((WINDOW_SIZE_X/2-MOUSE_POS_X)/90,(WINDOW_SIZE_Y/2-MOUSE_POS_Y)/90);"
		"float4 t = float4((WINDOW_SIZE_X-MOUSE_POS_X)*3/WINDOW_SIZE_X,(WINDOW_SIZE_Y-MOUSE_POS_Y)*3/WINDOW_SIZE_Y,20,0);\n" 
		"float4 s = float4(1,1,1,1);\n"
		"float4 q = float4(sin(DELTA_LAST_KEY)*3.14,0,0,0);\n"
		"OUT.position = ApplyTransQuatScale(mvp, float4(IN.position, 1), t, s, YPRToQuat(q));\n"
		"OUT.color = float4(IN.uv.x,IN.uv.y,(IN.uv.x+IN.uv.y)/2.0f,1.0f);\n"
		"OUT.PositionWS = float4(IN.position,1.0f);\n"
		"OUT.uv = IN.uv;\n"
		"return OUT;\n"
		"}\n"

	);
	std::string PsString = std::string(

		"float4 SimplePS(VertexOut IN) : SV_TARGET{\n"
		"float4 Color;\n"
		"Color.r = IN.color.x;\n"
		"Color.g = IN.color.y;\n"
		"Color.b = IN.color.z;\n"
		"Color.w = IN.color.a;\n"
		"return Color;\n"
		"}\n"

	);
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
		//get default global string with other pre defined globals,
		//DONT get user added globals
		std::string s = std::string( //defaults
			"////Globals\n"
			"//Auto_Added_Globals\n"
			"struct Vertex{\n"
			"	float3 position : POSITION;\n"
			"	float3 normal : NORMAL;\n"
			"	float3 binormal : BINORMAL;\n"
			"	float3 tangent : TANGENT;\n"
			"	float2 uv : TEXCOORD;\n"
			"	int4 bID : BLENDID;\n"
			"	float4 bW : BLENDWEIGHT;\n"
			"};\n"
		
			"struct VertexOut{\n"
			"	float4 position : SV_POSITION;\n"
			"	float4 color: COLOR;\n"
			"	float2 uv : TEXCOORD0;\n"
			"	float4 PositionWS : TEXCOORD1;\n"
			"};\n"

			"cbuffer DefaultWorldMatrixS : register(b0){\n"
			"	Matrix DefaultWorldMatrix;\n"
			"};\n"
			"cbuffer DefaultViewMatrixS : register(b1){\n"
			"	Matrix DefaultViewMatrix;\n"
			"};\n"
			"cbuffer DefaultProjectionMatrixS : register(b2){\n"
			"	Matrix DefaultProjectionMatrix;\n"
			"};\n"
			"cbuffer WorldMatrixS : register(b3){\n"
			"	Matrix WorldMatrix;\n"
			"};\n"
			"cbuffer ViewMatrixS : register(b4){\n"
			"	Matrix ViewMatrix;\n"
			"};\n"
			"cbuffer ProjectionMatrixS : register(b5){\n"
			"	Matrix ProjectionMatrix;\n"
			"};\n"
			
			
		);

		for (const auto& i : AutoAddGlobalsRTV) {
			s += i;
		}
		for (const auto& i : AutoAddGlobalsDEPTH) {
			s += i;
		}
		for (const auto& i : AutoAddGlobalsPredefined) {
			s += i;
		}
		for (const auto& i : AutoAddGlobalsImages) {
			s += i;
		}
		for (const auto& i : AutoAddGlobalsConstants) {
			s += i;
		}


		return std::move(s);

	}

	std::string GetStringWithGlobalsText() {

		std::string s = "";
		s += GetStringWithNoGlobals();
		s += Globals;
		return std::move(s);

	}

	void DrawGlobalsText() {
		//draw Imgui globals text
		std::string s = GetStringWithNoGlobals();
		ImGui::Text(s.c_str());
		ImGui::Text("//");
		ImGui::InputTextMultilineQuick("T0", &Globals, &TextType);
		ImGui::Text("////");
	}

	void DrawVertexShaderText() {
		if (ImGui::CollapsingHeader("EditorVertexCollapse")) {
			ImGui::Text("//VertexShaders"); //add - hint: use 'Vertex' as your input structure type. Auto_Added_Globals shows the variables features for ALL vertex buffers loaded
			HelpMarker("Input to Vertex Shader\nMUST be 'Vertex'\n\n");
			ImGui::InputTextMultilineQuick("T1", &VsString, &TextType);
		}
	}

	void DrawPixelShaderText() {
		if (ImGui::CollapsingHeader("EditorPixelCollapse")) {
			ImGui::Text("//PixelShaders");
			ImGui::InputTextMultilineQuick("T2", &PsString, &TextType);
		}
	}

	virtual void BasicViewDraw(GroupData* GD) {
		if (DrawBasicWindow(this, GD, "Editor:")) {
			/*
				//Draw menu of text inputs

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
