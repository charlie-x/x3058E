
// RMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RM3058E.h"
#include "RMDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
    public:
        CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
        enum { IDD = IDD_ABOUTBOX };
#endif

    protected:
        virtual void DoDataExchange ( CDataExchange* pDX ); // DDX/DDV support

// Implementation
    protected:
        DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx ( IDD_ABOUTBOX )
{
}

void CAboutDlg::DoDataExchange ( CDataExchange* pDX )
{
    CDialogEx::DoDataExchange ( pDX );
}

BEGIN_MESSAGE_MAP ( CAboutDlg, CDialogEx )
END_MESSAGE_MAP()


// CRMDlg dialog

CRMDlg::CRMDlg ( CWnd* pParent /*=NULL*/ )
    : CDialogEx ( IDD_RM3058E_DIALOG, pParent ) , font ( NULL ), minimum_value ( FLT_MAX ), maximum_value ( FLT_MIN ), history_index ( 0 )
{
    m_hIcon = AfxGetApp()->LoadIcon ( IDR_MAINFRAME );
    RG = NULL;

    memset ( &history[0], 0, sizeof ( history ) );

    rangeIndex = 0;

    resolution = RGDM3K_VAL_DEF;
    auto_range = RGDM3K_VAL_AUTO_RANGE_OFF;
    measuring = RGDM3K_VAL_DC_VOLTS;

    // my device.
    setDeviceID ( "USB::0x1AB1::0x09C4::DM3R171600643::INSTR" );
}

void CRMDlg::DoDataExchange ( CDataExchange * pDX )
{
    CDialogEx::DoDataExchange ( pDX );
    DDX_Control ( pDX, IDC_DMM_VALUE, m_DMMValue );
    DDX_Control ( pDX, IDC_DC_VOLT_SELECT, m_DCVoltageSelect );
    DDX_Control ( pDX, IDC_AC_VOLT_SELECT, m_ACVoltage );
    DDX_Control ( pDX, IDC_AMP_DC , m_AMPDC );
    DDX_Control ( pDX, IDC_AMP_AC, m_AMPAC );
    DDX_Control ( pDX, IDC_OHMS, m_OHMS );
    DDX_Control ( pDX, IDC_CAPACITANCE, m_Capacitance );
    DDX_Control ( pDX, IDC_DIODE_CHECK, m_DiodeCheck );
    DDX_Control ( pDX, IDC_CONTINUITY, m_Continuity );
    DDX_Control ( pDX, IDC_2ND_LOCK, m_2NDLock );
    DDX_Control ( pDX, IDC_SENSEP_IN, m_SenseP );
    DDX_Control ( pDX, IDC_P_IN, m_InputP );
    DDX_Control ( pDX, IDC_LO_IN, m_LO_N );
    DDX_Control ( pDX, IDC_N_IN, m_LO1 );
    DDX_Control ( pDX, IDC_I_IN, m_IN_C );
}

BEGIN_MESSAGE_MAP ( CRMDlg, CDialogEx )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_BN_CLICKED ( IDC_AUTO, &CRMDlg::OnBnClickedAuto )
    ON_BN_CLICKED ( IDC_DC_VOLT_SELECT, &CRMDlg::OnBnClickedDcVoltSelect )
    ON_BN_CLICKED ( IDC_AC_VOLT_SELECT, &CRMDlg::OnBnClickedAcVoltSelect )
    ON_BN_CLICKED ( IDC_RUN_STOP, &CRMDlg::OnBnClickedRunStop )
    ON_BN_CLICKED ( IDC_SINGLE, &CRMDlg::OnBnClickedSingle )
    ON_BN_CLICKED ( IDC_AMP_DC, &CRMDlg::OnBnClickedAmpDc )
    ON_BN_CLICKED ( IDC_AMP_AC, &CRMDlg::OnBnClickedAmpAc )
    ON_BN_CLICKED ( IDC_OHMS, &CRMDlg::OnBnClickedOhms )
    ON_BN_CLICKED ( IDC_CAPACITANCE, &CRMDlg::OnBnClickedCapacitance )
    ON_BN_CLICKED ( IDC_CONTINUITY, &CRMDlg::OnBnClickedContinuity )
    ON_BN_CLICKED ( IDC_DIODE_CHECK, &CRMDlg::OnBnClickedDiodeCheck )
    ON_BN_CLICKED ( IDC_FREQUENCY, &CRMDlg::OnBnClickedFrequency )
    ON_BN_CLICKED ( IDC_SENSOR, &CRMDlg::OnBnClickedSensor )
    ON_BN_CLICKED ( IDC_PRESET, &CRMDlg::OnBnClickedPreset )
    ON_BN_CLICKED ( IDC_2ND_LOCK, &CRMDlg::OnBnClicked2ndLock )
    ON_BN_CLICKED ( IDC_MEASURE, &CRMDlg::OnBnClickedMeasure )
    ON_BN_CLICKED ( IDC_HELP_BUTTON, &CRMDlg::OnBnClickedHelp )
    ON_BN_CLICKED ( IDC_MATH, &CRMDlg::OnBnClickedMath )
    ON_BN_CLICKED ( IDC_SAVE, &CRMDlg::OnBnClickedSave )
    ON_BN_CLICKED ( IDC_TRIG, &CRMDlg::OnBnClickedTrig )
    ON_BN_CLICKED ( IDC_UTILITY, &CRMDlg::OnBnClickedUtility )
    ON_BN_CLICKED ( IDC_RANGE_PLUS, &CRMDlg::OnBnClickedRangePlus )
    ON_BN_CLICKED ( IDC_RANGE_MINUS, &CRMDlg::OnBnClickedRangeMinus )
    ON_BN_CLICKED ( IDC_LEFT, &CRMDlg::OnBnClickedLeft )
    ON_BN_CLICKED ( IDC_RIGHT, &CRMDlg::OnBnClickedRight )
    ON_BN_CLICKED ( IDC_AUTO_SELECT, &CRMDlg::OnBnClickedAutoSelect )
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED ( IDC_POWER, &CRMDlg::OnBnClickedPower )
    ON_BN_CLICKED ( IDC_RNGM, &CRMDlg::OnBnClickedRangeMinus )
    ON_BN_CLICKED ( IDC_RNGP, &CRMDlg::OnBnClickedRangePlus )
END_MESSAGE_MAP()


// CRMDlg message handlers

BOOL CRMDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT ( ( IDM_ABOUTBOX & 0xFFF0 ) == IDM_ABOUTBOX );
    ASSERT ( IDM_ABOUTBOX < 0xF000 );

    CMenu* pSysMenu = GetSystemMenu ( FALSE );

    if ( pSysMenu != NULL ) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString ( IDS_ABOUTBOX );
        ASSERT ( bNameValid );

        if ( !strAboutMenu.IsEmpty() ) {
            pSysMenu->AppendMenu ( MF_SEPARATOR );
            pSysMenu->AppendMenu ( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
        }
    }

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon ( m_hIcon, TRUE );			// Set big icon
    SetIcon ( m_hIcon, FALSE );		// Set small icon

    bool smooth = false;

    m_SenseP.SetSmoothing ( smooth );
    m_InputP.SetSmoothing ( smooth );
    m_LO_N.SetSmoothing ( smooth );
    m_LO1.SetSmoothing ( smooth );
    m_IN_C.SetSmoothing ( smooth );

    int fontHeight = 120;

    font = new CFont();

    LOGFONT lf;
    memset ( &lf, 0, sizeof ( LOGFONT ) );

    lf.lfHeight = fontHeight;

    //_tcscpy ( lf.lfFaceName, _T ( "Digital Play St" ) );
    //_tcscpy ( lf.lfFaceName, _T ( "The Display St" ) );
    _tcscpy ( lf.lfFaceName, _T ( "Alarm Clock" ) );

    m_DMMValue.SetFont ( font );
    font->CreateFontIndirect ( &lf );

    m_DMMValue.SetFont ( font );

    m_DMMValue.SetWindowText ( _T ( "NAN" ) );

    {
        // determine the rectangle for the control
        CRect rect;
        GetDlgItem ( IDC_OSCOPE )->GetWindowRect ( rect );
        ScreenToClient ( rect );

        // create the control
        m_OScopeCtrl.Create ( WS_VISIBLE | WS_CHILD, rect, this );

        m_OScopeCtrl.SetRange ( 0.0, 10.0, 1 );
        m_OScopeCtrl.SetYUnits ( _T ( "Volts" ) );
        m_OScopeCtrl.SetXUnits ( _T ( "Samples (100 msec)" ) );

        if ( 1 ) {
            m_OScopeCtrl.SetBackgroundColor ( RGB ( 0, 0, 0 ) );
            m_OScopeCtrl.SetGridColor ( RGB ( 192, 255, 192 ) );
            m_OScopeCtrl.SetPlotColor ( RGB ( 255, 255, 255 ) );

        } else {
            m_OScopeCtrl.SetBackgroundColor ( RGB ( 0, 0, 64 ) );
            m_OScopeCtrl.SetPlotColor ( RGB ( 255, 255, 255 ) );
            m_OScopeCtrl.SetGridColor ( RGB ( 192, 192, 255 ) );
        }
    }

    setBitmap ( IDC_DC_VOLT_SELECT, IDB_VDC, IDB_VDC );
    setBitmap ( IDC_AC_VOLT_SELECT, IDB_VAC, IDB_VAC );

    setBitmap ( IDC_AMP_DC, IDB_IDC_OFF, IDB_IDC_ON );
    setBitmap ( IDC_AMP_AC, IDB_IAC_OFF , IDB_IAC_ON );

    setBitmap ( IDC_OHMS, IDB_OHM_OFF , IDB_OHM_ON );

    setBitmap ( IDC_DIODE_CHECK, IDB_DIODE_OFF, IDB_DIODE_ON );

    setBitmap ( IDC_2ND_LOCK, IDB_2ND_OFF, IDB_2ND_ON );

    setBitmap ( IDC_CAPACITANCE, IDB_CAP_OFF, IDB_CAP_ON );

    setBitmap ( IDC_CONTINUITY, IDB_CONT_OFF, IDB_CONT_ON );

    ChangeColours();

    // end of GUI setup

    char        instrDescr[VI_FIND_BUFLEN];

    memset ( instrDescr, 0, sizeof ( instrDescr ) );

    if ( listResources ( &instrDescr[0] ) == 0 ) {
        setDeviceID ( instrDescr );
    }

    // mine
    // USB::0x1AB1::0x09C4::DM3R171600643::INSTR
    checkErr ( RGDM3K_InitWithOptions (
                   deviceID,
                   VI_FALSE, VI_FALSE,
                   "Simulate=0,RangeCheck=1,QueryInstrStatus=1,Cache=1",
                   &RG ) );


    setupInstrument();

    checkErr ( RGDM3K_ConfigureTrigger ( RG, RGDM3K_VAL_IMMEDIATE, 500 ) );

    RGDM3K_DisplayMessage ( RG, "charliex" );

    SetTimer ( 0, 100, NULL );

    return TRUE;  // return TRUE  unless you set the focus to a control

Error:

    if ( error != VI_SUCCESS ) {
        ViChar   errStr[2048];

        RGDM3K_GetError ( RG, &error, 2048, errStr );
    }

    if ( RG )
    { RGDM3K_close ( RG ); }

    m_DMMValue.SetWindowText ( _T ( "N/C" ) );

    return FALSE;  // return TRUE  unless you set the focus to a control


}

void CRMDlg::OnSysCommand ( UINT nID, LPARAM lParam )
{
    if ( ( nID & 0xFFF0 ) == IDM_ABOUTBOX ) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();

    } else {
        CDialogEx::OnSysCommand ( nID, lParam );
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRMDlg::OnPaint()
{
    if ( IsIconic() ) {

        CPaintDC dc ( this ); // device context for painting

        SendMessage ( WM_ICONERASEBKGND, reinterpret_cast<WPARAM> ( dc.GetSafeHdc() ), 0 );

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics ( SM_CXICON );
        int cyIcon = GetSystemMetrics ( SM_CYICON );
        CRect rect;
        GetClientRect ( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // Draw the icon
        dc.DrawIcon ( x, y, m_hIcon );

    } else {

        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRMDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR> ( m_hIcon );
}

void CRMDlg::OnTimer ( UINT_PTR nIDEvent )
{
    ViStatus status;

    // 5000 ms max time
    status = RGDM3K_Read ( RG, 5000, &reading );

    if ( status == 0 ) {
        ViBoolean isOverRange = VI_FALSE;

        status = RGDM3K_IsOverRange ( RG, reading, &isOverRange );

        if ( reading == 9000000000000000.0 ) {

            if ( RGDM3K_VAL_CONTINUITY == measuring ) {
                m_DMM_Reading = _T ( "OPEN         " );

            } else {
                m_DMM_Reading = _T ( "OVERLOAD " );
            }

        } else {
            if ( status == 0 && isOverRange == VI_TRUE ) {
                m_DMM_Reading = _T ( "OVERRANGE " );

            } else {

                m_DMM_Reading.Format ( _T ( "%.6f       " ), ( double ) reading );

                // store in circular buffer
                history[history_index] = reading;
                history_index++;
                history_index %= HISTORY_SIZE;

                minimum_value = min ( minimum_value, reading );
                maximum_value = max ( maximum_value, reading );

                {
                    static double old_minimum = -1, old_maximum = -1;

                    if ( old_minimum != minimum_value || old_maximum != maximum_value ) {

                        int digits = 3;


                        // store for later.
                        old_minimum = minimum_value;
                        old_maximum = maximum_value;

                        // adjust the digits to match the digits
                        if ( fabs ( maximum_value - minimum_value ) < 1 ) {
                            digits = 6;
                        }

                        if ( fabs ( maximum_value - minimum_value ) > 1000 ) {
                            digits = 2;
                        }

                        // Clears the data from the graph
                        m_OScopeCtrl.SetRange (
                            minimum_value - ( ( minimum_value / 100.0 ) * 2.0 ),
                            maximum_value + ( ( maximum_value / 100.0 ) * 2.0 ),
                            digits
                        );

                        // re-add the data
                        for ( unsigned int i = 0; i < HISTORY_SIZE ; i++ ) {

                            m_OScopeCtrl.AppendPoint ( history[ ( ( history_index ) + i )  % HISTORY_SIZE] );

                        }

                    } else     {
                        m_OScopeCtrl.AppendPoint ( reading );
                    }

                }
            }
        }

        // change the value on the control
        m_DMMValue.SetWindowText ( m_DMM_Reading );

        UpdateWindow();
    }


    CDialogEx::OnTimer ( nIDEvent );
}

void CRMDlg::OnBnClickedAuto()
{
    auto_range = RGDM3K_VAL_AUTO_RANGE_ON;
    setupInstrument();

}

void CRMDlg::OnBnClickedDcVoltSelect()
{
    m_OScopeCtrl.SetYUnits ( _T ( "V DC" ) );

    measuring = RGDM3K_VAL_DC_VOLTS;
    setupInstrument();
}


void CRMDlg::OnBnClickedAcVoltSelect()
{
    m_OScopeCtrl.SetYUnits ( _T ( "V AC" ) );
    measuring = RGDM3K_VAL_AC_VOLTS;
    setupInstrument();
}


void CRMDlg::OnBnClickedRunStop()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedSingle()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedAmpDc()
{
    m_OScopeCtrl.SetYUnits ( _T ( "A  DC" ) );

    measuring = RGDM3K_VAL_DC_CURRENT;
    setupInstrument();
}


void CRMDlg::OnBnClickedAmpAc()
{
    m_OScopeCtrl.SetYUnits ( _T ( "A  AC" ) );

    measuring = RGDM3K_VAL_AC_CURRENT;
    setupInstrument();
}


void CRMDlg::OnBnClickedOhms()
{
    m_OScopeCtrl.SetYUnits ( _T ( "Ohms" ) );

    // toggle modes
    if ( measuring == RGDM3K_VAL_2_WIRE_RES ) {
        measuring = RGDM3K_VAL_4_WIRE_RES;

    } else {
        measuring = RGDM3K_VAL_2_WIRE_RES;
    }

    setupInstrument();
}


void CRMDlg::OnBnClickedCapacitance()
{
    m_OScopeCtrl.SetYUnits ( _T ( "uF" ) );
    measuring = RGDM3K_VAL_CAPACITANCE;
    setupInstrument();

}

void CRMDlg::OnBnClickedContinuity()
{
    m_OScopeCtrl.SetYUnits ( _T ( "Cont" ) );
    measuring = RGDM3K_VAL_CONTINUITY;
    auto_range = RGDM3K_VAL_AUTO_RANGE_ON;
    setupInstrument();
}


void CRMDlg::OnBnClickedDiodeCheck()
{
    m_OScopeCtrl.SetYUnits ( _T ( "Diode" ) );
    measuring = RGDM3K_VAL_DIODE;
    auto_range = RGDM3K_VAL_AUTO_RANGE_ON;
    setupInstrument();
}


void CRMDlg::OnBnClickedFrequency()
{
    m_OScopeCtrl.SetYUnits ( _T ( "Frq" ) );
    measuring = RGDM3K_VAL_FREQ;
    auto_range = RGDM3K_VAL_AUTO_RANGE_ON;
    setupInstrument();
}


void CRMDlg::OnBnClickedSensor()
{
    measuring = RGDM3K_VAL_TEMPERATURE;
    setupInstrument();
}


void CRMDlg::OnBnClickedPreset()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClicked2ndLock()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedMeasure()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedHelp()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedMath()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedSave()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedTrig()
{
    // TODO: Add your control notification handler code here
}


void CRMDlg::OnBnClickedUtility()
{
    // TODO: Add your control notification handler code here
}

static ViReal64 rangeList[] = {
    RGDM3K_VAL_AUTO_RANGE_ON,
    RGDM3K_VAL_AUTO_RANGE_OFF,
    0.002	,
    0.02	,
    0.2	,
    1.0	,
    10.0
};



void CRMDlg::OnBnClickedRangePlus()
{
    rangeIndex++;
    rangeIndex %= 7;

    switch ( measuring ) {
        case RGDM3K_VAL_DIODE:
        case RGDM3K_VAL_CONTINUITY:
        case RGDM3K_VAL_PERIOD:
        case RGDM3K_VAL_FREQ:

            rangeIndex = 0;
            break;

        default:
            break;
    }

    auto_range = rangeList[rangeIndex];

    _RPT2 ( _CRT_WARN, "range = %d, %f\n", rangeIndex, rangeList[rangeIndex] );

    setupInstrument();
}


void CRMDlg::OnBnClickedRangeMinus()
{
    rangeIndex--;

    if ( rangeIndex < 0 ) {
        rangeIndex = 6;
    }

    switch ( measuring ) {
        case RGDM3K_VAL_DIODE:
        case RGDM3K_VAL_CONTINUITY:
        case RGDM3K_VAL_PERIOD:
        case RGDM3K_VAL_FREQ:

            rangeIndex = 0;
            break;

        default:
            break;
    }

    auto_range = rangeList[rangeIndex];
    _RPT2 ( _CRT_WARN, "range = %d, %f\n", rangeIndex, rangeList[rangeIndex] );

    setupInstrument();

}


void CRMDlg::OnBnClickedLeft()
{

}


void CRMDlg::OnBnClickedRight()
{

}



void CRMDlg::OnBnClickedAutoSelect()
{

}


void CRMDlg::OnBnClickedPower()
{
    PostQuitMessage ( 0 );
}

typedef struct connections {
    COLORREF c1, c2, c3, c4, c5;
    bool b1, b2, b3, b4, b5;
} connections;

connections connectionList[] = {
    // VDC
    // VAC
    // 2 WIRE RES
    { RGB ( 102, 0, 0 ), RGB ( 255, 0, 0 ), RGB ( 0, 0, 0 ), RGB ( 0, 0, 0 ), RGB ( 102, 0, 0 ), 0, 1, 0, 1, 0 },
    // DCA
    { RGB ( 102, 0, 0 ), RGB ( 102, 0, 0 ), RGB ( 0, 0, 0 ), RGB ( 0, 0, 0 ), RGB ( 255, 0, 0 ), 0, 0, 0, 1, 1 },
    //4 wire OHMS
    { RGB ( 255, 0, 0 ), RGB ( 255, 0, 0 ), RGB ( 0, 0, 0 ), RGB ( 0, 0, 0 ), RGB ( 102, 0, 0 ), 1, 1, 1, 1, 0 },
};


void CRMDlg::ChangeColours ( void )
{
    int index = 0;

    switch ( measuring ) {
        case RGDM3K_VAL_AC_VOLTS:
        case RGDM3K_VAL_2_WIRE_RES:
        case RGDM3K_VAL_CAPACITANCE:
        case RGDM3K_VAL_CONTINUITY:
        case RGDM3K_VAL_DIODE:
        case RGDM3K_VAL_FREQ:
        case RGDM3K_VAL_PERIOD:
            index = 0;

            break;

        case RGDM3K_VAL_DC_CURRENT:
        case RGDM3K_VAL_AC_CURRENT:
            index = 1;
            break;

        case RGDM3K_VAL_4_WIRE_RES:
            index = 2;
            break;
    }

    setBitmap ( IDC_DC_VOLT_SELECT, IDB_VDC );
    setBitmap ( IDC_AC_VOLT_SELECT, IDB_VAC );

    setBitmap ( IDC_AMP_DC, IDB_IDC_OFF );
    setBitmap ( IDC_AMP_AC, IDB_IAC_OFF );
    setBitmap ( IDC_OHMS  , IDB_OHM_OFF );
    setBitmap ( IDC_CAPACITANCE, IDB_CAP_OFF );
    setBitmap ( IDC_DIODE_CHECK, IDB_DIODE_OFF );
    setBitmap ( IDC_CAPACITANCE, IDB_CAP_OFF );
    setBitmap ( IDC_CONTINUITY, IDB_CONT_OFF );


    switch ( measuring ) {

        case RGDM3K_VAL_DC_VOLTS:
            setBitmap ( IDC_DC_VOLT_SELECT, IDB_VDC_ON );
            break;

        case RGDM3K_VAL_AC_VOLTS:
            setBitmap ( IDC_AC_VOLT_SELECT, IDB_VAC_ON );
            break;

        case RGDM3K_VAL_DC_CURRENT:
            setBitmap ( IDC_AMP_DC, IDB_IDC_ON );
            break;

        case RGDM3K_VAL_AC_CURRENT:
            setBitmap ( IDC_AMP_AC, IDB_IAC_ON );
            break;

        case RGDM3K_VAL_2_WIRE_RES:
        case RGDM3K_VAL_4_WIRE_RES:
            setBitmap ( IDC_OHMS, IDB_OHM_ON );
            break;

        case RGDM3K_VAL_CAPACITANCE:
            setBitmap ( IDC_CAPACITANCE, IDB_CAP_ON );
            break;

        case RGDM3K_VAL_CONTINUITY:
            setBitmap ( IDC_CONTINUITY, IDB_CONT_ON );
            break;

        case RGDM3K_VAL_DIODE:
            setBitmap ( IDC_DIODE_CHECK, IDB_DIODE_ON );
            break;

        case RGDM3K_VAL_FREQ:
            break;

        case RGDM3K_VAL_PERIOD:
            break;


    }



    //setBitmap(IDC_2ND_LOCK, IDB_2ND_OFF);


    // messy solution
    SetButtonColour ( m_SenseP, connectionList[index].c1 );
    SetButtonColour ( m_InputP, connectionList[index].c2 );
    SetButtonColour ( m_LO_N, connectionList[index].c3 );
    SetButtonColour ( m_LO1, connectionList[index].c4 );
    SetButtonColour ( m_IN_C, connectionList[index].c5 );

    m_SenseP.setHighlight ( connectionList[index].b1 );
    m_InputP.setHighlight ( connectionList[index].b2 );
    m_LO_N.setHighlight ( connectionList[index].b3 );
    m_LO1.setHighlight ( connectionList[index].b4 );
    m_IN_C.setHighlight ( connectionList[index].b5 );

    Invalidate ( 1 );
    UpdateWindow();
}
