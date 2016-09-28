// MyHttpsServer.cpp : �������̨Ӧ�ó������ڵ㡣
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


/*������Ҫ�Ĳ�����Ϣ���ڴ˴���#define����ʽ�ṩ*/ 
#define CERTF "C:\\test\\nginx-1.11.3\\conf\\server.crt" /*����˵�֤��(�辭CAǩ��)*/ 
#define KEYF  "C:\\test\\nginx-1.11.3\\conf\\server.key" /*����˵�˽Կ(������ܴ洢)*/ 
#define CACERT "ca.crt" /*CA ��֤��*/ 
#define PORT 443 /*׼���󶨵Ķ˿�*/ 

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }

#include "HttpSendParser.h"
#include "PublicFun.h"


int _tmain(int argc, _TCHAR* argv[])
{
	int err;
	int listen_sd;
	int sd;
	int client_len;
	SSL_CTX* ctx;
	SSL*     ssl;
	X509*    client_cert;
	char*    str;
	char     buf [4096];
	SSL_METHOD *meth;
	WSADATA wsaData;
	struct sockaddr_in sa_serv;
	struct sockaddr_in sa_cli;

	if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
	{ 
		printf("WSAStartup()fail:%d\n",GetLastError()); 
		return -1;
	} 

	/* SSL preliminaries. We keep the certificate and key with the context. */
	SSL_load_error_strings(); /*Ϊ��ӡ������Ϣ��׼��*/ 
	OpenSSL_add_ssl_algorithms(); /*��ʼ��*/ 

	meth = SSLv3_server_method(); /*����ʲôЭ��(SSLv2/SSLv3/TLSv1)�ڴ�ָ��,TLSv1_server_method,SSLv23_server_method()*/ 

	ctx = SSL_CTX_new (meth);
	//CHK_NULL(ctx);
	if (!ctx) 
	{
		printf("����SSL_CTXʧ��!\n");
		exit(2);
	}

	//����ΪҪ��ǿ��У��Է����ͻ��ˣ�֤��SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT
	SSL_CTX_set_verify(ctx,SSL_VERIFY_NONE,NULL); /*��֤���SSL_VERIFY_PEER*/ 
	//SSL_CTX_load_verify_locations(ctx,CACERT,NULL); /*����֤,�����CA֤��*/

	if (SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0) 
	{
		printf("����֤��ʧ��!\n");
		exit(3);
	}

	if (SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0)
	{
		printf("����˽Կʧ��!\n");
		exit(4);
	}

	if (!SSL_CTX_check_private_key(ctx)) {
		printf("��Կ֤�鲻ƥ�䣡\n");
		exit(5);
	}

	SSL_CTX_set_cipher_list(ctx,"RC4-MD5"); 
	SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);

	/* ----------------------------------------------- */
	/* Prepare TCP socket for receiving connections */
	/*��ʼ������TCP socket����.................................*/ 
	printf("Begin TCP socket...\n"); 


	listen_sd = socket (AF_INET, SOCK_STREAM, 0);   
	CHK_ERR(listen_sd, "socket");

	memset (&sa_serv, '\0', sizeof(sa_serv));
	sa_serv.sin_family      = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port        = htons (PORT);          /* Server Port number */

	err = bind(listen_sd, (struct sockaddr*) &sa_serv, sizeof (sa_serv));                   
	CHK_ERR(err, "bind");

	/*����TCP����*/ 
	err = listen (listen_sd, 5);                    
	CHK_ERR(err, "listen");

	client_len = sizeof(sa_cli);

	while (TRUE)
	{
		sd = accept (listen_sd, (struct sockaddr*) &sa_cli, (int*)&client_len);
		CHK_ERR(sd, "accept");
		//closesocket (listen_sd);

		printf ("Connection from %s, port %d\n",
			inet_ntoa(sa_cli.sin_addr), sa_cli.sin_port);


		/* ----------------------------------------------- */
		/*TCP�����ѽ���,���з���˵�SSL����. */ 
		printf("Begin server side SSL\n"); 

		ssl = SSL_new (ctx);  
		CHK_NULL(ssl);
		SSL_set_fd (ssl, sd);
		err = SSL_accept (ssl);
		printf("SSL_accept finished\n"); 
		CHK_SSL(err);

		/*��ӡ���м����㷨����Ϣ(��ѡ)*/ 
		printf ("SSL connection using %s\n", SSL_get_cipher (ssl));

		/*�õ��ͻ��˵�֤�鲢��ӡЩ��Ϣ(��ѡ) */ 
		/*client_cert = SSL_get_peer_certificate (ssl);
		if (client_cert != NULL) 
		{
		printf ("Client certificate:\n");

		str = X509_NAME_oneline (X509_get_subject_name (client_cert), 0, 0);
		CHK_NULL(str);
		printf ("\t subject: %s\n", str);
		OPENSSL_free (str);

		str = X509_NAME_oneline (X509_get_issuer_name  (client_cert), 0, 0);
		CHK_NULL(str);
		printf ("\t issuer: %s\n", str);
		OPENSSL_free (str);

		//�粻����Ҫ,�轫֤���ͷ�
		X509_free (client_cert);
		} 
		else
		printf ("Client does not have certificate.\n");*/

		/* ���ݽ�����ʼ,��SSL_write,SSL_read����write,read */ 

		CStringA strUrl;

		int inlen = 0, nread = 0;
		for ( nread = 0; nread < sizeof(buf);  )
		{
			inlen = SSL_read(ssl, buf+nread,sizeof(buf)-nread);
			CHK_SSL(inlen);
			if ( inlen <=0 ) break;//�����ͻ���������
			fwrite(buf+nread, 1,inlen, stdout);//�����յ�����Ϣ��ӡ����׼���

			nread += inlen;
			CHttpSendParser sendparser;
			if(sendparser.ParseData(buf,nread))
			{

				strUrl = sendparser.GetParseUrl();
				break;
			}
		}

		buf[nread] = '\0';
		printf ("Got %d chars:'%s'\n", nread, buf);

		CString strWebContent;
		//strWebContent=GetHttpString(CString(/*strUrl*/"https://www.2345.com/"));
		strWebContent=L"<html><body><script> window.location = \"https://www.2345.com/\" </script></body></html>";

		CStringA strResponse;
		strResponse.Format("HTTP/1.1 200 OK\r\nCache-Control: private\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s",strWebContent.GetLength(),CStringA(strWebContent).GetBuffer());
		err = SSL_write (ssl, strResponse.GetBuffer(), strResponse.GetLength());
		CHK_SSL(err);

		SSL_shutdown(ssl);//�ر�SSL����

		closesocket (sd);
		SSL_free (ssl);
	}

	SSL_CTX_free (ctx);

	getchar();
	return 0;
}
/* EOF - serv.cpp */

