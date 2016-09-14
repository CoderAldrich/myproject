#ifndef	__ARP_H
#define	__ARP_H

#define	ARP_REQUEST		1
#define	ARP_RESPONSE		2
#define	RARP_REQUEST		3
#define	RARP_RESPONSE		4

#pragma pack(1)
 
typedef struct _ARP_HEADER //以太网arp字段长度14  28
{
	unsigned short arp_hard; //硬件类型  2
	unsigned short arp_Protocol; //协议类型 2
	unsigned char arp_hardlen; //硬件地址长度（6）1
	unsigned char arp_ProtocolLen; //协议地址长度（4）1
	unsigned short arp_op; //回应还是请求  1为查询，2为查询结果

	unsigned char sourceMac[6]; //发送者MAC地址
	unsigned char arp_spa[4]; //发送者IP
	unsigned char destMac[6]; //接收者MAC地址
	unsigned char arp_tpa[4]; //接收者IP
}ARP_HEADER,*PARP_HEADER;

#endif