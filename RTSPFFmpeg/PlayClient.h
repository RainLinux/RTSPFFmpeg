#include <list>
#include <map>

#include "RtspRequest.h"   
#include "Rtp.h"

#include "VBuffer.h"

#include "Decode.h"

class RTSPCLientClass
{
public:
    RTSPCLientClass();
    ~RTSPCLientClass();
private:
    DWORD threadID;
public:
    int InputURL(char* URL, char* UserName, char* PWD);
    int PlayURL(HWND hd);
    int stopURL();
public:
    char * Url;
    int INSTANCE;
    char* UserName;
    char* Pwd;
    bool circulation;
    int ans;//0 ��ʼ״̬��1 ����ѭ����2 �˳��߳�
    HWND hd;
    PFCALLBACK func;
    TDrawLineCallBack funcD;
    TBmpCallBack bmpFunc;
    TDrawRectCallBack fillbmp;
    TYUVCallBack YUVFunc;
    TH264CallBack H264Func;
    void * YUVEx;
    bool nHWAcceleration;
};

DWORD WINAPI RTSPVideo(LPVOID lpParam);