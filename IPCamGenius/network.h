#ifndef _NETWORK_H
#define _NETWORK_H

#include <WinSock.h>

#define NETWORK_MAX_BUFFER_SIZE 4096
#define NETWORK_MAX_PACKET_SIZE 8192

struct IPCameraInfo
{
	char ip[16];
	char mask[16];
	char gateway[16];
	unsigned short port_http;
	unsigned short port_rtsp;
	char cameraName[64];
	char externalIPAddress[64];
	int adapter_idx;
};

struct WIFI_Entry
{
	char ssid[256];
	char MAC[20];
	int security;
	int signal_strength;
	int mode;
};

char *fetchLine(char *& buf);
int getsockaddr(const char *host, unsigned short port, void *saddr);
int httpRequest(const char *ip, unsigned short port, const char *request, CString *xbuf = NULL);
bool broadcast_packet(unsigned short port, const char *packet, int packet_size, unsigned short iport = 0);
bool recive_packet(unsigned short port_bind, char *packet, int &packet_size, CString *ip = NULL);
unsigned long ip_s2ul(const char *ip);
bool getGatewayInfo(CString &gateway, CString &mask);
CString getIPByOffset(const unsigned char *data, int offset);

#endif