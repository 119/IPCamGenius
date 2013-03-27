#pragma once
#include "network.h"
#include <vector>

class CWIFIListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CWIFIListDlg)
public:
	CWIFIListDlg();
	virtual ~CWIFIListDlg();
	enum { IDD = IDD_DIALOG_WIFILIST };
	void setPointer(void *pthis);
	void setIPCameraInfo(IPCameraInfo &ipcam_info);

	void f_wifi_list_clear();
	void f_wifi_list_refresh();
protected:
	void *p_this;
	IPCameraInfo info;
	std::vector<WIFI_Entry> vec_wifi;

	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonScanWifi();
	CListCtrl m_wifi_list;
	afx_msg void OnLvnItemchangedListWifiList(NMHDR *pNMHDR, LRESULT *pResult);
};
