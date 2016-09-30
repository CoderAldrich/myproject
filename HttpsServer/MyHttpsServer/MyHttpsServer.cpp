// MyHttpsServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
//#include <sys/types.h>
#include <winsock2.h>


#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libeay32.lib")

#pragma comment(lib,"ws2_32.lib")


/*所有需要的参数信息都在此处以#define的形式提供*/ 
#define CERTF "server.crt" /*服务端的证书(需经CA签名)*/ 
#define KEYF  "server.key" /*服务端的私钥(建议加密存储)*/ 
#define CACERT "ca.crt" /*CA 的证书*/ 
#define PORT 443 /*准备绑定的端口*/ 


#include "HttpSendParser.h"
#include "PublicFun.h"


typedef struct _tagREQUEST_PARAM
{
	SOCKET sockClient;
	SSL_CTX* ctx;
}REQUEST_PARAM,*PREQUEST_PARAM;
DWORD WINAPI HandleRequestThread( PVOID pParam )
{

	PREQUEST_PARAM pRequestParam = (PREQUEST_PARAM)pParam;

	if (!pRequestParam)
	{
		return -1;
	}

	SOCKET sockClient = INVALID_SOCKET;
	SSL_CTX* ctx = NULL ;
	if ( pRequestParam )
	{
		sockClient = pRequestParam->sockClient;
		ctx = pRequestParam->ctx;

		delete pRequestParam;
		pRequestParam = NULL;
	}

	if ( !ctx)
	{
		return -2;
	}

	SSL* ssl = NULL;
	
	do 
	{
		ssl = SSL_new (ctx);  
		if ( NULL == ssl )
		{
			break;
		}

		SSL_set_fd (ssl, sockClient);
		int err = SSL_accept (ssl);
		if ( -1 == err)
		{
			//ERR_print_errors_fp(stderr);
			break;
		}

		char     buf [4096];

		CStringA strUrl;
		int inlen = 0, nread = 0;
		for ( nread = 0; nread < sizeof(buf);  )
		{
			inlen = SSL_read(ssl, buf+nread,sizeof(buf)-nread);
			if ( inlen == -1 )
			{
				break;
			}

			if ( inlen <=0 ) break;//出错或客户端无数据

			//fwrite(buf+nread, 1,inlen, stdout);//将接收到的信息打印到标准输出

			nread += inlen;
			CHttpSendParser sendparser;
			if(sendparser.ParseData(buf,nread))
			{
				strUrl = sendparser.GetParseUrl();

				printf(strUrl+"\r\n");

				break;
			}
		}

		buf[nread] = '\0';

		CString strWebContent;
		//strWebContent=GetHttpString(CString(strUrl));
		strWebContent=L"<html><body>Hello World!!</body></html>";

		CStringA strResponse;
		strResponse.Format("HTTP/1.1 200 OK\r\nCache-Control: private\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s",strWebContent.GetLength(),CStringA(strWebContent).GetBuffer());
		err = SSL_write (ssl, strResponse.GetBuffer(), strResponse.GetLength());

	} while (FALSE);
	

	if (ssl)
	{
		SSL_shutdown(ssl);//关闭SSL连接
		SSL_free (ssl);
	}

	closesocket (sockClient);


	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	int listen_sd;
	
	int client_len;
	SSL_CTX* ctx;

	/*const*/ SSL_METHOD *meth;
	WSADATA wsaData;
	struct sockaddr_in sa_serv;
	struct sockaddr_in sa_cli;

	if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
	{ 
		printf("WSAStartup()fail:%d\n",GetLastError()); 
		return -1;
	} 

	/* SSL preliminaries. We keep the certificate and key with the context. */
	SSL_load_error_strings(); /*为打印调试信息作准备*/ 
	OpenSSL_add_ssl_algorithms(); /*初始化*/ 

	meth = (SSL_METHOD *)TLSv1_server_method(); /*采用什么协议(SSLv2/SSLv3/TLSv1)在此指定,TLSv1_server_method,SSLv23_server_method()*/ 

	ctx = SSL_CTX_new (meth);
	//CHK_NULL(ctx);
	if (!ctx) 
	{
		printf("创建SSL_CTX失败!\n");
		exit(2);
	}

	//设置为要求强制校验对方（客户端）证书SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT
	SSL_CTX_set_verify(ctx,SSL_VERIFY_NONE,NULL); /*验证与否SSL_VERIFY_PEER*/ 
	//SSL_CTX_load_verify_locations(ctx,CACERT,NULL); /*若验证,则放置CA证书*/

	if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) 
	{
		printf("加载证书失败!\n");
		exit(3);
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0)
	{
		printf("加载私钥失败!\n");
		exit(4);
	}

	if (!SSL_CTX_check_private_key(ctx)) {
		printf("密钥证书不匹配！\n");
		exit(5);
	}

	SSL_CTX_set_cipher_list(ctx,"DES-CBC3-SHA"); 
	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

	/* ----------------------------------------------- */
	/* Prepare TCP socket for receiving connections */
	/*开始正常的TCP socket过程.................................*/ 
	printf("Begin TCP socket...\n"); 


	listen_sd = socket (AF_INET, SOCK_STREAM, 0);   
	if( SOCKET_ERROR == listen_sd )
	{
		return 0;
	}

	memset (&sa_serv, '\0', sizeof(sa_serv));
	sa_serv.sin_family      = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port        = htons (PORT);          /* Server Port number */

	err = bind(listen_sd, (struct sockaddr*) &sa_serv, sizeof (sa_serv));                   
	if( SOCKET_ERROR == err )
	{
		return 0;
	}

	/*接受TCP链接*/ 
	err = listen (listen_sd, 5); 
	if( SOCKET_ERROR == err )
	{
		return 0;
	}

	client_len = sizeof(sa_cli);

	while (TRUE)
	{
		SOCKET sockClient = accept (listen_sd, (struct sockaddr*) &sa_cli, (int*)&client_len);
		if ( SOCKET_ERROR == sockClient )
		{
			continue;
		}

		PREQUEST_PARAM pRequestParam = new REQUEST_PARAM;
		pRequestParam->ctx = ctx;
		pRequestParam->sockClient = sockClient;

		HANDLE hThread = CreateThread(NULL,0,HandleRequestThread,pRequestParam,0,NULL);
		CloseHandle(hThread);
	}

	SSL_CTX_free (ctx);

	getchar();
	return 0;
}
/* EOF - serv.cpp */

