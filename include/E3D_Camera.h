//////////////////////////////////////////////////////////////////////
//
// E3D_Camera.h: interface for the E3D_Camera class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class E3D_Camera  
{
public:
	E3D_Camera();
	virtual ~E3D_Camera();

	void SetCamera(const Vector &posn, const Vector &dirn, const Vector &up, float fov);
	void SetPosition(float x1, float y1, float z1);
	void SetPosition(const Vector &p);
	Vector GetPosition() const;
	void SetDirection(float x1, float y1, float z1);
	void SetDirection(const Vector &dir);
	Vector GetDirection() const;
	void SetUpVector(float x1, float y1, float z1);
	void SetUpVector(const Vector &up);
	Vector GetUpVector() const;
	void SetFOV(float fov);
	float GetFOV() const;
	void SetRoll(float r1);
	float GetRoll() const;

	void LookAt(float x1, float y1, float z1);
	void LookAt(const Vector &target);

	/// Get "look at" camera matrix
	void GetLookAtMx(float *mx);

private:
	Vector m_pos;					// position
	float m_roll;
	Vector m_dirn;				// direction
	Vector m_up;					// "up" vector
	float m_FOV;					// field of view (in radians, not degrees)
};

