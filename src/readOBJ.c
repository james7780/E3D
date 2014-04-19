// FOR REFERENCE ONLY - NOT PART OF E3D!!!

#include <stdio.h>
#include <string.h>
#include "e3d.h"

#define TRUE	1
#define FALSE	0

//extern void ResetModel(void);

// Load in a maya/alias wavefront OBJ model
// note: fix return values
int LoadOBJ(char *filename, ThreeDeeModel *pmodel) {

	int validcmd, validparam, end, n, i, j, line;
	int numverts, numnorms, numpolys, vert;
    int group[5], numgroups, numactivegroups;
    int p[6];
	float x, y, z, r, g, b;
	char scmd[128], stemp[128];
    char sface[6][20];
    char strobj[128];
    char *pc;
	FILE *pfile;

	fprintf(stderr, "Opening OBJ model file %s\n", filename);
	pfile = fopen(filename, "r");
	// check if file open failed
	if(pfile == NULL) {
		fprintf(stderr, "Model file open failed.\n");
        return(5);
	}

	// Reset model values
    //ResetModel();

	// Parse model file
	end = FALSE;
	numverts = 0;
    numnorms = 0;
	numpolys = 0;
	line = 0;
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
        fprintf(stderr, "Line %d: %s", line, pc);

        // get command
		sscanf(pc, "%s", &scmd[0]);
		fprintf(stderr, "Command is %s\n", scmd);

        // comment
		if(scmd[0] == '#') {
			validcmd = TRUE;
			fprintf(stderr, "Comment encountered.\n");
		}

        // colour/material for poly
		if(strcmp(scmd, "o") == 0) {
			validcmd = TRUE;
			sscanf(pc, "%s", &stemp[0]);
			fprintf(stderr, "Object name: %s\n", stemp);
			strcpy(pmodel->name, stemp);
		}

		if(strcmp(scmd, "v") == 0) {
        	if(numverts == MAX_VERTS_PER_MODEL - 1 ) {
        		fprintf(stderr, "Error - too many verts! Exiting...\n");
				fclose(pfile);
				return(6);		// error
			}
			validcmd = TRUE;
			sscanf(pc, "%s %f %f %f",
				&scmd[0], &x, &y, &z);
			fprintf(stderr, "vert %d:  %3.3f %3.3f %3.3f\n",
				numverts, x, y, z);
			pmodel->vert[numverts].x = x;
			pmodel->vert[numverts].y = y;
			pmodel->vert[numverts].z = z;
            // verts default colour
			pmodel->vert[numverts].r = 0.5;
			pmodel->vert[numverts].g = 0.5;
			pmodel->vert[numverts].b = 0.5;
			// set groups
            for(i=0; i<5; i++) {
            	pmodel->vert[numverts].group[i] = group[i];
			}
			numverts++;
		}

		if(strcmp(scmd, "vn") == 0) {
        	if(numnorms == MAX_VERTS_PER_MODEL - 1 ) {
        		fprintf(stderr, "Error - too many normals! Exiting...\n");
				fclose(pfile);
				return(6);		// error
			}
			validcmd = TRUE;
			sscanf(pc, "%f %f %f",
				&x, &y, &z);
			fprintf(stderr, "norm: xyz %3.3f %3.3f %3.3f\n",
				x, y, z);
			pmodel->norm[numnorms].x = x;
			pmodel->norm[numnorms].y = y;
			pmodel->norm[numnorms].z = z;
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
            	n = sscanf(sface[i], "%s", &stemp);
            	fprintf(stderr, "groupname %d: %s %d\n", i, stemp, n);
               	if(n == 1) {			// valid string
                   	// check / add group
                    for(j=0; j<numgroups; j++) {
                    	if(strcmp(pmodel->group[j].name, stemp) == 0) {
                       		group[numactivegroups++] = j;
							j = 999;
						}
					}
                    if(j == numgroups) {		// not found - add it
			           	strcpy(pmodel->group[numgroups++].name, stemp);
            			group[numactivegroups++] = j;
					}
				}
                else {
                	i = 6;		// jump out of loop
				}
			}

            fprintf(stderr, "Active groups: %d\n", numactivegroups);
        }

		if(strcmp(scmd, "f") == 0) {
        	if(numpolys == MAX_POLYS_PER_MODEL - 1 ) {
        		fprintf(stderr, "Error - too many polys! Exiting...\n");
				fclose(pfile);
				return(7);		// error
			}
			validcmd = TRUE;
            // set poly default values
			pmodel->poly[numpolys].shade = 0;
			pmodel->poly[numpolys].gouraud = 0;
            pmodel->poly[numpolys].filling = 0;
            pmodel->poly[numpolys].colour.r = 0.5;
            pmodel->poly[numpolys].colour.g = 0.5;
            pmodel->poly[numpolys].colour.b = 0.5;
            // one normal per poly, for now...
            pmodel->poly[numpolys].n[0] = numpolys;
			// clear face data
            for(i=0; i<6; i++) strcpy(sface[i], "@@@@@@@@");
            // scan face data
           	sscanf(pc, "%s %s %s %s %s %s %s", &scmd[0], &sface[0][0],
           				&sface[1][0], &sface[2][0], &sface[3][0],
           				&sface[4][0], &sface[5][0]);
			for(i=0; i<6; i++) {
            	n = sscanf(sface[i], "%d", &j);
            	fprintf(stderr, "Facedata %d: %s %d\n", i, sface[i], n);
                if(n) {			// valid integer scanned
                	p[i] = j - 1;			// set temp vert ptr
				}
                else {
                    pmodel->poly[numpolys].numverts = i;
                    n = i;
                	i = 6;		// jump out of loop
				}
			}
            // reverse face
			for(i=0; i<n; i++) pmodel->poly[numpolys].p[i] = p[n-1-i];
			// set groups
            for(i=0; i<5; i++) {
            	pmodel->poly[numpolys].group[i] = group[i];
			}
			// calculate a normal for this poly

			// set texture index here
			numpolys++;
		}

        // colour/material for poly
		if(strcmp(scmd, "matlib") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			sscanf(pc, "%s",
				&stemp[0]);
			fprintf(stderr, "material set to %s\n", stemp);
		}

        // texture co-ordinates
		if(strcmp(scmd, "vt") == 0) {
			validcmd = TRUE;
            j = numpolys - 1;
			sscanf(pc, "%f %f", &x, &y);
			fprintf(stderr, "texvert %d   %3.1f %3.1f\n", n, x, y);
           	if(n <= pmodel->poly[j].numverts) {
				pmodel->poly[j].texp[n].x = x;
				pmodel->poly[j].texp[n].y = y;
			}
             else {
		    	fprintf(stderr, "Error - texture vertex index exceeds poly numverts! Exiting...\n");
				fclose(pfile);
				return(2);		// error
			}
		}

        // smoothing group
		if(strcmp(scmd, "s") == 0) {
			validcmd = TRUE;
			fprintf(stderr, "Smoothing group - ignored.\n");
		}

		if(strcmp(scmd, "end") == 0) {
			validcmd = TRUE;
			fprintf(stderr, "End of model file.\n");
			end = TRUE;
			// set model parameters
			pmodel->numverts = numverts;
            pmodel->numnorms = numnorms;
			pmodel->numpolys = numpolys;
			strcpy(pmodel[0].name, filename);
			fprintf(stderr, "%d verts and %d polys loaded.\n", numverts, numpolys);
		}


		// check for unsupported command
		if(!validcmd) {
			fprintf(stderr, "Model command unsupported...\n");
		}

		// check if EOF
		if(feof(pfile)) {
        	end = TRUE;
		}

        line++;

	}

	fclose(pfile);
	// set model parameters
	pmodel->numverts = numverts;
    pmodel->numnorms = numnorms;
	pmodel->numpolys = numpolys;
    pmodel->numgroups = numgroups;
	fprintf(stderr, "%d verts and %d polys loaded.\n", numverts, numpolys);
    // DEBUG - print groups
    for(i=0; i<numgroups; i++) {
    	fprintf(stderr, "group %d: name %s\n", i,
        	pmodel->group[i].name);
	}
	return(0);		// success!
}
