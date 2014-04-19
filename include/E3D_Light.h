//////////////////////////////////////////////////////////////////////
//
// E3D_Light.h: interface for the E3D_Light class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class E3D_Light  
{
public:
	E3D_Light();
	virtual ~E3D_Light();

	enum E3D_LightType {
		DISTANT = 0,				// "inifinite light" (parallel rays)
		OMNI = 1,					// point light source
		SPOT = 2					// spotlight (not supported yet)
	};

	void SetType(int type);
	int GetType(void);
	void SetColor(float r, float g, float b);
	void SetColor(Colour color);
	void SetColour(float r, float g, float b);
	void SetColour(Colour colour);
	Colour GetColour(void);
	Colour GetColor(void);
	void SetAmbient(float r, float g, float b);
	void SetAmbient(Colour color);
	Colour GetAmbient(void);
	void SetDirection(float dx, float dy, float dz);
	void SetDirection(Vector dirn);
	Vector GetDirection(void);
	//void PointAt();			// for future spotlight
	void MoveTo(float x1, float y1, float z1);
	void MoveTo(Vector posn);

private:
	int m_type;				// DISTANT / OMNI / 
	float x, y, z;			// position vector (OMNI) OR direction vector (unit vector) (DISTANT)
	Colour m_colour;		// diffuse colour component of this light
	Colour m_ambient;		// ambient colour component of this light

};

