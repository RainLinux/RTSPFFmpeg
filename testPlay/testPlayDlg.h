
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
    enum
    {
        IDD = IDD_TESTPLAY_DIALOG
    };

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
    lpFuncConnect            Connect;
    lpFuncPlay               Play;
    lpFuncPause              Pause;
    lpFuncDisConnect         DisConnect;
    lpFuncInitRtspVideoParam InitRtspVideoParam;
    lpFuncRevoHWAcceleration RevoHWAcceleration;
    lpFuncGetRtspINSTANCE    GetRtspINSTANCE;

    int m_INSTANCE;

public:
    afx_msg void OnClickedButtonPlay();
    virtual BOOL DestroyWindow();
    afx_msg void OnClickedButtonPause();
    afx_msg void OnClickedButtonConnect();
    afx_msg void OnClickedButtonDisconnect();
};
