#pragma once
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
class CGLMaterial
{
public:
	CGLMaterial(void);
	virtual ~CGLMaterial(void);

	void Select();
	void SetAmbient(float am1, float am2, float am3, float am4);
	void SetDiffuse(float df1, float df2, float df3, float df4);
	void SetSpecular(float sp1, float sp2, float sp3, float sp4);
	void SetEmission(float em1, float em2, float em3, float em4);
	void SetShininess(float shininess);

protected:
	float m_vAmbient[4];
	float m_vDiffuse[4];
	float m_vSpecular[4];
	float m_vEmission[4];
	float m_fShininess;
};

