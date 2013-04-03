#include "stdafx.h"
#include <string.h>
#include "CameraAdapterF.h"

CameraAdapterF::CameraAdapterF()
{
	isGatewayInfoValid = getGatewayInfo(c_gateway, c_netmask);
}

char CameraAdapterF::getFlag()
{
	return 'F';
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

IPCameraInfo CameraAdapterF::parsePacket()
{
	IPCameraInfo caminfo;

	caminfo.port_rtsp = 0;
	strcpy(caminfo.ip, getIPByOffset(last_recv_packet, offset_ip));
	strcpy(caminfo.mask, getIPByOffset(last_recv_packet, offset_mask));
	strcpy(caminfo.gateway, getIPByOffset(last_recv_packet, offset_gateway));
	memcpy(&(caminfo.port_http), (char *)last_recv_packet + offset_port_http, 2);
	strcpy(caminfo.cameraName, (char *)last_recv_packet + offset_name);
	strcpy(caminfo.mac, (char *)last_recv_packet + offset_mac);

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
		else if (ret_code == 401) msgbox(_T("密码错误！"));
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

bool CameraAdapterF::set_network(const IPCameraInfo &info, const IPCameraInfo &ori)
{
	char buf[100];
	CString user, pwd;
	static unsigned char _packet_set[0x54] = {
		0x4d, 0x4f, 0x5f, 0x49, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3d
	};
	memset(_packet_set + 0x10, 0, 0x44);
	strcpy((char *)_packet_set + 0x1B, info.mac);
	if (!getAuth(user, pwd)) return false;
	user = CStringTruncate(user, 13);
	pwd = CStringTruncate(pwd, 13);
	memset(buf, 0, sizeof(buf)); memcpy(buf, user, user.GetLength());
	memcpy((char *)_packet_set + 0x28, buf, 13);
	memset(buf, 0, sizeof(buf)); memcpy(buf, pwd, pwd.GetLength());
	memcpy((char *)_packet_set + 0x35, buf, 13);
	fillOffsetByIP(_packet_set, 0x42, info.ip);
	fillOffsetByIP(_packet_set, 0x46, info.mask);
	fillOffsetByIP(_packet_set, 0x4A, info.gateway);
	fillOffsetByIP(_packet_set, 0x4E, "8.8.8.8");
	_packet_set[0x52] = (unsigned char)((info.port_http >> 8) & 255);
	_packet_set[0x53] = (unsigned char)(info.port_http & 255);

	return broadcast_packet(port_destination, (char *)_packet_set, sizeof(_packet_set), port_source);
}

bool CameraAdapterF::ipcam_reboot(const IPCameraInfo &info, CString user, CString pwd)
{
	int rb = AfxMessageBox(_T("设置成功，重启摄像机后生效，是否现在重启？"), MB_OKCANCEL);
	if (rb == IDCANCEL) return false;
	CString base64 = getAuth_Base64(user, pwd);
	CString request = "GET /reboot.cgi HTTP/1.1\r\nAuthorization: Basic " + base64 + "\r\n\r\n";
	httpRequest(info.ip, info.port_http, request);
	startProgressDlg(_T("摄像机正在重启中，请等待30秒钟"), 30);

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
	startProgressDlg(_T("WIFI扫描中，请等待5秒种"), 5);
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
		else if (ret_code == 401) msgbox(_T("密码错误！"));
	}
	if (ret_code != 200) return false;
	return parseWIFIEntries(xbuf, vec);
}

bool CameraAdapterF::set_wifi(const IPCameraInfo &info, const WIFI_Entry &entry, CString wifi_pwd)
{
	int ret_code;
	char req_common[1024];
	CString auth_base64;
	CString request, user = "adminx", pwd = "";
	CString wpa_psk, key1, keyformat, buf;
	CString nssid, ssid = CString(entry.ssid);

	auth_base64 = getAuth_Base64(user, pwd);
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
	request = "GET /set_wifi.cgi?user=" + user + 
			"&pwd=" + pwd + "&" + req_common + " HTTP/1.1 \r\nAuthorization: Basic " + auth_base64 + "\r\n\r\n";
	
	if ((ret_code = httpRequest(info.ip, info.port_http, request, &buf)) < 0) return false;
	while (strstr(buf.GetBuffer(0), "user or passwd is error")
			|| strstr(buf.GetBuffer(0), "Unauth") || strstr(buf.GetBuffer(0), "401")) {
		if (!getAuth(user, pwd)) return false;
		auth_base64 = getAuth_Base64(user, pwd);
		CStringSpecialChars(user);
		CStringSpecialChars(pwd);
		request = "GET /set_wifi.cgi?user=" + user + 
			"&pwd=" + pwd + "&" + req_common + " HTTP/1.1 \r\nAuthorization: Basic " + auth_base64 + "\r\n\r\n";
		httpRequest(info.ip, info.port_http, request, &buf);
	}
	if (!strstr(buf.GetBuffer(0), "ok")) return false;
	if (!get_params_ssid(info, nssid)) return false;
	if (nssid != ssid) return false;
	ipcam_reboot(info, user, pwd);

	return true;
}