#pragma once
#define WIN32_LEAN_AND_MEAN
#define MAX_PATH 256

#include <Windows.h>
#include "ofbxMini.h"
#include <algorithm>
#include "DirectXMath.h"
#include <iostream>
#include <unordered_map>
#include <array>
#include "stb_image.h"
#include "filesystem"

using namespace DirectX;



//vertex per bone data
struct VertexBoneData {
	std::vector<float> IDs; //float for predictable shader pass
	std::vector<float> weights;

	VertexBoneData() {
		IDs.reserve(5);
		weights.reserve(5);
	}
};

//bone ID 
struct TopBoneIDs {
	int32_t id[4] = { 0,0,0,0 };

	void ZeroID() {
		for (int i = 0; i < sizeof(id) / sizeof(int32_t); i++) {
			id[i] = 0;
		}
	}
};

//Bone weight
struct TopBoneWeight {
	float w[4] = { 0,0,0,0 };

	void ZeroW() {
		for (int i = 0; i < sizeof(w) / sizeof(float); i++) {
			w[i] = 0;
		}
	}
};

//Vertex
struct VNT
{
	XMFLOAT3 Position = XMFLOAT3();
	XMFLOAT4 Color = XMFLOAT4(); //tint
	XMFLOAT3 Normal = XMFLOAT3();
	XMFLOAT3 BiNormal = XMFLOAT3();
	XMFLOAT3 Tangent = XMFLOAT3();
	XMFLOAT2 Tex = XMFLOAT2();
	TopBoneIDs tbi = TopBoneIDs(); //uint 4
	TopBoneWeight tbw = TopBoneWeight(); //float 4
};

//bone joint manager struct
struct Joint {
	int32_t id; // index/ID of bone
	std::string name;
	const ofbx::Object* Bone;

	//transformation, rotation, scale bone node
	std::vector <const ofbx::AnimationCurveNode*> tNode;
	std::vector <const ofbx::AnimationCurveNode*> rNode;
	std::vector <const ofbx::AnimationCurveNode*> sNode;

	XMMATRIX LBT;
	XMMATRIX InvBP = XMMatrixIdentity();
	XMMATRIX BP = XMMatrixIdentity();
	XMMATRIX SkinM = XMMatrixIdentity();

	XMMATRIX BoneOffsetMatrix;

	std::vector<std::shared_ptr<Joint>> children;
	std::shared_ptr<Joint> parent;

	
	void GetInvBindT(XMMATRIX parentBindT) {
		//get inverse bind transform matrix from recursive calc
		XMMATRIX BT = parentBindT * LBT;

		BP = BT;
		InvBP = XMMatrixInverse(nullptr, BT);
		SkinM = (BP * InvBP);

		for (int i = 0; i < children.size(); i++) {
			children[i]->GetInvBindT(LBT);
		}
	}
};


//ofbx to XM matrix stuff
XMFLOAT4X4 ofbxMatToXM(ofbx::Matrix* TMPtm) {
	return XMFLOAT4X4(static_cast<float>(TMPtm->m[0]), static_cast<float>(TMPtm->m[1]), static_cast<float>(TMPtm->m[2]), static_cast<float>(TMPtm->m[3]), static_cast<float>(TMPtm->m[4]), static_cast<float>(TMPtm->m[5]), static_cast<float>(TMPtm->m[6]), static_cast<float>(TMPtm->m[7]), static_cast<float>(TMPtm->m[8]), static_cast<float>(TMPtm->m[9]), static_cast<float>(TMPtm->m[10]), static_cast<float>(TMPtm->m[11]), static_cast<float>(TMPtm->m[12]), static_cast<float>(TMPtm->m[13]), static_cast<float>(TMPtm->m[14]), static_cast<float>(TMPtm->m[15]));
}
ofbx::Matrix XMToofbxMat(XMFLOAT4X4* TMPtm) {
	return { TMPtm->m[0][0],TMPtm->m[0][1],TMPtm->m[0][2],TMPtm->m[0][3],TMPtm->m[1][0],TMPtm->m[1][1],TMPtm->m[1][2],TMPtm->m[1][3] ,TMPtm->m[2][0],TMPtm->m[2][1],TMPtm->m[2][2],TMPtm->m[2][3] ,TMPtm->m[3][0],TMPtm->m[3][1],TMPtm->m[3][2],TMPtm->m[3][3] };

}
//


//object modification of treansform manager
struct ObjTuneStatReg { //TODO: make scale and translate work in shader - make obj visible
	std::array<float, 3> Translate = { 0.0f,0.0f,0.0f }; //not using xm float, so sad... :(
	float pad1 = 0.0f;
	std::array<float, 3> Scale = { 1.0f,1.0f,1.0f };
	float pad2 = 0.0f;
	XMFLOAT4 Quat = { 0.0f,0.0f,0.0f,0.0f };
};

struct ModelCheckDat_s {
	bool HasColor = false;
	bool HasUV = false;
	bool HasNormals = false;
	bool HasTangents = false;
	bool hasIndices = false;
};

struct MaterialEnum {
	enum {
		AMBIENT = 0,
		EMISSIVE,
		DIFFUSE,
		SPECULAR,
		SPECULARPOW,
		NORMAL,
		REFLECTANCE,
		OPACITY,
		TEX_COUNT
	};
};

struct MaterialData : MaterialEnum { //seperate struct to send to resource
/*

*/

	XMFLOAT4 Emissive = { 0,0,0,1 };
	XMFLOAT4 Ambient = { 1,1,1,1 };
	XMFLOAT4 Diffuse = { 0.1,0.1,0.1,1 };
	XMFLOAT4 Specular = { 0.1,0.1,0.1,1 };
	XMFLOAT4 Reflectance = { 1.0f,1.0f,1.0f,1.0f };

	float Opacity = 1.0f;
	float SpecularStr = { 128 };
	float SpecularScale = 1.0f;
	float IndexOfRefraction = 0;

	float BumpIntensity = 0.0f;
	float AlphaThreshold = 0.0f; //until cutoff
	UINT Lit = false;
	UINT UseVertexColor = false;

	INT32 TexOn[TEX_COUNT] = { 0,0,0,0,0,0,0,0 };
	

};


struct MaterialDataNamePair : MaterialEnum{
	std::string s = "";
	uint8_t* d = nullptr;
	int width = 0;
	int height = 0;
	int channels = 4;
};

struct MaterialDataName : MaterialEnum{ //seperate struct to send to resource
	std::vector<MaterialDataNamePair> Tex;
	MaterialDataName() {
		Tex.resize(TEX_COUNT);
	}
};

void XM4Pass(XMFLOAT4& dat, const ofbx::Color& other) {
	dat = { other.r, other.g, other.b, 1.0f };
}
void XM4Pass(XMFLOAT4& dat, const ofbx::Color& other, float factor) {
	dat = { other.r * factor, other.g * factor, other.b * factor, 1.0f };
}
bool StringToOFBXDataViewPass(MaterialDataNamePair& mdn, const ofbx::DataView& v, std::string& DirPath) {
	mdn.s.resize(MAX_PATH);
	char charTMP[MAX_PATH];
	v.toString(charTMP);
	mdn.s = std::string(charTMP);

	int index = mdn.s.size()-1;
	while (mdn.s[index] != '\\') {
		index -= 1;
	}
	mdn.s = mdn.s.substr(index, mdn.s.size() - 1);

	if (std::filesystem::exists(DirPath + mdn.s)) {
		mdn.d = stbi_load((DirPath + mdn.s).c_str(), &mdn.width, &mdn.height, &mdn.channels, 4);
		mdn.channels = 4;
		return true;
	}

	return false;
}

struct M3DR : MaterialEnum { //model Resource data only 
	/*
	abstract general 
	*/

	std::string FilePath = "";
	std::string DirPath = "";

	std::vector<MaterialData> MatData = {};
	std::vector<MaterialDataName> MatDataName = {};

	float ScaleVertex = 1.0f;

	bool UpdateVDat = false;
	bool UpdateAnimDat = false;

	ObjTuneStatReg ObjTune; //object transform manager
	std::vector < std::vector<VNT> > modelDat; //model vertex data vector
	std::vector<ModelCheckDat_s> modelDatCheck;


	std::vector<Joint> Bones; //bones vector
	const ofbx::Object* rootObj; //root obj link
	std::vector<XMFLOAT4X4> BoneDataTLMA; //Transform link matrix
	std::unordered_map<std::string, int> animNameS; //contains anim position in stack
	std::unordered_map<int, std::string> animNameI; //contains anim position in stack reverse
	std::vector< const ofbx::AnimationStack* > animStack;
	std::vector< float > animStackMaxTime; //anim max time vector
	std::vector< bool > AnimVCacheMade; //bone pose pre calc is made or not?*
	std::vector< std::vector< std::vector<XMFLOAT4X4> > > AnimVCacheData; //per anim, per interval - I have a set of matrix for VCache pre calc in order
	std::vector<XMMATRIX> animDat; //animation data collection
	std::vector<const ofbx::Cluster*> ClusterObject; 
	std::vector < std::map<int, VertexBoneData> > VboneDat; //Vertex bone data
	std::vector < std::vector<UINT> > Indice; //indices vector
	XMMATRIX globalInverseTransform; 
	XMMATRIX globalTransform;



	const ofbx::AnimationCurveNode* GetNodeCurve(int Anim, std::string valueType, const ofbx::Object* Bone) {


		for (int ii = 0; animStack[Anim]->getLayer(ii); ++ii)
		{

			const ofbx::AnimationLayer* layer = animStack[Anim]->getLayer(ii);

			if (layer->getCurveNode(*Bone, valueType.c_str())) {
				return layer->getCurveNode(*Bone, valueType.c_str());

			}
			else {
				return nullptr;
			}
		}
	}

	void PostProcessNormals() {
		for (int i = 0; i < modelDat.size(); i++) {
			if (modelDatCheck[i].HasNormals == false) {
				for (int x = 0; x < modelDat[i].size(); x += 3) {
					XMVECTOR A = XMLoadFloat3(&modelDat[i][x].Position);
					XMVECTOR B = XMLoadFloat3(&modelDat[i][x + 1].Position);
					XMVECTOR C = XMLoadFloat3(&modelDat[i][x + 2].Position);

					A = XMVector3Cross((B - A), (C - A));

					XMStoreFloat3(&modelDat[i][x].Normal, A);

					XMStoreFloat3(&modelDat[i][x + 1].Normal, A);

					XMStoreFloat3(&modelDat[i][x + 2].Normal, A);

				}
			}
		}
	}

	void AutoFillIndice(int Obj) {
		if (Obj >= Indice.size()) { Indice.resize(Obj + 1); }
		Indice[Obj].resize(modelDat[Obj].size());
		for (int i = 0; i < modelDat[Obj].size(); i++) {
			Indice[Obj][i] = i;
		}
	}

	void PostProcessTangents() {
		for (int i = 0; i < modelDat.size(); i++) {
			if (modelDatCheck[i].HasTangents == false) {

				XMVECTOR* tan1 = new XMVECTOR[modelDat[i].size() * 2];
				ZeroMemory(tan1, modelDat[i].size() * sizeof(XMFLOAT3) * 2);

				for (int x = 0; x < modelDat[i].size(); x += 3) {


					XMFLOAT3* A = (&modelDat[i][x].Position);
					XMFLOAT3* B = (&modelDat[i][x + 1].Position);
					XMFLOAT3* C = (&modelDat[i][x + 2].Position);

					XMFLOAT2* UA = (&modelDat[i][x].Tex);
					XMFLOAT2* UB = (&modelDat[i][x + 1].Tex);
					XMFLOAT2* UC = (&modelDat[i][x + 2].Tex);

					float x1 = B->x - A->x;
					float x2 = C->x - A->x;
					float y1 = B->y - A->y;
					float y2 = C->y - A->y;
					float z1 = B->z - A->z;
					float z2 = C->z - A->z;

					float s1 = UB->x - UA->x;
					float s2 = UC->x - UA->x;
					float t1 = UB->y - UA->y;
					float t2 = UC->y - UA->y;

					float r = 1.0f / (s1 * t2 - s2 * t1);

					XMFLOAT3 sdir = { (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
						(t2 * z1 - t1 * z2) * r };

					tan1[x] += XMLoadFloat3(&sdir);
					tan1[x + 1] += XMLoadFloat3(&sdir);
					tan1[x + 2] += XMLoadFloat3(&sdir);

				}

				for (long a = 0; a < modelDat[i].size(); a++)
				{
					XMVECTOR n = XMLoadFloat3(&modelDat[i][a].Normal);
					XMVECTOR t = tan1[a];

					XMStoreFloat3(&modelDat[i][a].Tangent, XMVector3Normalize((t - n * XMVector3Dot(n, t))));
				}
				delete[] tan1;
			}
		}

	}

	void PostProcessBinormals() {
		for (int i = 0; i < modelDat.size(); i++) {
			for (int x = 0; x < modelDat[i].size(); x++) {

				XMStoreFloat3(&modelDat[i][x].BiNormal, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&modelDat[i][x].Normal), XMLoadFloat3(&modelDat[i][x].Tangent))));

			}
		}
	}
	void VertexPostProcess() {
		PostProcessNormals();
		PostProcessTangents();
		PostProcessBinormals();
	}
	void RESIZE_VECTORS_OBJ_LOAD(int i) {
		Indice.resize(i);
		modelDat.resize(i);
		MatData.resize(i);
		MatDataName.resize(i);
		modelDatCheck.resize(i);
		VboneDat.resize(i);
	}

	int AddIfN(int v) {
		if (v < 0) return std::abs(v) - 1;
		return v;
	}

	//I feel like not doing intrinsics
	XMFLOAT3 MatrixMul(const ofbx::Matrix& m, std::vector<float> v) {
		v = { v[0] * float(m.m[0 * 4 + 0]) + v[1] * float(m.m[1 * 4 + 0]) + v[2] * float(m.m[2 * 4 + 0]) + float(m.m[3 * 4 + 0]),
			  v[0] * float(m.m[0 * 4 + 1]) + v[1] * float(m.m[1 * 4 + 1]) + v[2] * float(m.m[2 * 4 + 1]) + float(m.m[3 * 4 + 1]),
			  v[0] * float(m.m[0 * 4 + 2]) + v[1] * float(m.m[1 * 4 + 2]) + v[2] * float(m.m[2 * 4 + 2]) + float(m.m[3 * 4 + 2])
		};
		return {v[0],v[1],v[2]}; //basically just return the same object in
	}
	template<typename T>
	XMFLOAT3 Vec3Scale(T v, float s) {
		return {v.x*s,v.y*s,v.z*s};
	}
	

	void GetTransMFromParentAndCurr(ofbx::Object& obj, ofbx::Matrix& m) {
		using namespace ofbx;
		if (obj.getParent() != nullptr) {
			ofbx::Object& mesh = *obj.getParent();
			ofbx::Matrix GT = mesh.getGlobalTransform();
			if (mesh.getType() == ofbx::Object::Type::MESH) {
				GT = GT*(((ofbx::Mesh&)obj).getGeometricMatrix());
			}

			m = m * GT; 
			return;
			GetTransMFromParentAndCurr(mesh, m);
			
		}
		return;
	}

	void LoadFBXFile(std::string path) {
		FILE* fp;

		fopen_s(&fp, path.c_str(), "rb");

		if (!fp) { std::cout << "no file found at fbx path"; }
		else {
			fseek(fp, 0, SEEK_END);
			long file_size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			ofbx::u8* content = new ofbx::u8[file_size];
			fread(content, 1, file_size, fp);

			ofbx::IScene* g_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);

			Bones.clear();
			BoneDataTLMA.clear();
			animNameI.clear();
			animNameS.clear();
			animStackMaxTime.clear();
			AnimVCacheData.clear();
			ClusterObject.clear();
			modelDat.clear();
			Indice.clear();
			VboneDat.clear();
			Bones.clear();
			BoneDataTLMA.clear();
			animDat.clear();
			animStack.clear();
			
			VNT tmpV;

			XMFLOAT3 TmpPos;
			XMFLOAT3 TmpNor;
			XMFLOAT2 TmpTex;
			UINT TmpInd;

			int obj_idx = 0;
			int indices_offset = 0;
			int normals_offset = 0;
			int mesh_count = g_scene->getMeshCount();
			
			RESIZE_VECTORS_OBJ_LOAD(mesh_count);

			rootObj = g_scene->getRoot();
			ofbx::Matrix emp = rootObj->getGlobalTransform();

			XMFLOAT4X4 tmpgl = ofbxMatToXM(&emp);

			globalTransform = XMLoadFloat4x4(&tmpgl);

			globalInverseTransform = XMMatrixInverse(nullptr, globalTransform);

			for (int i = 0; i < mesh_count; i++)
			{

				const ofbx::Mesh& mesh = *g_scene->getMesh(i);
			
				
				//const ofbx::Matrix GM = mesh.getGeometricMatrix();
				const ofbx::Matrix GT = mesh.getGlobalTransform();
				const ofbx::Matrix GM = mesh.getGeometricMatrix();
				using namespace ofbx;
				ofbx::Matrix TM = GT * GM;
				
				//ofbx::Matrix TM = ofbx::makeIdentity();
				//GetTransMFromParentAndCurr((ofbx::Object&)mesh, TM);
				
				const ofbx::Geometry& geom = *mesh.getGeometry();

				int vertex_count = geom.getVertexCount();
				const ofbx::Vec3* vertices = geom.getVertices();
				
				const ofbx::Vec4* colors = geom.getColors();
				if (colors != nullptr) modelDatCheck[i].HasColor = true;

				const ofbx::Vec2* uvs = geom.getUVs();
				if (uvs != nullptr) modelDatCheck[i].HasUV = true;
				
				const int indiceC = geom.getIndexCount();

				const ofbx::Vec3* normals = geom.getNormals();
				if (normals != nullptr) modelDatCheck[i].HasNormals = true;
				
				const ofbx::Vec3* tangents = geom.getTangents();
				if (tangents != nullptr) modelDatCheck[i].HasTangents = true;
				
				const int* indices = geom.getFaceIndices();
				if (indices != nullptr) modelDatCheck[i].hasIndices = true;

				const ofbx::Pose* cPose = mesh.getPose();
						//POS NORMAL  TEX
				
				const int MeshMatC = mesh.getMaterialCount();

				if (MeshMatC > 0) {
					const ofbx::Material* matDatTmp = mesh.getMaterial(0);
					XM4Pass(MatData[i].Ambient, matDatTmp->getAmbientColor());//, (float)matDatTmp->getAmbientFactor());
					MatData[i].BumpIntensity = matDatTmp->getBumpFactor();
					XM4Pass(MatData[i].Diffuse, matDatTmp->getDiffuseColor());//, (float)matDatTmp->getDiffuseFactor());
					XM4Pass(MatData[i].Emissive, matDatTmp->getEmissiveColor());//, (float)matDatTmp->getEmissiveFactor());
					XM4Pass(MatData[i].Specular, matDatTmp->getSpecularColor());//, (float)matDatTmp->getSpecularFactor());
					MatData[i].SpecularStr = matDatTmp->getShininess();
					MatData[i].SpecularScale = matDatTmp->getShininessExponent();

					//doing loading code 1 by 1 manually
					const ofbx::Texture* matTexDat = matDatTmp->getTexture(ofbx::Texture::AMBIENT);
					if (matTexDat != nullptr) {
						MatData[i].TexOn[MaterialData::AMBIENT] = StringToOFBXDataViewPass(MatDataName[i].Tex[AMBIENT], matTexDat->getFileName(), DirPath);
					}
					matTexDat = matDatTmp->getTexture(ofbx::Texture::DIFFUSE);
					if (matTexDat != nullptr) {
						if(StringToOFBXDataViewPass(MatDataName[i].Tex[DIFFUSE], matTexDat->getFileName(), DirPath))
						MatData[i].TexOn[MaterialData::DIFFUSE] = true;
					}
					matTexDat = matDatTmp->getTexture(ofbx::Texture::EMISSIVE);
					if (matTexDat != nullptr) {
						MatData[i].TexOn[MaterialData::EMISSIVE] = StringToOFBXDataViewPass(MatDataName[i].Tex[EMISSIVE], matTexDat->getFileName(), DirPath);
					}
					matTexDat = matDatTmp->getTexture(ofbx::Texture::SPECULAR);
					if (matTexDat != nullptr) {
						MatData[i].TexOn[MaterialData::SPECULAR] = StringToOFBXDataViewPass(MatDataName[i].Tex[SPECULAR], matTexDat->getFileName(), DirPath);
						
					}
					matTexDat = matDatTmp->getTexture(ofbx::Texture::SHININESS);
					if (matTexDat != nullptr) {
						MatData[i].TexOn[MaterialData::SPECULARPOW] = StringToOFBXDataViewPass(MatDataName[i].Tex[SPECULARPOW], matTexDat->getFileName(), DirPath);
						
					}
					matTexDat = matDatTmp->getTexture(ofbx::Texture::NORMAL);
					if (matTexDat != nullptr) {
						MatData[i].TexOn[MaterialData::NORMAL] = StringToOFBXDataViewPass(MatDataName[i].Tex[NORMAL], matTexDat->getFileName(), DirPath);
						
					}
					matTexDat = matDatTmp->getTexture(ofbx::Texture::REFLECTION);
					if (matTexDat != nullptr) {
						MatData[i].TexOn[MaterialData::REFLECTANCE] = StringToOFBXDataViewPass(MatDataName[i].Tex[REFLECTANCE], matTexDat->getFileName(), DirPath);
						
					}

				}

				for (int ii = 0; ii < vertex_count; ii++)
				{

					tmpV.Position = Vec3Scale(MatrixMul(TM, { static_cast<float>(vertices[ii].x), static_cast<float>(vertices[ii].y), static_cast<float>(vertices[ii].z)}), ScaleVertex);

					tmpV.BiNormal = { 0.0f,0.0f,0.0f };
					
					if (colors != nullptr) {
						tmpV.Color = { static_cast <float>(colors[ii].x),static_cast <float>(colors[ii].y),static_cast <float>(colors[ii].z),static_cast <float>(colors[ii].w) };
					}
					else {
						tmpV.Color = { 0.0f,0.0f,0.0f,0.0f };
					}
					if (normals != nullptr) {
						tmpV.Normal = { static_cast < float>(normals[ii].x),static_cast <float>(normals[ii].y),static_cast <float>(normals[ii].z)};
					}
					else {
						tmpV.Normal = { 0.0f,0.0f,0.0f };
					}
					if (tangents != nullptr) {
						tmpV.Tangent = { static_cast <float>(tangents[ii].x),static_cast <float>(tangents[ii].y),static_cast <float>(tangents[ii].z)};
					}
					else {
						tmpV.Tangent = { 0.0f,0.0f,0.0f };
					}
					if (uvs != nullptr) {
						tmpV.Tex = { static_cast<float>(uvs[ii].x),1.0f - static_cast<float>(uvs[ii].y) };
					}
					else {
						tmpV.Tex = { 0.0f,0.0f };
					}

					modelDat[i].push_back(tmpV);
				}
				for (int ii = 0; ii < indiceC; ii++) {
					if (modelDatCheck[i].hasIndices) {
						Indice[i].push_back(AddIfN(indices[ii]));
					}
					else {
						Indice[i].push_back(Indice[i].size());
					}
				}

				//VboneDat[i].resize(modelDat[i].size());

				const ofbx::Skin* skin = geom.getSkin();
				if (skin) {
					int gctR = skin->getClusterCount();
					for (int ii = 0; ii < gctR; ii++)
					{

						const ofbx::Cluster* cluster = skin->getCluster(ii);

						if (cluster) {
							int indiceCount = cluster->getIndicesCount();
							//int counter = 0;
							if (indiceCount > 0) {
								const int* indList = cluster->getIndices();
								const double* tmpW = cluster->getWeights();

								for (int iii = 0; iii < indiceCount; iii++) {
									VboneDat[i][indList[iii]].weights.push_back(static_cast<float>(tmpW[iii]));
									VboneDat[i][indList[iii]].IDs.push_back(ii); //bone id
								}
							}
							ofbx::Matrix TMPtm = cluster->getTransformMatrix();


							ofbx::Matrix TMPtlm = cluster->getTransformLinkMatrix(); //correct matrix

							ofbx::Matrix GeoTrans = mesh.getGeometricMatrix();

							ofbx::Matrix ModelMatrix = cluster->getLink()->getGlobalTransform();

							ofbx::Matrix LocalMatrix = cluster->getLink()->getLocalTransform();

							XMFLOAT4X4 XMIB = ofbxMatToXM(&GeoTrans);
							XMMATRIX tmpMMM = XMLoadFloat4x4(&XMIB);
							//		ofbx::Matrix linkGeo = cluster->getLink()->getGlobalTransform();




							ClusterObject.push_back(cluster);
							//TMPtlm = GeoTrans* TMPtlm;
							//TMPtm = TMPtm*linkGeo;

							XMFLOAT4X4 tmpTMtmp = ofbxMatToXM(&LocalMatrix);
							XMMATRIX tmpTM = XMLoadFloat4x4(&tmpTMtmp);

							XMFLOAT4X4 tmpGtmp = ofbxMatToXM(&ModelMatrix);
							XMMATRIX tmpG = XMLoadFloat4x4(&tmpGtmp);


							XMFLOAT4X4 tmpTLMF = ofbxMatToXM(&TMPtlm);
							XMMATRIX tmpTLMM = XMLoadFloat4x4(&tmpTLMF);
							tmpTLMM = XMMatrixInverse(nullptr, tmpTLMM);

							XMFLOAT4X4 tmptmpTLMMma = ofbxMatToXM(&TMPtm);
							XMMATRIX tmpTLMMma = XMLoadFloat4x4(&tmptmpTLMMma);

							//

							Joint nJ;
							nJ.Bone = cluster->getLink();
							nJ.name = nJ.Bone->name;
							nJ.id = Bones.size();
							nJ.LBT = (tmpTM)*XMMatrixInverse(nullptr, tmpG);
							nJ.BoneOffsetMatrix = tmpTLMMma;
							Bones.push_back(nJ);
						}
					}
				}
			}

			BoneDataTLMA.resize(Bones.size());
			animDat.resize(Bones.size());

			int animCount = g_scene->getAnimationStackCount();

			for (int xx = 0; xx < animCount; xx++)
			{
				const ofbx::AnimationStack* astack = g_scene->getAnimationStack(xx);

				animStack.push_back(astack);
				animNameI[xx] = astack->name;
				animNameS[astack->name] = xx;
			}

			//load data from g_scene now
		}

		fclose(fp);
	}
	float EndTimeOfAnim(int Anim) {
		//get end of anim time if valid int
		if (Anim >= animStackMaxTime.size()) { return 0; }
		return animStackMaxTime[Anim];
	}
	void SetAnimEndTime(int Anim) {
		//set actual end of animation time
		float max = 0; // max time getter func - iterate through keys

		for (int i = 0; i < Bones.size(); i++) {

			for (int x = 0; x < 3; x++) {
				ofbx::i64 tcnV = 0.0f;
				ofbx::i64 rcnV = 0.0f;
				ofbx::i64 scnV = 0.0f;

				if (Bones[i].tNode[Anim] != NULL) {
					if (Bones[i].tNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* tc = Bones[i].tNode[Anim]->getCurve(x);
						int tcn = tc->getKeyCount() - 1;
						tcnV = tc->getKeyTime()[tcn];
					}
				}

				if (Bones[i].rNode[Anim] != NULL) {
					if (Bones[i].rNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* rc = Bones[i].rNode[Anim]->getCurve(x);
						int rcn = rc->getKeyCount() - 1;
						rcnV = rc->getKeyTime()[rcn];
					}
				}

				if (Bones[i].sNode[Anim] != NULL) {
					if (Bones[i].sNode[Anim]->getCurve(x)) {
						const ofbx::AnimationCurve* sc = Bones[i].sNode[Anim]->getCurve(x);
						int scn = sc->getKeyCount() - 1;
						scnV = sc->getKeyTime()[scn];
					}
				}
				//std::max not working...
				ofbx::i64 maxTmp;
				if (tcnV > rcnV) {
					maxTmp = tcnV;
				}
				else {
					maxTmp = rcnV;
				}
				if (maxTmp < scnV) maxTmp = scnV;

				if (max < maxTmp) max = static_cast<float>(ofbx::fbxTimeToSeconds(maxTmp));
			}

		}
		animStackMaxTime[Anim] = max;
	}

	void GetAllAnimEndTime() {
		//get all end of animation times
		for (int i = 0; i < animStack.size(); i++) {
			SetAnimEndTime(i);
		}
	}

	void AddBoneChildren(Joint* BoneP) {
		//add child to bone
		for (int i = 0; i < Bones.size(); i++) {
			if (Bones[i].Bone->getParent() == BoneP->Bone) {
				BoneP->children.push_back(std::make_shared<Joint>(Bones[i]));
			}
		}
	}
	void SetAllBonesChildren() {

		for (int i = 0; i < Bones.size(); i++) {
			AddBoneChildren(&Bones[i]);
		}
	}
	void SetAllBonesParent() {
		for (int i = 0; i < Bones.size(); i++) {
			for (int ii = 0; ii < Bones.size(); ii++) {
				if (Bones[i].Bone->getParent() == Bones[ii].Bone) {
					Bones[i].parent = std::make_shared<Joint>(Bones[ii]);
				}
			}
		}
	}
	void SetAllBonesGetInvBindT() {
		if (Bones.size() > 0) {
			Bones[0].GetInvBindT(XMMatrixIdentity());
		}
	}
	void ResizeAllBonesNodeArr() {
		for (int i = 0; i < Bones.size(); i++) {
			Bones[i].tNode.resize(animStack.size());
			Bones[i].rNode.resize(animStack.size());
			Bones[i].sNode.resize(animStack.size());
		}
	}
	void GetLTRSNode(int Bone, int Anim) {
		Bones[Bone].tNode[Anim] = GetNodeCurve(Anim, "Lcl Translation", Bones[Bone].Bone);
		Bones[Bone].rNode[Anim] = GetNodeCurve(Anim, "Lcl Rotation", Bones[Bone].Bone);
		Bones[Bone].sNode[Anim] = GetNodeCurve(Anim, "Lcl Scaling", Bones[Bone].Bone);
	}
	void GetAllLTRSNodeForEveryAnim(int Bone) {
		for (int i = 0; i < animStack.size(); i++) {
			GetLTRSNode(Bone, i);
		}
	}
	void GetAllBoneNodes() {
		for (int i = 0; i < Bones.size(); i++) {
			GetAllLTRSNodeForEveryAnim(i);
		}
	}
	void ResizeAndPrepAnimBoneStuff() {
		animStackMaxTime.resize(animStack.size());
		AnimVCacheMade.resize(animStack.size());
		AnimVCacheData.resize(animStack.size());
	}
	void BindPoseBones() {

		for (int i = 0; i < Bones.size(); i++) {
			XMStoreFloat4x4(&BoneDataTLMA[i], (Bones[i].SkinM));
		}
	}
	void FillTopBones() { 

		for (int B = 0; B < Indice.size(); B++) { //for indice size, start filling bones 

			for (int z = 0; z < Indice[B].size(); z++) {
				int i = Indice[B][z];
				
				if (VboneDat[B].find(i) == VboneDat[B].end()) {

					VboneDat[B][i] = {};

					//fill top 4 if no contest in indice weights (only 4 or less weights

					if (VboneDat[B][i].weights.size() < 4) {
						VboneDat[B][i].weights.resize(4); VboneDat[B][i].IDs.resize(4);
						for (int xx = 0; xx < VboneDat[B][i].weights.size(); xx++) {
							if (VboneDat[B][i].weights[xx] == 0) {
								VboneDat[B][i].weights[xx] = 0.0f;
								VboneDat[B][i].IDs[xx] = 0;
							}
						}
					}

					float tmpWH = 0.0f;
					int tmpIDH = 0;

					//add 4 strongestweights if not only 4 on vertex
					for (int ii = 0; ii < VboneDat[B][i].weights.size(); ii++) {
						for (int iii = VboneDat[B][i].weights.size() - 1; iii > ii; iii--) {

							if (VboneDat[B][i].weights[iii] > VboneDat[B][i].weights[ii]) {
								tmpWH = VboneDat[B][i].weights[ii];
								VboneDat[B][i].weights[ii] = VboneDat[B][i].weights[iii];
								VboneDat[B][i].weights[iii] = tmpWH;

								tmpIDH = VboneDat[B][i].IDs[ii];
								VboneDat[B][i].IDs[ii] = VboneDat[B][i].IDs[iii];
								VboneDat[B][i].IDs[iii] = tmpIDH;

								ii = 0;
								iii = VboneDat[B][i].weights.size() - 1;
							}
						}
					}
					//set actual bone weights and id from tmps
					for (int y = 0; y < 4; y++) {
						modelDat[B][z].tbw.w[y] = VboneDat[B][i].weights[y];

						modelDat[B][z].tbi.id[y] = VboneDat[B][i].IDs[y];
					}
				}
			}

			//normalize bone weights based on man
			for (int z = 0; z < Indice.size(); z++) {
				
				float max = 0.0f;

				for (int ii = 0; ii < 4; ii++) {
					max += modelDat[B][z].tbw.w[ii];
				}

				if (max > 0) { //to stop error of 1
					for (int ii = 0; ii < 4; ii++) {
						modelDat[B][z].tbw.w[ii] = modelDat[B][z].tbw.w[ii] / max;
					}
				}
			}
		}
	}
	void CropPathToGetDir(std::string& path, std::string& out) {
		int counter = path.size() - 1;
		
		while (path[counter] != '\\') {
			counter -= 1;
		}

		out = path.substr(0, counter);
	}
	void LoadFBXFileWithVertex(std::string path) {
		FilePath = path;
		
		CropPathToGetDir(FilePath, DirPath);

		LoadFBXFile(path);

		VertexPostProcess();

		SetAllBonesChildren();
		SetAllBonesParent();
		SetAllBonesGetInvBindT();

		ResizeAllBonesNodeArr();
		GetAllBoneNodes();

		ResizeAndPrepAnimBoneStuff();

		GetAllAnimEndTime();

		BindPoseBones();

		FillTopBones();
	}

	~M3DR() {
		//TODO: make deconstructor
	}

};