#include "stdafx.h"
#include "gateway.h"
#include "supplementary.h"

#include <bitset>

Gateway::Gateway()
{
	is_ready = false;
	is_error = false;
}

Gateway::~Gateway()
{
	FreeUPNPUrls(&urls);
}

bool Gateway::init()
{
	if (!_getGatewayInfo()) {
		is_error = true;
		return false;
	}
	if (!_getExternalIPAddress()) {
		is_error = true;
		return false;
	}
	if (!_getUPnPMapList()) {
		is_error = true;
		return false;
	}
	if (!_getRouterInfo()) {
		is_error = true;
		return false;
	}
	is_ready = true;
	return true;
}

bool Gateway::isError() const
{
	return is_error;
}
bool Gateway::isReady() const
{
	return is_ready;
}

CString Gateway::getExternalIPAddress() const
{
	return externalIPAddress;
}

CString Gateway::getGatewayAddress() const
{
	return CString(gateway);
}

bool Gateway::_getGatewayInfo()
{
	int error;
	bool ret = true;
	char lanaddr[64];
	struct UPNPDev *devlist = NULL;

	devlist = upnpDiscover(2000, 0, 0, 0, 0, &error);
	if (devlist == NULL) ret = false;
	else if (UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr)) != 1) {
		ret = false;
	}

	freeUPNPDevlist(devlist);

	return ret;
}

bool Gateway::_getExternalIPAddress()
{
	if (UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, tmp) != 0) {
		return false;
	}

	externalIPAddress = CString(tmp);

	return true;
}

bool Gateway::_getUPnPMapList()
{
	int ret, i;
	char index[6];
	char extPort[6];
	char intPort[6];
	char protocol[5];
	char desc[80];
	char intClient[40];
	char enabled[6];
	char rHost[64];
	char duration[16];
	UPNPMap ipmap;

	slot.reset();
	upnp_map.clear();

	protocol[4] = 0;
	for (i = 0; ; i++) {
		sprintf(index, "%d", i);
		ret = UPNP_GetGenericPortMappingEntry(urls.controlURL, 
		                               data.first.servicetype,
		                               index,
		                               extPort, intClient, intPort,
									   protocol, desc, enabled,
									   rHost, duration);
		if (ret != 0) break;

		if (protocol[0] != 'T') continue;
		ipmap.set(intClient, atoi(intPort));
		upnp_map[ipmap] = atoi(extPort);
		slot[atoi(extPort)] = 1;
	}

	return true;
}

bool Gateway::getUPnPMap(UPNPMap &ipmap, unsigned short &eport)
{
	if (!isReady()) return false;

	eport = upnp_map[ipmap];
	if (upnp_map[ipmap] == 0) return false;
	
	return true;
}

bool Gateway::isTCPPortUsed(unsigned short eport)
{
	std::map<UPNPMap, unsigned short>::iterator i;

	for (i = upnp_map.begin(); i != upnp_map.end(); i++) 
		if ((*i).second == eport) break;
	return i != upnp_map.end();
}

bool Gateway::getNextAvailableTCPPort(unsigned short iport, unsigned short &eport)
{
	int j;
	std::map<UPNPMap, unsigned short>::iterator i;

	if (!slot[iport]) {
		eport = iport;
		return true;
	}
	for (j = 80; j < 65536; j++) 
		if (!slot[j]) break;
	if (j == 65536) return false;

	eport = j;
	return true;
}

bool Gateway::getNextAvailableTCPPorts(unsigned short iport_1, unsigned short iport_2, 
			unsigned short &eport_1, unsigned short &eport_2)
{
	int j;
	std::map<UPNPMap, unsigned short>::iterator i;

	if (!slot[iport_1]) {
		eport_1 = iport_1;
		slot[eport_1] = 1;
		if (!slot[iport_2]) {
			eport_2 = iport_2;
		}
		else {
			for (j = 80; j < 65536; j++) 
				if (!slot[j]) break;
			if (j == 65536) return false;
			eport_2 = j;
		}
		return true;
	}
	for (j = 80; j < 65536; j++) 
		if (!slot[j]) break;
	if (j == 65536) return false;
	eport_1 = j;
	slot[eport_1] = 1;
	if (!slot[iport_2]) {
		eport_2 = iport_2;
	}
	else {
		for (j = 80; j < 65536; j++) 
			if (!slot[j]) break;
		if (j == 65536) return false;
		eport_2 = j;
	}
	return true;
}

bool Gateway::addTCPPortMapping(const char *ip, unsigned short iport, unsigned short eport)
{
	if (UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, ulong2CString(iport),
				ulong2CString(eport),  ip, "IPCameraGenius", "TCP", 0, 0) != 0) {
			return false;
	}
	return true;
}

bool Gateway::_getRouterInfo()
{
	CString c_gateway, c_mask;
	if (!getGatewayInfo(c_gateway, c_mask)) return false;
	strcpy(gateway, c_gateway);
	strcpy(mask, c_mask);
	return true;
}