
// GL.h : main header file for the GL application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CGLApp:
// See GL.cpp for the implementation of this class
//

class CGLApp : public CWinApp
{
public:
	CGLApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGLApp theApp;
