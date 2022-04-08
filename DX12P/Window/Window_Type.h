#pragma once
#include <vector>
#include <string>
#include <functional>
#include <map>

struct GroupData;



enum WIN_TYPE_ENUM { // not used for now, but important to share all "scene" data for the most part
	W_SETTING = 1,
	W_SCENE = 2,
	W_EDITOR = 3,
	W_PIPELINE = 4,
	W_FILEMANAGER = 5,
};

struct WIN_TYPE {

	inline static UINT GLOBAL_WINDOW_ID = 0;

	inline static std::map<int, int> Win_Type_ID_Vector; // associated with Win_Type index for ease of adding more Win_type's
	inline static std::map<int, std::string> Win_Type_Name_Vector; // associated with Win_Type index for ease of adding more Win_type's
	inline static std::map<int, std::function<void(GroupData*)>> Win_Type_Initialization_Vector_Of_Type; //if int is equal, you run the function inside
};

UINT GLOBAL_WINDOW_ID_I() {
	WIN_TYPE::GLOBAL_WINDOW_ID += 1;
	return WIN_TYPE::GLOBAL_WINDOW_ID - 1;
}

int Add_New_Win_Type(std::function<void(GroupData*)> InitializationFunction, std::string name) { //returns new win type
	for (int i = WIN_TYPE::Win_Type_ID_Vector.size() + 1; i > 0; i++) {
		if (WIN_TYPE::Win_Type_ID_Vector.count(i) == 0) {

			WIN_TYPE::Win_Type_ID_Vector[i] = i;
			WIN_TYPE::Win_Type_Name_Vector[i] = name;
			WIN_TYPE::Win_Type_Initialization_Vector_Of_Type[i] = InitializationFunction;

			return i;
		}
	}

	return 0;
}
//