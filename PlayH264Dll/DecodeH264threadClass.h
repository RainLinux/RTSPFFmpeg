#include <windows.h>
#include <winsock2.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <concurrent_queue.h>

extern "C"
{
#include <libavcodec\avcodec.h>
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
}


#pragma comment(lib,"Ws2_32.lib")  
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avdevice.lib")
#pragma comment(lib,"avfilter.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swresample.lib")
#pragma comment(lib,"swscale.lib")

//#pragma comment(lib,"libx264-120.lib")
//#pragma comment(lib,"libx264.lib")

#define PLAYVIDEO 0
#define STOPVIDEO -2
#define PAUSEVIDEO -1
#define ErrorNoMemory -2
#define CLASSDLL_API __declspec(dllexport)
#define BUFH264 1
#define BUFBMP 2
typedef struct SwsContext SwScaleContext;    //just for convenience
const int ListCount = 100;

//static SDL_Surface *screen;

typedef int (WINAPI *PFCALLBACK)(int INSTANCE, int width, int height, char *buf, int bufsize, int buftype);//define my CallBack Func

//////////////////////////////////////////////////////////////////////////
//��͸��������λص�������bmpͼƬBuffer��bmpͼƬwid��bmpͼƬhigh��
typedef int(WINAPI *TDrawRectCallBack)(char*, int, int);
//////////////////////////////////////////////////////////////////////////
typedef int (WINAPI *TDrawLineCallBack)(int, HDC);//define my CallBack Func
//////////////////////////////////////////////////////////////////////////
//TBmpCallBack����˵����char*��BMPbufer��,int(buferSize),bmpWidth,bmpHight,int(֡��),int(ͨ����),int(��������),HWND,���
typedef int (WINAPI *TBmpCallBack)(char*, int, int, int, int, int, int, HWND);
//////////////////////////////////////////////////////////////////////////
//TYUVCallBack����˵��: yuvbuffer����ͼƬ���Ϳ�
typedef int (WINAPI *TYUVCallBack)(unsigned char *buffer, int width, int length, void *);

typedef int (WINAPI *TH264CallBack)(int, char *, int len, int wid, int height);
//�յ��������ݿ�
typedef struct
{
    int  fileSize;
    int  FrameNum;
    int  readPos;
    char picBuffer[1024 * 300];
}netBuf, *LPnetBuf;

typedef struct
{
    int width;             //ʵ�ʿ��
    int height;            //ʵ�ʸ߶�
    int playWidth;         //��ʾ���
    int playHeight;        //��ʾ�߶�
    int fps;               //����Ƶ��
    HWND playHandle;       //���ž��
    int stopPlay;          //���ſ���
    int playChannle;
    bool isDecode;        //�Ƿ����
    //int  VideoCode;       //��������  0��H264  1 mpeg4 
}myparamInput;

typedef struct
{
    int size;
    unsigned char * data;
}dataNode;


class playH264VideoClass
{
public:
    playH264VideoClass();
    ~playH264VideoClass();

    void  dataQueueClean();
private:
    //LPnetBuf   BuffList[ListCount];
    Concurrency::concurrent_queue<dataNode *> m_DataQueue;


    unsigned long int readNetBufIndex;
    unsigned long int writeNetBufIndex;
    unsigned long int wcount;
    int readNetBufIndexPosize;
    HANDLE hThreadDecode, writewait;
    DWORD threadID;
    BITMAPFILEHEADER bmpheader;

    int bpp;
    HBITMAP  hbit;
    HBITMAP  OldBitmap;
    uint8_t *bufptr;


public:
    int writeNetBuf(int num, unsigned char *buf, int bufsize);
    //int getNetBuf(AVCodecContext* m_pCodecContext,AVCodecParserContext * m_parser,char *buf,int bufsize);
    int setReadPosize(int index, int readsize);
    int getNextNetBuf(char *buf, int bufsize);
    int playResize(int newWidth, int newHeight);
    //int SaveAsBMPbuf(AVFrame *pFrameRGB, int width, int height,uint8_t *BMPbuf);
    int playBMPbuf(AVFrame *pFrameRGB, int width, int height, int playW, int playH, HDC playHD, HDC hmemDC, uint8_t *BMPbuf, HWND);
public:
    myparamInput paramUser;
    int InputParam(myparamInput *p1);
    int freeParam(void);
    int playVideo();
    int pauseVideo();
    PFCALLBACK func;
    TDrawLineCallBack funcD;
    TBmpCallBack bmpFunc;
    TDrawRectCallBack fillbmp;
    TYUVCallBack yuvFunc;
    TH264CallBack H264Func;

    BITMAPINFOHEADER bmpinfo;
    int INSTANCE;
    void abc() { writeNetBufIndex++; }
    unsigned long int getW() { return writeNetBufIndex; }
    unsigned long int getR() { return readNetBufIndex; }
public:
    int type;//�������� 1,h264,2,mpeg,3,FLV
    void *userBuffer;
    bool nHWAcceleration;
};

DWORD WINAPI videoDecodeQueue(LPVOID lpParam);
//int SaveAsBMPbuf(AVFrame *pFrameRGB, int width, int height,uint8_t *BMPbuf);
//void DrawImage(HDC hdc,HBITMAP hbmp,COLORREF color,int x,int y,int width,int height,int SrcX,int SrcY);



