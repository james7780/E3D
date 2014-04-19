//////////////////////////////////////////////////////////////////////
//
// E3D_Scene.h: interface for the E3D_Scene class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

// overwrite WINGDI GetObject define
//#define GetObject GetObject
#undef GetObject

typedef std::vector<E3D_Object> OBJLIST;

#define NUM_OBJLISTS	10

class E3D_Scene  
{
public:
	E3D_Scene();
	virtual ~E3D_Scene();

	// scene loading error codes
	enum E3D_SceneError { 
		FILE_OPEN_FAIL = 2,
		BAD_COMMAND = 3,
		EOF_BEFORE_END = 4,
		BAD_LIGHT_TYPE = 5,
		MODEL_LOAD_FAIL = 6,
		BAD_OBJECT_SPEC = 7
	};

	void Start();
	void RemoveAllObjects();
	void SetRenderDistance(float z);
	float GetRenderDistance(void);
	void SetFogDistance(float z);
	float GetFogDistance(void);
	void SetFogColour(float r, float g, float b);
	Colour GetFogColour(void);
	void SetBackColour(float r, float g, float b);
	Colour GetBackColour(void);
	
	int Load(char *filename);
	E3D_Light * AddLight();
	E3D_Camera * AddCamera();
	E3D_Model * AddModel(char *name);
	E3D_Model * AddModel(char *name, char *filename);
	int FindModel(const char *modelName);
	E3D_Object * AddObject(unsigned short whichList, char *modelName);
	bool AttachObject(unsigned short whichList, unsigned short objectNumber, unsigned short parentObjectNumber);

	int Render(float viewWidth, float viewHeight, bool clear);
	E3D_Camera *GetCamera(int index);
	E3D_Light *GetLight(int index);
	E3D_Model *GetModel(int index);
	E3D_Object *GetObject(unsigned short whichList, unsigned short index);
	OBJLIST* GetObjectList(unsigned short whichList) { return &m_objList[whichList]; };
	int GetNumCameras(void);
	int GetNumLights(void);
	int GetNumModels(void);
	int GetNumObjects(void);

	// deprecated
	void SetClipping();
	void LoadTrack(char *filename);

private:
	int m_numModels;
	int m_numObjects;
	int m_numTextures;
	int m_numLights;
	int m_numCameras;
	Colour m_backColour;
	Colour m_fogColour;
	float m_fogDistance;
	float m_renderDistance;

	E3D_Model *m_pModel[MAX_MODELS_PER_SCENE];
	//E3D_Object *m_pObject[MAX_OBJECTS_PER_SCENE];
	E3D_Light *m_pLight[MAX_LIGHTS_PER_SCENE];
	E3D_Camera *m_pCamera[MAX_CAMERAS_PER_SCENE];


	// For 3D object lists
	OBJLIST m_objList[NUM_OBJLISTS];

};

