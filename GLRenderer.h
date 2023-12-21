#pragma once
#include "GLMaterial.h"
class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);

	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase,

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 

	CGLMaterial* m_roomMaterial;
	CGLMaterial* m_baseMaterial;

	CGLMaterial* m_lightBlueMaterial;
	CGLMaterial* m_pinkMaterial;
	
	CGLMaterial* m_redSphereMaterial;
	CGLMaterial* m_greenSphereMaterial;
	CGLMaterial* m_blueSphereMaterial;
	CGLMaterial* m_pinkSphereMaterial;

	CGLMaterial* axisMaterial;

	const int rLight = GL_LIGHT1;
	const int pLight = GL_LIGHT2;
	const int bLight = GL_LIGHT3;

protected: 
	void DrawSphere(bool isHalf, double r, int nSegAlpha, int nSegBeta);
	void DrawCylinderRegular(double r, double h, int nSeg);
	void DrawAxis(double length);

	//lab 5 functions
	void DrawPiece(double r1, double r2, double height, int nSeg);
	void DrawObject();
	void DrawBase();
	void DrawNormals(double r1, double r2, double height, int nSeg, double normalLength);

	void DrawWall(double width, double height, int nSeg);
	void DrawRoom(double width, double height, double depth);

	//lighting
	void SetupLighting();
	void SetupMaterial();

	void RedLight();
	void PinkLight();
	void BlueLight();

public:

	float m_yaw = 0.0f;
	float m_pitch = 0.0f;
	bool displayNormals = false;
	bool redLight = false;
	bool pinkLight = false;
	bool blueLight = false;
};

