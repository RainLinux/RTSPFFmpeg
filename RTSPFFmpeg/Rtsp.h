// Rtsp.h: interface for the Rtsp class.
//
//////////////////////////////////////////////////////////////////////

//Ϊ�˷��㣬�������е�rtp��rtcpͨѶȫ����rtsp������ɣ�����ɶ����ں����汾�����


#define RTCP_PT_START		1
#define RTCP_PT_SIZE		1

#define RTCP_LENGTH_START	2
#define RTCP_LENGTH_SIZE	2

#define RTCP_SSRC_START		4
#define RTCP_SSRC_SIZE		4

#define RTCP_NTPTIME_START  8
#define RTCP_NTPTIME_SIZE   8

#define RTCP_RTPTIME_START	16
#define RTCP_RTPTIME_SIZE	4

#define RTCP_PACKET_START	20
#define RTCP_PACKET_SIZE	4

#define RTCP_PLAYLOAD_START 24
#define RTCP_PLAYLOAD_SIZE  4

#if !defined(AFX_RTSP_H__8C4DD607_2EBB_4D85_B489_E739ACD1D93F__INCLUDED_)
#define AFX_RTSP_H__8C4DD607_2EBB_4D85_B489_E739ACD1D93F__INCLUDED_

#include "Tcp.h"

#define RTSP_PROTOCOL_PORT 554

typedef struct 
{
	UINT8 head;
	UINT8 PT;
	UINT8 length[2];
	UINT8 SSRC[4];
	UINT8 NTP[8];
	UINT8 RTP[4];
	UINT8 packetCount[4];
	UINT8 octetCount[4];
} RtcpSR;

typedef struct 
{
	UINT8 head;
	UINT8 PT;
	UINT8 length[2];
	UINT8 SSRC[4];
	UINT8 SSRC_1[4];//SSRC of first source��ӦSR��SSRC
	UINT8 fractionLost;//���ϸ�sr��rr�����Ķ�����,����Ϊ���շ��˶�ʱ�����ڴ���RTP�������յ���RTP����Ŀ�Ĳ�ֵ�����ڴ���RTP���ĵ���Ŀ�ı�ֵ����Ϊ��ֵ����Ϊ0
	UINT8 cumulationLost[3];//�ۼƶ�����
	UINT8 EHSNR[4];//extended highest sequence number received
	UINT8 interJitter[4];//����ʱ�䶶��
	UINT8 LSR[4];//last SR
	UINT8 DLSR[4];//delay since last SR
} RtcpRR;

typedef struct 
{
	UINT8 head;
	UINT8 PT;
	UINT8 length[2];
	UINT8 SSRC[4];//
	UINT8 user[100];//�û�����
}RtcpSDES;//�����Э��Ϊ���ţ�������ֻȡ��һ���ݿ����

typedef struct 
{
	RtcpSR SR;
	RtcpSDES SDES;
}recieveSRFrom;

typedef struct
{
	RtcpRR RR;
	RtcpSDES SDES;
} sendRRTo;

typedef enum serverState
{
	stateInit = 0,//0
	stateConnected,//1
	stateReady,//2
	statePlaying,//3
	statePause,//4
	stateRecording//5
};

typedef enum streamingTransportMode
{
	transportModeRtpUdp = 1,
	transportModeRtpTcp,
	transportModeRawUdp
};

static const int MAX_FIELDS_COUNT = 256;

class Rtsp  : public Tcp
{
public:
	int Decode;//������ѡ��1 h264��2 mpeg
	int ID;//��������
	bool nfirst;//�ж��Ƿ�Ϊ��һ����
public:
	Rtsp();
	virtual ~Rtsp();

	virtual int  Write(string str);
	virtual int  Read(string& str);

	virtual void AddField(string field);
	virtual void WriteFields();

	virtual BOOL ParseMrl(string mrl, string* pPreSuffix, string* pSuffix, int* pPort);

	virtual int Read_Start(int& type,short int *size);

	virtual int Read_Head();

	virtual int Read_PlayLoad(short int len);

	virtual int Read_Leave(int len);

	virtual int Read_RTCPVideo(int len);

	virtual int Write_RTCPVideo(UINT nTimeout = 10);

	virtual int Handle_RTCPVideo(BYTE * buffer,int buffersize);

	virtual void initSdt();

	virtual int Read_Test();

protected:
	int		m_State;
	int		m_CSeq;
	//INT64	m_Session;
	char	m_Session[50];

	vector<string> m_Fields;

public:
	//���´�˸�ʽ
	int initS;//ȷ����һ�����Ƿ���
	unsigned char sSeNum[2];//����İ���
	unsigned char lSeNum[2];//��һ�η���RR�İ���
	unsigned char eSeNum[2];//���һ�εİ���
	UINT16 allGet;//�ܽ�����
	UINT8 perGet;//���ν�����
	unsigned char LSR[4];//:��reportee������յ���Sender Report��NTP timestamp����32bits.(����Ϊ0) 
	time_t  lTime;//SSRC_nԴ���ϸ�SR

	int R_S;//R-S
	int jitter;//ʱ�䶶��

	recieveSRFrom rcvf;
	sendRRTo sdt;

	static int ssrc;

};

#endif // !defined(AFX_RTSP_H__8C4DD607_2EBB_4D85_B489_E739ACD1D93F__INCLUDED_)
