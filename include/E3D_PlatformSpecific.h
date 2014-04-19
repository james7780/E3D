//////////////////////////////////////////////////////////////////////
//
// E3D_PlatformSpecific.h: interface for platform-specific stuff
//
//////////////////////////////////////////////////////////////////////

#pragma once

class E3D_PlatformSpecific  
{
public:
	E3D_PlatformSpecific();
	virtual ~E3D_PlatformSpecific();

	static void RenderScene(E3D_Scene *scene, float viewWidth, float viewHeight, bool clear);
	static void DrawModel(E3D_Model *pModel);


public:
	static E3D_Scene * m_pScene;

};

