#pragma once
#include "string"
#include "imgui.h"
#include "imgui_stdlib.h"
//TODO: click compile compiles all code and order of data pass


struct BasePipeline {
	inline static uint64_t globalCont;

	uint64_t control = 0;
	
	bool On = true;
	std::string name = "";

	std::string ShaderTypeName = "";

	std::string ErrorMessage_s = "";

	BasePipeline() {
		control = globalCont;
		globalCont += 1;
	}

	void ErrorMessage() {
		ImGui::Text("Error:");
		ImGui::SameLine();
		ImGui::Text(ErrorMessage_s.c_str());
	}
	
	void Input() {
		ImGui::Text((ShaderTypeName + " Function Name: ").c_str());
		ImGui::InputText(("##"+ ShaderTypeName+ std::string(control, ' ')).c_str(), &name, ImGuiInputTextFlags_CharsNoBlank);
		ErrorMessage();
	}

};
