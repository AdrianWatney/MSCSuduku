
// MFCSuduku.h : main header file for the MFCSuduku application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMFCSudukuApp:
// See MFCSuduku.cpp for the implementation of this class
//

class CMFCSudukuApp : public CWinApp
{
public:
	CMFCSudukuApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCSudukuApp theApp;
