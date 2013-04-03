#ifndef _CAMERA_ADAPTER_F_H
#define _CAMERA_ADAPTER_F_H

#include "CameraAdapter.h"

class CameraAdapterF: public CameraAdapter
{
public:
	CameraAdapterF();
	char getFlag();
	bool sendPacket(int n);
	bool recvPacket();
	int getPacketType() const;
	IPCameraInfo parsePacket();
	bool get_params_ssid(const IPCameraInfo &info, CString &ssid);
	bool set_network(const IPCameraInfo &info, const IPCameraInfo &ori);
	bool wifi_scan(const IPCameraInfo &info);
	bool get_wifi_scan_result(const IPCameraInfo &info, std::vector<WIFI_Entry> &vec);
	bool set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString wifi_pwd);
	bool ipcam_reboot(const IPCameraInfo &info, CString user, CString pwd);
private:
	static const unsigned short port_source = 10000;
	static const unsigned short port_destination = 10000;

	static const unsigned short offset_port_http = 0x56;
	static const unsigned short offset_name = 0x24;
	static const unsigned short offset_ip = 0x39;
	static const unsigned short offset_mask = 0x3D;
	static const unsigned short offset_gateway = 0x41;
	static const unsigned short offset_mac = 0x17;

	bool sendPacket_1();
	bool isType_0() const;

	bool _get_params(const IPCameraInfo &info, CString &params_buf);

	bool isGatewayInfoValid;
	CString c_gateway, c_netmask;
	CString last_sender_ip;
};

#endif
