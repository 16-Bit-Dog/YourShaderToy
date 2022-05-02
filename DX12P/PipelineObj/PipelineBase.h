#pragma once
#include "string"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "ImGuiHelper.h"
#include "functional"

//TODO: click compile compiles all code and order of data pass


struct BasePipeline {
	/*
	base abstract class to inherit by all pipelines for handling generic data
	*/
	//base piepline global handler

	inline static void Empty() {

	}

	inline static std::function<void()> StartPipelineCompileTimer = Empty;

	enum {
		VERTEX = 0,
		PIXEL = 1,
		COMPUTE = 2,
	};

	inline static uint64_t globalCont = 0; // global pipeline counter to iterate id of pipeline

	uint64_t control = 0; //the id of pipeline
	
	UINT T_P = 0;

	bool On = true; // is pipeline used ?
	std::string name = ""; //name of piepline

	inline static std::array<std::string,12> code;
	
	inline static std::array<std::vector<std::string>,12> codeV;

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

	inline static void GenCode(int TYPE) {
		BasePipeline::codeV[TYPE].clear();
		int L = 0;
		for (int i = 1; i < code[TYPE].size(); i++) {
			if (BasePipeline::code[TYPE].substr(i - 1, 1) == "\n") {
				BasePipeline::codeV[TYPE].push_back(std::to_string(codeV[TYPE].size() + 1) + "| " + BasePipeline::code[TYPE].substr(L, i - L));
				L = i;
			}
		}
		BasePipeline::codeV[TYPE].push_back(std::to_string(BasePipeline::codeV[TYPE].size() + 1) + "| " + BasePipeline::code[TYPE].substr(L, BasePipeline::code[TYPE].size() - L));
	}
	void GetAndShowErrorMessage(){
		
		ImGui::TextColored({ 1.0f,0.1f,0.1f,1.0f }, ErrorMessage_s.c_str());

		if(ImGui::CollapsingHeader(("Code:##FunctionNameInputForShader" + ShaderTypeName + Spacing()).c_str())) {
			ImGui::Indent();
			for (const auto& i : codeV[T_P]) {
				ImGui::Text(i.c_str());
			}
			ImGui::Unindent();
		}
	
	}

	bool tmpHeaderHandle = false;

	void PrintError() {
		
		if (ErrorMessage_s.size() > 2) {
			ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 0.1f, 0.1f, 1.0f });
		}

		ImGui::Indent();

		bool tmpHeaderHandle = ImGui::CollapsingHeader(GetName().c_str(), NULL); //TODO: indent these future items
		ImGui::Indent();
		if (ErrorMessage_s.size() > 2) {
			ImGui::PopStyleColor();
		}

		if (tmpHeaderHandle) {
			GetAndShowErrorMessage();
		}
		ImGui::Unindent();

		ImGui::Unindent();

	}

	void Input() {
		ImGui::Text((ShaderTypeName + " Function Name: ").c_str());
		ImGui::InputText(("##Name the Shader Function to run"+ ShaderTypeName + Spacing()).c_str(), &name, ImGuiInputTextFlags_CharsNoBlank);
	}

};
