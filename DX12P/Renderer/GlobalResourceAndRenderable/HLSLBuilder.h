#pragma once

#include <ResourceObjectBase.h>
#pragma region ResourceToHLSL

namespace HLSLBuilder {
	static void AddItemTextDefault(std::vector<std::string>* v, PredefinedToRenderer_Base* obj) {

		if (obj != NULL) {
			v->push_back("cbuffer " + obj->Name + ": register(b" + obj->CBName() + ") { \n");
			for (int i = 0; i < obj->typesInOrderName.size(); i++) {
				if (obj->typesInOrder[i] == INT_OBJ) {
					v->push_back("	int " + obj->typesInOrderName[i] + ";\n");
				}
				else if (obj->typesInOrder[i] == UINT_OBJ) {
					v->push_back("	uint " + obj->typesInOrderName[i] + ";\n");
				}
				else if (obj->typesInOrder[i] == FLOAT_OBJ) {
					v->push_back("	float " + obj->typesInOrderName[i] + ";\n");
				}
			}
			v->push_back("};\n");
		}
	}
	static void AddItemTextImages(std::vector<std::string>* v, ImageObjectToRenderer_Base* obj) {

			v->push_back("Texture2D " + obj->name + " : register(t" + obj->SRVName() + ");//bound in all non compute shaer items\n");
			if (obj->HasRW) v->push_back("RWTexture2D<" + obj->formatString + "> " + obj->nameRW + " : register(u" + obj->UAVName() + "); //bound in compute shader\n");
			v->push_back("sampler " + obj->samplerName + " : register(s" + obj->SamplerName() + "); \n");
	}
	static void AddItemTextRTV(std::vector<std::string>* v, RTVData_Base* obj) {
			v->push_back("RWTexture2D<unorm float4> " + obj->name + " : register(u" + obj->UAVName() + "); \n");
	}
	static void AddItemTextDEPTH(std::vector<std::string>* v, DEPTHData_Base* obj) {
			v->push_back("Texture2D<float> " + obj->name + " : register(t" + obj->SRVName() + "); \n");
	}

	static void AddItemTextModels(std::vector<std::string>* v, ModelToRenderer_Base* obj) {

		//nothig, since these are the VetexShaderInput -- set these as input using name in Pipeline Input - TODO:

	}

	static void AddItemTextConstants(std::vector<std::string>* v, StructObjectToRenderer_Base* obj) {

			v->push_back("struct " + obj->StructName + "{\n");
			for (int y = 0; y < obj->typesInOrder.size(); y++) {
				if (obj->typesInOrder[y] == INT_OBJ) {
					v->push_back("	int " + obj->typesInOrderName[y] + ";\n");
				}
				else if (obj->typesInOrder[y] == UINT_OBJ) {
					v->push_back("	uint " + obj->typesInOrderName[y] + ";\n");
				}
				else if (obj->typesInOrder[y] == FLOAT_OBJ) {
					v->push_back("	float " + obj->typesInOrderName[y] + ";\n");
				}
			}
			v->push_back("};\n");
			//

			//constant buf
			v->push_back("cbuffer " + obj->Name + ": register(b" + obj->CBName() + ") { \n");
			v->push_back(obj->StructName + " " + obj->StructElementName + ";\n");
			v->push_back("};\n");
			//

			//uav buf
			if (obj->HasRW) v->push_back("RWStructuredBuffer<" + obj->StructName + "> " + obj->NameRW + " : register(u" + obj->UAVName() + ");\n");
			v->push_back("StructuredBuffer<" + obj->StructName + "> " + obj->NameRW_SRV + " : register(t" + obj->SRVName() + ");\n");

			//TODO: add RW Object name
		

	}
}
#pragma endregion
