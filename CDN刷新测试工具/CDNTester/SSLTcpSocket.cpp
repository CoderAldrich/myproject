#include "stdafx.h"
#include "SSLTcpSocket.h"

CSSLTcpSocket::CSSLTcpSocket()
{
	m_ctx = NULL;
	m_meth = NULL;
	m_ssl = NULL;
}

CSSLTcpSocket::CSSLTcpSocket(SSL* ssl)
{
	m_ctx = NULL;
	m_meth = NULL;
	m_ssl = ssl;
}
CSSLTcpSocket::~CSSLTcpSocket()
{
	if (m_ctx)
	{
		SSL_CTX_free (m_ctx);
	}

 	if (m_ssl)
 	{
 		SSL_shutdown(m_ssl);//�ر�SSL����
 		SSL_free (m_ssl);
 	}
}

BOOL CSSLTcpSocket::CreateSSLTcpSocketForServer( LPCSTR pszCertFile , LPCSTR pszKeyFile )
{
	SSL_load_error_strings(); /*Ϊ��ӡ������Ϣ��׼��*/ 
	OpenSSL_add_ssl_algorithms(); /*��ʼ��*/ 

	m_meth = (SSL_METHOD *)TLSv1_server_method(); /*����ʲôЭ��(SSLv2/SSLv3/TLSv1)�ڴ�ָ��,TLSv1_server_method,SSLv23_server_method()*/ 

	m_ctx = SSL_CTX_new (m_meth);

	//����ΪҪ��ǿ��У��Է����ͻ��ˣ�֤��SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT
	SSL_CTX_set_verify(m_ctx,SSL_VERIFY_NONE,NULL); /*��֤���SSL_VERIFY_PEER*/ 
	//SSL_CTX_load_verify_locations(ctx,CACERT,NULL); /*����֤,�����CA֤��*/

	if (SSL_CTX_use_certificate_file(m_ctx, pszCertFile, SSL_FILETYPE_PEM) <= 0) 
	{
		printf("����֤��ʧ��!\n");
		
	}

	if (SSL_CTX_use_PrivateKey_file(m_ctx, pszKeyFile, SSL_FILETYPE_PEM) <= 0)
	{
		printf("����˽Կʧ��!\n");
		
	}

	if (!SSL_CTX_check_private_key(m_ctx)) {
		printf("��Կ֤�鲻ƥ�䣡\n");
	}

	SSL_CTX_set_cipher_list(m_ctx,"DES-CBC3-SHA"); 
	SSL_CTX_set_mode(m_ctx, SSL_MODE_AUTO_RETRY);

	return CreateTcpSocket();
}
BOOL CSSLTcpSocket::CreateSSLTcpSocketForClient()
{
	SSL_load_error_strings(); /*Ϊ��ӡ������Ϣ��׼��*/ 
	OpenSSL_add_ssl_algorithms(); /*��ʼ��*/ 

	m_meth = (SSL_METHOD *)TLSv1_client_method(); /*����ʲôЭ��(SSLv2/SSLv3/TLSv1)�ڴ�ָ��,TLSv1_server_method,SSLv23_server_method()*/ 

	m_ctx = SSL_CTX_new (m_meth);

	//����ΪҪ��ǿ��У��Է����ͻ��ˣ�֤��SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT
	SSL_CTX_set_verify(m_ctx,SSL_VERIFY_NONE,NULL); /*��֤���SSL_VERIFY_PEER*/ 
	//SSL_CTX_load_verify_locations(ctx,CACERT,NULL); /*����֤,�����CA֤��*/


	//RAND_seed

// 	if (!SSL_CTX_check_private_key(m_ctx)) {
// 		printf("��Կ֤�鲻ƥ�䣡\n");
// 	}

// 	SSL_CTX_set_cipher_list(m_ctx,"DES-CBC3-SHA"); 
// 	SSL_CTX_set_mode(m_ctx, SSL_MODE_AUTO_RETRY);

	CreateTcpSocket();
	
	m_ssl = SSL_new (m_ctx);
	SSL_set_fd (m_ssl, m_TcpSock);

	return FALSE;
}
BOOL CSSLTcpSocket::SSLConnect(LPCSTR pszTargetIP,USHORT nTargetPort)
{
	BOOL bRes = Connect(pszTargetIP,nTargetPort);
	int nRet = SSL_connect (m_ssl);
	return bRes;
}

CSSLTcpSocket * CSSLTcpSocket::SSLAccept(sockaddr_in *psiconnect)
{
	CSSLTcpSocket *pNewSSLSocket = NULL;
	SOCKET sockClient = INVALID_SOCKET;
	SSL *pTempSSL = NULL;
	do 
	{
		sockClient = CTcpSocket::Accept(psiconnect);;

		pTempSSL = SSL_new (m_ctx);  
		if ( NULL == pTempSSL )
		{
			break;
		}

		SSL_set_fd (pTempSSL, sockClient);
		int err = SSL_accept (pTempSSL);
		if ( -1 == err)
		{
			break;
		}

		pNewSSLSocket = new CSSLTcpSocket(pTempSSL);
		pNewSSLSocket->Attach(sockClient);

	} while (FALSE);



	return pNewSSLSocket;
}
int CSSLTcpSocket::SendData(PVOID pDataBuf, int nDataLen)
{
	return SSL_write (m_ssl, pDataBuf, nDataLen);
}
int CSSLTcpSocket::RecvData(PVOID pDataBuf, int nBufLen)
{
	return SSL_read(m_ssl, pDataBuf,nBufLen);
}
