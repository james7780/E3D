//////////////////////////////////////////////////////////////////////
//
// Helper functions to load a tempest web
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////

//#define DO_LOG

#include <stdio.h>
#include <string.h>

#include "../../../include/E3D.h"
using namespace E3D;

#include "webfest.h"

bool GotoSection(FILE *pf, char *sectionName)
{
	char buffer[256];

	// rewind file pointer
	fseek(pf, 0, SEEK_SET);

	while(!feof(pf))
		{
		//fscanf(pf, "%s", buffer);
		fgets(buffer, 256, pf);
		// remove \n off end
		int len = strlen(buffer);
		if(buffer[len-1] == '\n')
			buffer[len-1] = 0;
		// remove \r off end
		if(buffer[len-2] == '\r')
			buffer[len-2] = 0;
		if(0 == strcmp(sectionName, buffer))
			{
			return true;
			}
		}

	return false;
}

/// Get the filename for a web, given it's index number
int	GetWebFilename(int webIndex, char *filename)
{
	sprintf(filename, "webs/%02d.web", webIndex);
	return 0;
}


/// Load this web model with data from a 3DO model file
int LoadWeb(char *filename, TUBEDATA *tube)
{
	int end;
	int numverts;
	float a;
	Vector v;
	char buffer[256];
	FILE *pfile;

#ifdef DO_LOG
	FILE *plog;
	plog = fopen("loadweb.log", "w");
	fprintf(plog, "Opening web file %s\n", filename);
	fflush(plog);
#endif

	pfile = fopen(filename, "r");
	// check if file open failed
	if(pfile == NULL)
		{
#ifdef DO_LOG
		fprintf(plog, "Web file open failed.\n");
#endif
        return E3D_FAIL;
		}

	// load tube name
	strncpy(tube->name, "UNNAMED", TUBE_NAME_LENGTH);
	if(GotoSection(pfile, "[name]"))
		{
		//fscanf(pfile, "%s", buffer);
		fgets(buffer, 256, pfile);
		strncpy(tube->name, buffer, TUBE_NAME_LENGTH);
#ifdef DO_LOG
		fprintf(plog, "name: %s\n", tube->name);
#endif
		}
	else
		{
#ifdef DO_LOG
		fprintf(plog, "Could not find [name] section in web file.\n");
#endif
		}

	// find start pt section in web file
	if(GotoSection(pfile, "[start]"))
		{
		fscanf(pfile, "%f", &a);
		v.x = a;
		fscanf(pfile, "%f", &a);
		v.y = a;
		}
	else
		{
#ifdef DO_LOG
		fprintf(plog, "Error: Could not find [start] section in web file!\n");
		fclose(plog);
#endif
		fclose(pfile);
        return E3D_FAIL;
		}

	numverts = 0;
	tube->vertex[0] = v;
	numverts++;

	// find web section in web file
	if(!GotoSection(pfile, "[web]"))
		{
#ifdef DO_LOG
		fprintf(plog, "Error: Could not find [web] section in web file!\n");
		fclose(plog);
#endif
		fclose(pfile);
        return E3D_FAIL;
		}

	// Reset web
	tube->numVertices = 0;
	tube->closed = false;
	tube->startingSegment = 0;

	// Parse web file
	end = FALSE;
	while(!end)
		{
		fscanf(pfile, "%s", buffer);
		//fprintf(plog, "%s %s %f\n", buffer, buffer2, a);
		if(0 == stricmp(buffer, "end"))
			{
			end = TRUE;
			}
		else
			{
			a = (float)atof(buffer);
			v.x += (float)cos(DEG2RAD(a));
			v.y += (float)sin(DEG2RAD(a));
			tube->vertex[numverts] = v;
			numverts++;
			}
		}

	tube->numVertices = numverts;
#ifdef DO_LOG
	fprintf(plog, "Tube with %d segments read successfully :)\n", numverts);
#endif

	// is this tube closed (ie: last vert ~= first vert?)
	v = tube->vertex[numverts-1] - tube->vertex[0];
	if(v.GetLength() < 0.1f)
		{
		tube->closed = true;
#ifdef DO_LOG
		fprintf(plog, "Tube is closed.\n");
#endif
		}

	fclose(pfile);

#ifdef DO_LOG
	fclose(plog);
#endif

	return E3D_SUCCESS;		// success!
}


