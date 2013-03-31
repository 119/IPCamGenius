#include "stdafx.h"
#include "IPCamGenius.h"
#include "NetworkSettingDlg.h"
#include "afxdialogex.h"


IMPLEMENT_DYNAMIC(CNetworkSettingDlg, CDialogEx)

CNetworkSettingDlg::CNetworkSettingDlg(CWnd* pParent)
	: CDialogEx(CNetworkSettingDlg::IDD, pParent)
{
	m_port_http = 0;
	m_port_rtsp = 0;
	m_isRTSPEnabled = true;
	m_gateway = 0;
	m_ip = 0;
	m_mask = 0;
}

CNetworkSettingDlg::~CNetworkSettingDlg()
{
}

void CNetworkSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT_HTTP, m_port_http);
	DDV_MinMaxUInt(pDX, m_port_http, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_PORT_RTSP, m_port_rtsp);
	DDV_MinMaxUInt(pDX, m_port_rtsp, 0, 65535);
	DDX_IPAddress(pDX, IDC_IPADDRESS_GATEWAY, m_gateway);
	DDX_IPAddress(pDX, IDC_IPADDRESS_IP, m_ip);
	DDX_IPAddress(pDX, IDC_IPADDRESS_MASK, m_mask);
}


BEGIN_MESSAGE_MAP(CNetworkSettingDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL_NETWORK_SETTING, &CNetworkSettingDlg::OnBnClickedNetworkSetting)
	ON_BN_CLICKED(IDOK_NETWORK_SETTING, &CNetworkSettingDlg::OnBnClickedNetworkSetting_OK)
END_MESSAGE_MAP()

void CNetworkSettingDlg::setPortHTTP(unsigned short port)
{
	m_port_http = port;
}

void CNetworkSettingDlg::setPortRTSP(unsigned short port)
{
	m_port_rtsp = port;
}

void CNetworkSettingDlg::setIP(CString ip)
{
	m_ip = IPConvert(ip);
}

void CNetworkSettingDlg::setMask(CString mask)
{
	m_mask = IPConvert(mask);
}

void CNetworkSettingDlg::setGateway(CString gateway)
{
	m_gateway = IPConvert(gateway);
}

void CNetworkSettingDlg::setRTSPEnable(bool enable)
{
	m_isRTSPEnabled = enable;
}

DWORD CNetworkSettingDlg::IPConvert(CString ip)
{
	DWORD a, b, c, d;
	DWORD x = inet_addr(ip);

	a = (x >> 24) & 0xFF;
	b = (x >> 16) & 0xFF;
	c = (x >> 8) & 0xFF;
	d = x & 0xFF;

	return ((d << 24) | (c << 16) | (b << 8) | a);
}

CString CNetworkSettingDlg::IPConvert2(DWORD x) const
{
	char buf[64];
	int a, b, c, d;

	a = (x >> 24) & 0xFF;
	b = (x >> 16) & 0xFF;
	c = (x >> 8) & 0xFF;
	d = x & 0xFF;

	sprintf(buf, "%d.%d.%d.%d", a, b, c, d);
	return buf;
}

CString CNetworkSettingDlg::getIP() const
{
	return IPConvert2(m_ip);
}

CString CNetworkSettingDlg::getMask() const
{
	return IPConvert2(m_mask);
}

CString CNetworkSettingDlg::getGateway() const
{
	return IPConvert2(m_gateway);
}

unsigned short CNetworkSettingDlg::getPortHTTP() const
{
	return m_port_http;
}

unsigned short CNetworkSettingDlg::getPortRTSP() const
{
	return m_port_rtsp;
}


void CNetworkSettingDlg::OnBnClickedNetworkSetting()
{
	return CDialogEx::OnCancel();
}


void CNetworkSettingDlg::OnBnClickedNetworkSetting_OK()
{
	return CDialogEx::OnOK();
}
