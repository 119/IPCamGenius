#ifndef _CAMERA_ADAPTER_H_H
#define _CAMERA_ADAPTER_H_H

#include "CameraAdapter.h"

class CameraAdapterH: public CameraAdapter
{
public:
	bool sendPacket(int n);
	bool recvPacket();
	int getPacketType() const;
	IPCameraInfo parsePacket() const;
	bool get_params_ssid(const IPCameraInfo &info, CString &ssid);
	bool set_network(const IPCameraInfo &info);
	bool wifi_scan(const IPCameraInfo &info);
	bool get_wifi_scan_result(const IPCameraInfo &info, std::vector<WIFI_Entry> &vec);
	bool set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString wifi_pwd);
	bool ipcam_reboot(const IPCameraInfo &info, CString user, CString pwd);
private:
	static const unsigned short port_source = 6800;
	static const unsigned short port_destination = 6801;

	static const unsigned short offset_ip = 12;
	static const unsigned short offset_mask = 28;
	static const unsigned short offset_gateway = 44;
	static const unsigned short offset_port_http = 92;
	static const unsigned short offset_port_rtsp = 98;
	static const unsigned short offset_name = 116;

	bool sendPacket_1();
	bool isType_0() const;

	bool _get_params(const IPCameraInfo &info, CString &params_buf);
};

#endif
