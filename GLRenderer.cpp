#include "pch.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#define _USE_MATH_DEFINES
#include<math.h>
#include <iostream>
#include <stdlib.h>
//#pragma comment(lib, "GL\\glut32.lib")

CGLRenderer::CGLRenderer(void)
{
	m_yaw = 0;
	m_pitch = 0;
	m_roomMaterial = new CGLMaterial();
	m_baseMaterial = new CGLMaterial();
	m_lightBlueMaterial = new CGLMaterial();
	m_pinkMaterial = new CGLMaterial();
	m_redSphereMaterial = new CGLMaterial();
	m_pinkSphereMaterial = new CGLMaterial();
	m_blueSphereMaterial = new CGLMaterial();
	axisMaterial = new CGLMaterial();
	SetupMaterial();
}

CGLRenderer::~CGLRenderer(void)
{
	delete m_roomMaterial;
	delete m_baseMaterial;
	delete m_lightBlueMaterial;
	delete m_pinkMaterial;
	delete m_redSphereMaterial;
	delete m_greenSphereMaterial;
	delete m_blueSphereMaterial;
	delete m_pinkSphereMaterial;
	delete axisMaterial;
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);

	if (!bResult) return false;

	m_hrc = wglCreateContext(pDC->m_hDC);

	if (!m_hrc) return false;

	return true;
}

void CGLRenderer::PrepareScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClearColor(0, 0, 0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
	GLfloat lmodel_ambient[] = { 0.9, 0.9, 0.9, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float distance = 6.5;
	float azimuthRad = m_pitch * (M_PI / 180.0f);
	float elevationRad = m_yaw * (M_PI / 180.0f);
	float x = distance * cos(elevationRad) * sin(azimuthRad);
	float y = distance * sin(elevationRad);
	float z = distance * cos(elevationRad) * cos(azimuthRad);
	glLoadIdentity();
	gluLookAt(x, y, z,
		0.0, 2.0, 0.0,
		0.0, 1.0, 0.0);

	SetupLighting();

	glDisable(GL_LIGHTING);
	DrawAxis(4.5);
	glEnable(GL_LIGHTING);

	RedLight();
	PinkLight();
	BlueLight();

	glPushMatrix();
	glTranslatef(0,5, 0);
	m_roomMaterial->Select();
	glColor3f(0.8, 0.8, 0.8);
	DrawRoom(10,10,10);
	glPopMatrix();

	m_baseMaterial->Select();
	DrawBase();

	glPushMatrix();
	glTranslatef(0, 1.7, 0);
	DrawObject();
	glPopMatrix();

	glFlush();

	//---------------------------------
	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (double)w / (double)h, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	wglMakeCurrent(NULL, NULL);
	if (m_hrc)
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawSphere(bool isHalf, double r, int nSegAlpha, int nSegBeta)
{
	int jloop = nSegBeta;
	if (isHalf) jloop = nSegBeta/2;
	for (int i = 0; i <= nSegAlpha; i++) {
		double lat0 = M_PI * (-0.5 + (double)(i - 1) / nSegAlpha);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = M_PI * (-0.5 + (double)i / nSegAlpha);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= jloop; j++) {
			double lng = 2 * M_PI * (double)j / nSegBeta;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3d(x * zr0, y * zr0, z0);
			glVertex3d(r * x * zr0, r * y * zr0, r * z0);
			glNormal3d(x * zr1, y * zr1, z1);
			glVertex3d(r * x * zr1, r * y * zr1, r * z1);
		}
		glEnd();
	}
}

void CGLRenderer::DrawCylinderRegular(double r, double h, int nSeg)
{
	double step = 2 * M_PI / nSeg;
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		double x = cos(angle);
		double z = sin(angle);

		glNormal3d(x, 0.0, z);
		glVertex3d(r * x, h, r * z);
		glVertex3d(r * x, 0.0, r * z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, h, 0.0);
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		glVertex3d(r * cos(angle), h, r * sin(angle));
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, -1.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	for (int i = 0; i <= nSeg; i++) {
		double angle = i * step;
		glVertex3d(r * cos(angle), 0.0, r * sin(angle));
	}
	glEnd();
}

void CGLRenderer::DrawAxis(double length)
{
	glBegin(GL_LINES);
	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(-length, 0.0, 0.0);

	glColor3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, length, 0.0);

	glColor3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, length);
	glEnd();
}

void CGLRenderer::DrawPiece(double r1, double r2, double height, int nSeg)
{
	double step = 2 * 3.14 / nSeg;

	int v = r2 > r1 ? -1 : 1;
	double r = abs(r1 - r2);
	double L = sqrt(r * r + height * height);
	double ny = r / L;
	double nr = height / L;

	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= nSeg; i++) {
		double alpha = step * i;
		glNormal3f(nr * cos(alpha), ny * v, nr * sin(alpha));
		glVertex3f(cos(alpha) * r1, 0, sin(alpha) * r1);
		glVertex3f(cos(alpha) * r2, height, sin(alpha) * r2);
	}
	glEnd();
	
	if (displayNormals) {
		DrawNormals(r1,r2,height,nSeg,0.15);
	}
}

void CGLRenderer::DrawObject()
{
	double diff = 0.2;
	double radiusLarge = 0.75;
	double radiusMedium = radiusLarge - diff;
	double radiusSmall = radiusMedium - diff;
	double h = 0.2;
	int nSeg = 30;

	m_lightBlueMaterial->Select();
	DrawPiece(radiusLarge, radiusMedium, h, nSeg);
	glTranslatef(0, h, 0);
	m_pinkMaterial->Select();
	DrawPiece(radiusMedium, radiusSmall, h, nSeg);
    glTranslatef(0, h, 0);
	m_lightBlueMaterial->Select();
	DrawPiece(radiusSmall, radiusSmall, h, nSeg);
	glTranslatef(0, h, 0);
	m_pinkMaterial->Select();
	DrawPiece(radiusSmall, radiusSmall, h, nSeg);
	glTranslatef(0, h, 0);
	m_lightBlueMaterial->Select();
	DrawPiece(radiusSmall, radiusMedium, h, nSeg);
	glTranslatef(0, h, 0);
	m_pinkMaterial->Select();
	DrawPiece(radiusMedium, radiusSmall, h, nSeg);
    glTranslatef(0, h, 0);
	m_lightBlueMaterial->Select();
	DrawPiece(radiusSmall, radiusMedium, h, nSeg);
    glTranslatef(0, h, 0);
	m_pinkMaterial->Select();
	DrawPiece(radiusMedium, radiusLarge, h, nSeg);
    glTranslatef(0, h, 0);
	m_lightBlueMaterial->Select();
	DrawPiece(radiusLarge, radiusMedium, h, nSeg);
	glTranslatef(0, h, 0);
	m_pinkMaterial->Select();
	DrawPiece(radiusMedium, radiusSmall, h, nSeg);
	glTranslatef(0, h, 0);
	m_lightBlueMaterial->Select();
	DrawPiece(radiusSmall, radiusMedium, h, nSeg);
	glTranslatef(0, h, 0);
	m_pinkMaterial->Select();
	DrawPiece(radiusMedium, radiusSmall, h, nSeg);
	glTranslatef(0, h, 0);
	m_lightBlueMaterial->Select();
	DrawPiece(radiusSmall, radiusMedium, h, nSeg);
	glTranslatef(0, h, 0);
	m_pinkMaterial->Select();
	DrawPiece(radiusMedium, radiusLarge, h, nSeg);
}

void CGLRenderer::DrawBase()
{
	glPushMatrix();
	glColor3f(0.8, 0.8, 0.8);
	DrawSphere(true, 0.8, 30, 30);
	glTranslatef(0.0, 0.5, 0.0);
	DrawCylinderRegular(0.4, 0.8, 8);
	glTranslatef(0.0, 0.8, 0.0);

		glPushMatrix();
		glRotatef(45, 0.0, 1.0, 0.0);
		DrawCylinderRegular(1.5, 0.4, 4);
		glPopMatrix();
	glPopMatrix();
}

void CGLRenderer::DrawNormals(double r1, double r2, double height, int nSeg, double normalLength)
{
	axisMaterial->SetEmission(0, 1, 0, 1);
	axisMaterial->SetAmbient(0, 1, 0, 1);
	axisMaterial->Select();
	glColor3f(0.0, 1.0, 0.0);
	double step = 2 * M_PI / nSeg;
	double hDiff = height;

	glBegin(GL_LINES);
	for (int i = 0; i < nSeg; i++) {
		double angle = i * step;
		double x = cos(angle);
		double z = sin(angle);

		// Calculate the normal direction based on the slant of the vase's layer
		double nx = x; // horizontal component
		double nz = z; // horizontal component
		double ny = (r2 - r1) / hDiff; // vertical component based on the slope of the layer

		// Normalize the normal vector
		double length = sqrt(nx * nx + ny * ny + nz * nz);
		nx /= length;
		ny /= length;
		nz /= length;

		ny = -ny; // flip the normal vector to point in the correct direction

		// Draw the normal for the outer edge
		glVertex3d(r2 * x, height, r2 * z);
		glVertex3d(r2 * x + normalLength * nx, height + normalLength * ny, r2 * z + normalLength * nz);

		// Draw the normal for the inner edge if r1 is greater than 0
		if (r1 > 0) {
			glVertex3d(r1 * x, 0.0, r1 * z);
			glVertex3d(r1 * x + normalLength * nx, normalLength * ny, r1 * z + normalLength * nz);
		}
	}
	glEnd();
}

void CGLRenderer::DrawWall(double width, double height, int nSeg)
{
	float wStep = width / nSeg;
	float hStep = height / nSeg;

	glBegin(GL_QUADS);
	for (int i = 0; i < nSeg; ++i) {
		for (int j = 0; j < nSeg; ++j) {
			float x = i * wStep - width / 2;
			float y = j * hStep - height / 2;

			glNormal3f(0.0f, 0.0f, 1.0f); // Assuming wall is facing +z direction
			glVertex3f(x, y, 0.0f);
			glVertex3f(x + wStep, y, 0.0f);
			glVertex3f(x + wStep, y + hStep, 0.0f);
			glVertex3f(x, y + hStep, 0.0f);
		}
	}
	glEnd();
}

void CGLRenderer::DrawRoom(double width, double height, double depth)
{

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, depth / 2);
	DrawWall(width, height, 30); // Adjust grid size as needed
	glPopMatrix();

	// Draw Back Wall
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -depth / 2);
	glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();

	// Draw Left Wall
	glPushMatrix();
	glTranslatef(-width / 2, 0.0f, 0.0f);
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();

	// Draw Right Wall
	glPushMatrix();
	glTranslatef(width / 2, 0.0f, 0.0f);
	glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();

	// Draw Floor
	glPushMatrix();
	glTranslatef(0.0f, -height / 2, 0.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	DrawWall(width, height, 30);
	glPopMatrix();
}

void CGLRenderer::SetupLighting()
{
	GLfloat lmodel_ambient[] = { 0.9, 0.9, 0.9, 1.0 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_specular[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_position[] = { 1, 8, 1, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);
}

void CGLRenderer::SetupMaterial()
{

	//soba
	m_roomMaterial->SetAmbient(0.1, 0.1, 0.1, 1.0f);
	m_roomMaterial->SetDiffuse(0.8f, 0.8f, 0.8f, 1.0f);
	m_roomMaterial->SetSpecular(0.2f, 0.2f, 0.2f, 1.0f);
	m_roomMaterial->SetEmission(0.1f, 0.1f, 0.1f, 1.0f);
	m_roomMaterial->SetShininess(0);

	//osnova
	m_baseMaterial->SetAmbient(0.35, 0.35, 0.35, 1.0f);
	m_baseMaterial->SetDiffuse(0.7, 0.7, 0.7, 1.0f);
	m_baseMaterial->SetSpecular(0.0, 0.0, 0.0, 1.0f);
	m_baseMaterial->SetShininess(0);

	//delovi vaze
	m_lightBlueMaterial->SetAmbient(0.0f, 0.2f, 0.2f, 1.0f);
	m_lightBlueMaterial->SetDiffuse(0.0f, 1.0f, 1.0f, 1.0f);
	m_lightBlueMaterial->SetSpecular(0.2f, 0.2f, 0.2f, 1.0f);
	m_lightBlueMaterial->SetShininess(20.0f);

	m_pinkMaterial->SetAmbient(0.2f, 0.0f, 0.2f, 1.0f);
	m_pinkMaterial->SetDiffuse(1.0f, 0.0f, 1.0f, 1.0f);
	m_pinkMaterial->SetSpecular(0.2f, 0.2f, 0.2f, 1.0f);
	m_pinkMaterial->SetShininess(10.0f);

	//sfere
	m_redSphereMaterial->SetAmbient(0.2f, 0, 0, 1.0f);
	m_redSphereMaterial->SetDiffuse(1.0, 0, 0, 1.0f);
    m_redSphereMaterial->SetSpecular(1, 0.0f, 0.0f, 1.0f);
	m_redSphereMaterial->SetEmission(1, 0.0f, 0.0f, 1.0f);
	m_redSphereMaterial->SetShininess(0);

	m_pinkSphereMaterial->SetAmbient(0.2f, 0.0f, 0.2f, 1.0f);
	m_pinkSphereMaterial->SetDiffuse(1.0, 0.0, 1.0, 1.0f);
	m_pinkSphereMaterial->SetSpecular(1, 0.0f, 1.0f, 1.0f);
	m_pinkSphereMaterial->SetEmission(1, 0.0f, 1.0f, 1.0f);
	m_pinkSphereMaterial->SetShininess(0);

	m_blueSphereMaterial->SetAmbient(0, 0, 0.2, 1.0f);
	m_blueSphereMaterial->SetDiffuse(0, 0, 1.0, 1.0f);
	m_blueSphereMaterial->SetSpecular(0, 0.0f, 1.0f, 1.0f);
	m_blueSphereMaterial->SetEmission(0, 0.0f, 1.0f, 1.0f);
	m_blueSphereMaterial->SetShininess(0);

}

void CGLRenderer::RedLight()
{
	GLfloat light_ambient[] = { 0.5, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat light_specular[] = {1.0, 0.0, 0.0, 1.0 };
	GLfloat light_position[] = { 0.0, 4.5, 4.5, 1.0 };

	glLightfv(rLight, GL_AMBIENT, light_ambient);
	glLightfv(rLight, GL_DIFFUSE, light_diffuse);
	glLightfv(rLight, GL_SPECULAR, light_specular);
	glLightfv(rLight, GL_POSITION, light_position);

	GLfloat spot_direction[] = { 0.0, 0.0, -1.0 };
	glLightfv(rLight, GL_SPOT_DIRECTION, spot_direction);
	glLightf(rLight,GL_CONSTANT_ATTENUATION,1.0);
	glLightf(rLight, GL_SPOT_EXPONENT, 2.0);
	glLightf(rLight, GL_SPOT_CUTOFF, 20.0);
		

	if (redLight) {
		glPushMatrix();
		glTranslatef(0, 4.5, 4.5);
		m_redSphereMaterial->Select();
		DrawSphere(false, 0.2, 30, 30);
		glPopMatrix();
		glEnable(rLight);
	}
	else {
		glDisable(rLight);
	}
}

void CGLRenderer::PinkLight()
{
	GLfloat light_ambient[] = { 0.5, 0.0, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 0.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 0.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 4.5, -4.5, 1.0 };

	glLightfv(pLight, GL_AMBIENT, light_ambient);
	glLightfv(pLight, GL_DIFFUSE, light_diffuse);
	glLightfv(pLight, GL_SPECULAR, light_specular);
	glLightfv(pLight, GL_POSITION, light_position);

	GLfloat spot_direction[] = { 0.0, 0.0, 1.0 };
	glLightfv(pLight, GL_SPOT_DIRECTION, spot_direction);
	glLightf(pLight, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(pLight, GL_SPOT_EXPONENT, 2.0);
	glLightf(pLight, GL_SPOT_CUTOFF, 20.0);

	if (pinkLight) {
		glPushMatrix();
		glTranslatef(0, 4.5, -4.5);
		m_pinkSphereMaterial->Select();
		DrawSphere(false, 0.2, 30, 30);
		glPopMatrix();
		glEnable(pLight);
	}
	else {
		glDisable(pLight);
	}
}

void CGLRenderer::BlueLight()
{
	GLfloat light_ambient[] = { 0.0, 0.0, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 10, 0.0, 1.0 };

	glLightfv(bLight, GL_AMBIENT, light_ambient);
	glLightfv(bLight, GL_DIFFUSE, light_diffuse);
	glLightfv(bLight, GL_SPECULAR, light_specular);
	glLightfv(bLight, GL_POSITION, light_position);

	GLfloat spot_direction[] = { 0.0, -1.0, 0.0 };
	glLightfv(bLight, GL_SPOT_DIRECTION, spot_direction);
	glLightf(bLight, GL_CONSTANT_ATTENUATION, 1.0);
	glLightf(bLight, GL_SPOT_EXPONENT, 2.0);
	glLightf(bLight, GL_SPOT_CUTOFF, 20.0);

	if (blueLight) {
		glPushMatrix();
		glTranslatef(0, 4.5, 0);
		m_blueSphereMaterial->Select();
		DrawSphere(false, 0.2, 30, 30);
		glPopMatrix();
		glEnable(bLight);
	}
	else {
		glDisable(bLight);
	}
}
