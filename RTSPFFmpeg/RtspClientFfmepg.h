// RtspClientFfmepg.h : RtspClientFfmepg DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRtspClientFfmepgApp
// �йش���ʵ�ֵ���Ϣ������� RtspClientFfmepg.cpp
//

class CRtspClientFfmepgApp : public CWinApp
{
public:
	CRtspClientFfmepgApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
