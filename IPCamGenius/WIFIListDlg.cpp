#include "stdafx.h"
#include "IPCamGenius.h"
#include "WIFIListDlg.h"
#include "afxdialogex.h"
#include "IPCamGeniusDlg.h"
#include "resource.h"

IMPLEMENT_DYNAMIC(CWIFIListDlg, CDialogEx)

CWIFIListDlg::CWIFIListDlg()
	: CDialogEx(CWIFIListDlg::IDD, NULL)
{
}

CWIFIListDlg::~CWIFIListDlg()
{
}

void CWIFIListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WIFI_LIST, m_wifi_list);
}

void CWIFIListDlg::setPointer(void *pthis)
{
	p_this = pthis;
}

void CWIFIListDlg::setIPCameraInfo(IPCameraInfo &ipcam_info)
{
	info = ipcam_info;
}

void CWIFIListDlg::f_wifi_list_clear()
{
	m_wifi_list.DeleteAllItems();
	m_wifi_list.DeleteColumn(0);
	m_wifi_list.InsertColumn(0, _T("无线网络列表"), LVCFMT_CENTER, 300);
	m_wifi_list.SetColumnWidth(0, 185);
}


void CWIFIListDlg::f_wifi_list_refresh()
{
	CIPCamGeniusDlg *p = (CIPCamGeniusDlg *)p_this;

	p->network_controller.get_wifi_scan_result(info, vec_wifi);

	f_wifi_list_clear();
	for (unsigned int i = 0; i < vec_wifi.size(); i++) 
		m_wifi_list.InsertItem(i, CString(vec_wifi[i].ssid));
}


BEGIN_MESSAGE_MAP(CWIFIListDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_WIFI, &CWIFIListDlg::OnBnClickedButtonScanWifi)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_WIFI_LIST, &CWIFIListDlg::OnLvnItemchangedListWifiList)
END_MESSAGE_MAP()


void CWIFIListDlg::OnBnClickedButtonScanWifi()
{
	CIPCamGeniusDlg *p = (CIPCamGeniusDlg *)p_this;

	f_wifi_list_clear();
	if (!p->network_controller.wifi_scan(info)) {
		msgbox("扫描失败！");
		return;
	}
	f_wifi_list_refresh();
}


void CWIFIListDlg::OnLvnItemchangedListWifiList(NMHDR *pNMHDR, LRESULT *pResult)
{
	int idx;
	CIPCamGeniusDlg *p = (CIPCamGeniusDlg *)p_this;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->uNewState != (LVIS_SELECTED | LVIS_FOCUSED)) return;

	idx = pNMLV->iItem;
	p->m_dialog_wifi_auth.ShowWindow(SW_SHOW);
	p->m_dialog_wifi_auth.setWifiEntry(vec_wifi[idx]);
	p->m_dialog_wifi_auth.setIPCameraInfo(info);
	p->m_dialog_wifi_auth.setPointer(p_this);

	*pResult = 0;
}
