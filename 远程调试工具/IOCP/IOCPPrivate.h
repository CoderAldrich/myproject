#pragma once

typedef enum{
	TC_ERROR = 0,
	TC_TRANSFER_DATA=1,
	TC_HEART_BEAT = 2
}TCP_CTRL_CODE;

typedef struct tagIOCP_TCP_HEAD
{
	TCP_CTRL_CODE tcpCtrlCode;
	DWORD dwDataLen;
	BYTE  pData[0];
}IOCP_TCP_HEAD,*PIOCP_TCP_HEAD;

