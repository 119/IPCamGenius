#include "stdafx.h"
#include "IPCamGenius.h"
#include "WIFIAuthDlg.h"
#include "afxdialogex.h"
#include "IPCamGeniusDlg.h"


IMPLEMENT_DYNAMIC(CWIFIAuthDlg, CDialogEx)

CWIFIAuthDlg::CWIFIAuthDlg(CWnd* pParent)
	: CDialogEx(CWIFIAuthDlg::IDD, pParent)
{

}

CWIFIAuthDlg::~CWIFIAuthDlg()
{
}

void CWIFIAuthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_SSID, m_ssid);
	DDX_Control(pDX, IDC_EDIT_SAFE_MODE, m_safe_mode);
	DDX_Control(pDX, IDC_EDIT_NETWORK_TYPE, m_network_type);
	DDX_Control(pDX, IDC_EDIT_WIFI_PASSWORD, m_wifi_password);
}


BEGIN_MESSAGE_MAP(CWIFIAuthDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_RETURN, &CWIFIAuthDlg::OnBnClickedButtonReturn)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CWIFIAuthDlg::OnBnClickedButtonApply)
END_MESSAGE_MAP()

void CWIFIAuthDlg::setWifiEntry(const WIFI_Entry &entry)
{
	we = entry;
	reset_ui();
}

void CWIFIAuthDlg::setIPCameraInfo(const IPCameraInfo &ipcam_info)
{
	info = ipcam_info;
}

void CWIFIAuthDlg::reset_ui()
{
	static CString network_type[2] = {
		_T("Infra"), 
		_T("Adhoc")
	};
	static CString safe_mode[6] = {
		_T("None"), 
		_T("WEP"), 
		_T("WPA TKIP"), 
		_T("WPA AES"), 
		_T("WPA2 AES"), 
		_T("WPA2 TKIP+AES")
	};

	m_ssid.SetWindowTextA(we.ssid);
	m_network_type.SetWindowTextA(network_type[we.mode]);
	m_safe_mode.SetWindowTextA(safe_mode[we.security]);
	m_wifi_password.SetReadOnly(we.security == 0);
}

void CWIFIAuthDlg::OnBnClickedButtonReturn()
{
	ShowWindow(SW_HIDE);
}

void CWIFIAuthDlg::setPointer(void *pthis)
{
	p_this = pthis;
}

void CWIFIAuthDlg::OnBnClickedButtonApply()
{
	UpdateData();

	CString pwd;
	CIPCamGeniusDlg *p = (CIPCamGeniusDlg *)p_this;

	m_wifi_password.GetWindowTextA(pwd);
	if (we.security != 0 && strlen(pwd) == 0) {
		AfxMessageBox("密码不可为空！");
		return;
	}
	if (!p->network_controller.set_wifi(info, we, pwd)) {
		AfxMessageBox("WIFI设置失败！");
		return;
	}
	AfxMessageBox("WIFI设置成功！");
}
