#ifndef _SUPPLEMENTARY_H
#define _SUPPLEMENTARY_H

#include "base64.h"
#include "network.h"
#include "resource.h"
#include "Authorization.h"
#include "ProgressDialog.h"

#include <vector>

void msgbox(const CString msg);
void msgbox(const char *msg);
bool initSocket();
void InitConsoleWindow();
CString ulong2CString(unsigned long n);
bool getAuth(CString &user, CString &pwd);
bool getAuth_Base64(CString &base64);
bool parseValue(CString &content, CString variable, CString &value);
void startProgressDlg(CString caption, unsigned short s);
void CStringSpecialChars(CString &str);
bool parseWIFIEntries(CString xbuf, std::vector<WIFI_Entry> &vec);
CString CStringTruncate(CString ori, int len);

#endif