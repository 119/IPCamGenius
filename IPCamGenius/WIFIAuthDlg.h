#pragma once

#include "network.h"

class CWIFIAuthDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWIFIAuthDlg)

public:
	CWIFIAuthDlg(CWnd* pParent = NULL);
	virtual ~CWIFIAuthDlg();

	enum { IDD = IDD_DIALOG_WIFI_AUTH };

	void setPointer(void *pthis);
	void setWifiEntry(const WIFI_Entry &entry);
	void setIPCameraInfo(const IPCameraInfo &ipcam_info);
	void reset_ui();
private:
	void *p_this;
	WIFI_Entry we;
	IPCameraInfo info;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ssid;
	CEdit m_safe_mode;
	CEdit m_network_type;
	CEdit m_wifi_password;
	afx_msg void OnBnClickedButtonReturn();
	afx_msg void OnBnClickedButtonApply();
};
