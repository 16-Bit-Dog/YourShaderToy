#pragma once
#include "string"
#include "PipelineBase.h"

struct ComputeShaderPipeline : BasePipeline {
	//Abstract pipeline to repisent Pixel pipeline object and settings

	std::string NameOfCompute;
	int Order = -1;

	uint32_t BlockSizeX;
	uint32_t BlockSizeY;
	uint32_t BlockSizeZ;

	ComputeShaderPipeline(int OrderN = 0) {
		T_P = COMPUTE;
		Order = OrderN;

		NameOfCompute = "ComputeNew "+std::to_string(Spacing().size());
		ShaderTypeName = "Compute";
		this->name = name + "SimpleCS";
	}

	
	void scalarUintHandle(const std::string ToolTipN, uint32_t& B) {
		ImGui::InputScalar((ToolTipN + Spacing()).c_str(), ImGuiDataType_U32, &B);
	}

	void DrawBlockInputs() {
		scalarUintHandle("Dim-X #inputDimXOfCompute", BlockSizeX);
		scalarUintHandle("Dim-Y #inputDimYOfCompute", BlockSizeY);
		scalarUintHandle("Dim-Z #inputDimZOfCompute", BlockSizeZ);
	}

	void DrawNameSwitch() {
		ImGui::Text("Set Compute Name: ");
		ImGui::InputText(("##ComputeNameAdjust" + ShaderTypeName + Spacing()).c_str(), &NameOfCompute, ImGuiInputTextFlags_CharsNoBlank);
		if (NameOfCompute == "") NameOfCompute = " ";
	}
};
