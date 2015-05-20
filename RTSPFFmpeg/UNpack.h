//#include "stdafx.h"
#include <stdio.h>  

#include <stdlib.h>  

#include <string.h>   

#include <errno.h>  

#include <sys/types.h>  

#include <fcntl.h>  

#include "PlayClient.h"

typedef struct
{
    unsigned char version;              //!< Version, 2 bits, MUST be 0x2  
    unsigned char padding;              //!< Padding bit, Padding MUST NOT be used  
    unsigned char extension;            //!< Extension, MUST be zero  
    unsigned char cc;                   //!< CSRC count, normally 0 in the absence of RTP mixers           
    unsigned char marker;               //!< Marker bit  
    unsigned char pt;                   //!< 7 bits, Payload Type, dynamically established  
    unsigned int seq_no;                //!< RTP sequence number, incremented by one for each sent packet   
    unsigned int timestamp;        //!< timestamp, 27 MHz for H.264  
    unsigned int ssrc;             //!< Synchronization Source, chosen randomly  
    unsigned char * payload;      //!< the payload including payload headers  
    unsigned int paylen;           //!< length of payload in bytes  
} RTPpacket_t;

typedef struct
{
    /*  0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |V=2|P|X|  CC   |M|     PT      |       sequence number         |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                           timestamp                           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |           synchronization source (SSRC) identifier            |
    +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
    |            contributing source (CSRC) identifiers             |
    |                             ....                              |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    //intel ��cpu ��intelΪС���ֽ��򣨵Ͷ˴浽�׵�ַ�� ��������Ϊ����ֽ��򣨸߶˴浽�͵�ַ��  
    /*intel ��cpu �� �߶�->csrc_len:4 -> extension:1-> padding:1 -> version:2 ->�Ͷ�
     ���ڴ��д洢 ��
     ��->4001���ڴ��ַ��version:2
     4002���ڴ��ַ��padding:1
     4003���ڴ��ַ��extension:1
     ��->4004���ڴ��ַ��csrc_len:4

     ���紫����� �� �߶�->version:2->padding:1->extension:1->csrc_len:4->�Ͷ�  (Ϊ��ȷ���ĵ�������ʽ)

     ��������ڴ� ��
     ��->4001���ڴ��ַ��version:2
     4002���ڴ��ַ��padding:1
     4003���ڴ��ַ��extension:1
     ��->4004���ڴ��ַ��csrc_len:4
     �����ڴ���� ���߶�->csrc_len:4 -> extension:1-> padding:1 -> version:2 ->�Ͷ� ��
     ����
     unsigned char csrc_len:4;        // expect 0
     unsigned char extension:1;       // expect 1
     unsigned char padding:1;         // expect 0
     unsigned char version:2;         // expect 2
     */
    /* byte 0 */
    unsigned char csrc_len : 4;        /* expect 0 */
    unsigned char extension : 1;       /* expect 1, see RTP_OP below */
    unsigned char padding : 1;         /* expect 0 */
    unsigned char version : 2;         /* expect 2 */
    /* byte 1 */
    unsigned char payloadtype : 7;     /* RTP_PAYLOAD_RTSP */
    unsigned char marker : 1;          /* expect 1 */
    /* bytes 2,3 */
    unsigned int seq_no;
    /* bytes 4-7 */
    unsigned int timestamp;
    /* bytes 8-11 */
    unsigned int ssrc;              /* stream number is used here. */
} RTP_FIXED_HEADER;


typedef struct
{
    unsigned char forbidden_bit;           //! Should always be FALSE  
    unsigned char nal_reference_idc;       //! NALU_PRIORITY_xxxx  
    unsigned char nal_unit_type;           //! NALU_TYPE_xxxx    
    unsigned int startcodeprefix_len;      //! ǰ׺�ֽ���  
    unsigned int len;                      //! ����nal ͷ��nal ���ȣ��ӵ�һ��00000001����һ��000000001�ĳ���  
    unsigned int max_size;                 //! ����һ��nal �ĳ���  
    unsigned char * buf;                   //! ����nal ͷ��nal ����  
    unsigned int lost_packets;             //! Ԥ��  
} NALU_t;

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*/
typedef struct
{
    //byte 0  
    unsigned char TYPE : 5;
    unsigned char NRI : 2;
    unsigned char F : 1;
} NALU_HEADER; // 1 BYTE   

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|F|NRI|  Type   |
+---------------+
*/
typedef struct
{
    //byte 0  
    unsigned char TYPE : 5;
    unsigned char NRI : 2;
    unsigned char F : 1;
} FU_INDICATOR; // 1 BYTE   

/*
+---------------+
|0|1|2|3|4|5|6|7|
+-+-+-+-+-+-+-+-+
|S|E|R|  Type   |
+---------------+
*/
typedef struct
{
    //byte 0  
    unsigned char TYPE : 5;
    unsigned char R : 1;
    unsigned char E : 1;
    unsigned char S : 1;
} FU_HEADER;   // 1 BYTES   

#define  MAXDATASIZE 1500  
#define PORT    1234  
#define BUFFER_SIZE 10   

NALU_t *AllocNALU(int buffersize)
{
    NALU_t *n;

    if((n = (NALU_t*)calloc(1, sizeof(NALU_t))) == NULL)
    {
        printf("AllocNALU Error: Allocate Meory To NALU_t Failed ");
        exit(0);
    }
    return n;
}

void FreeNALU(NALU_t *n)
{
    if(n)
    {
        free(n);
    }
}

/*
*bufIn:rtppackage
*len: the lengthe of rtppackage
*/
void rtp_unpackage(char *bufIn, int len, int ID, bool  *nfirst)
{

    unsigned char poutfile[1600];
    RTPpacket_t *p = NULL;
    RTP_FIXED_HEADER * rtp_hdr = NULL;
    NALU_HEADER * nalu_hdr = NULL;
    NALU_t * n = NULL;
    FU_INDICATOR    *fu_ind = NULL;
    FU_HEADER       *fu_hdr = NULL;
    int total_bytes = 0;                 //��ǰ������������  
    static int total_recved = 0;         //һ�����������  
    int fwrite_number = 0;               //�����ļ������ݳ���  

    //memcpy(recvbuf,bufIn, len);          //����rtp��  

    //printf("������+ rtpͷ��   = %d\n",len);  

    //////////////////////////////////////////////////////////////////////////  
    //begin rtp_payload and rtp_header  

    //p = (RTPpacket_t*)&recvbuf[0];  
    if((p = (RTPpacket_t *)malloc(sizeof(RTPpacket_t))) == NULL)
    {
        //printf ("RTPpacket_t MMEMORY ERROR\n");  
    }
    if((p->payload = (unsigned char *)malloc(MAXDATASIZE)) == NULL)
    {
        //printf ("RTPpacket_t payload MMEMORY ERROR\n");  
    }

    rtp_hdr = (RTP_FIXED_HEADER*)&bufIn[0];
    // printf("�汾��     : %d\n",rtp_hdr->version);  
    p->version = rtp_hdr->version;
    p->padding = rtp_hdr->padding;
    p->extension = rtp_hdr->extension;
    p->cc = rtp_hdr->csrc_len;
    //printf("��־λ     : %d\n",rtp_hdr->marker);  
    p->marker = rtp_hdr->marker;
    //printf("��������    :%d\n",rtp_hdr->payloadtype);  
    p->pt = rtp_hdr->payloadtype;
    // printf("����      : %d \n",rtp_hdr->seq_no);  
    p->seq_no = rtp_hdr->seq_no;
    //printf("ʱ���     : %d\n",rtp_hdr->timestamp);  
    p->timestamp = rtp_hdr->timestamp;
    //printf("֡��      : %d\n",rtp_hdr->ssrc);  
    p->ssrc = rtp_hdr->ssrc;

    //end rtp_payload and rtp_header  
    //////////////////////////////////////////////////////////////////////////  
    //begin nal_hdr  
    if(!(n = AllocNALU(4096)))          //Ϊ�ṹ��nalu_t�����Աbuf����ռ䡣����ֵΪָ��nalu_t�洢�ռ��ָ��  
    {
        //printf("NALU_t MMEMORY ERROR\n");  
    }
    //if ((nalu_hdr = (NALU_HEADER *)malloc(sizeof(NALU_HEADER))) == NULL)  
    //   {  
    //       //printf("NALU_HEADER MEMORY ERROR\n");  
    //   }  

    nalu_hdr = (NALU_HEADER*)&bufIn[12];                        //���紫��������ֽ��� ���������ڴ滹�Ǻ��ĵ��������෴��ֻҪƥ�������ֽ�����ĵ��������ɴ�����ȷ��  
    //printf("forbidden_zero_bit: %d\n",nalu_hdr->F);              //���紫���еķ�ʽΪ��F->NRI->TYPE.. �ڴ��д洢��ʽΪ TYPE->NRI->F (��nalͷƥ��)��  
    n->forbidden_bit = nalu_hdr->F << 7;                          //�ڴ��е��ֽ���  
    // printf("nal_reference_idc:  %d\n",nalu_hdr->NRI);  
    n->nal_reference_idc = nalu_hdr->NRI << 5;
    //printf("nal ��������:       %d\n",nalu_hdr->TYPE);  
    n->nal_unit_type = nalu_hdr->TYPE;

    //end nal_hdr  
    //////////////////////////////////////////////////////////////////////////  
    //��ʼ���  
    if(nalu_hdr->TYPE != 7 && (*nfirst))  //����67��ͷ�İ������һ��ǵ�һ����
    {
        // printf("������д���0�޶���\n"); 
        return;
    }
    *nfirst = false;
    if(nalu_hdr->TYPE > 0 && nalu_hdr->TYPE < 24)  //����  
    {
        //printf("��ǰ��Ϊ����\n");  
        poutfile[0] = 0x00;
        poutfile[1] = 0x00;
        poutfile[2] = 0x00;
        poutfile[3] = 0x01;
        //д����ʼ�ֽ�0x00000001  
        total_bytes += 4;
        memcpy(p->payload, &bufIn[13], len - 13);
        p->paylen = len - 13;
        memcpy(poutfile + 4, nalu_hdr, 1);  //дNAL_HEADER  
        total_bytes += 1;
        memcpy(poutfile + 5, p->payload, p->paylen);//дNAL����
        total_bytes += p->paylen;
        poutfile[total_bytes] = '\0';
        //printf("������ + nal= %d\n",total_bytes);  
    }

    else if(nalu_hdr->TYPE == 28)                    //FU-A��Ƭ��������˳��ʹ���˳����ͬ  
    {
        /*if ((fu_ind = (FU_INDICATOR *)malloc(sizeof(FU_INDICATOR))) == NULL)
        {
        printf("FU_INDICATOR MEMORY ERROR\n");
        }
        if ((fu_hdr = (FU_HEADER *)malloc(sizeof(FU_HEADER))) == NULL)
        {
        printf("FU_HEADER MEMORY ERROR\n");
        }  */

        fu_ind = (FU_INDICATOR*)&bufIn[12];     //��Ƭ���õ���FU_INDICATOR������NALU_HEADER  
        //printf("FU_INDICATOR->F     :%d\n",fu_ind->F);  
        n->forbidden_bit = fu_ind->F << 7;
        //printf("FU_INDICATOR->NRI   :%d\n",fu_ind->NRI);  
        n->nal_reference_idc = fu_ind->NRI << 5;

        n->nal_unit_type = fu_ind->TYPE;

        fu_hdr = (FU_HEADER*)&bufIn[13];        //FU_HEADER��ֵ  
        //printf("FU_HEADER->S        :%d\n",fu_hdr->S);  
        //printf("FU_HEADER->E        :%d\n",fu_hdr->E);  
        //printf("FU_HEADER->R        :%d\n",fu_hdr->R);  
        //printf("FU_HEADER->TYPE     :%d\n",fu_hdr->TYPE);  
        n->nal_unit_type = fu_hdr->TYPE;               //Ӧ�õ���FU_HEADER��TYPE  

        if(rtp_hdr->marker == 1)                      //��Ƭ�����һ����  
        {
            //printf("��ǰ��ΪFU-A��Ƭ�����һ����\n");  
            memcpy(p->payload, &bufIn[14], len - 14);
            p->paylen = len - 14;
            memcpy(poutfile, p->payload, p->paylen);  //дNAL����  
            total_bytes = p->paylen;
            poutfile[total_bytes] = '\0';
            //printf("������ + FU = %d\n",total_bytes);    
        }
        else if(rtp_hdr->marker == 0)                 //��Ƭ�� ���������һ����  
        {
            if(fu_hdr->S == 1)                        //��Ƭ�ĵ�һ����  
            {
                unsigned char F;
                unsigned char NRI;
                unsigned char TYPE;
                unsigned char nh;

                poutfile[0] = 0x00;
                poutfile[1] = 0x00;
                poutfile[2] = 0x00;
                poutfile[3] = 0x01;               //д��ʼ�ֽ���0x00000001  
                total_bytes += 4;

                F = fu_ind->F << 7;
                NRI = fu_ind->NRI << 5;
                TYPE = fu_hdr->TYPE;                                            //Ӧ�õ���FU_HEADER��TYPE  
                //nh = n->forbidden_bit|n->nal_reference_idc|n->nal_unit_type;  //�������ļ�Ҳ�ǰ� ���ֽ���洢  
                nh = F | NRI | TYPE;

                poutfile[4] = nh;              //дNAL HEADER  

                total_bytes += 1;
                memcpy(p->payload, &bufIn[14], len - 14);
                p->paylen = len - 14;
                memcpy(poutfile + 5, p->payload, p->paylen);  //дNAL����  
                total_bytes += p->paylen;
                poutfile[total_bytes] = '\0';
                //printf("������ + FU_First = %d\n",total_bytes);      
            }
            else                                      //������ǵ�һ����  
            {
                //printf("��ǰ��ΪFU-A��Ƭ��\n");  
                memcpy(p->payload, &bufIn[14], len - 14);
                p->paylen = len - 14;
                memcpy(poutfile, p->payload, p->paylen);  //дNAL����  
                total_bytes = p->paylen;
                poutfile[total_bytes] = '\0';
                //printf("������ + FU = %d\n",total_bytes);    
            }
        }
    }
    else if(nalu_hdr->TYPE == 29)                //FU-B��Ƭ��������˳��ʹ���˳����ͬ  
    {
        if(rtp_hdr->marker == 1)                  //��Ƭ�����һ����  
        {
            //printf("��ǰ��ΪFU-B��Ƭ�����һ����\n");  

        }
        else if(rtp_hdr->marker == 0)             //��Ƭ�� ���������һ����  
        {
            //printf("��ǰ��ΪFU-B��Ƭ��\n");  
        }
    }
    else
    {
        //printf("������д���30-31 û�ж���\n");  
    }
    /*total_recved += total_bytes;
    printf("total_recved = %d\n",total_recved);  */

    //memset(recvbuf,0,1500);  
    free(p->payload);
    free(p);
    FreeNALU(n);

    fDecodeVideo DecodeVideo;
    // *****************************************************************************************
    HANDLE hDllhandle = GetModuleHandle(L"RtspClientFfmepg.dll");
    if(hDllhandle == NULL)
    {
        MessageBox(NULL, L"��ȡ��̬����ʧ��", L"", MB_OK);
    }

    TCHAR path1[1024], path2[1024];
    memset(path1, 0, 1024 * sizeof(TCHAR));
    memset(path2, 0, 1024 * sizeof(TCHAR));

    GetModuleFileName((HMODULE)hDllhandle, path1, 1000);

    int lenPath = wcslen(path1) - wcslen(L"RtspClientFfmepg.dll");
    wcsncpy(path2, path1, lenPath);
    wcscat(path2, L"playH264ThreadDLL1.dll");
    //***********************************************************************************************

    HINSTANCE hdll = LoadLibraryEx(path2, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    //HINSTANCE hdll = LoadLibrary("playH264ThreadDLL1.dll");
    DecodeVideo = (fDecodeVideo)GetProcAddress(hdll, L"DecodeVideo");
    DecodeVideo(ID, poutfile, total_bytes);

    //char a[10] = "c:\\��";
    //itoa(ID,a+5,10);
    //FILE *fp;
    //fp = fopen(a,"ab+");
    //fwrite(poutfile,1,total_bytes,fp);
    //fclose(fp);


    //�������  
    //////////////////////////////////////////////////////////////////////////  
    return;
}
int ftyp = 0;
int moov = 0;


void rtp_unpackage_mpeg(char *bufIn, int len, int ID, bool  *nfirst)
{



    unsigned char recvbuf[1500];
    memcpy(recvbuf, bufIn, len);

    if(*nfirst)
    {
        if(!(recvbuf[12] == 0x00 && recvbuf[13] == 0x00 && recvbuf[14] == 0x01 && recvbuf[15] == 0xB0))
            return;
        *nfirst = false;
    }

    //FILE *fp;
    //fp = fopen("c:\\20150209.m4v","ab+");
    //fwrite(recvbuf+12,1,len-12,fp);
    //fclose(fp);

    fDecodeVideo DecodeVideo;
    // *****************************************************************************************
    HANDLE hDllhandle = GetModuleHandle("RtspClientFfmepg.dll");
    if(hDllhandle == NULL)
    {
        MessageBox(NULL, "��ȡ��̬����ʧ��", "", MB_OK);
    }

    char path1[1024], path2[1024];
    memset(path1, 0, 1024);
    memset(path2, 0, 1024);

    GetModuleFileName((HMODULE)hDllhandle, path1, 1000);

    int lenPath = strlen(path1) - strlen("RtspClientFfmepg.dll");
    strncpy(path2, path1, lenPath);
    strcat(path2, "playH264ThreadDLL1.dll");
    //***********************************************************************************************

    HINSTANCE hdll = LoadLibraryEx(path2, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    //HINSTANCE hdll = LoadLibrary("playH264ThreadDLL1.dll");
    DecodeVideo = (fDecodeVideo)GetProcAddress(hdll, "DecodeVideo");
    DecodeVideo(ID, recvbuf + 12, len - 12);
}

