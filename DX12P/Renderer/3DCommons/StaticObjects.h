#pragma once

#include "3DCommons/3DCommon.h"
#include <iostream>
#include <string>
#include <fstream>
//TODO: implemnet model static loader and more statics

/*
#my python program to static get fbx v data -- TODO: make obj parser
s = str'''

'''
#str from v studio debugger v get of M3DR
s = s.replace("DirectX::XMFLOAT3",",")
s = s.replace("x=","")
s = s.replace("y=",",")
s = s.replace("z=",",")

s= s.replace("]	","")
s= s.replace("[","")
s= s.replace("]","")
s= s.replace("+		","")

for x in range(0,10):
	for i in range(0,10):
		s= s.replace(str(i)+"{","{")

print(s)

*/
/*
auto file maker to copy pasta into code

*/

void OutputStringToFileForCopyPata(M3DR* obj) {
	std::ofstream outfile("ModelStat.txt", trunc | std::ofstream::out);

	std::vector<XMFLOAT3> PositionV = {
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }
	};

	std::vector<XMFLOAT2> TexV = {
	{ 0.0f, 0.0f},
	{ 0.0f, 0.0f}
	};

	//pos V
	outfile << "std::vector<XMFLOAT3> Name = {";
	for (int i = 0l; i < obj->modelDat.size(); i++) {
		for (int ii = 0l; ii < obj->modelDat[i].size(); ii++) {
			outfile << "{";
			outfile << std::to_string(obj->modelDat[i][ii].Position.x) + "f";
			outfile << ",";
			outfile << std::to_string(obj->modelDat[i][ii].Position.y) + "f";
			outfile << ",";
			outfile << std::to_string(obj->modelDat[i][ii].Position.z) + "f";
			outfile << "},\n";
		}
	}
	outfile << "};\n\n";
	//
	
	//tex V
	for (int i = 0l; i < obj->modelDat.size(); i++) {
		for (int i = 0l; i < obj->modelDat.size(); i++) {
			for (int ii = 0l; ii < obj->modelDat[i].size(); ii++) {
				outfile << "{";
				outfile << std::to_string(obj->modelDat[i][ii].Tex.x) + "f";
				outfile << ",";
				outfile << std::to_string(obj->modelDat[i][ii].Tex.y) + "f";;
				outfile << "},\n";
			}
		}
	}
	outfile << "};\n\n";
	//


	outfile.close();
}

static std::vector<VNT>* ReturnCube() {

}
static std::vector<VNT>* ReturnPlane() {

}
static std::vector<VNT>* ReturnFullScreenTri() {

}