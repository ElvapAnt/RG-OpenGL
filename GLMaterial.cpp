#include "pch.h"
#include "GLMaterial.h"

CGLMaterial::CGLMaterial(void)
{
	//default values
	m_vAmbient[0] = 0.2;
	m_vAmbient[1] = 0.2;
	m_vAmbient[2] = 0.2;
	m_vAmbient[3] = 1;
	m_vDiffuse[0] = 0.8;
	m_vDiffuse[1] = 0.8;
	m_vDiffuse[2] = 0.8;
	m_vDiffuse[3] = 1;
	m_vSpecular[0] = 0;
	m_vSpecular[1] = 0;
	m_vSpecular[2] = 0;
	m_vSpecular[3] = 1;
	m_vEmission[0] = 0;
	m_vEmission[1] = 0;
	m_vEmission[2] = 0;
	m_vEmission[3] = 1;
	m_fShininess = 64;
}

CGLMaterial::~CGLMaterial(void)
{
}

void CGLMaterial::Select()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, m_vAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_vDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_vSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, m_vEmission);
	glMaterialf(GL_FRONT, GL_SHININESS, m_fShininess);
}

void CGLMaterial::SetAmbient(float am1, float am2, float am3, float am4)
{
	m_vAmbient[0] = am1;
	m_vAmbient[1] = am2;
	m_vAmbient[2] = am3;
	m_vAmbient[3] = am4;
}

void CGLMaterial::SetDiffuse(float df1, float df2, float df3, float df4)
{
	m_vDiffuse[0] = df1;
	m_vDiffuse[1] = df2;
	m_vDiffuse[2] = df3;
	m_vDiffuse[3] = df4;
}

void CGLMaterial::SetSpecular(float sp1, float sp2, float sp3, float sp4)
{
	m_vSpecular[0] = sp1;
	m_vSpecular[1] = sp2;
	m_vSpecular[2] = sp3;
	m_vSpecular[3] = sp4;
}

void CGLMaterial::SetEmission(float em1, float em2, float em3, float em4)
{
	m_vEmission[0] = em1;
	m_vEmission[1] = em2;
	m_vEmission[2] = em3;
	m_vEmission[3] = em4;
}

void CGLMaterial::SetShininess(float shininess)
{
	m_fShininess = shininess;
}