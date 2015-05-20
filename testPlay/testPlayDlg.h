
// testPlayDlg.h : header file
//

#pragma once


// CtestPlayDlg dialog
class CtestPlayDlg : public CDialogEx
{
    // Construction
public:
    CtestPlayDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_TESTPLAY_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
private:
    HMODULE m_hDll;

    lpFuncInitRtspDLL        InitRtspDLL;
    lpFuncFreeRtspDll        FreeRtspDll;
    lpFuncPlayRtsp           PlayRtsp;
    lpFuncStopRtsp           StopRtsp;
    lpFuncInitRtspVideoParam InitRtspVideoParam;
    lpFuncRevoHWAcceleration RevoHWAcceleration;
    lpFuncGetRtspINSTANCE    GetRtspINSTANCE;
public:
    afx_msg void OnClickedButtonPlay();
    virtual BOOL DestroyWindow();
};
