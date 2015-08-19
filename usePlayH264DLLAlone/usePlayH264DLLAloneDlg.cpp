
// usePlayH264DLLAloneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "usePlayH264DLLAlone.h"
#include "usePlayH264DLLAloneDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CusePlayH264DLLAloneDlg dialog



CusePlayH264DLLAloneDlg::CusePlayH264DLLAloneDlg(CWnd* pParent /*=NULL*/): CDialogEx(CusePlayH264DLLAloneDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_pSizeList = NULL;
    m_pBuffer = NULL;
    m_instance = -1;
}

void CusePlayH264DLLAloneDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CusePlayH264DLLAloneDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CusePlayH264DLLAloneDlg message handlers

BOOL CusePlayH264DLLAloneDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here

    FILE* pDataFile;
    errno_t error = fopen_s(&pDataFile, "D:\\test_video_data.h264", "rb");
    FILE* pSizeFile;
    error = fopen_s(&pSizeFile, "D:\\test_video_size", "rb");

    fseek(pDataFile, 0, SEEK_END);
    fseek(pSizeFile, 0, SEEK_END);

    m_test_video_data_file_size = ftell(pDataFile);
    m_test_video_size_file_size = ftell(pSizeFile);

    rewind(pDataFile);
    rewind(pSizeFile);

    m_pSizeList = new UINT[m_test_video_size_file_size / 4];
    m_pBuffer = new UCHAR[m_test_video_data_file_size];

    fread(m_pSizeList, m_test_video_size_file_size, 1, pSizeFile);
    fread(m_pBuffer, m_test_video_data_file_size, 1, pDataFile);

    fclose(pSizeFile);
    fclose(pDataFile);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CusePlayH264DLLAloneDlg::OnPaint()
{
    if(IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CusePlayH264DLLAloneDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}



BOOL CusePlayH264DLLAloneDlg::DestroyWindow()
{
    // TODO: Add your specialized code here and/or call the base class
    if(NULL != m_pSizeList)
    {
        delete[] m_pSizeList;
        m_pSizeList = NULL;
    }

    if(NULL != m_pBuffer)
    {
        delete[] m_pBuffer;
        m_pBuffer = NULL;
    }

    return CDialogEx::DestroyWindow();
}
