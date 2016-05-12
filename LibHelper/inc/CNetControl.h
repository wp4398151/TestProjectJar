#pragma once

#include <WinSock2.h>

bool InitWinsock(BYTE hi, BYTE low);
/////////////////////////////////////////////////////
// convert domain to unsigned long format.
// wupeng
// return 0 on error. success is opposite.
unsigned long getAddrByDomain(char* pAddr);

bool InitWinsock(BYTE hi, BYTE low);

void EnableNonblock(SOCKET sock);
void DisableNonblock(SOCKET sock);