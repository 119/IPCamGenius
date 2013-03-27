#include "stdafx.h"
#include "UPnPMap.h"

UPNPMap::UPNPMap(char *_ip, unsigned short _port)
{
	set(_ip, _port);
}

void UPNPMap::set(char *_ip, unsigned short _port)
{
	if (_ip == NULL) {
		ip = iport_tcp = 0;
	}
	else {
		ip = inet_addr(_ip);
		iport_tcp = _port;
	}
}

bool operator < (const UPNPMap &A, const UPNPMap &B)
{
	if (A.ip == B.ip) return A.iport_tcp < B.iport_tcp;
	return A.ip < B.ip;
}