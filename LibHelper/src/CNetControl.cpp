#pragma once
#include "Helper.h"
#include "CNetControl.h"
unsigned long getAddrByDomain(char* pAddr)
{
	ASSERT_NOTNULLRET(pAddr, 0);
	struct hostent* ph;
	struct in_addr in;
	ph = gethostbyname(pAddr);

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int getaddrinfo(
		_In_opt_       PCSTR      pNodeName,
		_In_opt_       PCSTR      pServiceName,
		_In_opt_ const ADDRINFOA  *pHints,
		_Out_          PADDRINFOA *ppResult
		);

	if (!ph)
	{
		DOLOG("gethostbyname Error:" + WSAGetLastError());
		return 0;
	}
	memcpy(&in, ph->h_addr_list, ph->h_length);
	return in.s_addr;
}

bool InitWinsock(BYTE hi, BYTE low)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err = 0;
	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0){
		DOLOG("WSAStartup error");
		return false;
	}
	if (HIBYTE(wsaData.wVersion) != low ||
		LOBYTE(wsaData.wVersion) != hi)
	{
		DOLOG("winsock version not support");
		WSACleanup();
		return false;
	}
	return true;
}

void EnableNonblock(SOCKET sock)
{
	ULONG val = 1;
	ioctlsocket(sock, FIONBIO, &val);
}

void DisableNonblock(SOCKET sock)
{
	ULONG val = 0;
	ioctlsocket(sock, FIONBIO, &val);
}

SOCKET CreateSocket(int type, int protocol)
{
	SOCKET newSock = 0;
	newSock = socket(AF_INET, type, protocol);
	if (newSock == INVALID_SOCKET)
	{
		DOLOG("create socket Error:" + WSAGetLastError());
		return INVALID_SOCKET;
	}
	return newSock;
}


bool ConnectToServerByDomain(SOCKET sock, LPSTR lpDomain, short port)
{
	if (lpDomain == NULL)
	{
		DOLOG("IP string can't be NULL!");
		return false;
	}
	SOCKADDR_IN addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);
	addr_in.sin_addr.S_un.S_addr = getAddrByDomain(lpDomain);
	if (addr_in.sin_addr.S_un.S_addr == 0){
		DOLOG("getAddrByDomain error!");
		return false;
	}

	if (SOCKET_ERROR == connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in)))
	{
		DOLOG("connect error! code:" + WSAGetLastError());
		return false;
	}

	return true;
}

bool ConnectToServerByIP(SOCKET sock, LPSTR lpIP, unsigned short port)
{
	if (lpIP == NULL)
	{
		DOLOG("IP string can't be NULL!");
		return false;
	}
	SOCKADDR_IN addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);
	addr_in.sin_addr.S_un.S_addr = inet_addr(lpIP);
	if (SOCKET_ERROR == connect(sock, (SOCKADDR*)&addr_in, sizeof(addr_in)))
	{
		DOLOG("connect error! code:" + WSAGetLastError());
		return false;
	}

	return true;
}

bool BindServerSock(SOCKET sock, unsigned short port)
{
	SOCKADDR_IN addr_in;
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(port);
	addr_in.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(sock, (SOCKADDR*)&addr_in, sizeof(addr_in))){
		DOLOG("bind socket error! code:" + WSAGetLastError());
		return false;
	}
	return true;
}

SOCKET AcceptNewConnection(SOCKET sock){
	SOCKET newSock;
	SOCKADDR_IN addr_in;
	int addrSize = sizeof(addr_in);
	if (INVALID_SOCKET == (newSock = accept(sock, (SOCKADDR*)&addr_in, &addrSize)))
	{
		DOLOG("accept error! code:" + WSAGetLastError());
		return INVALID_SOCKET;
	}
	return newSock;
}

int SendData(SOCKET sock, const char*buf, int size)
{
	int sendSize = 0;
	int code = 0;
	sendSize = send(sock, buf, size, 0);
	if (sendSize == SOCKET_ERROR){
		if ((code = WSAGetLastError()) == WSAEWOULDBLOCK){
			return 0;
		}
	}
	if ((size - sendSize) != 0)
	{
		return -1;
	}
	return sendSize;
}

int RecvData(SOCKET sock, char* recvBuf, int size)
{
	int recvSize = 0;
	recvSize = recv(sock, recvBuf, size, 0);

	// 0 就是正常停止
	if (recvSize == 0){
		return SOCKET_ERROR;
	}
	if (recvSize != SOCKET_ERROR){
		return recvSize;
	}
	if (WSAGetLastError() == WSAEWOULDBLOCK){
		return 0;
	}
	return SOCKET_ERROR;
}

#define DEFAULT_LOGFILE "log.txt"
void Log(char *buf, ...)
{
	char temp_buf1[1024] = { 0 };
	char temp_buf2[1024] = { 0 };
	va_list args;
	FILE* fp;

	time_t timeVal;
	time(&timeVal);
	struct tm* currTM = localtime(&timeVal);
	fp = fopen("log.txt", "w+");
	if (!fp){
		return;
	}
	va_start(args, buf);
	vsprintf(temp_buf1, buf, args);
	va_end(args);
	sprintf(temp_buf2, "[%d/%02d/%02d %02d:%02d:%02d]%s",
		currTM->tm_year + 1900,
		currTM->tm_mon + 1,
		currTM->tm_mday,
		currTM->tm_hour,
		currTM->tm_min,
		currTM->tm_sec,
		temp_buf1
		);
	strcpy(temp_buf1, temp_buf2);
	fprintf(fp, temp_buf1);

	fclose(fp);
}

// 设置close是对于没有处理完的数据的方法
bool SetCloseLinger(SOCKET sock)
{
	struct linger lingerStruct;
	lingerStruct.l_linger = 0;
	lingerStruct.l_onoff = 0;
	int res = setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&lingerStruct, sizeof(lingerStruct));
	if (res == SOCKET_ERROR){
		DOLOG("setsockopt SO_LINGER Error! code:" + WSAGetLastError());
		return false;
	}
	return true;
}

bool ListenSocket(SOCKET sock, int backLog)
{
	int res = listen(sock, backLog);
	if (res == SOCKET_ERROR){
		DOLOG("listen Error! code:" + WSAGetLastError());
		return false;
	}

	return true;
}