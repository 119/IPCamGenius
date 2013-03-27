#include "stdafx.h"
#include "IPCamGeniusDlg.h"
#include "NetworkController.h"
#include "CameraAdapterH.h"
#include "CameraAdapterF.h"

NetworkController::NetworkController(void *p)
{
	p_this = p;
	table.clear();
	vec_info.clear();
	camera_adapters_num = 2;
	camera_adapters = new CameraAdapter*[camera_adapters_num];
	camera_adapters[0] = new CameraAdapterH();
	camera_adapters[1] = new CameraAdapterF();
}

NetworkController::~NetworkController()
{
	for (int i = 0; i < camera_adapters_num; i++) {
		delete camera_adapters[i];
	}
	delete []camera_adapters;
}

bool NetworkController::push_hash(const IPCameraInfo &info)
{
	bool ret = true;
	unsigned long uip;
	CIPCamGeniusDlg *p = (CIPCamGeniusDlg *)p_this;

	if (!mutex.pop()) return false;
	uip = ip_s2ul(info.ip);
	if (table.find(uip) != table.end()) ret = false;
	if (ret) {
		table.insert(uip);
		vec_info.push_back(info);
		p->f_list_cameras_update();
	}
	mutex.push();

	return ret;
}

bool NetworkController::reset_network()
{
	if (!mutex.pop()) return false;
	table.clear();
	vec_info.clear();
	mutex.push();
	return gateway._getUPnPMapList();
}

bool NetworkController::wifi_scan(IPCameraInfo &info)
{
	return camera_adapters[info.adapter_idx]->wifi_scan(info);
}

bool NetworkController::get_wifi_scan_result(IPCameraInfo &info, std::vector<WIFI_Entry> &vec)
{
	return camera_adapters[info.adapter_idx]->get_wifi_scan_result(info, vec);
}

bool NetworkController::set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString pwd)
{
	return camera_adapters[info.adapter_idx]->set_wifi(info, entry, pwd);
}