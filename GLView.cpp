
// GLView.cpp : implementation of the CGLView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GL.h"
#endif

#include "GLDoc.h"
#include "GLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGLView

IMPLEMENT_DYNCREATE(CGLView, CView)

BEGIN_MESSAGE_MAP(CGLView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CGLView construction/destruction

CGLView::CGLView() noexcept
{
	// TODO: add construction code here

	m_isDragging = false;
}

CGLView::~CGLView()
{
}

BOOL CGLView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGLView drawing

void CGLView::OnDraw(CDC* pDC)
{
	CGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_glRenderer.DrawScene(pDC);
	// TODO: add draw code for native data here
}


// CGLView printing

BOOL CGLView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGLView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGLView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CGLView diagnostics

#ifdef _DEBUG
void CGLView::AssertValid() const
{
	CView::AssertValid();
}

void CGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGLDoc* CGLView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLDoc)));
	return (CGLDoc*)m_pDocument;
}
#endif //_DEBUG


// CGLView message handlers


int CGLView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pDC = GetDC();
	m_glRenderer.CreateGLContext(pDC);
	ReleaseDC(pDC);

	return 0;
}


void CGLView::OnDestroy()
{
	CView::OnDestroy();

	CDC* pDC = GetDC();
	m_glRenderer.DestroyScene(pDC);
	ReleaseDC(pDC);
}


BOOL CGLView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}


void CGLView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CDC* pDC = GetDC();
	m_glRenderer.Reshape(pDC, cx, cy);
	ReleaseDC(pDC);
}

void CGLView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CDC* pDC = GetDC();
	m_glRenderer.PrepareScene(pDC);
	ReleaseDC(pDC);
}


void CGLView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_isDragging = true;
	m_lastMousePos = point;
	SetCapture();
}


void CGLView::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_isDragging = false;
	ReleaseCapture(); // pustanje misa
}


void CGLView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isDragging)
	{
		CPoint delta = point - m_lastMousePos;
		m_lastMousePos = point;

		const float rotationSensitivityX = 0.5f;
		const float rotationSensitivityY = 0.5f;

		m_glRenderer.m_pitch += delta.x * rotationSensitivityX;
		m_glRenderer.m_yaw += delta.y * rotationSensitivityY;

		m_glRenderer.m_yaw = max(-89.0f, min(89.0f, m_glRenderer.m_yaw));

		Invalidate();
	}
	CView::OnMouseMove(nFlags, point);
}


void CGLView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	
	if(nChar == VK_UP)
		m_glRenderer.displayNormals = !m_glRenderer.displayNormals;
	if(nChar == VK_F1)
		m_glRenderer.redLight = !m_glRenderer.redLight;
	if(nChar == VK_F2)
		m_glRenderer.pinkLight = !m_glRenderer.pinkLight;
	if(nChar == VK_F3)
		m_glRenderer.blueLight = !m_glRenderer.blueLight;

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
