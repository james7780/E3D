//////////////////////////////////////////////////////////////////////
//
// E3D_Model.cpp: implementation of the E3D_Model class.
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "string.h"
#include "../include/E3D.h"

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

static int debug = 0;
char mystring[256];

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
void E3D_Model::SetName(char *name)
{
	strncpy(m_name, name, E3D_MODEL_NAME_LENGTH);
	m_name[99] = 0;
}

/// Set the name of this model
char *E3D_Model::GetName(void)
{
	return m_name;
}

// Model Creation Functions

/// Add a vertex to this model
Vertex *E3D_Model::AddVertex()
{
	int n = m_numVerts;
	m_numVerts++;
	return &m_vert[n];
}

/// Add a vertex to this model
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
Vertex *E3D_Model::AddVertex(Vertex v)
{
	int n = m_numVerts;
	m_vert[n] = v;

	m_numVerts++;
	return &m_vert[n];
}

/// Add a normal to this model
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
Line *E3D_Model::AddLine(int p1, int p2)
{
	int n = m_numLines;
	m_line[n].vert1 = p1;
	m_line[n].vert2 = p2;

	m_numLines++;
	return &m_line[n];
}

/// Add a polygon (face) to this model
Face *E3D_Model::AddPoly()
{
	int n = m_numPolys;

	m_numPolys++;
	return &m_poly[n];
}

/// Add a polygon (face) to this model
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
int E3D_Model::Load(char *filename)
{
	// find file extension
	int len = strlen(filename);
	char ext[5];
	strcpy(ext, filename + (len - 4));

	if(0 == stricmp(ext, ".3DO")) {
		if(E3D_SUCCESS == Load3DO(filename)) {
			return E3D_SUCCESS;
		}
	}

	else if(0 == stricmp(ext, ".OBJ")) {
		if(E3D_SUCCESS == LoadOBJ(filename)) {
			return E3D_SUCCESS;
		}
	}

	return E3D_FAIL;
}

/// Reset model data to an empty model
void E3D_Model::Reset() {
	int i, j;
    m_numVerts = 0;
	m_numLines = 0;
    m_numPolys = 0;
	m_numNorms = 0;

    // Really only important to reset poly data (colours etc)
    for(i=0; i < MAX_POLYS_PER_MODEL; i++) {
    	m_poly[i].type = 0;
    	m_poly[i].shade = 0;
    	m_poly[i].gouraud = 0;
    	m_poly[i].filling = 0;
   		m_poly[i].colour.r = 0.0;
   		m_poly[i].colour.g = 0.0;
   		m_poly[i].colour.b = 0.0;
        for(j=0; j < 6; j++) {
    		m_poly[i].vcol[j].r = 0.0;
    		m_poly[i].vcol[j].g = 0.0;
    		m_poly[i].vcol[j].b = 0.0;
		}
	}	// next i

}

/// Calculate normals for this model
// ??? (amend to calc poly and vertex normals later)
int E3D_Model::CalculateNormals() {
	int i, p1, p2, p3;
    Vector tnorm;

	E3D_Utility util;

	for(i=0; i < m_numPolys; i++) {
		p1 = m_poly[i].p[0];
		p2 = m_poly[i].p[1];
		p3 = m_poly[i].p[2];
		util.cross_product_f(m_vert[p2].x - m_vert[p1].x,
            			m_vert[p2].y - m_vert[p1].y,
                        m_vert[p2].z - m_vert[p1].z,
                        m_vert[p2].x - m_vert[p3].x,
                        m_vert[p2].y - m_vert[p3].y,
                        m_vert[p2].z - m_vert[p3].z,
                        &tnorm.x, &tnorm.y, &tnorm.z);
		// check for bad normal
		if(tnorm.GetLength() < 0.00001)
			tnorm.Set(0.0f, 0.0f, 1.0f);
		util.normalize_Vector(&tnorm.x, &tnorm.y, &tnorm.z);
        // write model normals
        m_norm[i].x = tnorm.x;
        m_norm[i].y = tnorm.y;
        m_norm[i].z = tnorm.z;
        // set poly normal index
        m_poly[i].n[0] = i;
	}
    m_numNorms = i;
	return(i);
}

/// Load this model with data from a 3DO model file
int E3D_Model::Load3DO(char *filename) {

	int validcmd, validparam, end, n, i, j, v;
	int numverts, numnorms, numpolys;
	float x, y, z, r, g, b;
	char scmd[80], stemp[80];
	FILE *pfile;
	FILE *plog;

	if (debug)
		{
		plog = fopen("load3do.log", "w");
		fprintf(plog, "Opening model file %s\n", filename);
		fflush(plog);
		}

	pfile = fopen(filename, "r");
	// check if file open failed
	if(pfile == NULL) {
		if (debug) fprintf(plog, "Model file open failed.\n");
        return FILE_OPEN_FAIL;
	}

	// Reset model values
    Reset();

	// Parse model file
	end = FALSE;
	numverts = 0;
    numnorms = 0;
	numpolys = 0;
	while(!end) {
		// reset valid flags to trap errors
		validcmd = FALSE; validparam = FALSE;

		fscanf(pfile, "%s", &scmd[0]);
		if(debug) fprintf(plog, "Command is %s\n", scmd);
		if(scmd[0] == '!' || scmd[0] == '#') {
			validcmd = TRUE;
			if(debug) fprintf(plog, "Comment encountered. Scanning till eol.\n");
			// read till end of line (character 10)
			fscanf(pfile, "%c", &stemp[0]);
			while(stemp[0] != 10) fscanf(pfile, "%c", &stemp[0]);
		}
		if(strcmp(scmd, "vert") == 0) {
        	if(numverts == MAX_VERTS_PER_MODEL - 1 ) {
        		if (debug) fprintf(plog, "Error - too many verts! Exiting...\n");
				fclose(pfile);
				return TOO_MANY_VERTS;		// error
			}
			validcmd = TRUE;
			fscanf(pfile, "%f %f %f",
				&x, &y, &z);
			if(debug) fprintf(plog, "vert: xyz %3.3f %3.3f %3.3f\n", x, y, z);
			m_vert[numverts].x = x;
			m_vert[numverts].y = y;
			m_vert[numverts].z = z;
            // verts default colour
			m_vert[numverts].r = 0.5;
			m_vert[numverts].g = 0.5;
			m_vert[numverts].b = 0.5;
			numverts++;
		}

		if(strcmp(scmd, "norm") == 0) {
        	if(numnorms == MAX_VERTS_PER_MODEL - 1 ) {
        		if (debug) fprintf(plog, "Error - too many normals! Exiting...\n");
				fclose(pfile);
				return TOO_MANY_NORMS;		// error
			}
			validcmd = TRUE;
			fscanf(pfile, "%f %f %f",
				&x, &y, &z);
			if(debug) fprintf(plog, "norm: xyz %3.3f %3.3f %3.3f\n", x, y, z);
			m_norm[numnorms].x = x;
			m_norm[numnorms].y = y;
			m_norm[numnorms].z = z;
			numnorms++;
		}

		if(strcmp(scmd, "poly") == 0) {
        	if(numpolys == MAX_POLYS_PER_MODEL - 1 ) {
        		if (debug) fprintf(plog, "Error - too many polys! Exiting...\n");
				fclose(pfile);
				return TOO_MANY_POLYS;		// error
			}
			validcmd = TRUE;
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
			fscanf(pfile, "%d", &n);
			if(debug) fprintf(plog, "poly: %d verts   ", n);
			m_poly[numpolys].numverts = n;
			// read in vert indices
			for(i=0; i<n; i++) {
				fscanf(pfile, "%d", &v);
				if (debug) fprintf(plog, "%d ", v);
				m_poly[numpolys].p[i] = v-1;
			}
			// read in texture name
			fscanf(pfile, "%s", &stemp[0]);
			if(debug) fprintf(plog, "%s\n", stemp);
			// set texture index here
			numpolys++;
		}

        // colour for poly and for verts
		if(strcmp(scmd, "colour") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			fscanf(pfile, "%d %f %f %f",
				&n, &r, &g, &b);
			if(debug) fprintf(plog, "poly %d colour %d   %3.3f %3.3f %3.3f\n", j, n, r, g, b);
			if(n == 0) {
				m_poly[j].colour.r = r;
				m_poly[j].colour.g = g;
				m_poly[j].colour.b = b;
				m_poly[j].vcol[0].r = r;
				m_poly[j].vcol[0].g = g;
				m_poly[j].vcol[0].b = b;
			}
            else {
            	if(n <= m_poly[j].numverts) {
					m_poly[j].vcol[n-1].r = r;
					m_poly[j].vcol[n-1].g = g;
					m_poly[j].vcol[n-1].b = b;
				}
                else {
			    	if (debug) fprintf(plog, "Error - vertex index exceeds poly numverts! Exiting...\n");
					fclose(pfile);
					return BAD_VERT_INDEX;		// error
				}
			}
		}

        // texture map ("texture page") index
		if(strcmp(scmd, "texture") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			fscanf(pfile, "%d", &n);
			if(debug) fprintf(plog, "poly %d texture %d\n", j, n);
            	if((n > -1) && (n < 16)) {
					m_poly[j].texmap = n;
				}
                else {
			    	if (debug) fprintf(plog, "Error - texture map index out of bounds! Exiting...\n");
					fclose(pfile);
					return BAD_TEXMAP_INDEX;		// error
				}
		}

        // texture co-ordinates
		if(strcmp(scmd, "texvert") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			fscanf(pfile, "%d %f %f",
				&n, &x, &y);
			if(debug) fprintf(plog, "poly %d texvert %d   %3.1f %3.1f\n", j, n, x, y);
            	if(n <= m_poly[j].numverts) {
					m_poly[j].texp[n].x = x;
					m_poly[j].texp[n].y = y;
				}
                else {
			    	if (debug) fprintf(plog, "Error - texture vertex index exceeds poly numverts! Exiting...\n");
					fclose(pfile);
					return BAD_TEX_VERT_INDEX;		// error
				}
		}

		if(strcmp(scmd, "filling") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			fscanf(pfile, "%d",
				&n);
			if(debug) fprintf(plog, "poly %d filling %d\n",	j, n);
			m_poly[j].filling = n;
		}

		if(strcmp(scmd, "shade") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			fscanf(pfile, "%d",
				&n);
			if(debug) fprintf(plog, "poly %d shade %d\n", j, n);
			m_poly[j].shade = n;
		}

		if(strcmp(scmd, "gouraud") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			fscanf(pfile, "%d",
				&n);
			if(debug) fprintf(plog, "poly %d gouraud %d\n", j, n);
			m_poly[j].gouraud = n;
		}

		if(strcmp(scmd, "end") == 0) {
			validcmd = TRUE;
			if (debug) fprintf(plog, "End of model file.\n");
			end = TRUE;
			// set model parameters
			m_numVerts = numverts;
            m_numNorms = numnorms;
			m_numPolys = numpolys;
			// we now specify name in E3D_Scene.AddModel(name) function
			//strcpy(m_name, filename);
			if(debug) fprintf(plog, "%d verts and %d polys loaded.\n", numverts, numpolys);
		}

		// check if no valid command found (ie: parse error)
		if(!validcmd) {
			if (debug) fprintf(plog, "Error - Model Command not recognised! Exiting...\n");
			fclose(pfile);
			return BAD_COMMAND;		// error
		}

  // check if EOF before end
  if(feof(pfile) && !end) {
			if (debug) fprintf(plog, "Error - EOF before end marker! Exiting...\n");
			fclose(pfile);
			return EOF_BEFORE_END;		// error
		}

	}

	fclose(pfile);

	if (debug) fclose(plog);

	CalculateNormals();
	return E3D_SUCCESS;		// success!
}


/// Load this model with data from a maya/alias wavefront OBJ model file
// note: fix return values
int E3D_Model::LoadOBJ(char *filename) {

	int validcmd, validparam, end, n, i, j;
    int group[5], numgroups, numactivegroups;
    int p[6];
	float x, y, z; //, r, g, b;
	char scmd[128], stemp[128];
    char sface[6][20];
    char strobj[128];
	char matlibpath[256];
	E3D_MaterialLib materialList;
	E3D_Material *currentMaterial = materialList.GetMaterial(0);
    char *pc;
	FILE *pfile;
	FILE *plog;

	if (debug)
		{
		plog = fopen("loadobj.log", "w");
		fprintf(plog, "Opening model file %s\n", filename);
		fflush(plog);
		}

	pfile = fopen(filename, "r");
	// check if file open failed
	if(pfile == NULL) {
		if (debug) fprintf(plog, "Model file open failed.\n");
        return FILE_OPEN_FAIL;
	}

	// Reset model values
    Reset();

	// Parse model file
	end = FALSE;
	int numverts = 0;
    int numnorms = 0;
	int numlines = 0;
	int numpolys = 0;
	int line = 0;
    group[0] = 0;
    group[1] = group[2] = group[3] = group[4] = -1;
    numgroups = 0;
    numactivegroups = 0;

	while(!end) {
		// reset valid flags to trap errors
		validcmd = FALSE; validparam = FALSE;

        // get next line of file
		pc = fgets(strobj, 128, pfile);
        if(pc == NULL) { end = TRUE; continue; }

		// DEBUG
        if (debug) fprintf(plog, "Line %d: %s \n", line, pc);

        // get command
		sscanf(pc, "%s", &scmd[0]);

        // comment
		if(scmd[0] == '#') {
			validcmd = TRUE;
			if (debug) fprintf(plog, "Comment encountered.\n");
		}

		// material lib
		if(strcmp(scmd, "mtllib") == 0) {
			validcmd = TRUE;
			sscanf(pc, "%s %s", &stemp[0], &stemp[0]);
			strcpy(matlibpath, filename);
			E3D_Utility::ReplaceFilename(matlibpath, stemp);	// make mat lib path same as the model path
			if(E3D_SUCCESS == materialList.LoadMTL(matlibpath)) {
				if (debug) fprintf(plog, "Material lib %s loaded\n", matlibpath);
			}
			else {
				// mtllib load failed
				if (debug) fprintf(plog, "Error: Failed to load Material lib %s !\n", matlibpath);
			}
		}

		// set current material
		if(strcmp(scmd, "usemtl") == 0) {
			validcmd = TRUE;
			sscanf(pc, "%s %s", &stemp[0], &stemp[0]);
			int index = materialList.GetMaterialIndex(stemp);
			currentMaterial = materialList.GetMaterial(index);
		}

        // colour/material for poly
		if(strcmp(scmd, "o") == 0) {
			validcmd = TRUE;
			sscanf(pc, "%s", &stemp[0]);
			if (debug) fprintf(plog, "Object name: %s\n", stemp);
			// we specify the model's name in the E3D_Scene.AddModel(name) command
			//strcpy(m_name, stemp);
		}

		if(strcmp(scmd, "v") == 0) {
        	if(numverts == MAX_VERTS_PER_MODEL - 1 ) {
        		if (debug) fprintf(plog, "Error - too many verts! Exiting...\n");
				fclose(pfile);
				return TOO_MANY_VERTS;		// error
			}
			validcmd = TRUE;
			sscanf(pc, "%s %f %f %f",
				&scmd[0], &x, &y, &z);
			if (debug) fprintf(plog, "vert %d:  %3.3f %3.3f %3.3f\n", numverts, x, y, z);
			m_vert[numverts].x = x;
			m_vert[numverts].y = y;
			m_vert[numverts].z = z;
            // verts default colour
			m_vert[numverts].r = 0.5;
			m_vert[numverts].g = 0.5;
			m_vert[numverts].b = 0.5;
			// set groups
            for(i=0; i<5; i++) {
            	m_vert[numverts].group[i] = group[i];
			}
			numverts++;
		}

		if(strcmp(scmd, "vn") == 0) {
        	if(numnorms == MAX_VERTS_PER_MODEL - 1 ) {
        		if (debug) fprintf(plog, "Error - too many normals! Exiting...\n");
				fclose(pfile);
				return TOO_MANY_NORMS;		// error
			}
			validcmd = TRUE;
			sscanf(pc, "%f %f %f", &x, &y, &z);
			if (debug) fprintf(plog, "norm: xyz %3.3f %3.3f %3.3f\n", x, y, z);
			m_norm[numnorms].x = x;
			m_norm[numnorms].y = y;
			m_norm[numnorms].z = z;
			numnorms++;
		}

        if((strcmp(scmd, "g") == 0) && (numgroups < 35)) {
			// Check if group already exist
			// if so, then set model number index of object
			validcmd = TRUE;
            numactivegroups = 0;
            group[0] = group[1] = group[2] = group[3] = group[4] = -1;		// inactive
			// clear input string data
   	        for(i=0; i<6; i++) strcpy(sface[i], "");
            // if no group names specified, use default
            if(strlen(pc) < 3) {
                // set input string 0 to default "all"
                strcpy(sface[0], "all");
			}
            // scan group names
	        sscanf(pc, "%s %s %s %s %s %s", &scmd[0], &sface[0][0],
     	      				&sface[1][0], &sface[2][0], &sface[3][0],
        	   				&sface[4][0]);
			for(i=0; i<5; i++) {
               	strcpy(stemp, "");
            	n = sscanf(sface[i], "%s", stemp);
            	if (debug) fprintf(plog, "groupname %d: %s %d\n", i, stemp, n);
               	if(n == 1) {			// valid string
                   	// check / add group
                    for(j=0; j<numgroups; j++) {
                    	if(strcmp(m_group[j].name, stemp) == 0) {
                       		group[numactivegroups++] = j;
							j = 999;
						}
					}
                    if(j == numgroups) {		// not found - add it
			           	strcpy(m_group[numgroups++].name, stemp);
            			group[numactivegroups++] = j;
					}
				}
                else {
                	i = 6;		// jump out of loop
				}
			}

            if (debug) fprintf(plog, "Active groups: %d\n", numactivegroups);
        }

		// lines
		if(strcmp(scmd, "l") == 0) {
        	if(numpolys == MAX_LINES_PER_MODEL - 1 ) {
        		if (debug) fprintf(plog, "Error - too many lines! Exiting...\n");
				fclose(pfile);
				return TOO_MANY_LINES;		// error
			}
			validcmd = TRUE;
            // set line default values
            m_line[numlines].colour.r = currentMaterial->diffuse.r;
            m_line[numlines].colour.g = currentMaterial->diffuse.g;
            m_line[numlines].colour.b = currentMaterial->diffuse.b;
            // scan line data
           	sscanf(pc, "%s %d %d", &scmd[0], &p[0], &p[1]);
           	if (debug) fprintf(plog, "Line data: %d %d\n", p[0], p[1]);
            m_line[numlines].vert1 = p[0] - 1;
            m_line[numlines].vert2 = p[1] - 1;
          	//m_line[numlines].group = ???;
			numlines++;
		}

		// faces
		if(strcmp(scmd, "f") == 0) {
        	if(numpolys == MAX_POLYS_PER_MODEL - 1 ) {
        		if (debug) fprintf(plog, "Error - too many polys! Exiting...\n");
				fclose(pfile);
				return TOO_MANY_POLYS;		// error
			}
			validcmd = TRUE;
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
            for(i=0; i<6; i++) strcpy(sface[i], "@@@@@@@@");
            // scan face data
           	sscanf(pc, "%s %s %s %s %s %s %s", &scmd[0], &sface[0][0],
           				&sface[1][0], &sface[2][0], &sface[3][0],
           				&sface[4][0], &sface[5][0]);
			for(i=0; i<6; i++) {
            	n = sscanf(sface[i], "%d", &j);
            	if (debug) fprintf(plog, "Facedata %d: %s %d\n", i, sface[i], n);
                if(n) {			// valid integer scanned
                	p[i] = j - 1;			// set temp vert ptr
				}
                else {
                    m_poly[numpolys].numverts = i;
                    n = i;
                	i = 6;		// jump out of loop
				}
			}
            // reverse face
			for(i=0; i<n; i++) m_poly[numpolys].p[i] = p[n-1-i];
			// set groups
            for(i=0; i<5; i++) {
            	m_poly[numpolys].group[i] = group[i];
			}
			// calculate a normal for this poly

			// set texture index here
			numpolys++;
		}

        // texture co-ordinates
		if(strcmp(scmd, "vt") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			sscanf(pc, "%f %f", &x, &y);
			if (debug) fprintf(plog, "texvert %d   %3.1f %3.1f\n", n, x, y);
           	if(n <= m_poly[j].numverts) {
				m_poly[j].texp[n].x = x;
				m_poly[j].texp[n].y = y;
			}
             else {
		    	if (debug) fprintf(plog, "Error - texture vertex index exceeds poly numverts! Exiting...\n");
				fclose(pfile);
				return BAD_TEX_VERT_INDEX;		// error
			}
		}

        // smoothing group
		if(strcmp(scmd, "s") == 0) {
			validcmd = TRUE;
			if (debug) fprintf(plog, "Smoothing group - ignored.\n");
		}

		// check for unsupported command
		if(!validcmd) {
			if (debug) fprintf(plog, "Model command unsupported...\n");
		}

		// check if EOF
		if(feof(pfile)) {
        	end = TRUE;
		}

        line++;

	}

	fclose(pfile);
	// set model parameters
	m_numVerts = numverts;
    m_numNorms = numnorms;
	m_numLines = numlines;
	m_numPolys = numpolys;
    m_numGroups = numgroups;
	if (debug) fprintf(plog, "%d verts, %d lines and %d polys loaded.\n", numverts, numlines, numpolys);

	// if no polys or lines, then generate lines between consecutive verts
	if(numverts > 0 && 0 == numlines && 0 == numpolys)
		{
		for(i = 0; i < numverts-1; i += 2)
			{
            m_line[numlines].colour.r = currentMaterial->diffuse.r;
            m_line[numlines].colour.g = currentMaterial->diffuse.g;
            m_line[numlines].colour.b = currentMaterial->diffuse.b;
            // line vert indices
            m_line[numlines].vert1 = i;
            m_line[numlines].vert2 = i+1;
          	//m_line[numlines].group = ???;
			numlines++;
			}
		m_numLines = numlines;
		if (debug) fprintf(plog, "%d lines generated from vertex data.\n", numlines);
		}

	// calculate normals in case they are missing
	CalculateNormals();

    // DEBUG - print groups
    for(i=0; i<numgroups; i++) {
    	if (debug) fprintf(plog, "group %d: name %s\n", i, m_group[i].name);
	}

	if (debug) fclose(plog);

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
	if(index >= 0 && index < m_numMaterials)
		return &m_material[index];
	else
		return NULL;
}

/// Get the index of the material named 'name'
int E3D_MaterialLib::GetMaterialIndex(char *name)
{
	for(int i = 0; i<m_numMaterials; i++) {
		if(0 == strcmp(name, m_material[i].name)) {
			return i;
		}
	}

	return 0; // default if not found
}

/// Set the material at position index in the material list
void E3D_MaterialLib::SetMaterial(int index, E3D_Material *material)
{
	if(index >= 0 && index < m_numMaterials) {
		m_material[index] = *material;
	}
}

/// Add a material to the material list
/// @return the index of the newly-added material
int E3D_MaterialLib::AddMaterial(E3D_Material *material)
{
	if(m_numMaterials < E3D_MODEL_MAX_MATERIAL) {
		m_material[m_numMaterials] = *material;
		m_numMaterials++;
		return (m_numMaterials - 1);
	}
	else {
		return -1;
	}
}


// Load this material lib from a wavefront .MTL file
// note: fix return values
int E3D_MaterialLib::LoadMTL(char *filename) {

	int validcmd, validparam, end, line;
	float r, g, b;
	char scmd[128], stemp[128];
    char strobj[128];
	E3D_MaterialLib materialList;
	E3D_Material material;
    char *pc;
	FILE *pfile;
	FILE *plog;

	if (debug)
		{
		plog = fopen("loadmtl.log", "w");
		fprintf(plog, "Opening MTL material file %s\n", filename);
		fflush(plog);
		}

	pfile = fopen(filename, "r");
	// check if file open failed
	if(pfile == NULL) {
		if (debug) fprintf(plog, "Material file open failed.\n");
        return E3D_FAIL;
	}

	// Reset material values
    m_numMaterials = 0;

	// Parse model file
	line = 0;
	end = FALSE;
	while(!end) {
		// reset valid flags to trap errors
		validcmd = FALSE; validparam = FALSE;

        // get next line of file
		pc = fgets(strobj, 128, pfile);
        if(pc == NULL) { end = TRUE; continue; }

		// DEBUG
        if (debug) fprintf(plog, "Line %d: %s", line, pc);

        // get command
		sscanf(pc, "%s", &scmd[0]);
		if (debug) fprintf(plog, "Command is %s\n", scmd);

        // comment
		if(scmd[0] == '#') {
			validcmd = TRUE;
			if (debug) fprintf(plog, "Comment encountered.\n");
		}

		// new material
		if(strcmp(scmd, "newmtl") == 0) {
			validcmd = TRUE;
			sscanf(pc, "%s %s", &stemp[0], &stemp[0]);
			if (debug) fprintf(plog, "Material %d - %s\n", m_numMaterials, stemp);
			// set material name
			strcpy(material.name, stemp);
			// set up default values
			material.diffuse.r = material.diffuse.g = material.diffuse.b = 0.5;
			material.ambient.r = material.ambient.g = material.ambient.b = 0.0;
			material.specular.r = material.specular.g = material.specular.b = 1.0;
			AddMaterial(&material);
		}


        // diffuse colour for the current material
		if(strcmp(scmd, "Kd") == 0) {
			validcmd = TRUE;
			sscanf(pc, "%s %f %f %f", &stemp[0], &r, &g, &b);
			if (debug) fprintf(plog, "diffuse: %f %f %f\n", r, g, b);
			E3D_Material *pMaterial = &m_material[m_numMaterials-1];
			pMaterial->diffuse.r = r;
			pMaterial->diffuse.g = g;
			pMaterial->diffuse.b = b;
		}

		// TODO: get ambient and specular colours

		// check if EOF
		if(feof(pfile)) {
        	end = TRUE;
		}

        line++;

	}

	fclose(pfile);

	if (debug) fprintf(plog, "%d materials loaded.\n", m_numMaterials);
	if (debug) fclose(plog);

	return E3D_SUCCESS;		// success!
}

