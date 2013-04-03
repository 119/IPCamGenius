#include "stdafx.h"
#include "supplementary.h"

#include <io.h>
#include <fcntl.h>

void msgbox(CString msg)
{
	AfxMessageBox(msg);
}

void msgbox(const char *msg)
{
	CString _msg(msg);
	msgbox(_msg);
}

bool initSocket()
{
	WSADATA wsaData;
	int nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(nResult != NO_ERROR) return false;
	return true;
}

void InitConsoleWindow()
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}

CString ulong2CString(unsigned long n)
{
	char buf[100];

	sprintf(buf, "%u", n);
	return CString(buf);
}

bool getAuth(CString &user, CString &pwd)
{
	CAuthorization auth;
	int ret = auth.DoModal();
	if (ret == IDOK) {
		auth.getAuth(user, pwd);
		return true;
	}
	return false;
}

bool getAuth_Base64(CString &base64)
{
	char auth_base64[128];
	CString user, pwd;
	if (!getAuth(user, pwd)) return false;
	Base64Encode(auth_base64, user + ":" + pwd);
	base64 = CString(auth_base64);
	return true;
}

CString getAuth_Base64(CString user, CString pwd)
{
	char auth_base64[128];

	Base64Encode(auth_base64, user + ":" + pwd);
	return auth_base64;
}

bool parseValue(CString &content, CString variable, CString &value)
{
	int i, j, offset;
	char tmp[128], buf[256];
	
	offset = strstr(content, variable) - content;
	if (offset < 0) return false;
	sscanf(content.GetBuffer(0) + offset, "%[^=]%c%s", tmp, &tmp[0], buf);

	value = CString(buf);
	for (i = 0; buf[i] && buf[i] != '"' && buf[i] != '\''; i++);
	if (buf[i] == '"' || buf[i] == '\'') {
		for (j = i + 1; buf[j] && buf[j] != '"' && buf[j] != '\''; j++);
		if (!buf[j]) return false;
		buf[j] = 0;
		value = CString(buf + i + 1);
	}
	return true;
}

void startProgressDlg(CString caption, unsigned short s)
{
	unsigned short t = 0;

	s = s * 100;
	CProgressDialog process_dlg;
	process_dlg.Create(IDD_DIALOG_PROGRESS, NULL);
	process_dlg.SetWindowTextA(caption);
	process_dlg.CenterWindow();
	process_dlg.ShowWindow(SW_SHOW);
	process_dlg.m_progress.SetRange(0, s);
	process_dlg.m_progress.SetStep(1);
	while (t < s) {
		t++;
		process_dlg.m_progress.StepIt();
		process_dlg.UpdateWindow();
		Sleep(10);
	}
}

void CStringSpecialChars(CString &str)
{
	int i = 0;
	char buf[4096];
	const char *p = str;
	while (*p) {
		if (*p == ' ') {
			buf[i++] = '%';
			buf[i++] = '2';
			buf[i++] = '0';
		}
		else buf[i++] = *p;
		p++;
	}
	buf[i++] = 0;
	str = CString(buf);
}


//parse wifi entries
#define BIT_MAX 2
#define BIT_AP_SSID 0
#define BIT_AP_SECURITY 1
#define BIT_AP_MODE 2
int fullFillBit()
{
	int i, x = 0;
	
	for (i = 0; i <= BIT_MAX; i++)
		x |= (1 << i);

	return x;
}

int clearBit(int ori, int BIT)
{
	if (ori & (1 << BIT))
		ori ^= (1 << BIT);
	return ori;
}
bool parseWIFIEntries(CString xbuf, std::vector<WIFI_Entry> &vec)
{
	int n;
	char *pstr, *str, ch;
	char buf[NETWORK_MAX_PACKET_SIZE], tmp[128];
	WIFI_Entry we;

	vec.clear();
	strcpy(buf, xbuf);

	n = 0; pstr = buf;
	while (str = fetchLine(pstr)) {
		if (strstr(str, "ap_number")) {
			sscanf(str, "%[^=]%c%d%c", tmp, &ch, &n, &ch);
			break;
		}
	}
	
	while (n--) {
		int bit = fullFillBit();
		while (bit && (str = fetchLine(pstr))) {
			if (strstr(str, "ap_ssid[")) {
				bit = clearBit(bit, BIT_AP_SSID);
				sscanf(str, "%[^']%c%[^[]%c%[^]]", tmp, &ch, we.ssid, &ch, we.MAC);
			}
			else if (strstr(str, "ap_security[")) {
				bit = clearBit(bit, BIT_AP_SECURITY);
				sscanf(str, "%[^=]%c%d", &tmp, &ch, &we.security);
			}
			else if (strstr(str, "ap_mode[")) {
				bit = clearBit(bit, BIT_AP_MODE);
				sscanf(str, "%[^=]%c%d", &tmp, &ch, &we.mode);
			}
		}
		if (bit) return false;
		vec.push_back(we);
	}
	return true;
}

CString CStringTruncate(CString ori, int len)
{
	int i;
	char buf[1024];
	const char *p = ori;
	memset(buf, 0, 1024);

	for (i = 0; i < len && i < ori.GetLength(); i++)
		*(buf + i) = *(p + i);
	for (; i < len; i++)
		*(buf + i) = 0;
	*(buf + len) = 0;
	return buf;
}