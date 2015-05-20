#pragma once

#include "Udp.h"
#include "VBuffer.h"
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

//typedef struct 
//{
//	UINT8 head;
//	UINT8 PT;
//	UINT8 length[2];
//	UINT8 SSRC[4];
//	UINT8 NTP[8];
//	UINT8 RTP[4];
//	UINT8 packetCount[4];
//	UINT8 octetCount[4];
//} RtcpSR;
//
//typedef struct 
//{
//	UINT8 head;
//	UINT8 PT;
//	UINT8 length[2];
//	UINT8 SSRC[4];
//	UINT8 SSRC_1[4];//SSRC of first source��ӦSR��SSRC
//	UINT8 fractionLost;//���ϸ�sr��rr�����Ķ�����,����Ϊ���շ��˶�ʱ�����ڴ���RTP�������յ���RTP����Ŀ�Ĳ�ֵ�����ڴ���RTP���ĵ���Ŀ�ı�ֵ����Ϊ��ֵ����Ϊ0
//	UINT8 cumulationLost[3];//�ۼƶ�����
//	UINT8 EHSNR[4];//extended highest sequence number received
//	UINT8 interJitter[4];//����ʱ�䶶��
//	UINT8 LSR[4];//last SR
//	UINT8 DLSR[4];//delay since last SR
//} RtcpRR;
//
//typedef struct 
//{
//	UINT8 head;
//	UINT8 PT;
//	UINT8 length[2];
//	UINT8 SSRC[4];//
//	UINT8 user[100];//�û�����
//}RtcpSDES;//�����Э��Ϊ���ţ�������ֻȡ��һ���ݿ����
//
//typedef struct 
//{
//	RtcpSR SR;
//	RtcpSDES SDES;
//}recieveSRFrom;
//
//typedef struct
//{
//	RtcpRR RR;
//	RtcpSDES SDES;
//} sendRRTo;

class Rtcp : public Udp
{
public:

	Rtcp(UINT mtu = 1500);
	virtual ~Rtcp();

	virtual int Read(BYTE* pBuffer, UINT16 bufferSize,UINT nTimeOut = 0);

	virtual int Write(UINT nTimeOut = 500000);

	virtual int Handle(BYTE* pBuffer, UINT16 bufferSize);

	void initSdt();

	static int ssrc;

	void ParseMrl(string mrl);

private:
	string IP;
	int port;
protected:

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
};

