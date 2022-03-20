#pragma once
#include "string"
#include "imgui.h"
#include "imgui_stdlib.h"
//TODO: click compile compiles all code and order of data pass

struct BasePipeline {
	bool On = true;
	std::string name = "";

	std::string ShaderTypeName = "";

	std::string ErrorMessage_s = "";

	BasePipeline() {

	}

	void ErrorMessage() {
		ImGui::Text("Error:");
		ImGui::SameLine();
		ImGui::Text(ErrorMessage_s.c_str());
	}
	
	void Input() {
		ImGui::InputText((ShaderTypeName+" Function Name: ").c_str(), &name, ImGuiInputTextFlags_CharsNoBlank);
		ErrorMessage();
	}

};
