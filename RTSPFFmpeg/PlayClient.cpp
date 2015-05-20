#include "stdafx.h"
#include <Windows.h>
#include "PlayClient.h"
#include <string>
#include <atlstr.h>
#include <fstream>
#include <sstream>
#include "Rtcp.h"

//#define log

bool inited = false;
setYUVCallBack setYUVCallBackFunc;
setH264CallBack setH264CallBackFunc; 
fSetCallBack SetCallBack;
finitVideoDLL initVideoDLL;
fGetIdlevideoINSTANCE GetIdlevideoINSTANCE;
fInitVideoParamNew InitVideoParamNew;
fInitVideoParam InitVideoParam;
fpauseVideos pauseVideos;
fplayVideos playVideos;
ffreeVideos freeVideos;
finputBuf inputBuf;
fresize resize;
fexitdll exitdll;
fSetDrawLineCallBack SetDrawLineCallBack;
fSetBmpCallBack SetBmpCallBack;
fSetFillBmpCallBack SetFillBmpCallBack;
revoHW revoHWFunc;

RTSPCLientClass::RTSPCLientClass()
{
	Url = NULL;
	UserName = NULL;
	Pwd = NULL;
}
RTSPCLientClass::~RTSPCLientClass()
{
	if(Url!=NULL)
	{
		delete Url;
		Url = NULL;
	}
	if(UserName!=NULL)
	{
		delete UserName;
		UserName = NULL;
	}
	if(Pwd!=NULL)
	{
		delete Pwd;
		Pwd = NULL;
	}
}

int RTSPCLientClass::InputURL(char* URL,char* UserName,char* PWD)
{
	this->UserName = new char[100];
	this->Pwd = new char[100];
	this->Url = new char[100];

	strcpy(this->UserName,UserName);
	strcpy(this->Pwd,PWD);
	strcpy(this->Url,URL);

	threadID = -1;
	circulation = false;

	hd = NULL;
    func = NULL;
    funcD = NULL;
    bmpFunc = NULL;
    fillbmp = NULL;
	YUVFunc = NULL;
	H264Func = NULL;
	YUVEx = NULL;
	INSTANCE = -1;
	nHWAcceleration = true;

	ans = 0;

	// *****************************************************************************************
	HANDLE hDllhandle = GetModuleHandle ("RtspClientFfmepg.dll");
	if (hDllhandle == NULL) 
	{
		MessageBox (NULL, "��ȡ��̬����ʧ��", "", MB_OK);
	}

	char path1[1024], path2[1024];
	memset (path1, 0, 1024);
	memset (path2, 0, 1024);

	GetModuleFileName ((HMODULE)hDllhandle,path1,1000);

	int len = strlen (path1)- strlen ("RtspClientFfmepg.dll");
	strncpy (path2, path1, len);
	strcat (path2, "playH264ThreadDLL1.dll");
	//***********************************************************************************************

	HINSTANCE hdll = LoadLibraryEx(path2, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	//HINSTANCE hdll = LoadLibrary("playH264ThreadDLL1.dll");
	initVideoDLL = (finitVideoDLL)GetProcAddress(hdll,"initVideoDLL");

	if(!inited)
	{
		initVideoDLL();
		inited = true;
	}
	

	return 1;
}

//�̺߳���
DWORD WINAPI RTSPVideo(LPVOID lpParam)
{
	//������׼������
	RTSPCLientClass *RCC = (RTSPCLientClass *)lpParam;
	myparamInput *Myparam=new myparamInput();//���Ž�����Ҫɾ��
    RECT *rect=new RECT;//���Ž�����Ҫɾ��
	GetWindowRect(RCC->hd,rect);
    Myparam->playHandle= RCC->hd;  // ȡ�ÿؼ��ľ��
    Myparam->stopPlay=0;
    Myparam->playChannle=1;
    Myparam->fps=25;
	Myparam->isDecode=true;
    Myparam->playHeight=rect->bottom-rect->top;
    Myparam->playWidth=rect->right-rect->left;
	/*FILE *fp;
	fp = fopen("C:\\1.log","a+");
	fputs("getintstance:",fp);
	fputc('\n',fp);*/
	Sleep(10);


		// *****************************************************************************************
	HANDLE hDllhandle = GetModuleHandle ("RtspClientFfmepg.dll");
	if (hDllhandle == NULL) 
	{
		MessageBox (NULL, "��ȡ��̬����ʧ��", "", MB_OK);
	}

	char path1[1024], path2[1024];
	memset (path1, 0, 1024);
	memset (path2, 0, 1024);

	GetModuleFileName ((HMODULE)hDllhandle,path1,1000);

	int len = strlen (path1)- strlen ("RtspClientFfmepg.dll");
	strncpy (path2, path1, len);
	strcat (path2, "playH264ThreadDLL1.dll");
	//***********************************************************************************************

	HINSTANCE hdll = LoadLibraryEx(path2, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	//HINSTANCE hdll = LoadLibrary("playH264ThreadDLL1.dll");
	GetIdlevideoINSTANCE = (fGetIdlevideoINSTANCE)GetProcAddress(hdll,"GetIdlevideoINSTANCE");
	RCC->INSTANCE = GetIdlevideoINSTANCE();
#ifdef log
	char a[10];
	itoa(RCC->INSTANCE,a,10);
#endif
	/*FILE *fp1;
	fp1 = fopen("c:\\instance1.log","a+");
	char a[10];
	itoa(RCC->INSTANCE,a,10);
	fputs(a,fp1);
	fputc('\n',fp1);
	fclose(fp1);*/
	
	/*Sleep(10);
	InitVideoParam = (fInitVideoParam)GetProcAddress(hdll,"InitVideoParam");
	InitVideoParam(RCC->INSTANCE,Myparam,);*/


#ifdef	log
	//char a[10];
	//itoa(initPort,a,10);
	FILE *fp;
	fp=fopen("c:\\test.log","ab+");
	fputs(a,fp);
	fputs(" ",fp);
	fputs("enter rtsp ",fp);
	fclose(fp);
#endif
//����ͨ��
	string setupName = "";
	srand( (unsigned int)time(0) );
	static int initPort = rand()%8000;
	INT rtpPort = 2000+6*initPort;
	initPort++;
	INT rtcpPort = rtpPort + 1;
	string sdp = "";
	//INT64 sess;
	char * sess = 0;

	//FILE *fp;
	//fp = fopen("c:\\port.log","a+");
	//char b[10];
	//itoa(initPort,b,10);
	//fputs(b,fp);
	//fputc('\n',fp);
	//fclose(fp);

	//��ȡ����IP
	CString ip;
	WORD wVersionRequested ;
	WSADATA wsaData;//��ʼ��
	char name[255];
	memset(name,'\0',255);

	PHOSTENT hostinfo = NULL;
	wVersionRequested = MAKEWORD( 2, 0 );

	if ( WSAStartup( wVersionRequested, &wsaData ) == 0 )
	{

		if( gethostname ( name, sizeof(name)) == 0)
		{
			if((hostinfo = (PHOSTENT)gethostbyname(name)) != NULL)
			{
				ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
			}
		}

		WSACleanup( );
	}
	
	RtspRequest *RTSPCLient = new RtspRequest;
	//����rtsp���� ͨ��tcp����
		
	if(!RTSPCLient->Open(RCC->Url, (LPSTR)(LPCTSTR)ip, rtpPort+5)){


#ifdef log
		fp=fopen("c:\\test.log","ab+");
		fputs(a,fp);
		fputs(" ",fp);
		fputs("open error!",fp);
		fclose(fp);
#endif
		
		//ʧ����Ҳ��Ҫ����
		/*int ret = freeVideos(RCC->INSTANCE);
			if(ret <0)
				return -1;*/
		if(Myparam!=NULL)
			{delete Myparam;Myparam = NULL;}
		if(rect!=NULL)
			{delete rect;rect = NULL;}
		if(RTSPCLient!=NULL)
			{delete RTSPCLient;RTSPCLient = NULL;}

		RCC->ans = 4; return -1;
	}
			
	if(!RTSPCLient->RequestOptions()) 
		if(!RTSPCLient->RequestOptions_test(RCC->UserName,RCC->Pwd)){
			
#ifdef log
		fp=fopen("c:\\test.log","ab+");
		fputs(a,fp);
		fputs(" ",fp);
		fputs("option error!",fp);
		fclose(fp);
#endif
		
		//ʧ����Ҳ��Ҫ����
		/*int ret = freeVideos(RCC->INSTANCE);
			if(ret <0)
				return -1;*/
		if(Myparam!=NULL)
			{delete Myparam;Myparam = NULL;}
		if(rect!=NULL)
			{delete rect;rect = NULL;}
		if(RTSPCLient!=NULL)
			{delete RTSPCLient;RTSPCLient = NULL;}
		
		RCC->ans = 4; return -1;}
		
	if(!RTSPCLient->RequestDescribe(&sdp))//��ʱ���������
		if(!RTSPCLient->RequestDescribe_test(&sdp,RCC->UserName,RCC->Pwd)) {
			
				
#ifdef log
			fp=fopen("c:\\test.log","ab+");
			fputs(a,fp);
			fputs(" ",fp);
			fputs("sdp error! ",fp);
			fclose(fp);
#endif
			
			//ʧ����Ҳ��Ҫ����
		/*int ret = freeVideos(RCC->INSTANCE);
			if(ret <0)
				return -1;*/
		if(Myparam!=NULL)
			{delete Myparam;Myparam = NULL;}
		if(rect!=NULL)
			{delete rect;rect = NULL;}
		if(RTSPCLient!=NULL)
			{delete RTSPCLient;RTSPCLient = NULL;}
			
		RCC->ans = 4; return -1;}
		
		//FILE *fp;
		//fp=fopen("c:\\sdp.log","ab+");
		//fputs(sdp.c_str(),fp);
		//fclose(fp);
	//video
	if((RTSPCLient->m_SetupName_video).length())
	{
		RTSPCLient->m_SetupName = RTSPCLient->m_SetupName_video;
		if(!RTSPCLient->RequestSetup(setupName.c_str(), transportModeRtpTcp, 0 , 1 , sess))
			if(!RTSPCLient->RequestSetup_test(setupName.c_str(), transportModeRtpTcp, 0 , 1 , sess,RCC->UserName,RCC->Pwd)){
#ifdef log
		fp=fopen("c:\\test.log","ab+");
		fputs(a,fp);
		fputs(" ",fp);
		fputs("setup error! ",fp);
		fclose(fp);
#endif
		
		//ʧ����Ҳ��Ҫ����
		/*int ret = freeVideos(RCC->INSTANCE);
			if(ret <0)
				return -1;*/
		if(Myparam!=NULL)
			{delete Myparam;Myparam = NULL;}
		if(rect!=NULL)
			{delete rect;rect = NULL;}
		if(RTSPCLient!=NULL)
			{delete RTSPCLient;RTSPCLient = NULL;}
		
		RCC->ans = 4; return -1;}
	}

	//audio
	if((RTSPCLient->m_SetupName_audio).length())
	{
		RTSPCLient->m_SetupName = RTSPCLient->m_SetupName_audio;
		if(!RTSPCLient->RequestSetup(setupName.c_str(), transportModeRtpTcp, 2 , 3 , sess)) 
			if(!RTSPCLient->RequestSetup_test(setupName.c_str(), transportModeRtpTcp, 2 , 3 , sess,RCC->UserName,RCC->Pwd)){
#ifdef log
		fp=fopen("c:\\test.log","ab+");
		fputs(a,fp);
		fputs(" ",fp);
		fputs("setup error! ",fp);
		fclose(fp);
#endif
		
		//ʧ����Ҳ��Ҫ����
		/*int ret = freeVideos(RCC->INSTANCE);
			if(ret <0)
				return -1;*/
		if(Myparam!=NULL)
			{delete Myparam;Myparam = NULL;}
		if(rect!=NULL)
			{delete rect;rect = NULL;}
		if(RTSPCLient!=NULL)
			{delete RTSPCLient;RTSPCLient = NULL;}
		
		RCC->ans = 4; return -1;}
	}

	RTSPCLient->m_SetupName = "";
	//RTSPCLient->m_SetupName_audio = "";
	//RTSPCLient->m_SetupName_video = "";
	if(!RTSPCLient->RequestPlay()) 
		/*if(!RTSPCLient->RequestPlay_test(RCC->UserName,RCC->Pwd))*/{

			
#ifdef log
		fp=fopen("c:\\test.log","ab+");
		fputs(a,fp);
		fputs(" ",fp);
		fputs("play error! ",fp);
		fclose(fp);
#endif
		
		//ʧ����Ҳ��Ҫ����
		/*int ret = freeVideos(RCC->INSTANCE);
			if(ret <0)
				return -1;*/
		if(Myparam!=NULL)
			{delete Myparam;Myparam = NULL;}
		if(rect!=NULL)
			{delete rect;rect = NULL;}
		if(RTSPCLient!=NULL)
			{delete RTSPCLient;RTSPCLient = NULL;}
		
		RCC->ans = 4; return -1;}
	

	//��ʼ��������dll
	if(RTSPCLient->frame!=-1)
		Myparam->fps = RTSPCLient->frame;

	Sleep(10);
	InitVideoParam = (fInitVideoParam)GetProcAddress(hdll,"InitVideoParam");
	//InitVideoParamNew = (fInitVideoParamNew)GetProcAddress(hdll,"InitVideoParamNew");
	InitVideoParam(RCC->INSTANCE,Myparam,RTSPCLient->Decode);
	//InitVideoParamNew(RCC->INSTANCE,Myparam,RTSPCLient->Decode,2);
	//���ûص�
	SetCallBack = (fSetCallBack)GetProcAddress(hdll,"SetCallBack");
	SetDrawLineCallBack = (fSetDrawLineCallBack)GetProcAddress(hdll,"SetDrawLineCallBack");
	SetBmpCallBack = (fSetBmpCallBack)GetProcAddress(hdll,"SetBmpCallBack");
	SetFillBmpCallBack = (fSetFillBmpCallBack)GetProcAddress(hdll,"SetFillBmpCallBack");
	setYUVCallBackFunc = (setYUVCallBack)GetProcAddress(hdll,"SetYUVCallBack");
	setH264CallBackFunc = (setH264CallBack)GetProcAddress(hdll,"SetH264CallBack");
	revoHWFunc = (revoHW)GetProcAddress(hdll,"RevoHWAcceleration");
	SetCallBack(RCC->INSTANCE,RCC->func);
	SetDrawLineCallBack(RCC->INSTANCE,RCC->funcD);
	SetBmpCallBack(RCC->INSTANCE,RCC->bmpFunc);
	SetFillBmpCallBack(RCC->INSTANCE,RCC->fillbmp);
	setYUVCallBackFunc(RCC->INSTANCE,RCC->YUVFunc,RCC->YUVEx);
	setH264CallBackFunc(RCC->INSTANCE,RCC->H264Func);
	if(!RCC->nHWAcceleration)
		revoHWFunc(RCC->INSTANCE);

	RTSPCLient->ID = RCC->INSTANCE;
	RTSPCLient->nfirst = true;
	RTSPCLient->initS = 0;

	if(RCC->circulation)
		return -1;//�Ѿ��ڲ�����

	RCC->circulation = true; 
	DWORD time1 = GetTickCount();
	DWORD time2 = GetTickCount();

	//while(1)
	//{
	//	RTSPCLient->Read_Test();
	//}
	RTSPCLient->initSdt();

	while(RCC->circulation)
	{
		RCC->ans = 1;

		int type = 0;
		short int size = 0;

		int rs = RTSPCLient->Read_Start(type ,&size);


		if(rs == -1)
			continue;



		if(size < 12)
			continue;
		if(type == 0)
		{
			int i =  RTSPCLient->Read_PlayLoad(size);
		}
		else if(type == 2||type == 3)//����2-3����ȫ������
		{
			RTSPCLient->Read_Leave(size);
		}
		else if(type == 1)
		{
			RTSPCLient->Read_RTCPVideo(size);
		}
		 
		
		//RTCPģ��,ÿ����Լ5s������һ�����ݣ����ʱ��ɸ���ʵ�ʴ�����е���
		time2 = GetTickCount();
		if(time2-time1>5000)
		{
			time1 = time2;
			//fraction lost
			char eTmp[2];
			eTmp[0] = RTSPCLient->eSeNum[1];
			eTmp[1] = RTSPCLient->eSeNum[0];

			char lTmp[2];
			lTmp[0] = RTSPCLient->lSeNum[1];
			lTmp[1] = RTSPCLient->lSeNum[0];
			
			RTSPCLient->sdt.RR.fractionLost = (UINT)((float)(((UINT16)eTmp-(UINT16)lTmp) - RTSPCLient->perGet)/(float)((UINT16)eTmp-(UINT16)lTmp)*256);

			RTSPCLient->perGet = 0;
			memcpy(RTSPCLient->lSeNum,RTSPCLient->eSeNum,2);
			//cumulation lost
			char sTmp[2];
			sTmp[0] = RTSPCLient->sSeNum[1];
			sTmp[1] = RTSPCLient->sSeNum[0];

			UINT16 lost = (UINT16)eTmp - (UINT16)sTmp - RTSPCLient->allGet;

			char l[2];

			memcpy(l+1,&lost,1);
			memcpy(l,&lost+1,1);
			
			memcpy(RTSPCLient->sdt.RR.cumulationLost+1,l,2);
			//interarrival jitter
			char jit[4];
			memcpy(jit,&RTSPCLient->jitter+3,1);
			memcpy(jit+1,&RTSPCLient->jitter+2,1);
			memcpy(jit+2,&RTSPCLient->jitter+1,1);
			memcpy(jit+3,&RTSPCLient->jitter+0,1);

			memcpy(RTSPCLient->sdt.RR.interJitter,jit,4);
			//last SR
			memcpy(RTSPCLient->sdt.RR.LSR,RTSPCLient->LSR,4);

			//DLSR
			time_t nowTime;
			time(&nowTime);
			
			DWORD delayTime = nowTime - RTSPCLient->lTime;
			
			char dTime[4];
			memcpy(dTime,&delayTime+3,1);
			memcpy(dTime+1,&delayTime+2,1);
			memcpy(dTime+2,&delayTime+1,1);
			memcpy(dTime+3,&delayTime,1);

			memcpy(RTSPCLient->sdt.RR.DLSR,dTime,4);

			memcpy(RTSPCLient->sdt.RR.SSRC_1,RTSPCLient->rcvf.SR.SSRC,4);

			RTSPCLient->Write_RTCPVideo(0);
		}
	}

#ifdef log
	fp=fopen("c:\\test.log","ab+");
	fputs(a,fp);
	fputs(" ",fp);
	fputs("out cir ",fp);
	fclose(fp);
#endif

	//FILE *fp;
	//fp =fopen("c:\\20150206.txt","a+");
	//fputs("�ر��˳�ѭ��",fp);
	//fclose(fp);

	//��ͨ��
	//if(!RTSPCLient->RequestPause())
	//	RTSPCLient->RequestPause_test(RCC->UserName,RCC->Pwd);


	//fp =fopen("c:\\20150206.txt","a+");
	//fputs("�رչ���1",fp);
	//fclose(fp);

	//if(!RTSPCLient->RequestTeardown())
	//	RTSPCLient->RequestTeardown_test(RCC->UserName,RCC->Pwd);


	//fp =fopen("c:\\20150206.txt","a+");
	//fputs("�رչ���2",fp);
	//fclose(fp);
	
	//pRtp->Close();
	//pRtcp->Close();

	//�ؽ���
	freeVideos = (ffreeVideos)GetProcAddress(hdll,"freeVideos");
	int ret = freeVideos(RCC->INSTANCE);
	if(ret <0)
		return -1;

	//fp =fopen("c:\\20150206.txt","a+");
	//fputs("�رչ���3",fp);
	//fclose(fp);

	//��ռ�
	//if(pRtp!=NULL)
	//{delete pRtp;pRtp = NULL;}
	if(Myparam!=NULL)
	{delete Myparam;Myparam = NULL;}
	if(rect!=NULL)
	{delete rect;rect = NULL;}
	if(RTSPCLient!=NULL)
	{delete RTSPCLient;RTSPCLient = NULL;}

	//fp =fopen("c:\\20150206.txt","a+");
	//fputs("�رչ���4",fp);
	//fclose(fp);

	//buff.FreeBuffer();

	RCC->ans = 2;

#ifdef log
	fp=fopen("c:\\test.log","ab+");
	fputs(a,fp);
	fputs(" ",fp);
	fputs("end play ",fp);
	fclose(fp);
#endif


	return 1;
}

//
//typedef DWORD WINAPI (* beginrecv)(LPVOID lpParam);
//**************************************************
//�������ܣ�ʵʱ����
//���������
//��������� 
//����ֵ��  1���ųɹ���-1����ʧ��
//**************************************************
int RTSPCLientClass::PlayURL(HWND hd)
{
	//�����߳�
	this->hd = hd;
	CreateThread(NULL,0,RTSPVideo,this,0,&threadID);
	while(ans != 1&&ans != 4)
		Sleep(1);
	if(threadID == -1||ans ==4)
		return -1;//�����߳�ʧ��
	return 1;
}

//**************************************************
//�������ܣ��ر�ʵʱ����
//���������
//��������� 
//����ֵ��  -1�ر�ʧ�ܣ�1�رճɹ�
//**************************************************
int RTSPCLientClass::stopURL()
{
	//��ѭ��
	/*FILE *fp;
	fp =fopen("c:\\20150206.txt","a+");
	fputs("�رտ�ʼ",fp);
	fclose(fp);*/

	circulation = false;
	while(ans!=2&&ans != 4)
		Sleep(1);
	

	/*fp =fopen("c:\\20150206.txt","a+");
	fputs("�رճɹ�",fp);
	fclose(fp);*/
	
	return 1;
}
