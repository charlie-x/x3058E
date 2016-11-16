
// RM3058E.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CRMApp:
// See RM3058E.cpp for the implementation of this class
//

class CRMApp : public CWinApp
{
public:
	CRMApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CRMApp theApp;