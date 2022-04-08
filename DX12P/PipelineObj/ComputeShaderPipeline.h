#pragma once
#include "string"
#include "PipelineBase.h"

struct ComputeShaderPipeline : BasePipeline {
	//Abstract pipeline to repisent Pixel pipeline object and settings

	std::string NameOfCompute;
	int Order = -1;

	ComputeShaderPipeline(int OrderN = 0) {
		Order = OrderN;

		NameOfCompute = "ComputeNew "+std::to_string(Spacing().size());
		ShaderTypeName = "Compute";
		this->name = name + "SimpleCS";
	}

	

	void DrawNameSwitch() {
		ImGui::Text("Set Compute Name: ");
		ImGui::InputText(("##ComputeNameAdjust" + ShaderTypeName + Spacing()).c_str(), &NameOfCompute, ImGuiInputTextFlags_CharsNoBlank);
		if (NameOfCompute == "") NameOfCompute = " ";
	}
};
