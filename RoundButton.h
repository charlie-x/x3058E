#if !defined(AFX_ROUNDBUTTON_H__5254170E_59CF_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_ROUNDBUTTON_H__5254170E_59CF_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// RoundButton.h : header file
//
// Round buttons!
//
// Written by Chris Maunder (cmaunder@mail.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included. If
// the source code in  this file is used in any commercial application
// then a simple email woulod be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
//
// Please use and enjoy. Please let me know of any bugs/mods/improvements
// that you have found/implemented and I will fix/incorporate them into this
// file.
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CRoundButton window

class CRoundButton : public CMFCButton
{
// Construction
    public:
        CRoundButton();

// Attributes
    public:
        BOOL GetSmoothing()							{ return m_bRemoveSpots;		}
        void SetSmoothing ( BOOL bSmoothing = TRUE )   { m_bRemoveSpots = bSmoothing;	}

        // sets background rectangle colour
        void setColorBtnFace ( COLORREF ref ) {
            colorBtnFace = ref;
        }

        // ring around the circle
        void setColor3dHightLight ( COLORREF ref ) {
            color3dHighLight = ref;
        }
        void setColor3DDKSHADOW ( COLORREF ref ) {
            color3DDKSHADOW = ref;
        }
        void setColor3DSHADOW ( COLORREF ref ) {
            color3DSHADOW = ref;
        }
        void setColor3DLIGHT ( COLORREF ref ) {
            color3DLIGHT = ref;
        }

        void setHighlight ( bool flag ) {
            m_highlighted = flag;
        }


// Operations
    public:

// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CRoundButton)
    public:
        virtual void DrawItem ( LPDRAWITEMSTRUCT lpDrawItemStruct );
    protected:
        virtual void PreSubclassWindow();
        //}}AFX_VIRTUAL

// Implementation
    public:
        virtual ~CRoundButton();

        bool m_highlighted;

        CRgn   m_rgn;
        CPoint m_ptCentre;
        int    m_nRadius;
        BOOL   m_bDrawDashedFocusCircle;
        BOOL   m_bRemoveSpots;

        COLORREF colorBtnFace;
        COLORREF color3dHighLight;
        COLORREF color3DDKSHADOW;
        COLORREF color3DSHADOW;
        COLORREF color3DLIGHT;


        // Generated message map functions
    protected:
        //{{AFX_MSG(CRoundButton)
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROUNDBUTTON_H__5254170E_59CF_11D1_ABBA_00A0243D1382__INCLUDED_)
