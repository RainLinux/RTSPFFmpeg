
//#pragma comment(lib,"playH264ThreadDLL1.lib")


//���벥��ʱ�õ������ݽṹ
typedef struct
{
    int width;             //ʵ�ʿ��
    int height;            //ʵ�ʸ߶�
    int playWidth;         //��ʾ���
    int playHeight;        //��ʾ�߶�
    int fps;               //����Ƶ��
    HWND playHandle;       //���ž��
    int stopPlay;          //���ſ���
    int playChannle;		//���ŵ�ͨ����
    bool isDecode;
}myparamInput;

typedef int(_cdecl *PFCALLBACK)(int INSTANCE, int width, int height, char *buf, int bufsize, int buftype);
typedef int(_cdecl *TDrawRectCallBack)(char*, int, int);
typedef int(_cdecl *TDrawLineCallBack)(int, HDC);//define my CallBack Func
typedef int(_cdecl *TBmpCallBack)(char*, int, int, int, int, int, int, HWND);
typedef int(_cdecl *TYUVCallBack)(char *, int len, int wid, void *);//YUV�ص�
typedef int (WINAPI *TH264CallBack)(int, char *, int len, int wid, int height);//H264�ص�

typedef int(_cdecl *setYUVCallBack)(int, TYUVCallBack, void *);
typedef int(_cdecl *setH264CallBack)(int, TH264CallBack);
typedef int(_cdecl *fSetCallBack)(int INSTANCE, PFCALLBACK f1);
typedef void(_cdecl *fDecodeVideo)(int num, unsigned char * pInBuffer, int size);
typedef int(_cdecl *finitVideoDLL)();
typedef int(_cdecl *fGetIdlevideoINSTANCE)();
typedef int(_cdecl *fInitVideoParam)(int INSTANCE, myparamInput *Myparam, int);
typedef int(_cdecl *fInitVideoParamNew)(int, myparamInput *, int, int);
typedef int(_cdecl *fpauseVideos)(int INSTANCE);
typedef int(_cdecl *fplayVideos)(int INSTANCE);
typedef int(_cdecl *ffreeVideos)(int INSTANCE);
typedef int(_cdecl *finputBuf)(int INSTANCE, char *buf, int bufsize);
typedef int(_cdecl *fresize)(int INSTANCE, int width, int height);
typedef void(_cdecl *fexitdll)();
typedef int(_cdecl *fSetDrawLineCallBack)(int INSTANCE, TDrawLineCallBack f1);
typedef int(_cdecl *fSetBmpCallBack)(int INSTANCE, TBmpCallBack bmp1);
typedef int(_cdecl *fSetFillBmpCallBack)(int INSTANCE, TDrawRectCallBack bmpf);
typedef int(_cdecl *revoHW)(int INSTANCE);

