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

	std::string code = "";
	std::vector<std::string> codeV;

	std::string ShaderTypeName = ""; //shader type name

	std::string ErrorMessage_s = ""; //err of pipeline compile

	bool CompileError = false;

	BasePipeline() {
		control = globalCont;
		globalCont += 1;
	}
	
	std::string Spacing() {
		return std::move(std::string(control, ' '));
	}

	std::string GetName() {
		return std::move(ShaderTypeName + this->Spacing());
	}
	void GetAndShowErrorMessage(){
		
		if (CompileError == false) {
			codeV.clear();
			int L = 0;
			for (int i = 1; i < code.size(); i++) { 
				if (code.substr(i - 1, 1) == "\n") {
					codeV.push_back(std::to_string(codeV.size()+1)+"| "+code.substr(L, i-L));
					L = i;
				}
			}
			codeV.push_back(std::to_string(codeV.size() + 1) + "| " + code.substr(L, code.size() - L));

			CompileError = true;

		}

		ImGui::TextColored({ 1.0f,0.1f,0.1f,1.0f }, ErrorMessage_s.c_str());

		if(ImGui::CollapsingHeader(("Code:##FunctionNameInputForShader" + ShaderTypeName + Spacing()).c_str(), NULL)) {
			for (const auto& i : codeV) {
				ImGui::Text(i.c_str());
			}
			
		}
	}

	void PrintError() {
		
		if (ImGui::CollapsingHeader(GetName().c_str(), NULL)) {

			GetAndShowErrorMessage();

		}

	}

	void Input() {
		ImGui::Text((ShaderTypeName + " Function Name: ").c_str());
		ImGui::InputText(("##"+ ShaderTypeName + Spacing()).c_str(), &name, ImGuiInputTextFlags_CharsNoBlank);
	}

};
