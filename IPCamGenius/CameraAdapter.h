#ifndef _CAMERA_ADAPTER_H
#define _CAMERA_ADAPTER_H

#include <vector>
#include "base64.h"
#include "network.h"
#include "supplementary.h"

class CameraAdapter
{
public:
	virtual char getFlag() = 0;
	virtual bool sendPacket(int n) = 0;
	virtual bool recvPacket() = 0;
	virtual int getPacketType() const = 0;
	virtual IPCameraInfo parsePacket() = 0;
	virtual bool get_params_ssid(const IPCameraInfo &info, CString &ssid) = 0;
	virtual bool set_network(const IPCameraInfo &info, const IPCameraInfo &ori) = 0;
	virtual bool wifi_scan(const IPCameraInfo &info) = 0;
	virtual bool get_wifi_scan_result(const IPCameraInfo &info, std::vector<WIFI_Entry> &vec) = 0;
	virtual bool set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString wifi_pwd) = 0;
	virtual bool ipcam_reboot(const IPCameraInfo &info, CString user, CString pwd) = 0;
protected:
	int last_recv_packet_size;
	unsigned char last_recv_packet[NETWORK_MAX_PACKET_SIZE];
};

#endif