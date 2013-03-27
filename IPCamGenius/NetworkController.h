#ifndef _NETWORK_CONTROLLER_H
#define _NETWORK_CONTROLLER_H

#include "Mutex.h"
#include "network.h"
#include "Gateway.h"
#include "CameraAdapter.h"


#include <set>
#include <vector>

class NetworkController
{
public:
	NetworkController(void *p);
	~NetworkController();
	bool push_hash(const IPCameraInfo &info);
	bool reset_network();
	bool wifi_scan(IPCameraInfo &info);
	bool get_wifi_scan_result(IPCameraInfo &info, std::vector<WIFI_Entry> &vec);
	bool set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString pwd);
public:
	void *p_this;

	std::set<unsigned long> table;
	std::vector<IPCameraInfo> vec_info;

	Mutex mutex;
	Gateway gateway;

	int camera_adapters_num;
	CameraAdapter **camera_adapters;
};

#endif