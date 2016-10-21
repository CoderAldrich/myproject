#pragma one
#include "TcpSocket.h"

#include <openssl/rsa.h>       /* SSLeay stuff */
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "ssleay32.lib")
#pragma comment(lib, "libeay32.lib")

#pragma comment(lib,"ws2_32.lib")

class CSSLTcpSocket:public CTcpSocket
{
protected:
	SSL_CTX* m_ctx;
	SSL_METHOD *m_meth;
	SSL* m_ssl;
public:
	CSSLTcpSocket();
	CSSLTcpSocket(SSL* ssl);
	~CSSLTcpSocket();
	BOOL CreateSSLTcpSocketForServer(LPCSTR pszCertFile , LPCSTR pszKeyFile);
	BOOL CreateSSLTcpSocketForClient();
	BOOL SSLConnect(LPCSTR pszTargetIP,USHORT nTargetPort);
	CSSLTcpSocket *SSLAccept(sockaddr_in *psiconnect);
	int SendData(PVOID pDataBuf, int nDataLen);
	int RecvData(PVOID pDataBuf, int nBufLen);
};
