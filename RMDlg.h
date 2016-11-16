
// RMDlg.h : header file
//

#pragma once
#include "RGDM3K.h"
#include "afxwin.h"

#define HISTORY_SIZE (100)

// CRMDlg dialog
class CRMDlg : public CDialogEx
{
        // Construction
    public:
        CRMDlg ( CWnd* pParent = NULL );	// standard constructor
        ~CRMDlg() {

            if ( font ) {
                delete font;
                font = NULL;
            }

            if ( RG ) {
                RGDM3K_close ( RG );
                RG = NULL;
            }


        }

        void setDeviceID ( ViRsrc device ) {
            deviceID = device;
        }


        void SetButtonColour ( CRoundButton &button, COLORREF ref ) {

            // ring around the circle
            button.setColor3dHightLight ( ref );
            button.setColor3DDKSHADOW ( ref );

            // ring highlight
            button.setColor3DSHADOW ( ref );
            button.setColor3DLIGHT ( ref );
        }

// Dialog Data
#ifdef AFX_DESIGN_TIME
        enum { IDD = IDD_RM3058E_DIALOG };
#endif

    protected:
        virtual void DoDataExchange ( CDataExchange* pDX );	// DDX/DDV support

        double history[HISTORY_SIZE];
        unsigned int history_index;
        ViRsrc deviceID;
        CFont *font;

        int rangeIndex ;
        ViSession RG;
        ViStatus  error = VI_SUCCESS;
        ViReal64  reading;
        ViChar    msgStr[256];

        CString m_DMM_Reading;

        ViReal64 auto_range;
        ViInt32 measuring;
        ViReal64 resolution;
        COScopeCtrl m_OScopeCtrl;
        double minimum_value;
        double maximum_value;

    public:

        // set a buttons bitmap
        bool setBitmap ( int button, int idd, int idd1 = 0 ) {

            CBitmap bmp;

            if ( 1 == bmp.LoadBitmap ( idd ) ) {

                CButton* pButton = ( CButton* ) GetDlgItem ( button );

                if ( pButton ) {

                    pButton->ModifyStyle ( 0, BS_BITMAP );

                    pButton->SetBitmap ( bmp );

                    return true;
                }
            }


            return false;
        };

        void ChangeColours ( void );

        bool setupInstrument ( void ) {

            ViStatus status;

            // the error checking is a bit terse
            checkErr ( RGDM3K_ConfigureMeasurement ( RG, measuring, auto_range, resolution ) );

            memset ( &history[0], 0, sizeof ( history ) );

            //history_index = 0;
            minimum_value = DBL_MAX;
            maximum_value = DBL_MIN;

            ChangeColours();

            return true;

            // and the internets flips out
Error:

            ViChar   errStr[2048];

            RGDM3K_GetError ( RG, &status, 2048, errStr );


            _RPT2 ( _CRT_WARN, "RGDM3K_ConfigureMeasurement failed = 0x%x (%s)\n", status, errStr );


            return false;


        };

// Implementation
    protected:
        HICON m_hIcon;

        // Generated message map functions
        virtual BOOL OnInitDialog();

        afx_msg void OnSysCommand ( UINT nID, LPARAM lParam );

        afx_msg void OnPaint();

        afx_msg HCURSOR OnQueryDragIcon();

        DECLARE_MESSAGE_MAP()
    public:
        // DMM read value
        CStatic m_DMMValue;

        afx_msg void OnTimer ( UINT_PTR nIDEvent );

        CButton m_DCVoltageSelect;

        afx_msg void OnBnClickedAuto();

        afx_msg void OnBnClickedDcVoltSelect();

        CButton m_ACVoltage;
        CButton m_AMPDC;
        CButton m_AMPAC;
        CButton m_OHMS;
        CButton m_Capacitance;
        CButton m_DiodeCheck;
        CButton m_Continuity;
        CButton m_2NDLock;

        afx_msg void OnBnClickedDcVoltSelect2();

        afx_msg void OnBnClickedAcVoltSelect();

        afx_msg void OnBnClickedRunStop();

        afx_msg void OnBnClickedSingle();

        afx_msg void OnBnClickedAmpDc();

        afx_msg void OnBnClickedAmpAc();

        afx_msg void OnBnClickedOhms();

        afx_msg void OnBnClickedCapacitance();

        afx_msg void OnBnClickedContinuity();

        afx_msg void OnBnClickedDiodeCheck();

        afx_msg void OnBnClickedFrequency();

        afx_msg void OnBnClickedSensor();

        afx_msg void OnBnClickedPreset();

        afx_msg void OnBnClicked2ndLock();

        afx_msg void OnBnClickedMeasure();

        afx_msg void OnBnClickedHelp();

        afx_msg void OnBnClickedMath();

        afx_msg void OnBnClickedSave();

        afx_msg void OnBnClickedTrig();

        afx_msg void OnBnClickedUtility();

        afx_msg void OnBnClickedRangePlus();

        afx_msg void OnBnClickedRangeMinus();

        afx_msg void OnBnClickedLeft();

        afx_msg void OnBnClickedRight();

        afx_msg void OnBnClickedMeasure8();

        afx_msg void OnBnClickedAutoSelect();

        afx_msg void OnBnClickedPower();
        CRoundButton m_SenseP;
        CRoundButton m_InputP;
        CRoundButton m_LO_N;
        CRoundButton m_LO1;
        CRoundButton m_IN_C;
};
