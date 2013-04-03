#include "stdafx.h"
#include "network.h"

int getsockaddr(const char *host, unsigned short port, void *saddr)
{
	struct sockaddr_in *x = (struct sockaddr_in *)saddr;

	if (!strcmp(host, "any")) 
		x->sin_addr.S_un.S_addr = INADDR_ANY;
	else if (!strcmp(host, "255.255.255.255")) 
		x->sin_addr.S_un.S_addr = INADDR_BROADCAST;
	else {
		x->sin_addr.S_un.S_addr = inet_addr(host);
		if (x->sin_addr.S_un.S_addr == INADDR_NONE) {
			return -1;
		}
	}

	x->sin_family = AF_INET;
	x->sin_port = htons(port);

	return 0;
}

char *fetchLine(char *& buf)
{
	int i = 0;
	char *str = buf;

	if (buf[0] == '\0') return NULL;

	while (buf[i]) {
		if (buf[i] == '\r' && buf[i+1] == '\n') {
			buf[i++] = 0;
			buf[i++] = 0;
			break;
		}
		else if (buf[i] == '\n') {
			buf[i++] = 0;
			break;
		}
		i++;
	}
	buf += i;

	return str;
}

int httpRequest(const char *ip, unsigned short port, const char *request, CString *xbuf)
{
	int sfd, n, ret_code = 0;
	struct sockaddr dst_addr;
	char buf[NETWORK_MAX_BUFFER_SIZE+1], tmp[NETWORK_MAX_BUFFER_SIZE+1], *pstr, *line;

	if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) ret_code = -1;
	if (!ret_code && getsockaddr(ip, port, &dst_addr) < 0) ret_code = -1;
	if (!ret_code && connect(sfd, &dst_addr, sizeof(dst_addr)) < 0) ret_code = -1;
	
	if (!ret_code && send(sfd, request, strlen(request), 0) < 0) ret_code = -1;
	if (!ret_code && (n = recv(sfd, buf, NETWORK_MAX_BUFFER_SIZE, 0)) < 0) ret_code = -1;

	buf[n < 0 ? 0 : n] = 0;
	if (xbuf) *xbuf = CString(buf);

	if (!ret_code) {
		pstr = buf;
		line = fetchLine(pstr);
		sscanf(line, "%[^ ]%d", tmp, &ret_code);
	}

	while (xbuf && ret_code != -1 && (n = recv(sfd, buf, NETWORK_MAX_BUFFER_SIZE, 0)) > 0) {
		buf[n] = 0;
		*xbuf = *xbuf + CString(buf);
	}

	closesocket(sfd);

	return ret_code;
}

bool broadcast_packet(unsigned short port, const char *packet, int packet_size, unsigned short iport)
{
	int sfd;
	bool opt = true;
	struct sockaddr src_addr, dest_addr;

	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return false;
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0) return false;
	if (setsockopt(sfd, SOL_SOCKET, SO_BROADCAST, (const char *)&opt, sizeof(opt)) == SOCKET_ERROR) return false;
	if (getsockaddr("255.255.255.255", port, &dest_addr) < 0) return false;
	if (iport > 0) {
		if (getsockaddr("any", iport, &src_addr) < 0) return false;
		if (bind(sfd, &src_addr, sizeof(src_addr)) < 0) return false;
	}

	if (sendto(sfd, packet, packet_size, 0, &dest_addr, sizeof(dest_addr)) < 0) {
		return false;
	}

	closesocket(sfd);
	return true;
}

bool recive_packet(unsigned short port_bind, char *packet, int &packet_size, CString *ip)
{
	int sfd, len;
	bool opt = true;
	struct sockaddr src_addr, dst_addr;

	packet_size = 0;
	len = sizeof(dst_addr);
	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) return false;
	if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0) return false;
	if (getsockaddr("any", port_bind, &src_addr) < 0) return false;
	if (bind(sfd, &src_addr, sizeof(src_addr)) < 0) return false;
	packet_size = recvfrom(sfd, packet, NETWORK_MAX_PACKET_SIZE, 0, &dst_addr, &len);
	if (packet_size < 0) return false;
	if (ip) {
		struct sockaddr_in *p_inet = (struct sockaddr_in *)&dst_addr;
		*ip = inet_ntoa(p_inet->sin_addr);
	}

	closesocket(sfd);
	return true;
}

unsigned long ip_s2ul(const char *ip)
{
	return inet_addr(ip);
}

bool getGatewayInfo(CString &gateway, CString &mask)
{
	PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            return false;
        }
    }
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
		gateway = pAdapter->GatewayList.IpAddress.String;
		mask = pAdapter->GatewayList.IpMask.String;
	}
	return true;
}

CString getIPByOffset(const unsigned char *data, int offset)
{
	char ip[64];

	sprintf(ip, "%u.%u.%u.%u", data[offset], data[offset+1], data[offset+2], data[offset+3]);
	return ip;
}

void fillOffsetByIP(unsigned char *data, int offset, const char *ip)
{
	unsigned int a, b, c, d;

	sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d);
	data[offset] = (unsigned char)a;
	data[offset+1] = (unsigned char)b;
	data[offset+2] = (unsigned char)c;
	data[offset+3] = (unsigned char)d;
}

CString calculateSubnetAddress(CString gateway, CString mask)
{
	char subnet[16];
	unsigned short ga, gb, gc, gd;
	unsigned short ma, mb, mc, md;
	unsigned short sa, sb, sc, sd;

	sscanf(gateway, "%u.%u.%u.%u", &ga, &gb, &gc, &gd);
	sscanf(mask, "%u.%u.%u.%u", &ma, &mb, &mc, &md);
	sa = ga & ma; sb = gb & mb; sc = gc & mc; sd = gd & md;
	sprintf(subnet, "%u.%u.%u.%u", sa, sb, sc, sd);

	return subnet;
}