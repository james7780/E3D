//////////////////////////////////////////////////////////////////////
//
// E3D_Model.cpp: implementation of the E3D_Model class.
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "string.h"
#include <fstream>				// new io
#include "../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;
using namespace std;

// Set this to 0 to disable the model/material loading debug output
static int debug = 1;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

E3D_Model::E3D_Model()
{
	Reset();
}

E3D_Model::~E3D_Model()
{

}

/// Set the name of this model
/// @param[in] name				The name of the model
void E3D_Model::SetName(char *name)
{
	strncpy(m_name, name, E3D_MODEL_NAME_LENGTH);
	m_name[99] = 0;
}

/// Get the name of this model
/// @return				The name of the model
char *E3D_Model::GetName(void)
{
	return m_name;
}

// Model Creation Functions

/// Add a vertex to this model
/// @return				A pointer to the new vertex
Vertex *E3D_Model::AddVertex()
{
	int n = m_numVerts;
	m_numVerts++;
	return &m_vert[n];
}

/// Add a vertex to this model
/// @param x			X ordinate of vertex
/// @param y			Y ordinate of vertex
/// @param z			Z ordinate of vertex
/// @param r			Red component of vertex colour
/// @param g			Green component of vertex colour
/// @param b			Blue component of vertex colour
/// @return				A pointer to the new vertex
Vertex *E3D_Model::AddVertex(float x, float y, float z, float r, float g, float b)
{
	int n = m_numVerts;
	m_vert[n].x = x;
	m_vert[n].y = y;
	m_vert[n].z = z;
	m_vert[n].r = r;
	m_vert[n].g = g;
	m_vert[n].b = b;

	m_numVerts++;
	return &m_vert[n];
}

/// Add a vertex to this model
/// @param v			Vertex data to copy into the new vertex
/// @return				A pointer to the new vertex
Vertex *E3D_Model::AddVertex(const Vertex &v)
{
	int n = m_numVerts;
	m_vert[n] = v;

	m_numVerts++;
	return &m_vert[n];
}

/// Add a normal to this model
/// @param x			X component of normal
/// @param y			Y component of normal
/// @param z			Z component of normal
/// @return				A pointer to the new normal
Vector *E3D_Model::AddNormal(float x, float y, float z)
{
	int n = m_numNorms;
	m_norm[n].x = x;
	m_norm[n].y = y;
	m_norm[n].z = z;

	m_numNorms++;
	return &m_norm[n];
}

/// Add a line to this model
/// @param p1			Index of start vertex of this line in the model
/// @param p2			Index of end vertex of this line in the model
Line *E3D_Model::AddLine(int p1, int p2)
{
	int n = m_numLines;
	m_line[n].vert1 = p1;
	m_line[n].vert2 = p2;

	m_numLines++;
	return &m_line[n];
}

/// Add a polygon (face) to this model
/// @return				Pointer to the new face
Face *E3D_Model::AddPoly()
{
	int n = m_numPolys;

	m_numPolys++;
	return &m_poly[n];
}

/// Add a polygon (face) to this model
/// @return				Pointer to the new face
Face *E3D_Model::AddPoly3(int p1, int p2, int p3)
{
	int n = m_numPolys;
	m_poly[n].numverts = 3;
	m_poly[n].p[0] = p1;
	m_poly[n].p[1] = p2;
	m_poly[n].p[2] = p3;
	m_poly[n].colour.r = 0.5;
	m_poly[n].colour.g = 0.5;
	m_poly[n].colour.b = 0.5;
	m_numPolys++;
	return &m_poly[n];
}

/// Add a polygon (face) to this model
/// @return				Pointer to the new face
Face *E3D_Model::AddPoly4(int p1, int p2, int p3, int p4)
{
	int n = m_numPolys;
	m_poly[n].numverts = 4;
	m_poly[n].p[0] = p1;
	m_poly[n].p[1] = p2;
	m_poly[n].p[2] = p3;
	m_poly[n].p[3] = p4;
	m_poly[n].colour.r = 0.5;
	m_poly[n].colour.g = 0.5;
	m_poly[n].colour.b = 0.5;
	m_numPolys++;
	return &m_poly[n];
}

/// Populate this 3D model from a file
/// @param filename				Path to the model file
/// @return								E3D_SUCCESS or E3D_FAIL
int E3D_Model::Load(char *filename)
{
	// find file extension
	int len = strlen(filename);
	char ext[5];
	strcpy(ext, filename + (len - 4));

	if (0 == stricmp(ext, ".3DO"))
		{
		if (E3D_SUCCESS == Load3DO(filename))
			{
			return E3D_SUCCESS;
			}
		}

	else if (0 == stricmp(ext, ".OBJ"))
		{
		if (E3D_SUCCESS == LoadOBJ(filename))
			{
			return E3D_SUCCESS;
			}
		}

	return E3D_FAIL;
}

/// Reset model data to an empty model
void E3D_Model::Reset()
{
	int i, j;
	m_numVerts = 0;
	m_numLines = 0;
	m_numPolys = 0;
	m_numNorms = 0;

	// Really only important to reset poly data (colours etc)
	for (i = 0; i < MAX_POLYS_PER_MODEL; i++)
		{
		m_poly[i].type = 0;
		m_poly[i].shade = 0;
		m_poly[i].gouraud = 0;
		m_poly[i].filling = 0;
		m_poly[i].colour.r = 0.0;
		m_poly[i].colour.g = 0.0;
		m_poly[i].colour.b = 0.0;
		for (j = 0; j < 6; j++)
			{
			m_poly[i].vcol[j].r = 0.0;
			m_poly[i].vcol[j].g = 0.0;
			m_poly[i].vcol[j].b = 0.0;
			}
		}	// next i

}

/// Calculate normals for this model
// ??? (amend to calc poly and vertex normals later)
int E3D_Model::CalculateNormals()
{
	//E3D_Utility util;

	for (int i = 0; i < m_numPolys; i++)
		{
		int p1 = m_poly[i].p[0];
		int p2 = m_poly[i].p[1];
		int p3 = m_poly[i].p[2];
		Vector v1(m_vert[p2].x - m_vert[p1].x, m_vert[p2].y - m_vert[p1].y, m_vert[p2].z - m_vert[p1].z);
		Vector v2(m_vert[p2].x - m_vert[p3].x, m_vert[p2].y - m_vert[p3].y, m_vert[p2].z - m_vert[p3].z);
		Vector tnorm = v1.CrossProduct(v2);
		// check for bad normal
		if (tnorm.GetLength() < 0.00001)
			tnorm.Set(0.0f, 0.0f, 1.0f);
		tnorm.Normalise();
		// Set model normals
		m_norm[i] = tnorm;
		// Set poly normal index
		m_poly[i].n[0] = i;
		}

	m_numNorms = m_numPolys;
	return m_numNorms;
}

/// Load this model with data from a 3DO model file
/// @param[in] filename				.3do model file name
/// @return										E3D_SUCCESS if successful, else error code if failed
int E3D_Model::Load3DO(char *filename)
{
	int retval = E3D_SUCCESS;
	int n, i, j, v;

	ofstream *logFile = NULL; 
	if (debug)
		{
		logFile = new ofstream("load3do.log");
		*logFile << "Opening model file " << filename << '\n';
		//logFile->flush();
		}

	ifstream inputFile(filename);
	// check if file open failed
	if (inputFile.fail())
		{
		if (logFile)
			{
			*logFile << "Model file open failed.\n";
			logFile->close();
			delete logFile;
			}

		return FILE_OPEN_FAIL;
		}

	// Reset model values
	Reset();

	// Parse model file
	int numverts = 0;
	int numnorms = 0;
	int numpolys = 0;
	bool end = false;
	while (!end)
		{
		// reset valid flags to trap errors
		bool validCmd = false;
		//bool validParam = false;

		// get next line of file
		char data[512];
		inputFile.getline(data, 512);			// uses \n as end of line by default
		if (inputFile.eof())
			{
			end = true;
			continue;
			}

		// get command
		char *pc = data;
		char scmd[128];
		char stemp[128];
		sscanf_s(pc, "%s", &scmd[0], _countof(scmd));
		pc += strlen(scmd);

		if (logFile)
			*logFile << "Command is " << scmd << '\n';

		// Check for comment
		if (scmd[0] == '!' || scmd[0] == '#')
			{
			validCmd = true;
			if (logFile)
				*logFile << "Comment encountered. Scanning till eol.\n";
			}
		else if (strcmp(scmd, "vert") == 0)
			{
			if (numverts == MAX_VERTS_PER_MODEL - 1)
				{
				if (logFile)
					*logFile << "Error - too many verts! Exiting...\n";
				retval = TOO_MANY_VERTS;		// error
				break;
				}
			validCmd = true;
			float x, y, z;
			sscanf(pc, "%f %f %f", &x, &y, &z);
			if (logFile)
				*logFile << "vert: " << x << y << z << '\n';
			m_vert[numverts].x = x;
			m_vert[numverts].y = y;
			m_vert[numverts].z = z;
			// verts default colour
			m_vert[numverts].r = 0.5;
			m_vert[numverts].g = 0.5;
			m_vert[numverts].b = 0.5;
			numverts++;
			}
		else if (strcmp(scmd, "norm") == 0)
			{
			if (numnorms == MAX_VERTS_PER_MODEL - 1)
				{
				if (logFile)
					*logFile << "Error - too many normals! Exiting...\n";
				retval = TOO_MANY_NORMS;		// error
				break;
				}
			validCmd = true;
			float x, y, z;
			sscanf(pc, "%f %f %f", &x, &y, &z);
			if (logFile)
				*logFile << "norm: " << x << y << z << '\n';

			m_norm[numnorms].x = x;
			m_norm[numnorms].y = y;
			m_norm[numnorms].z = z;
			numnorms++;
			}
		else if (strcmp(scmd, "poly") == 0)
			{
			if (numpolys == MAX_POLYS_PER_MODEL - 1)
				{
				if (logFile)
					*logFile << "Error - too many polys! Exiting...\n";
				retval = TOO_MANY_POLYS;		// error
				break;
				}
			validCmd = true;
			// set poly default values
			m_poly[numpolys].shade = 0;
			m_poly[numpolys].gouraud = 0;
			m_poly[numpolys].filling = 0;
			m_poly[numpolys].colour.r = 0.5;
			m_poly[numpolys].colour.g = 0.5;
			m_poly[numpolys].colour.b = 0.5;
			// one normal per poly, for now...
			m_poly[numpolys].n[0] = numpolys;
			// get no of verts
			sscanf(pc, "%d", &n);
			if (logFile)
				*logFile << "poly: " << n << " verts\n";

			m_poly[numpolys].numverts = n;
			// read in vert indices
			for (i = 0; i < n; i++)
				{
				sscanf(pc, "%d", &v);
				if (logFile)
					*logFile << v << '\n';

				m_poly[numpolys].p[i] = v - 1;
				}
			// read in texture name
			sscanf_s(pc, "%s", &stemp[0], _countof(stemp));
			if (logFile)
				*logFile << stemp << '\n';
			// set texture index here
			numpolys++;
			}
		else if (strcmp(scmd, "colour") == 0)
			{
			// colour for poly and for verts
			validCmd = true;
			j = numpolys - 1;
			float r, g, b;
			sscanf(pc, "%d %f %f %f", &n, &r, &g, &b);
			if (logFile)
				*logFile << "poly " << j << " colour " << n << " RGB " << r << g << b << '\n';
			if (n == 0)
				{
				m_poly[j].colour.r = r;
				m_poly[j].colour.g = g;
				m_poly[j].colour.b = b;
				m_poly[j].vcol[0].r = r;
				m_poly[j].vcol[0].g = g;
				m_poly[j].vcol[0].b = b;
				}
			else
				{
				if (n <= m_poly[j].numverts)
					{
					m_poly[j].vcol[n - 1].r = r;
					m_poly[j].vcol[n - 1].g = g;
					m_poly[j].vcol[n - 1].b = b;
					}
				else
					{
					if (logFile)
						*logFile << "Error - vertex index exceeds poly numverts! Exiting...\n";
					retval = BAD_VERT_INDEX;		// error
					break;
					}
				}
			}
		else if (strcmp(scmd, "texture") == 0)
			{
			// texture map ("texture page") index
			validCmd = true;
			j = numpolys - 1;
			sscanf(pc, "%d", &n);
			if (logFile)
				*logFile << "poly " << j << " texture " << n << '\n';

			if ((n > -1) && (n < 16))
				{
				m_poly[j].texmap = n;
				}
			else
				{
				if (logFile)
					*logFile << "Error - texture map index out of bounds! Exiting...\n";
				retval = BAD_TEXMAP_INDEX;		// error
				break;
				}
			}
		else if (strcmp(scmd, "texvert") == 0)
			{
			// texture co-ordinates
			validCmd = true;
			j = numpolys - 1;
			float x, y;
			sscanf(pc, "%d %f %f", &n, &x, &y);
			if (logFile)
				*logFile << "poly " << j << " texvert " << n << ": " << x << y << '\n';
			if (n <= m_poly[j].numverts)
				{
				m_poly[j].texp[n].x = x;
				m_poly[j].texp[n].y = y;
				}
			else
				{
				if (logFile)
					*logFile << "Error - texture vertex index exceeds poly numverts! Exiting...\n";
				retval = BAD_TEX_VERT_INDEX;		// error
				break;
				}
			}
		else if (strcmp(scmd, "filling") == 0)
			{
			validCmd = true;
			j = numpolys - 1;
			sscanf(pc, "%d", &n);
			if (logFile)
				*logFile << "poly " << j << " filling " << n << '\n';
			m_poly[j].filling = n;
			}

		else if (strcmp(scmd, "shade") == 0)
			{
			validCmd = true;
			j = numpolys - 1;
			sscanf(pc, "%d", &n);
			if (logFile)
				*logFile << "poly " << j << " shade " << n << '\n';
			m_poly[j].shade = n;
			}
		else if (strcmp(scmd, "gouraud") == 0)
			{
			validCmd = true;
			j = numpolys - 1;
			sscanf(pc, "%d", &n);
			if (logFile)
				*logFile << "poly " << j << " gouraud " << n << '\n';
			m_poly[j].gouraud = n;
			}
		else if (strcmp(scmd, "end") == 0)
			{
			validCmd = true;
			if (logFile)
				*logFile << "End of model file.\n";
			end = TRUE;
			// set model parameters
			m_numVerts = numverts;
			m_numNorms = numnorms;
			m_numPolys = numpolys;
			// we now specify name in E3D_Scene.AddModel(name) function
			//strcpy(m_name, filename);
			if (logFile)
				*logFile << numverts << " verts and " << numpolys << " polys loaded.\n";
			}

		// check if no valid command found (ie: parse error)
		if (!validCmd)
			{
			if (logFile)
				*logFile << "Error - Model Command " << scmd << " not recognised! Exiting...\n";
			retval = BAD_COMMAND;		// error
			break;
			}

		// check if EOF before end
		if (inputFile.eof() && !end)
			{
			if (logFile)
				*logFile << "Error - EOF before end marker! Exiting...\n";
			retval = EOF_BEFORE_END;		// error
			break;
			}
		}

	inputFile.close();

	if (logFile)
		{
		logFile->close();
		delete logFile;
		}

	if (E3D_SUCCESS == retval)
		CalculateNormals();

	return retval;		// success!
}


/// Load this model with data from a maya/alias wavefront OBJ model file
/// @param[in] filename				.obj model file name
/// @return										E3D_SUCCESS if successful, else error code if failed
int E3D_Model::LoadOBJ(char *filename)
{
	E3D_MaterialLib materialList;
	E3D_Material *currentMaterial = materialList.GetMaterial(0);

	ofstream *logFile = NULL;
	if (debug)
		{
		logFile = new ofstream("loadobj.log");
		*logFile << "Opening model file " << filename << '\n';
		//logFile->flush();
		}

	ifstream inputFile(filename);
	// check if file open failed
	if (inputFile.fail())
		{
		if (logFile)
			*logFile << "Model file open failed.\n";

		return FILE_OPEN_FAIL;
		}

	// Reset model values
	Reset();

	// Parse model file
	int numverts = 0;
	int numnorms = 0;
	int numlines = 0;
	int numpolys = 0;
	int lineNumber = 0;
	int group[5];
	group[0] = 0;
	group[1] = group[2] = group[3] = group[4] = -1;
	int numgroups = 0;
	int numactivegroups = 0;
	int n = 0;
	bool end = false;
	while (!end)
		{
		// get next line of file
		char data[512];
		inputFile.getline(data, 512);			// uses \n as end of line by default
		if (inputFile.eof())
			{
			end = true;
			continue;
			}

		// DEBUG
		if (logFile)
			*logFile << "Line " << lineNumber << " loaded." << '\n';

		// reset valid flags to trap errors
		bool validCmd = false;
		//bool validParam = false;

		// get command
		char *pc = data;
		char scmd[128];
		char stemp[128];
		sscanf_s(pc, "%s", &scmd[0], _countof(scmd));

		// comment
		if (scmd[0] == '#')
			{
			validCmd = true;
			if (logFile)
				*logFile << "Comment encountered\n";
			}

		// material lib
		if (strcmp(scmd, "mtllib") == 0)
			{
			validCmd = true;
			sscanf_s(pc, "%s %s", &stemp[0], _countof(stemp), &stemp[0], _countof(stemp));
			char matlibpath[1024];
			strcpy(matlibpath, filename);
			E3D_Utility::ReplaceFilename(matlibpath, stemp);	// make mat lib path same as the model path
			if (E3D_SUCCESS == materialList.LoadMTL(matlibpath))
				{
				if (logFile)
					*logFile << "Material lib " << matlibpath << " loaded." << '\n';
				}
			else
				{
				// mtllib load failed
				if (logFile)
					*logFile << "Error: Failed to load Material lib " << matlibpath << '\n';
				}
			}

		// set current material
		if (strcmp(scmd, "usemtl") == 0)
			{
			validCmd = true;
			sscanf_s(pc, "%s %s", &stemp[0], _countof(stemp), &stemp[0], _countof(stemp));
			int index = materialList.GetMaterialIndex(stemp);
			currentMaterial = materialList.GetMaterial(index);
			}

		// colour/material for poly
		if (strcmp(scmd, "o") == 0)
			{
			validCmd = true;
			sscanf_s(pc, "%s", &stemp[0], _countof(stemp));
			if (logFile)
				*logFile << "Object name: " << stemp << '\n';

			// we specify the model's name in the E3D_Scene.AddModel(name) command
			//strcpy(m_name, stemp);
			}

		if (strcmp(scmd, "v") == 0)
			{
			if (numverts == MAX_VERTS_PER_MODEL - 1)
				{
				if (logFile)
					*logFile << "Error - too many verts! Exiting...\n";

				inputFile.close();
				return TOO_MANY_VERTS;		// error
				}
			validCmd = true;
			float x, y, z;
			sscanf_s(pc, "%s %f %f %f", &scmd[0], _countof(scmd), &x, &y, &z);
			if (logFile)
				*logFile << "vert " << numverts << ": " << x << y << z << '\n';

			m_vert[numverts].x = x;
			m_vert[numverts].y = y;
			m_vert[numverts].z = z;
			// verts default colour
			m_vert[numverts].r = 0.5;
			m_vert[numverts].g = 0.5;
			m_vert[numverts].b = 0.5;
			// set groups
			for (int i = 0; i < 5; i++)
				{
				m_vert[numverts].group[i] = group[i];
				}
			numverts++;
			}

		if (strcmp(scmd, "vn") == 0)
			{
			if (numnorms == MAX_VERTS_PER_MODEL - 1)
				{
				if (logFile)
					//fprintf(plog, "Error - too many normals! Exiting...\n");
					*logFile << "Error - too many normals! Exiting...\n";

				inputFile.close();
				return TOO_MANY_NORMS;		// error
				}
			validCmd = true;
			float x, y, z;
			sscanf(pc, "%f %f %f", &x, &y, &z);
			if (logFile)
				*logFile << "normal: " << x << y << z << '\n';

			m_norm[numnorms].x = x;
			m_norm[numnorms].y = y;
			m_norm[numnorms].z = z;
			numnorms++;
			}

		if ((strcmp(scmd, "g") == 0) && (numgroups < 35))
			{
			// Check if group already exist
			// if so, then set model number index of object
			validCmd = true;
			numactivegroups = 0;
			group[0] = group[1] = group[2] = group[3] = group[4] = -1;		// inactive
			// clear input string data
			char grpName[6][20];
			for (int i = 0; i < 6; i++)
				strcpy(grpName[i], "");
			// if no group names specified, use default
			if (strlen(pc) < 3)
				{
				// set input string 0 to default "all"
				strcpy(grpName[0], "all");
				}
			// scan group names
			sscanf(pc, "%s %s %s %s %s %s", &scmd[0], &grpName[0][0], &grpName[1][0], &grpName[2][0], &grpName[3][0], &grpName[4][0]);
			for (int i = 0; i < 5; i++)
				{
				strcpy(stemp, "");
				n = sscanf(grpName[i], "%s", stemp);
				if (logFile)
					*logFile << "groupname " << i << ": " << stemp << " " << n << '\n';

				if (1 == n)
					{			// valid string
					// check / add group
					int j = 0;
					for (j = 0; j < numgroups; j++)
						{
						if (strcmp(m_group[j].name, stemp) == 0)
							{
							group[numactivegroups++] = j;
							j = 999;
							}
						}
					if (j == numgroups)
						{		// not found - add it
						strcpy(m_group[numgroups++].name, stemp);
						group[numactivegroups++] = j;
						}
					}
				else
					{
					i = 6;		// jump out of loop
					}
				}

			if (debug)
				*logFile << "Active groups: " << numactivegroups << '\n';
			}

		// lines
		if (strcmp(scmd, "l") == 0)
			{
			if (numpolys == MAX_LINES_PER_MODEL - 1)
				{
				if (logFile)
					*logFile << "Error - too many lines! Exiting...\n";

				inputFile.close();
				return TOO_MANY_LINES;		// error
				}
			validCmd = true;
			// set line default values
			m_line[numlines].colour.r = currentMaterial->diffuse.r;
			m_line[numlines].colour.g = currentMaterial->diffuse.g;
			m_line[numlines].colour.b = currentMaterial->diffuse.b;
			// scan line data
			int p[2];
			sscanf_s(pc, "%s %d %d", &scmd[0], _countof(scmd), &p[0], &p[1]);
			if (logFile)
				*logFile << "Line data: " << p[0] << " to " << p[1] << '\n';
			m_line[numlines].vert1 = p[0] - 1;
			m_line[numlines].vert2 = p[1] - 1;
			//m_line[numlines].group = ???;
			numlines++;
			}

		// faces
		if (strcmp(scmd, "f") == 0)
			{
			if (numpolys == MAX_POLYS_PER_MODEL - 1)
				{
				if (logFile)
					*logFile << "Error - too many polys! Exiting...\n";
				inputFile.close();
				return TOO_MANY_POLYS;		// error
				}
			validCmd = true;
			// set poly default values
			m_poly[numpolys].shade = 0;
			m_poly[numpolys].gouraud = 0;
			m_poly[numpolys].filling = 0;
			m_poly[numpolys].colour.r = currentMaterial->diffuse.r;
			m_poly[numpolys].colour.g = currentMaterial->diffuse.g;
			m_poly[numpolys].colour.b = currentMaterial->diffuse.b;
			// one normal per poly, for now...
			m_poly[numpolys].n[0] = numpolys;
			// clear face data
			char sface[6][20];
			for (int i = 0; i < 6; i++)
				strcpy(sface[i], "@@@@@@@@");
			// scan face data
			sscanf(pc, "%s %s %s %s %s %s %s", &scmd[0], &sface[0][0], &sface[1][0], &sface[2][0], &sface[3][0], &sface[4][0], &sface[5][0]);
			int vertIndex[6];
			for (int i = 0; i < 6; i++)
				{
				int j = 0;
				n = sscanf(sface[i], "%d", &j);
				if (logFile)
					*logFile << "Facedata " << i << ": " << sface[i] << " " << n << '\n';
				if (n)
					{			// valid integer scanned
					vertIndex[i] = j - 1;			// set temp vert ptr
					}
				else
					{
					m_poly[numpolys].numverts = i;
					n = i;
					i = 6;		// jump out of loop
					}
				}
			// reverse face
			for (int i = 0; i < n; i++)
				m_poly[numpolys].p[i] = vertIndex[n - 1 - i];
			// set groups
			for (int i = 0; i < 5; i++)
				{
				m_poly[numpolys].group[i] = group[i];
				}
			// calculate a normal for this poly

			// set texture index here
			numpolys++;
			}

		// texture co-ordinates
		if (strcmp(scmd, "vt") == 0)
			{
			validCmd = true;
			int j = numpolys - 1;
			float x, y;
			sscanf(pc, "%f %f", &x, &y);
			if (logFile)
				*logFile << "textvert " << n << " - " << x << ", " << y << '\n';

			if (n <= m_poly[j].numverts)
				{
				m_poly[j].texp[n].x = x;
				m_poly[j].texp[n].y = y;
				}
			else
				{
				if (logFile)
					*logFile << "Error - texture vertex index exceeds poly numverts! Exiting...\n";

				inputFile.close();
				return BAD_TEX_VERT_INDEX;		// error
				}
			}

		// smoothing group
		if (strcmp(scmd, "s") == 0)
			{
			validCmd = true;
			if (logFile)
				*logFile << "Smoothing group - ignored.\n";
			}

		// check for unsupported command
		if (!validCmd)
			{
			if (logFile)
				*logFile << "Model command unsupported...\n";
			}

		lineNumber++;
		}

	inputFile.close();

	// set model parameters
	m_numVerts = numverts;
	m_numNorms = numnorms;
	m_numLines = numlines;
	m_numPolys = numpolys;
	m_numGroups = numgroups;
	if (logFile)
		*logFile << numverts << " verts, " << numlines << " lines and " << numpolys << " polys loaded.\n";

	// if no polys or lines, then generate lines between consecutive verts
	if (numverts > 0 && 0 == numlines && 0 == numpolys)
		{
		for (int i = 0; i < numverts - 1; i += 2)
			{
			m_line[numlines].colour.r = currentMaterial->diffuse.r;
			m_line[numlines].colour.g = currentMaterial->diffuse.g;
			m_line[numlines].colour.b = currentMaterial->diffuse.b;
			// line vert indices
			m_line[numlines].vert1 = i;
			m_line[numlines].vert2 = i + 1;
			//m_line[numlines].group = ???;
			numlines++;
			}
		m_numLines = numlines;
		if (logFile)
			*logFile << numlines << " lines generated from vertex data.\n";
		}

	// calculate normals in case they are missing
	CalculateNormals();

	// DEBUG - print groups
	for (int i = 0; i < numgroups; i++)
		{
		if (logFile)
			*logFile << "group " << i << ": name " << m_group[i].name << '\n';
		}

	if (logFile)
		{
		logFile->close();
		delete logFile;
		}

	return E3D_SUCCESS;		// success!
}




//////////////////////////////////////////////////////////////////////
//
// E3D_Material implementation
//
//////////////////////////////////////////////////////////////////////

E3D_Material::E3D_Material()
{
	// attributes
	strcpy(name, "*default");
	diffuse.r = 1.0;
	diffuse.g = 1.0;
	diffuse.b = 1.0;
	ambient.r = 0.0;
	ambient.g = 0.0;
	ambient.b = 0.0;
	specular.r = 1.0;
	specular.g = 1.0;
	specular.b = 1.0;
}

E3D_Material::~E3D_Material()
{

}

//////////////////////////////////////////////////////////////////////
//
// E3D_MaterialLib implementation
//
//////////////////////////////////////////////////////////////////////

E3D_MaterialLib::E3D_MaterialLib()
{
	m_numMaterials = 0;
	// set up a default material
	E3D_Material *material = new E3D_Material();
	AddMaterial(material);
}

E3D_MaterialLib::~E3D_MaterialLib()
{
	// TODO: clean up material 0
}

/// Get the material at position index in the material list
E3D_Material *E3D_MaterialLib::GetMaterial(int index)
{
	if (index >= 0 && index < m_numMaterials)
		return &m_material[index];
	else
		return NULL;
}

/// Get the index of the material named 'name'
int E3D_MaterialLib::GetMaterialIndex(char *name)
{
	for (int i = 0; i < m_numMaterials; i++)
		{
		if (0 == strcmp(name, m_material[i].name))
			{
			return i;
			}
		}

	return 0; // default if not found
}

/// Set the material at position index in the material list
void E3D_MaterialLib::SetMaterial(int index, E3D_Material *material)
{
	if (index >= 0 && index < m_numMaterials)
		{
		m_material[index] = *material;
		}
}

/// Add a material to the material list
/// @return the index of the newly-added material (or -1 on fail)
int E3D_MaterialLib::AddMaterial(E3D_Material *material)
{
	if (m_numMaterials < E3D_MODEL_MAX_MATERIAL)
		{
		m_material[m_numMaterials] = *material;
		m_numMaterials++;
		return (m_numMaterials - 1);
		}
	else
		{
		return -1;
		}
}


// Load this material lib from a wavefront .MTL file
// note: fix return values
int E3D_MaterialLib::LoadMTL(char *filename)
{
	ofstream *logFile = NULL;
	if (debug)
		{
		logFile = new ofstream("loadmtl.log");
		*logFile << "Opening MTL material file " << filename << '\n';
		//logFile->flush();
		}

	std::ifstream inputFile(filename, std::ios::in);
	// check if file open failed
	if (inputFile.fail())
		{
		if (logFile)
			*logFile << "Material file open failed.\n";
		return E3D_FAIL;
		}

	// Reset material values
	m_numMaterials = 0;

	// Parse model file
	int lineNumber = 0;
	bool end = false;
	while (!end)
		{
		// get next line of file
		char data[512];
		inputFile.getline(data, 512);			// uses \n as end of line by default
		if (inputFile.eof())
			{
			end = true;
			continue;
			}

		// DEBUG
		if (logFile)
			*logFile << "Line " << lineNumber << ": ";

		// get command
		bool validCmd = false;								// reset valid flags to trap errors
		//bool validParam = false;
		char *pc = data;
		char scmd[128];
		char stemp[128];
		sscanf_s(pc, "%s", &scmd[0], _countof(scmd));
		if (logFile)
			*logFile << "Command is " << scmd << '\n';

		// comment line?
		if (scmd[0] == '#')
			{
			validCmd = true;
			if (logFile)
				*logFile << "Comment encountered.\n";
			}

		// new material
		else if (0 == strcmp(scmd, "newmtl"))
			{
			validCmd = true;
			sscanf_s(pc, "%s %s", &stemp[0], _countof(stemp), &stemp[0], _countof(stemp));
			if (logFile)
				*logFile << "Material " << m_numMaterials << " - " << stemp << '\n';

			// set material name
			E3D_Material material;
			strcpy(material.name, stemp);
			// set up default values
			material.diffuse.r = material.diffuse.g = material.diffuse.b = 0.5;
			material.ambient.r = material.ambient.g = material.ambient.b = 0.0;
			material.specular.r = material.specular.g = material.specular.b = 1.0;
			AddMaterial(&material);
			}


		// diffuse colour for the current material
		else if (0 == strcmp(scmd, "Kd"))
			{
			validCmd = true;
			float r, g, b;
			sscanf_s(pc, "%s %f %f %f", &stemp[0], _countof(stemp), &r, &g, &b);
			if (logFile)
				*logFile << "diffuse: " << r << g << b << '\n';

			E3D_Material *pMaterial = &m_material[m_numMaterials - 1];
			pMaterial->diffuse.r = r;
			pMaterial->diffuse.g = g;
			pMaterial->diffuse.b = b;
			}

		// TODO: get ambient and specular colours
		lineNumber++;
		}

	if (logFile)
		*logFile << m_numMaterials << " materials loaded.\n";

	if (logFile)
		{
		logFile->close();
		delete logFile;
		}

	inputFile.close();

	return E3D_SUCCESS;		// success!
}

