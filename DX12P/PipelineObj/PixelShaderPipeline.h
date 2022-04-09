#pragma once
#include "string"
#include "PipelineBase.h"

struct PixelShaderPipeline : BasePipeline {
	//Abstract pipeline to repisent Pixel pipeline object and settings

	PixelShaderPipeline() {
		T_P = PIXEL;
		ShaderTypeName = "Pixel";
		this->name = name + "SimplePS";
	}

};
