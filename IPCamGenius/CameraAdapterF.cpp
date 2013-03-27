#include "stdafx.h"
#include <string.h>
#include "CameraAdapterF.h"

CameraAdapterF::CameraAdapterF()
{
	isGatewayInfoValid = getGatewayInfo(c_gateway, c_netmask);
}

bool CameraAdapterF::sendPacket(int n)
{
	bool ret = false;

	switch (n) {
	case 1:
		ret = sendPacket_1();
		break;
	}
	return ret;
}

bool CameraAdapterF::sendPacket_1()
{
	static unsigned char packet_1[27] = {
		0x4d, 0x4f, 0x5f, 0x49, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x01
	};
	return broadcast_packet(port_destination, (char *)packet_1, sizeof(packet_1), port_source);
}

bool CameraAdapterF::recvPacket()
{
	if (!recive_packet(port_source, (char *)last_recv_packet, last_recv_packet_size, &last_sender_ip)) return false;
	return true;
}

int CameraAdapterF::getPacketType() const
{
	if (isType_0()) return 0;

	return -1;
}

bool CameraAdapterF::isType_0() const
{
	static unsigned char _packet_0[5] = {
		0x4d, 0x4f, 0x5f, 0x49, 0x01
	};

	if (last_recv_packet_size != 218) return false;
	for (int i = 0; i < sizeof(_packet_0); i++) {
		if (last_recv_packet[i] != _packet_0[i]) return false;
	}

	return true;
}

IPCameraInfo CameraAdapterF::parsePacket() const
{
	IPCameraInfo caminfo;

	caminfo.port_rtsp = 0;
	strcpy(caminfo.ip, getIPByOffset(last_recv_packet, offset_ip));
	strcpy(caminfo.mask, getIPByOffset(last_recv_packet, offset_mask));
	strcpy(caminfo.gateway, getIPByOffset(last_recv_packet, offset_gateway));
	strcpy(caminfo.ip, last_sender_ip);
	memcpy(&(caminfo.port_http), (char *)last_recv_packet + offset_port_http, 2);
	strcpy(caminfo.cameraName, (char *)last_recv_packet + offset_name);

	return caminfo;
}

bool CameraAdapterF::_get_params(const IPCameraInfo &info, CString &params_buf)
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
		else if (ret_code == 401) msgbox(_T("�������"));
	}
	return true;
}

bool CameraAdapterF::get_params_ssid(const IPCameraInfo &info, CString &ssid)
{
	CString params_buf;

	if (!_get_params(info, params_buf)) return false;
	if (!parseValue(params_buf, "wifi_ssid", ssid)) return false;

	return true;
}

bool CameraAdapterF::set_network(const IPCameraInfo &info)
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
}

bool CameraAdapterF::ipcam_reboot(const IPCameraInfo &info, CString user, CString pwd)
{
	int rb = AfxMessageBox(_T("���óɹ����������������Ч���Ƿ�����������"), MB_OKCANCEL);
	if (rb == IDCANCEL) return false;
	CString request = "GET /reboot.cgi?user=" + user + "&pwd=" + pwd + " HTTP/1.1\r\n\r\n";
	httpRequest(info.ip, info.port_http, request);
	startProgressDlg(_T("��������������У���ȴ�30����"), 30);

	return true;
}

bool CameraAdapterF::wifi_scan(const IPCameraInfo &info)
{
	int ret_code;
	CString request_head = "GET /wifi_scan.cgi";
	CString request, user = "admin", pwd = "";
	
	CStringSpecialChars(user);
	CStringSpecialChars(pwd);
	request = request_head + "?user=" + user + "&pwd=" + pwd + " HTTP/1.1\r\n\r\n";
	
	ret_code = httpRequest(info.ip, info.port_http, request);
	while (ret_code == 401) {
		if (!getAuth(user, pwd)) return false;
		CStringSpecialChars(user);
		CStringSpecialChars(pwd);
		request = request_head + "?user=" + user + "&pwd=" + pwd + " HTTP/1.1\r\n\r\n";
		ret_code = httpRequest(info.ip, info.port_http, request);
	}
	if (ret_code != 200) return false;
	startProgressDlg(_T("WIFIɨ���У���ȴ�5����"), 5);
	return true;
}

bool CameraAdapterF::get_wifi_scan_result(const IPCameraInfo &info, std::vector<WIFI_Entry> &vec)
{
	int ret_code;
	CString request, auth_string = "YWRtaW46";
	CString xbuf, request_head = "GET /get_wifi_scan_result.cgi HTTP/1.1";

	request = request_head + "\r\n" + "Authorization: Basic " + auth_string + "\r\n\r\n";
	ret_code = httpRequest(info.ip, info.port_http, request, &xbuf);
	while (ret_code == 401) {
		if (!getAuth_Base64(auth_string)) return false;
		request = request_head + "\r\n" + "Authorization: Basic " + auth_string + "\r\n\r\n";
		ret_code = httpRequest(info.ip, info.port_http, request, &xbuf);
		if (ret_code < 0) return false;
		else if (ret_code == 401) msgbox(_T("�������"));
	}
	if (ret_code != 200) return false;
	return parseWIFIEntries(xbuf, vec);
}

bool CameraAdapterF::set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString wifi_pwd)
{
	int ret_code;
	char req_common[1024], req_user[1024];
	CString request, user = "admin", pwd = "";
	CString wpa_psk, key1, keyformat, buf;
	CString nssid, ssid = CString(entry.ssid);

	CStringSpecialChars(wifi_pwd);
	CStringSpecialChars(ssid);
	sprintf(req_user, "loginuse=%s&loginpas=%s", user, pwd);

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
	request = "GET /cgi-bin/set_wifi.cgi?loginuse=" + user + 
			"&loginpas=" + pwd + "&" + req_common + " HTTP/1.1 \r\n\r\n";
	
	if ((ret_code = httpRequest(info.ip, info.port_http, request, &buf)) < 0) return false;
	while (strstr(buf.GetBuffer(0), "user or passwd is error") != NULL) {
		if (!getAuth(user, pwd)) return false;
		CStringSpecialChars(user);
		CStringSpecialChars(pwd);
		request = "GET /cgi-bin/set_wifi.cgi?loginuse=" + user + 
			"&loginpas=" + pwd + "&" + req_common + " HTTP/1.1 \r\n\r\n";
		httpRequest(info.ip, info.port_http, request, &buf);
	}
	if (!strstr(buf.GetBuffer(0), "ok")) return false;
	if (!get_params_ssid(info, nssid)) return false;
	if (nssid != ssid) return false;
	ipcam_reboot(info, user, pwd);

	return true;
}