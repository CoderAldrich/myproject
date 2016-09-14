#ifndef	__ARP_H
#define	__ARP_H

#define	ARP_REQUEST		1
#define	ARP_RESPONSE		2
#define	RARP_REQUEST		3
#define	RARP_RESPONSE		4

#pragma pack(1)
 
typedef struct _ARP_HEADER //��̫��arp�ֶγ���14  28
{
	unsigned short arp_hard; //Ӳ������  2
	unsigned short arp_Protocol; //Э������ 2
	unsigned char arp_hardlen; //Ӳ����ַ���ȣ�6��1
	unsigned char arp_ProtocolLen; //Э���ַ���ȣ�4��1
	unsigned short arp_op; //��Ӧ��������  1Ϊ��ѯ��2Ϊ��ѯ���

	unsigned char sourceMac[6]; //������MAC��ַ
	unsigned char arp_spa[4]; //������IP
	unsigned char destMac[6]; //������MAC��ַ
	unsigned char arp_tpa[4]; //������IP
}ARP_HEADER,*PARP_HEADER;

#endif