#include "stdafx.h"
#include "IPCamGenius.h"
#include "IPCamGeniusDlg.h"
#include "afxdialogex.h"
#include "RadioUPnPMapDialog.h"
#include "NetworkSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIPCamGeniusDlg::CIPCamGeniusDlg(CWnd* pParent)
	: CDialogEx(CIPCamGeniusDlg::IDD, pParent), network_controller(this)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	isWifiButtonClicked = false;
}

void CIPCamGeniusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CAMERA_INFO, m_list_camera_info);
	DDX_Control(pDX, IDC_LIST_CAMERAS, m_list_cameras);
}

BEGIN_MESSAGE_MAP(CIPCamGeniusDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, &CIPCamGeniusDlg::OnBnClickedButtonExit)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CAMERAS, &CIPCamGeniusDlg::OnLvnItemchangedListCameras)
	ON_BN_CLICKED(IDC_BUTTON_ACCESS_ROUTER, &CIPCamGeniusDlg::OnBnClickedButtonAccessRouter)
	ON_BN_CLICKED(IDC_BUTTON_ACCESS_CAMERA, &CIPCamGeniusDlg::OnBnClickedButtonAccessCamera)
	ON_BN_CLICKED(IDC_BUTTON_ONEKEY_CONFIG, &CIPCamGeniusDlg::OnBnClickedButtonOnekeyConfig)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_NETWORK, &CIPCamGeniusDlg::OnBnClickedButtonRefreshNetwork)
	ON_BN_CLICKED(IDC_BUTTON_WIFI_CONFIG, &CIPCamGeniusDlg::OnBnClickedButtonWifiConfig)
	ON_BN_CLICKED(IDC_BUTTON_SET_NETWORK, &CIPCamGeniusDlg::OnBnClickedButtonSetNetwork)
END_MESSAGE_MAP()

BOOL CIPCamGeniusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	CRect r;
	m_dialog_wifi_list.Create(IDD_DIALOG_WIFILIST, this);
	GetDlgItem(IDC_STATIC_GROUPBOX_WIFILIST)->GetWindowRect(r);
	ScreenToClient(r);
	GetDlgItem(IDC_STATIC_GROUPBOX_WIFILIST)->DestroyWindow();
	m_dialog_wifi_list.MoveWindow(r);
	m_dialog_wifi_list.ShowWindow(SW_HIDE);

	m_dialog_wifi_auth.Create(IDD_DIALOG_WIFI_AUTH, this);
	GetDlgItem(IDC_STATIC_GROUPBOX_WIFIAUTH)->GetWindowRect(r);
	ScreenToClient(r);
	GetDlgItem(IDC_STATIC_GROUPBOX_WIFIAUTH)->DestroyWindow();
	m_dialog_wifi_auth.MoveWindow(r);
	m_dialog_wifi_auth.ShowWindow(SW_HIDE);

	f_list_camera_info_clear();
	f_list_cameras_clear();

	if (!initSocket()) {
		msgbox("Socket初始化失败！");
		CDialogEx::OnCancel();
	}

	CreateThread(0, 0, t_gateway_startup, this, 0, NULL);
	CreateThread(0, 0, t_send_packets, this, 0, NULL);

	void **args;
	for (int i = 0; i < network_controller.camera_adapters_num; i++) {
		args = new void*[2];
		args[0] = this;
		args[1] = new int(i);
		CreateThread(0, 0, t_recv_packets, args, 0, NULL);
	}

	CenterWindow();
	return TRUE;
}

void CIPCamGeniusDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CIPCamGeniusDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CIPCamGeniusDlg::OnBnClickedButtonExit()
{
	CDialogEx::OnCancel();
}

void CIPCamGeniusDlg::f_list_camera_info_clear()
{
	m_list_camera_info.DeleteAllItems();
	m_list_camera_info.DeleteColumn(0);
	m_list_camera_info.InsertColumn(0, _T("信息"), LVCFMT_CENTER, 300);
	m_list_camera_info.SetColumnWidth(0, 260);
}

void CIPCamGeniusDlg::f_list_camera_info_set(CString info[], int sz)
{
	static CString info_headers[] = {
		_T("网关IP: "), 
		_T("子网掩码: "), 
		_T("网络摄像机IP: "), 
		_T("HTTP访问端口: "), 
		_T("RTSP访问端口: "), 
		_T("公网IP: "), 
		_T("公网HTTP访问端口: "), 
		_T("公网RTSP访问端口: "), 
	};

	f_list_camera_info_clear();
	for (int i = 0; i < sz; i++) {
		m_list_camera_info.InsertItem(i, info_headers[i] + info[i]);
	}
}

void CIPCamGeniusDlg::f_list_cameras_clear()
{
	m_list_cameras.DeleteAllItems();
	m_list_cameras.DeleteColumn(0);
	m_list_cameras.InsertColumn(0, _T("摄像机列表"), LVCFMT_CENTER, 300);
	m_list_cameras.SetColumnWidth(0, 168);
}

void CIPCamGeniusDlg::f_list_cameras_update()
{
	f_list_cameras_clear();
	for (int i = 0; i < (int)network_controller.vec_info.size(); i++) {
		m_list_cameras.InsertItem(i, CString(network_controller.vec_info[i].cameraName));
	}
}

void CIPCamGeniusDlg::reset_network()
{
	network_controller.reset_network();
	f_list_cameras_clear();
	f_list_camera_info_clear();
	reset_ui();
}

void CIPCamGeniusDlg::reset_ui()
{
	GetDlgItem(IDC_BUTTON_ONEKEY_CONFIG)->ShowWindow(SW_SHOW);
	m_dialog_wifi_list.ShowWindow(SW_HIDE);
	m_dialog_wifi_auth.ShowWindow(SW_HIDE);
	isWifiButtonClicked = false;
}

void CIPCamGeniusDlg::OnLvnItemchangedListCameras(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int idx = pNMLV->iItem;
	unsigned short eport_http = 0, eport_rtsp = 0;
	UPNPMap ipmap;

	if (pNMLV->uNewState != (LVIS_SELECTED | LVIS_FOCUSED)) return;

	reset_ui();

	IPCameraInfo info = network_controller.vec_info[idx];
	ipmap.set(info.ip, info.port_http);
	network_controller.gateway.getUPnPMap(ipmap, eport_http);
	ipmap.set(info.ip, info.port_rtsp);
	network_controller.gateway.getUPnPMap(ipmap, eport_rtsp);

	CString cinfo[8] = {
		CString(info.gateway), 
		CString(info.mask), 
		CString(info.ip), 
		info.port_http ? ulong2CString(info.port_http) : 0, 
		info.port_rtsp ? ulong2CString(info.port_rtsp) : 0,  
		network_controller.gateway.getExternalIPAddress(), 
		eport_http ? ulong2CString(eport_http) : 0, 
		eport_rtsp ? ulong2CString(eport_rtsp) : 0
	};
	f_list_camera_info_set(cinfo);

	*pResult = 0;
}

void CIPCamGeniusDlg::OnBnClickedButtonAccessRouter()
{
	if (network_controller.gateway.isError()) {
		msgbox(_T("无法获取网关信息！"));
		return;
	}
	if (!network_controller.gateway.isReady()) {
		msgbox(_T("正在获取网关信息，请稍后再试！"));
		return;
	}

	CString url = _T("http://") + network_controller.gateway.getGatewayAddress() + _T("/");
	ShellExecute(m_hWnd, NULL, url, NULL, NULL, SW_SHOWMAXIMIZED);
}


void CIPCamGeniusDlg::OnBnClickedButtonAccessCamera()
{
	char buf[10];
	POSITION pos = m_list_cameras.GetFirstSelectedItemPosition();
	int idx = m_list_cameras.GetNextSelectedItem(pos);
	if (idx < 0) {
		msgbox("请先选择摄像机");
		return;
	}
	
	IPCameraInfo ipcaminfo = network_controller.vec_info[idx];

	CString s_ip(ipcaminfo.ip), s_port(_itoa(ipcaminfo.port_http, buf, 10));
	CString url = _T("http://") + s_ip + _T(":") + s_port + _T("/");
	ShellExecute(m_hWnd, NULL, url, NULL, NULL, SW_SHOWMAXIMIZED);
}


void CIPCamGeniusDlg::OnBnClickedButtonOnekeyConfig()
{
	POSITION pos = m_list_cameras.GetFirstSelectedItemPosition();
	int idx = m_list_cameras.GetNextSelectedItem(pos);
	IPCameraInfo info = network_controller.vec_info[idx];
	UPNPMap ipmap_http(info.ip, info.port_http), ipmap_rtsp(info.ip, info.port_rtsp);
	unsigned short eport_http = 0, eport_rtsp = 0, iport_http = 0, iport_rtsp = 0;
	Gateway *p_gateway = &network_controller.gateway;

	if (network_controller.gateway.isError()) {
		msgbox(_T("无法获取网关信息！"));
		return;
	}
	if (!network_controller.gateway.isReady()) {
		msgbox(_T("正在获取网关信息，请稍后再试！"));
		return;
	}

	p_gateway->getUPnPMap(ipmap_http, eport_http);
	p_gateway->getUPnPMap(ipmap_rtsp, eport_rtsp);

	if (eport_http && (eport_rtsp || !info.port_rtsp)) {
		msgbox("端口映射无需配置！");
		return;
	}
	if (!eport_http || info.port_rtsp && !eport_rtsp) {
		if (!p_gateway->getNextAvailableTCPPorts(info.port_http, info.port_rtsp, 
					eport_http, eport_rtsp)) {
			if (AfxMessageBox("端口映射已满，是否强制覆盖？", MB_OKCANCEL) == IDCANCEL) {
				msgbox("端口映射失败！");
				return;
			}
			p_gateway->addTCPPortMapping(info.ip, info.port_http, info.port_http);
			if (info.port_rtsp) p_gateway->addTCPPortMapping(info.ip, info.port_rtsp, info.port_rtsp);
			msgbox("端口映射完毕！");
			reset_network();
			return;
		}
		if (eport_http == info.port_http && (!info.port_rtsp || eport_rtsp == info.port_rtsp)) {
			p_gateway->addTCPPortMapping(info.ip, info.port_http, info.port_http);
			p_gateway->addTCPPortMapping(info.ip, info.port_rtsp, info.port_rtsp);
			msgbox("端口映射完毕！");
			reset_network();
			return;
		}
		else {
			int opt;
			iport_http = info.port_http;
			iport_rtsp = info.port_rtsp;
			CRadioUPnPMapDialog radio_upnp_map(&opt, &iport_http, &eport_http, 
					&iport_rtsp, &eport_rtsp);
			if (radio_upnp_map.DoModal() == IDCANCEL) {
				msgbox("端口映射失败！");
				return;
			}
			if (opt == CRadioUPnPMapDialog::COVER) {
				p_gateway->addTCPPortMapping(info.ip, info.port_http, info.port_http);
				if (info.port_rtsp) p_gateway->addTCPPortMapping(info.ip, info.port_rtsp, info.port_rtsp);
				msgbox("端口映射完毕！");
				reset_network();
				return;
			}
			else {
				int index = info.adapter_idx;
				p_gateway->addTCPPortMapping(info.ip, iport_http, eport_http);
				if (info.port_rtsp) p_gateway->addTCPPortMapping(info.ip, iport_rtsp, eport_rtsp);
				info.port_http = iport_http; info.port_rtsp = iport_rtsp;
				network_controller.camera_adapters[index]->set_network(info);
				msgbox("端口映射完毕！");
				reset_network();
				return;
			}
		}
	}
}


void CIPCamGeniusDlg::OnBnClickedButtonRefreshNetwork()
{
	reset_network();
}


void CIPCamGeniusDlg::OnBnClickedButtonWifiConfig()
{
	POSITION pos = m_list_cameras.GetFirstSelectedItemPosition();
	int idx = m_list_cameras.GetNextSelectedItem(pos);
	if (idx < 0) {
		msgbox("请先选择摄像机");
		return;
	}

	isWifiButtonClicked = !isWifiButtonClicked;

	if (!isWifiButtonClicked) {
		reset_ui();
		return;
	}
	GetDlgItem(IDC_BUTTON_ONEKEY_CONFIG)->ShowWindow(SW_HIDE);
	m_dialog_wifi_list.ShowWindow(SW_SHOW);
	m_dialog_wifi_list.setPointer(this);
	m_dialog_wifi_list.setIPCameraInfo(network_controller.vec_info[idx]);
	m_dialog_wifi_list.f_wifi_list_refresh();
}


void CIPCamGeniusDlg::OnBnClickedButtonSetNetwork()
{
	POSITION pos = m_list_cameras.GetFirstSelectedItemPosition();
	int idx = m_list_cameras.GetNextSelectedItem(pos);
	if (idx < 0) {
		msgbox("请先选择摄像机");
		return;
	}

	IPCameraInfo info = network_controller.vec_info[idx];

	CNetworkSettingDlg dlg;
	dlg.setIP(info.ip);
	dlg.setPortHTTP(info.port_http);
	dlg.setMask(info.mask);
	dlg.setGateway(info.gateway);
	if (dlg.DoModal() == IDOK) {
		strcpy(info.ip, dlg.getIP());
		strcpy(info.gateway, dlg.getGateway());
		strcpy(info.mask, dlg.getMask());
		info.port_http = dlg.getPortHTTP();
		info.port_rtsp = dlg.getPortRTSP();
		network_controller.camera_adapters[info.adapter_idx]->set_network(info);
		reset_network();
	}
}
