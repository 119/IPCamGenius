#ifndef _UPNP_MAP_H
#define _UPNP_MAP_H

struct UPNPMap
{
	UPNPMap(char *_ip = NULL, unsigned short _port = 0);
	void set(char *_ip, unsigned short _port);

	unsigned long ip;
	unsigned short iport_tcp;
};

bool operator < (const UPNPMap &A, const UPNPMap &B);

#endif