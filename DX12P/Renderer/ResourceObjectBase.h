#pragma once

#include <unordered_map>
#include <cinttypes>
//#include "FileManagerResourceStruct.h"

struct BuiltImage_c;
struct BuiltModel_c;
struct BuiltConstant_c;
struct BuiltPredefined_c;
struct TypeStorageMass;
struct DX11M3DR;

struct ResourceObjectBase {

	virtual void ClearAllPredefined() = 0;
	virtual void ClearAllImages() = 0;
	virtual void ClearAllModels() = 0;
	virtual void ClearAllConstants() = 0;
	virtual void ClearAllObjects() {
		ClearAllPredefined();
		ClearAllImages();
		ClearAllModels();
		ClearAllConstants();
	}
	virtual void LoadImageFromData(BuiltImage_c* bI) = 0;
	virtual void LoadModelFromData(BuiltModel_c* bI) = 0;
	virtual void LoadConstantFromData(BuiltConstant_c* bI) = 0;
	virtual void LoadPredefinedFromData(BuiltPredefined_c* bI) = 0;
	virtual void UpdatePredefinedFromData(BuiltPredefined_c* bI) = 0;

	~ResourceObjectBase() {
		ClearAllObjects();
	}
};