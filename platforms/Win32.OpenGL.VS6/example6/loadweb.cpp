//////////////////////////////////////////////////////////////////////
//
// Helper functions to load a tempest web
//
// Copyright 2006 James Higgs
// 
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>

#include "../../../include/E3D.h"
using namespace E3D;

#include "tempest.h"

static char mystring[256];

bool GotoSection(FILE *pf, char *sectionName) {
	char buffer[256];

	// rewind file pointer
	fseek(pf, 0, SEEK_SET);

	while(!feof(pf)) {
		fscanf(pf, "%s", buffer);
		if(0 == strcmp(sectionName, buffer)) {
			return true;
		}
	}

	return false;
}

/// Get the filename for a web, given it's index number
int	GetWebFilename(int webIndex, char *filename) {

	sprintf(filename, "webs/%02d.web", webIndex);
	return 0;
}


/// Load this web model with data from a 3DO model file
int LoadWeb(char *filename, TUBEDATA *tube) {

	int end;
	int numverts;
	float a;
	Vector v;
	char buffer[256];
	char buffer2[256];
	FILE *pfile;
	FILE *plog;

	plog = fopen("loadweb.log", "w");
	fprintf(plog, "Opening web file %s\n", filename);
	fflush(plog);

	pfile = fopen(filename, "r");
	// check if file open failed
	if(pfile == NULL) {
		fprintf(plog, "Web file open failed.\n");
        return E3D_FAIL;
	}

	// find start pt section in web file
	if(!GotoSection(pfile, "[start]")) {
		fprintf(plog, "Could not find [start] section in web file.\n");
		fclose(pfile);
		fclose(plog);
        return E3D_FAIL;
	}

	fscanf(pfile, "%s %s %f", buffer, buffer2, &a);
	v.x = a;
	fscanf(pfile, "%s %s %f", buffer, buffer2, &a);
	v.y = a;

	numverts = 0;
	tube->vertex[0] = v;
	numverts++;

	// find web section in web file
	if(!GotoSection(pfile, "[web]")) {
		fprintf(plog, "Could not find [web] section in web file.\n");
		fclose(pfile);
		fclose(plog);
        return E3D_FAIL;
	}

	// Reset web
	tube->numVertices = 0;
	tube->closed = false;
	tube->startingSegment = 0;

	// Parse web file
	end = FALSE;
	while(!end) {
		fscanf(pfile, "%s %s %f", buffer, buffer2, &a);
		fprintf(plog, "%s %s %f\n", buffer, buffer2, a);
		if(a == -9999) {
			end = TRUE;
		}
		else {
			v.x += (float)cos(DEG2RAD(a));
			v.y += (float)sin(DEG2RAD(a));
			tube->vertex[numverts] = v;
			numverts++;
		}
	}

	tube->numVertices = numverts;
	// is this tube closed (ie: last vert ~= first vert?)
	v = tube->vertex[numverts-1] - tube->vertex[0];
	if(v.GetLength() < 0.1f)
		tube->closed = true;

	fclose(pfile);
	fclose(plog);

	return E3D_SUCCESS;		// success!
}


