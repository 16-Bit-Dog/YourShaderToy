#pragma once
#include "string"
#include "imgui.h"
#include "imgui_stdlib.h"
//TODO: click compile compiles all code and order of data pass


struct BasePipeline {
	/*
	base abstract class to inherit by all pipelines for handling generic data
	*/
	//base piepline global handler
	inline static uint64_t globalCont; // global pipeline counter to iterate id of pipeline

	uint64_t control = 0; //the id of pipeline
	
	bool On = true; // is pipeline used ?
	std::string name = ""; //name of piepline

	std::string ShaderTypeName = ""; //shader type name

	std::string ErrorMessage_s = ""; //err of pipeline compile

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
