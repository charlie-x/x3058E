// RoundButton.cpp : implementation file
//
// Round Buttons!
//
// Written by Chris Maunder (cmaunder@mail.com)
// Copyright (c) 1997,1998.
//
// Modified: 2 Feb 1998 - Fix vis problem, CRgn resource leak,
//                        button reposition code redone. CJM.
//
// Modified: 10 June 2003 - Added option to remove "spots". Tom Kalmijn
//                        - Also use offset for selected focus circle.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included. If
// the source code in this file is used in any commercial application
// then a simple email would be nice.
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

#include "stdafx.h"
#include "math.h"
#include "RoundButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// prototypes
COLORREF GetColour ( double dAngle, COLORREF crBright, COLORREF crDark );
void DrawCircle ( CDC* pDC, CPoint p, LONG lRadius, COLORREF crColour, BOOL bDashed = FALSE );
void DrawCircle ( CDC* pDC, CPoint p, LONG lRadius, COLORREF crBright, COLORREF crDark );

// extra prototypes for pixel correction
void RemoveSpots ( CDC* pDC, RECT& rect );
float GetGrayness ( const COLORREF& color );
COLORREF GetAverageColor ( COLORREF colors[], int nCount );

// Calculate colour for a point at the given angle by performing a linear
// interpolation between the colours crBright and crDark based on the cosine
// of the angle between the light source and the point.
//
// Angles are measured from the +ve x-axis (i.e. (1,0) = 0 degrees, (0,1) = 90 degrees )
// But remember: +y points down!

COLORREF GetColour ( double dAngle, COLORREF crBright, COLORREF crDark )
{
#define Rad2Deg	180.0/3.1415
#define LIGHT_SOURCE_ANGLE	-2.356		// -2.356 radians = -135 degrees, i.e. From top left

    ASSERT ( dAngle > -3.1416 && dAngle < 3.1416 );
    double dAngleDifference = LIGHT_SOURCE_ANGLE - dAngle;

    if ( dAngleDifference < -3.1415 ) { dAngleDifference = 6.293 + dAngleDifference; }

    else
        if ( dAngleDifference > 3.1415 ) { dAngleDifference = 6.293 - dAngleDifference; }

    double Weight = 0.5 * ( cos ( dAngleDifference ) + 1.0 );

    BYTE Red   = ( BYTE ) ( Weight * GetRValue ( crBright ) + ( 1.0 - Weight ) * GetRValue ( crDark ) );
    BYTE Green = ( BYTE ) ( Weight * GetGValue ( crBright ) + ( 1.0 - Weight ) * GetGValue ( crDark ) );
    BYTE Blue  = ( BYTE ) ( Weight * GetBValue ( crBright ) + ( 1.0 - Weight ) * GetBValue ( crDark ) );

    //TRACE("LightAngle = %0.0f, Angle = %3.0f, Diff = %3.0f, Weight = %0.2f, RGB %3d,%3d,%3d\n",
    //	  LIGHT_SOURCE_ANGLE*Rad2Deg, dAngle*Rad2Deg, dAngleDifference*Rad2Deg, Weight,Red,Green,Blue);

    return RGB ( Red, Green, Blue );
}

void DrawCircle ( CDC* pDC, CPoint p, LONG lRadius, COLORREF crColour, BOOL bDashed )
{
    const int nDashLength = 1;
    LONG lError, lXoffset, lYoffset;
    int  nDash = 0;
    BOOL bDashOn = TRUE;

    //Check to see that the coordinates are valid
    ASSERT ( ( p.x + lRadius <= LONG_MAX ) && ( p.y + lRadius <= LONG_MAX ) );
    ASSERT ( ( p.x - lRadius >= LONG_MIN ) && ( p.y - lRadius >= LONG_MIN ) );

    //Set starting values
    lXoffset = lRadius;
    lYoffset = 0;
    lError   = -lRadius;

    do {
        if ( bDashOn ) {
            pDC->SetPixelV ( p.x + lXoffset, p.y + lYoffset, crColour );
            pDC->SetPixelV ( p.x + lXoffset, p.y - lYoffset, crColour );
            pDC->SetPixelV ( p.x + lYoffset, p.y + lXoffset, crColour );
            pDC->SetPixelV ( p.x + lYoffset, p.y - lXoffset, crColour );
            pDC->SetPixelV ( p.x - lYoffset, p.y + lXoffset, crColour );
            pDC->SetPixelV ( p.x - lYoffset, p.y - lXoffset, crColour );
            pDC->SetPixelV ( p.x - lXoffset, p.y + lYoffset, crColour );
            pDC->SetPixelV ( p.x - lXoffset, p.y - lYoffset, crColour );
        }

        //Advance the error term and the constant X axis step
        lError += lYoffset++;

        //Check to see if error term has overflowed
        if ( ( lError += lYoffset ) >= 0 )
        { lError -= --lXoffset * 2; }

        if ( bDashed && ( ++nDash == nDashLength ) ) {
            nDash = 0;
            bDashOn = !bDashOn;
        }

    } while ( lYoffset <= lXoffset );	//Continue until halfway point
}

void DrawCircle ( CDC* pDC, CPoint p, LONG lRadius, COLORREF crBright, COLORREF crDark )
{
    LONG lError, lXoffset, lYoffset;

    //Check to see that the coordinates are valid
    ASSERT ( ( p.x + lRadius <= LONG_MAX ) && ( p.y + lRadius <= LONG_MAX ) );
    ASSERT ( ( p.x - lRadius >= LONG_MIN ) && ( p.y - lRadius >= LONG_MIN ) );

    //Set starting values
    lXoffset = lRadius;
    lYoffset = 0;
    lError   = -lRadius;

    do {
        const double Pi = 3.141592654,
                     Pi_on_2 = Pi * 0.5,
                     Three_Pi_on_2 = Pi * 1.5;
        COLORREF crColour;
        double   dAngle = atan2 ( ( double ) lYoffset, ( double ) lXoffset );

        //Draw the current pixel, reflected across all eight arcs
        crColour = GetColour ( dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x + lXoffset, p.y + lYoffset, crColour );

        crColour = GetColour ( Pi_on_2 - dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x + lYoffset, p.y + lXoffset, crColour );

        crColour = GetColour ( Pi_on_2 + dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x - lYoffset, p.y + lXoffset, crColour );

        crColour = GetColour ( Pi - dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x - lXoffset, p.y + lYoffset, crColour );

        crColour = GetColour ( -Pi + dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x - lXoffset, p.y - lYoffset, crColour );

        crColour = GetColour ( -Pi_on_2 - dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x - lYoffset, p.y - lXoffset, crColour );

        crColour = GetColour ( -Pi_on_2 + dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x + lYoffset, p.y - lXoffset, crColour );

        crColour = GetColour ( -dAngle, crBright, crDark );
        pDC->SetPixelV ( p.x + lXoffset, p.y - lYoffset, crColour );

        //Advance the error term and the constant X axis step
        lError += lYoffset++;

        //Check to see if error term has overflowed
        if ( ( lError += lYoffset ) >= 0 )
        { lError -= --lXoffset * 2; }

    } while ( lYoffset <= lXoffset );	//Continue until halfway point
}


/////////////////////////////////////////////////////////////////////////////
// CRoundButton

CRoundButton::CRoundButton() : m_nRadius ( 0 )
{
    m_bDrawDashedFocusCircle = TRUE;
    m_bRemoveSpots = TRUE;
    colorBtnFace = ::GetSysColor ( COLOR_BTNFACE );
    color3dHighLight = ::GetSysColor ( COLOR_3DHIGHLIGHT );
    color3DDKSHADOW = ::GetSysColor ( COLOR_3DDKSHADOW );
    color3DSHADOW = ::GetSysColor ( COLOR_3DSHADOW );
    color3DLIGHT = ::GetSysColor ( COLOR_3DLIGHT );

    m_highlighted = false;

}

CRoundButton::~CRoundButton()
{
    m_rgn.DeleteObject();
}

BEGIN_MESSAGE_MAP ( CRoundButton, CMFCButton )
    //{{AFX_MSG_MAP(CRoundButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoundButton message handlers

void CRoundButton::PreSubclassWindow()
{
    CMFCButton::PreSubclassWindow();

    ModifyStyle ( 0, BS_OWNERDRAW );

    CRect rect;
    GetClientRect ( rect );

    // Resize the window to make it square
    rect.bottom = rect.right = min ( rect.bottom, rect.right );

    // Get the vital statistics of the window
    m_ptCentre = rect.CenterPoint();
    m_nRadius  = rect.bottom / 2 - 1;

    // Set the window region so mouse clicks only activate the round section
    // of the button
    m_rgn.DeleteObject();
    SetWindowRgn ( NULL, FALSE );
    m_rgn.CreateEllipticRgnIndirect ( rect );
    SetWindowRgn ( m_rgn, TRUE );

    // Convert client coords to the parents client coords
    ClientToScreen ( rect );
    CWnd* pParent = GetParent();

    if ( pParent ) { pParent->ScreenToClient ( rect ); }

    // Resize the window
    MoveWindow ( rect.left, rect.top, rect.Width(), rect.Height(), TRUE );
}

void CRoundButton::DrawItem ( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    ASSERT ( lpDrawItemStruct != NULL );

    CDC* pDC   = CDC::FromHandle ( lpDrawItemStruct->hDC );
    CRect rect = lpDrawItemStruct->rcItem;
    UINT state = lpDrawItemStruct->itemState;
    UINT nStyle = GetStyle();
    int nRadius = m_nRadius;

    int nSavedDC = pDC->SaveDC();

    pDC->SelectStockObject ( NULL_BRUSH );
    pDC->FillSolidRect ( rect, colorBtnFace );

    // Draw the focus circle around the button
    if ( ( state & ODS_FOCUS ) && m_bDrawDashedFocusCircle )
    { DrawCircle ( pDC, m_ptCentre, nRadius--, RGB ( 0, 0, 0 ) ); }

    // Draw the raised/sunken edges of the button (unless flat)
    if ( nStyle & BS_FLAT ) {
        DrawCircle ( pDC, m_ptCentre, nRadius--, RGB ( 0, 0, 0 ) );
        DrawCircle ( pDC, m_ptCentre, nRadius--, color3dHighLight );

    } else {
        if ( ( state & ODS_SELECTED ) )	{
            DrawCircle ( pDC, m_ptCentre, nRadius--,
                         color3DDKSHADOW, color3dHighLight );
            DrawCircle ( pDC, m_ptCentre, nRadius--,
                         color3DSHADOW, color3DLIGHT );

        } else {
            DrawCircle ( pDC, m_ptCentre, nRadius--,
                         color3dHighLight, color3DDKSHADOW );
            DrawCircle ( pDC, m_ptCentre, nRadius--,
                         color3DLIGHT, color3DSHADOW );
        }
    }

    if ( m_bRemoveSpots )
    { RemoveSpots ( pDC, rect ); }

    // draw the text if there is any
    CString strText;
    GetWindowText ( strText );

    if ( !strText.IsEmpty() ) {
        CRgn rgn;
        rgn.CreateEllipticRgn ( m_ptCentre.x - nRadius, m_ptCentre.y - nRadius,
                                m_ptCentre.x + nRadius, m_ptCentre.y + nRadius );
        pDC->SelectClipRgn ( &rgn );

        CSize Extent = pDC->GetTextExtent ( strText );
        CPoint pt = CPoint ( m_ptCentre.x - Extent.cx / 2, m_ptCentre.x - Extent.cy / 2 );

        if ( state & ODS_SELECTED ) { pt.Offset ( 1, 1 ); }

        pDC->SetBkMode ( TRANSPARENT );

        if ( state & ODS_DISABLED )
        { pDC->DrawState ( pt, Extent, strText, DSS_DISABLED, TRUE, 0, ( HBRUSH ) NULL ); }

        else
        { pDC->TextOut ( pt.x, pt.y, strText ); }

        pDC->SelectClipRgn ( NULL );
        rgn.DeleteObject();
    }

    // Draw the focus circle on the inside of the button
    if ( ( state & ODS_FOCUS ) && m_bDrawDashedFocusCircle ) {

        CPoint pt = CPoint ( m_ptCentre.x, m_ptCentre.y );

        if ( state & ODS_SELECTED ) { pt.Offset ( 1, 1 ); }

        DrawCircle ( pDC, pt, nRadius - 2, RGB ( 0, 0, 0 ), TRUE );


    }

    if ( m_highlighted ) {

        CPen penRed;
        CPen* pOldPen;

        penRed.CreatePen ( PS_SOLID | PS_COSMETIC, 5, color3dHighLight );
        pOldPen = pDC->SelectObject ( &penRed );

        int offset;

        offset = nRadius / 2;

        pDC->Ellipse ( m_ptCentre.x - offset, m_ptCentre.y - offset, m_ptCentre.x + offset, m_ptCentre.y + offset );

        pDC->SelectObject ( pOldPen );
    }

    pDC->RestoreDC ( nSavedDC );
}

// spot removal routines
//
// Note:
// These routines are not speed optimized.
// Where drawing speed is an issue I would
// suggest using HBITMAP members and buffer
// button images (one image per button state) tk.

float GetGrayness ( const COLORREF& color )
{
    int r = GetRValue ( color );
    int g = GetGValue ( color );
    int b = GetBValue ( color );

    int minval = min ( r, min ( g, b ) );
    int maxval = max ( r, max ( g, b ) );

    return ( float ) ( maxval + minval ) / 510.0f;
}

COLORREF GetAverageColor ( COLORREF colors[], int nCount )
{
    int r = 0;
    int g = 0;
    int b = 0;

    for ( int i = 0; i < nCount; i++ ) {
        r += GetRValue ( colors[i] );
        g += GetGValue ( colors[i] );
        b += GetBValue ( colors[i] );
    }

    return RGB ( r / nCount, g / nCount, b / nCount );
}

void RemoveSpots ( CDC* pDC, RECT& rect )
{
    COLORREF c[6];
    c[5] = pDC->GetPixel ( 0, 0 ); // determine background color from top/left corner

    for ( int x = 1; x < rect.right - 1; x++ ) { // don't process pixels at borders
        for ( int y = 1; y < rect.bottom - 1; y++ ) {
            c[0] = pDC->GetPixel ( x, y );

            if ( c[0] != c[5] )
            { continue; } // only try to fix pixels that haven't changed yet

            // get neighbouring pixels (suffice to check horz & vert only)
            c[1] = pDC->GetPixel ( x - 1, y );
            c[2] = pDC->GetPixel ( x + 1, y );
            c[3] = pDC->GetPixel ( x, y - 1 );
            c[4] = pDC->GetPixel ( x, y + 1 );

            int darkspot_score = 0;
            int lightspot_score = 0;
            float current = GetGrayness ( c[0] );

            for ( int i = 1; i <= 4; i++ ) {
                float neighbour = GetGrayness ( c[i] );

                if ( neighbour > current + 0.01f ) // 0.01f = arbitrary threshold to
                { lightspot_score++; }          // filter out rounding errors

                else
                    if ( current > neighbour + 0.01f )
                    { darkspot_score++; }
            }

            if ( darkspot_score == 4 || lightspot_score == 4 ) {
                pDC->SetPixelV ( x, y, GetAverageColor ( &c[1], 4 ) );
                //  pDC->SetPixelV(x, y, RGB(255,0,0)); // uncomment to see where spots are found
            }
        }
    }
}

