#include "stdafx.h"
#include "IPCamGeniusDlg.h"
#include "thread_functions.h"

DWORD WINAPI t_gateway_startup(LPVOID lpParam)
{
	CIPCamGeniusDlg *p_this = (CIPCamGeniusDlg *)lpParam;

	p_this->network_controller.gateway.init();

	return 0;
}

DWORD WINAPI t_send_packets(LPVOID lpParam)
{
	CIPCamGeniusDlg *p_this = (CIPCamGeniusDlg *)lpParam;

	while (1) {
		for (int i = 0; i < p_this->network_controller.camera_adapters_num; i++) {
			p_this->network_controller.camera_adapters[i]->sendPacket(1);
			Sleep(1000);
		}
	}
}

DWORD WINAPI t_recv_packets(LPVOID lpParam)
{
	void **args = (void **)lpParam;
	CIPCamGeniusDlg *p_this = (CIPCamGeniusDlg *)args[0];
	int type, idx = *(int *)args[1];
	CameraAdapter *camera_adapter = p_this->network_controller.camera_adapters[idx];

	while (1) {
		if (!camera_adapter->recvPacket()) continue;
		if ((type = camera_adapter->getPacketType()) < 0) continue;
		if (type > 0) camera_adapter->sendPacket(type);
		else {
			IPCameraInfo info = camera_adapter->parsePacket();
			info.adapter_idx = idx;
			p_this->network_controller.push_hash(info);
		}
	}
}