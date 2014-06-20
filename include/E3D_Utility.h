//////////////////////////////////////////////////////////////////////
//
// E3D_Utility.h: interface for the E3D_Utility class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class E3D_Utility  
{
public:
	E3D_Utility();
	virtual ~E3D_Utility();

	static float VectorLengthf(float x, float y, float z);
	static void NormaliseVectorf(float *x, float *y, float *z);
	static void CrossProductf(float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout);
	static void CrossProductf(const Vector &v1, const Vector &v2, Vector &vout);
	static float PolygonZNormalf(Vector *v1, Vector *v2, Vector *v3);
	static void ReplaceFilename(char *path, const char *newFilename);

};

