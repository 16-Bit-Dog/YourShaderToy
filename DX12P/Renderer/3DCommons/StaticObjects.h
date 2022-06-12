#pragma once

#include "3DCommons/3DCommon.h"
#include <iostream>
#include <string>
#include <fstream>
#include <tuple>
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
	//outputs string to copy paste into code for static infile object loading
	std::ofstream outfile("ModelStat.txt", std::ofstream::trunc | std::ofstream::out);

	std::vector<XMFLOAT3> PositionV = {
	{ 0.0f, 0.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f }
	};

	std::vector<XMFLOAT2> TexV = {
	{ 0.0f, 0.0f},
	{ 0.0f, 0.0f}
	};

	//pos V
	outfile << "std::vector<XMFLOAT3> Pos = {\n";
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
	outfile << "std::vector<XMFLOAT2> Tex = {\n";
	for (int i = 0l; i < obj->modelDat.size(); i++) {
		for (int i = 0l; i < obj->modelDat.size(); i++) {
			for (int ii = 0l; ii < obj->modelDat[i].size(); ii++) {
				outfile << "{";
				outfile << std::to_string(obj->modelDat[i][ii].Tex.x) + "f";
				outfile << ",";
				outfile << std::to_string(obj->modelDat[i][ii].Tex.y) + "f";
				outfile << "},\n";
			}
		}
	}
	outfile << "};\n\n";
	//


	outfile.close();
}

std::vector<VNT>* ReturnVNT(std::vector<XMFLOAT3>* Pos, std::vector<XMFLOAT2>* Tex) {
	//return struct data type for my obj loader by reading data
	std::vector<VNT>* v = new std::vector<VNT>();
	v->resize((*Pos).size());
	for (int i = 0; i < (*Pos).size(); i++) {
		(*v)[i].Position = (*Pos)[i];
		(*v)[i].Tex = (*Tex)[i];
	}
	return v;
}

static std::vector<VNT>* ReturnCube() {
	std::vector<XMFLOAT3> Pos = {
{1.000000f,1.000000f,1.000000f},
{-1.000000f,1.000000f,1.000000f},
{-1.000000f,-1.000000f,1.000000f},
{1.000000f,1.000000f,1.000000f},
{-1.000000f,-1.000000f,1.000000f},
{1.000000f,-1.000000f,1.000000f},
{1.000000f,-1.000000f,-1.000000f},
{1.000000f,-1.000000f,1.000000f},
{-1.000000f,-1.000000f,1.000000f},
{1.000000f,-1.000000f,-1.000000f},
{-1.000000f,-1.000000f,1.000000f},
{-1.000000f,-1.000000f,-1.000000f},
{-1.000000f,-1.000000f,-1.000000f},
{-1.000000f,-1.000000f,1.000000f},
{-1.000000f,1.000000f,1.000000f},
{-1.000000f,-1.000000f,-1.000000f},
{-1.000000f,1.000000f,1.000000f},
{-1.000000f,1.000000f,-1.000000f},
{-1.000000f,1.000000f,-1.000000f},
{1.000000f,1.000000f,-1.000000f},
{1.000000f,-1.000000f,-1.000000f},
{-1.000000f,1.000000f,-1.000000f},
{1.000000f,-1.000000f,-1.000000f},
{-1.000000f,-1.000000f,-1.000000f},
{1.000000f,1.000000f,-1.000000f},
{1.000000f,1.000000f,1.000000f},
{1.000000f,-1.000000f,1.000000f},
{1.000000f,1.000000f,-1.000000f},
{1.000000f,-1.000000f,1.000000f},
{1.000000f,-1.000000f,-1.000000f},
{-1.000000f,1.000000f,-1.000000f},
{-1.000000f,1.000000f,1.000000f},
{1.000000f,1.000000f,1.000000f},
{-1.000000f,1.000000f,-1.000000f},
{1.000000f,1.000000f,1.000000f},
{1.000000f,1.000000f,-1.000000f},
	};

	std::vector<XMFLOAT2> Tex = {
	{0.625000f,0.500000f},
	{0.875000f,0.500000f},
	{0.875000f,0.250000f},
	{0.625000f,0.500000f},
	{0.875000f,0.250000f},
	{0.625000f,0.250000f},
	{0.375000f,0.250000f},
	{0.625000f,0.250000f},
	{0.625000f,0.000000f},
	{0.375000f,0.250000f},
	{0.625000f,0.000000f},
	{0.375000f,0.000000f},
	{0.375000f,1.000000f},
	{0.625000f,1.000000f},
	{0.625000f,0.750000f},
	{0.375000f,1.000000f},
	{0.625000f,0.750000f},
	{0.375000f,0.750000f},
	{0.125000f,0.500000f},
	{0.375000f,0.500000f},
	{0.375000f,0.250000f},
	{0.125000f,0.500000f},
	{0.375000f,0.250000f},
	{0.125000f,0.250000f},
	{0.375000f,0.500000f},
	{0.625000f,0.500000f},
	{0.625000f,0.250000f},
	{0.375000f,0.500000f},
	{0.625000f,0.250000f},
	{0.375000f,0.250000f},
	{0.375000f,0.750000f},
	{0.625000f,0.750000f},
	{0.625000f,0.500000f},
	{0.375000f,0.750000f},
	{0.625000f,0.500000f},
	{0.375000f,0.500000f},
	};
	
	
	return ReturnVNT(&Pos, &Tex);
	
	

}

std::vector<VNT>* ReturnDeccerCubeBlank() {
	std::string* tmp = new std::string("Deccer_Cubes\\SM_Deccer_Cubes.fbx");
	return (std::vector<VNT>*)(tmp);
}
std::vector<VNT>* ReturnDeccerCubeColor() {
	std::string* tmp = new std::string("Deccer_Cubes\\SM_Deccer_Cubes_Colored.fbx");
	return (std::vector<VNT>*)(tmp);
}
std::vector<VNT>* ReturnDeccerCubeTex() {
	std::string* tmp = new std::string("Deccer_Cubes\\SM_Deccer_Cubes_Textured.fbx");
	return (std::vector<VNT>*)(tmp);
}
std::vector<VNT>* ReturnNOT_PART_OF_DECCER_CUBES_1() {
	std::string* tmp = new std::string("Deccer_Cubes\\NOT_PART_OF_DECCER_CUBES_1.fbx");
	return (std::vector<VNT>*)(tmp);
}
/*
static std::vector<VNT>* ReturnPlane() {

}
static std::vector<VNT>* ReturnFullScreenTri() {

}
*/

static std::vector<  std::tuple< std::string, std::function<std::vector<VNT>* ()>, char, float>  > StaticObjectPass{ //vector to static objects
	{"Cube", ReturnCube, 'd', 1.0f},
	{"Deccer Cube Blank", ReturnDeccerCubeBlank, 's', 0.01f},
	{"Deccer Cube Color", ReturnDeccerCubeColor, 's', 0.01f},
	{"Deccer Cube Texture", ReturnDeccerCubeTex, 's',  0.01f},
	{"NOT_PART_OF_DECCER_CUBES_1", ReturnNOT_PART_OF_DECCER_CUBES_1, 's', 0.01f},

};

const std::string stringSet() {

	std::string ToAdd = "";

	for (int i = 0; i < StaticObjectPass.size(); i++) {
		ToAdd += "\n" + std::to_string(i) + " is add " + std::get<0>(StaticObjectPass[i]);
	}

	return 	std::string("-1 is add through.fbx file selector" + ToAdd);

}

//string to pass to describe all constant objects built into program code
static std::string ToAddStaticObjectString = stringSet();