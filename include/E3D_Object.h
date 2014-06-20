//////////////////////////////////////////////////////////////////////
//
// E3D_Object.h: interface for the E3D_Object class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class E3D_Object  
{
public:
	E3D_Object();
	E3D_Object(const E3D_Object& lhs);
	//void E3D_Object::operator=( const E3D_Object &rhs );
	E3D_Object &operator=( const E3D_Object &rhs );
	//E3D_Object(int modelNumber);
	virtual ~E3D_Object();

	Vector position; //float x, y, z;
	Vector rotation; //float rx, ry, rz;
	Vector scale;    //float sx, sy, sz;
	Vector velocity; //float vx, vy, vz;

	int modelNumber;
	class E3D_Model *model;
	class E3D_Object *parent;
	//short model, attach;
	short shade, active, visible, clipped;
	short preshade;
	short flags;							// user flags
	Colour pscolour[30];						// inst preshade flag and preshaded poly colours
	float dist, maxdist;
	float speed, maxspeed, accel;		// speed of motion
	float turn, maxturn;					// manouvreability
	short energy, energymax;
	short behaviour, counter;				// behaviour stuff
    short nextbehaviour, nextcounter;		// behaviour stuff
	short var1, var2, var3;					// behaviour stuff
	float tx, ty, tz;						// behaviour stuff
	short effect;							// special effect for instance

	int GetModelNumber(void) { return modelNumber; };
	E3D_Model *GetModel(void) { return model; };

	Vector GetExtents();
	void SetRotation(float rx1, float ry1, float rz1);
	void SetScale(float sx1, float sy1, float sz1);
	void SetPosition(float x1, float y1, float z1);
	void SetVelocity(float dx, float dy, float dz);

	void AttachTo(int objectnumber);
};

