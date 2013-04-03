#ifndef _GATEWAY_H
#define _GATEWAY_H

#define GATEWAY_MAX_BUFFER_SIZE 1024

#include "UPnPMap.h"
#include "upnp\miniupnpc.h"
#include "upnp\upnpcommands.h"

#include <map>
#include <bitset>
#include <Windows.h>

class Gateway
{
public:
	friend class NetworkController;
	Gateway();
	~Gateway();
	bool init();
	bool isError() const;
	bool isReady() const;
	bool isGatewayReady() const;
	CString getExternalIPAddress() const;
	CString getGatewayAddress() const;
	CString getSubnetMask() const;
	CString getSubnetAddress();
	bool getUPnPMap(UPNPMap &ipmap, unsigned short &eport);
	bool isTCPPortUsed(unsigned short eport);
	bool getNextAvailableTCPPort(unsigned short iport, unsigned short &eport);
	bool getNextAvailableTCPPorts(unsigned short iport_1, unsigned short iport_2, 
			unsigned short &eport_1, unsigned short &eport_2);
	bool addTCPPortMapping(const char *ip, unsigned short iport, unsigned short eport);
protected:
	bool is_error;
	bool is_ready;
	bool is_gatewayReady;
	struct UPNPUrls urls;
	struct IGDdatas data;
	CString externalIPAddress;
	char gateway[64], mask[64];
	char tmp[GATEWAY_MAX_BUFFER_SIZE];
	std::bitset<65536> slot;
	std::map<UPNPMap, unsigned short> upnp_map;
private:
	bool _getGatewayInfo();
	bool _getExternalIPAddress();
	bool _getUPnPMapList();
	bool _getRouterInfo();
};

#endif