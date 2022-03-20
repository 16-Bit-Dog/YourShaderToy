#pragma once
#include "string"
#include "VertexShaderPipeline.h"
#include "PixelShaderPipeline.h"

//TODO: fix interact with name and create from inside pipeline state new pipeline
//TODO: make toggle work

struct PipelineObj {
	bool On = true;
	bool ComputeOnlyToggle = false;

	VertexShaderPipeline Vertex;

	PixelShaderPipeline Pixel;

	//TODO: set the toggle
	std::string name;
	int Order = -1;
	bool killP = false;
	std::string padN = " ";
	PipelineObj(int i = -1) {
		name = std::to_string(i) + " Pipeline";
		Order = i;
	}
};
