//////////////////////////////////////////////////////////////////////
//
// E3D_Model.h: interface for the E3D_Model class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define E3D_MODEL_NAME_LENGTH	100
#define E3D_MODEL_MAX_MATERIAL	50

// a material definition
class E3D_Material
{
public:
	E3D_Material();
	virtual ~E3D_Material();

	/*
	newmtl body
	Ns 100.000
	d 1.00000
	illum 2
	Kd 0.00000e+0 0.993333 0.00000e+0
	Ka 0.760240 0.760240 0.760240
	Ks 0.806860 0.806860 0.806860
	*/
	// attributes
	char name[E3D_MODEL_NAME_LENGTH];
	Colour diffuse;
	Colour ambient;
	Colour specular;
};


// a material collection
class E3D_MaterialLib
{
public:
	E3D_MaterialLib();
	virtual ~E3D_MaterialLib();

	int GetNumMaterials(void) {	return m_numMaterials; };
	int AddMaterial(E3D_Material *material);
	E3D_Material *GetMaterial(int index);
	int GetMaterialIndex(char *name);
	void SetMaterial(int index, E3D_Material *material);
	int LoadMTL(char *filename);

private:
	int m_numMaterials;
	E3D_Material m_material[E3D_MODEL_MAX_MATERIAL];
};

// A 3D Model
class E3D_Model
{
public:
	E3D_Model();
	virtual ~E3D_Model();

	// model file loading errors
	enum E3D_ModelError
		{
		FILE_OPEN_FAIL			= 2,
		BAD_COMMAND					= 3,
		EOF_BEFORE_END			= 4,
		TOO_MANY_VERTS			= 5,
		TOO_MANY_LINES			= 6,
		TOO_MANY_NORMS			= 7,
		TOO_MANY_POLYS			= 8,
		BAD_VERT_INDEX			= 9,
		BAD_TEXMAP_INDEX		= 10,
		BAD_TEX_VERT_INDEX	= 11
		};

	char m_name[E3D_MODEL_NAME_LENGTH];
	short m_numVerts, m_numNorms, m_numLines, m_numPolys;
	Vertex m_vert[MAX_VERTS_PER_MODEL];
	Vector m_norm[MAX_VERTS_PER_MODEL];
	Line m_line[MAX_LINES_PER_MODEL];
	Face m_poly[MAX_POLYS_PER_MODEL];
	short m_shade;							// flag for shading
	float m_maxdim;							// max dimension for inst clip
	Group m_group[40];
	int m_numGroups;

	void Reset();
	void SetName(char *name);
	char *GetName(void);
	int Load(char *filename);
	int Load3DO(char *filename);			// load from 3DO model file
	int LoadOBJ(char *filename);			// load from OBJ (wavefront) model file
	int CalculateNormals();					// recalculate the normals
	Vertex *AddVertex();
	Vertex *AddVertex(float x, float y, float z, float r, float g, float b);
	Vertex *AddVertex(const Vertex &v);
	Line *AddLine(int p1, int p2);
	Face *AddPoly();
	Face *AddPoly3(int p1, int p2, int p3);
	Face *AddPoly4(int p1, int p2, int p3, int p4);
	Vector *AddNormal(float x, float y, float z);
};

