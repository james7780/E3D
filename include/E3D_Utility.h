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

	static float vector_length_f(float x, float y, float z);
	static void normalize_Vector(float *x, float *y, float *z);
	static void cross_product_f(float x1, float y1, float z1, float x2, float y2, float z2, float *xout, float *yout, float *zout);
	static void cross_product_f(Vector v1, Vector v2, Vector *vout);
	static float polygon_z_normal_f(Vector *v1, Vector *v2, Vector *v3);
	static void ReplaceFilename(char *path, const char *newFilename);

};

