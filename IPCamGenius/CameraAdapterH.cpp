#include "stdafx.h"
#include <string.h>
#include "CameraAdapterH.h"

bool CameraAdapterH::sendPacket(int n)
{
	bool ret = false;

	switch (n) {
	case 1:
		ret = sendPacket_1();
		break;
	}
	return ret;
}

char CameraAdapterH::getFlag()
{
	return 'H';
}

bool CameraAdapterH::sendPacket_1()
{
	static unsigned char packet_1[8] = { 0x10, 0x10, 0x01, 0x0f, 0x00, 0x00, 0x00, 0x00 };
	return broadcast_packet(port_destination, (char *)packet_1, sizeof(packet_1), port_source);
}

bool CameraAdapterH::recvPacket()
{
	return recive_packet(port_source, (char *)last_recv_packet, last_recv_packet_size);
}

int CameraAdapterH::getPacketType() const
{
	if (isType_0()) return 0;

	return -1;
}

bool CameraAdapterH::isType_0() const
{
	static unsigned char _packet_0[7] = {
		0x10, 0x10, 0x01, 0x8f, 0x00, 0x00, 0x84
	};

	if (last_recv_packet_size != 140) return false;
	for (int i = 0; i < sizeof(_packet_0); i++) {
		if (last_recv_packet[i] != _packet_0[i]) return false;
	}

	return true;
}

IPCameraInfo CameraAdapterH::parsePacket()
{
	IPCameraInfo caminfo;

	strcpy(caminfo.ip, (char *)last_recv_packet + offset_ip);
	strcpy(caminfo.mask, (char *)last_recv_packet + offset_mask);
	strcpy(caminfo.gateway, (char *)last_recv_packet + offset_gateway);
	memcpy(&(caminfo.port_http), (char *)last_recv_packet + offset_port_http, 2);
	memcpy(&(caminfo.port_rtsp), (char *)last_recv_packet + offset_port_rtsp, 2);
	strcpy(caminfo.cameraName, (char *)last_recv_packet + offset_name);

	strcpy(caminfo.dns1, (char *)last_recv_packet + offset_dns1);
	strcpy(caminfo.dns2, (char *)last_recv_packet + offset_dns2);
	memcpy(caminfo.mac, (char *)last_recv_packet + offset_mac, 6);

	return caminfo;
}

bool CameraAdapterH::_get_params(const IPCameraInfo &info, CString &params_buf)
{
	int ret_code;
	CString request_head = "GET /get_params.cgi HTTP/1.1";
	CString auth_string = "YWRtaW46";
	CString request = request_head + "\r\n" + "Authorization: Basic " + auth_string + "\r\n\r\n";

	ret_code = httpRequest(info.ip, info.port_http, request, &params_buf);
	if (ret_code < 0) return false;
	while (ret_code == 401) {
		if (!getAuth_Base64(auth_string)) return false;
		request = request_head + "\r\n" + "Authorization: Basic " + auth_string + "\r\n\r\n";
		ret_code = httpRequest(info.ip, info.port_http, request, &params_buf);
		if (ret_code < 0) return false;
		else if (ret_code == 401) msgbox(_T("密码错误！"));
	}
	return true;
}

bool CameraAdapterH::get_params_ssid(const IPCameraInfo &info, CString &ssid)
{
	CString params_buf;

	if (!_get_params(info, params_buf)) return false;
	if (!parseValue(params_buf, "wifi_ssid", ssid)) return false;

	return true;
}

/*bool CameraAdapterH::set_network(const IPCameraInfo &info)
{
	CString buf;
	CString request_head = "GET /set_network.cgi?ip=" + CString(info.ip) + 
		"&mask=" + CString(info.mask) + "&gateway=" + CString(info.gateway) + 
		"&port=" + ulong2CString(info.port_http) + "&rtspport=" + ulong2CString(info.port_rtsp);
	CString loginuser = "admin", loginpwd = "";
	CString request = request_head + "&loginuse=" + loginuser + "&loginpas=" + loginpwd + " HTTP/1.1\r\n\r\n";

	httpRequest(info.ip, info.port_http, request, &buf);
	while (strstr(buf.GetBuffer(0), "user or passwd is error") != NULL) {
		if (!getAuth(loginuser, loginpwd)) return false;
		CStringSpecialChars(loginuser);
		CStringSpecialChars(loginpwd);
		request = request_head + "&loginuse=" + loginuser + "&loginpas=" + loginpwd + " HTTP/1.1\r\n\r\n";
		httpRequest(info.ip, info.port_http, request, &buf);
	}
	if (!strstr(buf.GetBuffer(0), "ok")) return false;
	ipcam_reboot(info, loginuser, loginpwd);

	return true;
}*/

bool CameraAdapterH::set_network(const IPCameraInfo &info, const IPCameraInfo &ori)
{
	const int offset_base = 0x80;
	const int offset_name = 0xf0;
	const int offset_port_http = 0xd8;
	const int offset_port_rtsp = 0xde;
	const int offset_ip = 0x88;
	const int offset_mask = 0x98;
	const int offset_gateway = 0xa8;
	const int offset_dns1 = 0xb8;
	const int offset_dns2 = 0xc8;
	const int offset_mac = 0xe0;
	const int offset_get_ip_type = 0xff;
	static unsigned char _packet_set[0x108] = {
		0x10, 0x10, 0x02, 0x0f, 0x00, 0x00, 0x00, 0x01
	};
	memset(_packet_set + 0x10, 0, 0xF8);
	strcpy((char *)_packet_set + offset_ip - offset_base, ori.ip);
	strcpy((char *)_packet_set + offset_gateway - offset_base, ori.gateway);
	strcpy((char *)_packet_set + offset_mask - offset_base, ori.mask);
	strcpy((char *)_packet_set + offset_dns1 - offset_base, ori.dns1);
	strcpy((char *)_packet_set + offset_dns2 - offset_base, ori.dns2);
	memcpy(_packet_set + offset_mac - offset_base, ori.mac, 6);
	for (int i = 0; i < 3; i++)
		memcpy(_packet_set + offset_port_http - offset_base + i * 2, &ori.port_http, sizeof(ori.port_http));
	memcpy(_packet_set + offset_port_rtsp - offset_base, &ori.port_rtsp, sizeof(ori.port_rtsp));
	strcpy((char *)_packet_set + offset_name - offset_base, ori.cameraName);

	strcpy((char *)_packet_set + offset_ip , info.ip);
	strcpy((char *)_packet_set + offset_gateway , info.gateway);
	strcpy((char *)_packet_set + offset_mask , info.mask);
	strcpy((char *)_packet_set + offset_dns1 , info.dns1);
	strcpy((char *)_packet_set + offset_dns2 , info.dns2);
	memcpy(_packet_set + offset_mac , info.mac, 6);
	for (int i = 0; i < 3; i++)
		memcpy(_packet_set + offset_port_http  + i * 2, &info.port_http, sizeof(info.port_http));
	memcpy(_packet_set + offset_port_rtsp , &info.port_rtsp, sizeof(info.port_rtsp));
	strcpy((char *)_packet_set + offset_name , info.cameraName);

	return broadcast_packet(port_destination, (char *)_packet_set, sizeof(_packet_set), port_source);
}

bool CameraAdapterH::ipcam_reboot(const IPCameraInfo &info, CString user, CString pwd)
{
	int rb = AfxMessageBox(_T("设置成功，重启摄像机后生效，是否现在重启？"), MB_OKCANCEL);
	if (rb == IDCANCEL) return false;
	CString request_header = "GET /cgi-bin/reboot.cgi";
	CString request = request_header + "?loginuse=" + user + "&loginpas=" + pwd + " HTTP/1.1\r\n\r\n";
	httpRequest(info.ip, info.port_http, request);
	request_header = "GET /reboot.cgi";
	request = request_header + "?loginuse=" + user + "&loginpas=" + pwd + " HTTP/1.1\r\n\r\n";
	httpRequest(info.ip, info.port_http, request);
	startProgressDlg(_T("摄像机正在重启中，请等待30秒钟"), 30);

	return true;
}

bool CameraAdapterH::wifi_scan(const IPCameraInfo &info)
{
	CString request = "GET /cgi-bin/wifi_scan.cgi HTTP/1.1\r\n\r\n";
	int ret_code = httpRequest(info.ip, info.port_http, request);
	if (ret_code != 200) {
		request = "GET /wifi_scan.cgi HTTP/1.1\r\n\r\n";
		ret_code = httpRequest(info.ip, info.port_http, request);
		if (ret_code != 200) return false;
	}
	startProgressDlg(_T("WIFI扫描中，请等待5秒种"), 5);
	return true;
}

bool CameraAdapterH::get_wifi_scan_result(const IPCameraInfo &info, std::vector<WIFI_Entry> &vec)
{
	CString xbuf, request = "GET /get_wifi_scan_result.cgi HTTP/1.1\r\n\r\n";
	int ret_code = httpRequest(info.ip, info.port_http, request, &xbuf);
	if (ret_code != 200) return false;
	return parseWIFIEntries(xbuf, vec);
}

bool CameraAdapterH::set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString wifi_pwd)
{
	int ret_code;
	char req_common[1024];
	CString request, user = "admin", pwd = "";
	CString wpa_psk, key1, keyformat, buf;
	CString nssid, ssid = CString(entry.ssid);
	CString request_header = "GET /cgi-bin/set_wifi.cgi";

	CStringSpecialChars(wifi_pwd);
	CStringSpecialChars(ssid);

	if (entry.security == 0) {
		wpa_psk = "";
		key1 = "";
		keyformat = "0";
	}
	else if (entry.security == 1) {
		wpa_psk = "";
		key1 = wifi_pwd;
		keyformat = "1";
	}
	else {
		wpa_psk = wifi_pwd;
		key1 = "";
		keyformat = "0";
	}
	sprintf(req_common, "keyformat=%s&key1=%s&key2=&key3=&key4=&key1_bits=0&key2_bits=0&key3_bits=0&key4_bits=0&enable=1&channel=5&authtype=0&defkey=0&ssid=%s&mode=%d&encrypt=%d&wpa_psk=%s", keyformat, key1, ssid, entry.mode, entry.security, wpa_psk);
	request = request_header + "?loginuse=" + user + 
			"&loginpas=" + pwd + "&" + req_common + " HTTP/1.1 \r\n\r\n";
	
	if ((ret_code = httpRequest(info.ip, info.port_http, request, &buf)) < 0) return false;
	if (ret_code == 404) {
		request_header = "GET /set_wifi.cgi";
		request = request_header + "?loginuse=" + user + 
			"&loginpas=" + pwd + "&" + req_common + " HTTP/1.1 \r\n\r\n";
		if ((ret_code = httpRequest(info.ip, info.port_http, request, &buf)) < 0) return false;
	}
	while (strstr(buf.GetBuffer(0), "user or passwd is error") != NULL) {
		if (!getAuth(user, pwd)) return false;
		CStringSpecialChars(user);
		CStringSpecialChars(pwd);
		request = "GET /cgi-bin/set_wifi.cgi?loginuse=" + user + 
			"&loginpas=" + pwd + "&" + req_common + " HTTP/1.1 \r\n\r\n";
		httpRequest(info.ip, info.port_http, request, &buf);
	}
	if (!get_params_ssid(info, nssid)) return false;
	if (nssid != ssid) return false;
	ipcam_reboot(info, user, pwd);

	return true;
}