#include "stdafx.h"
#include "IPCamGenius.h"
#include "RadioUPnPMapDialog.h"
#include "afxdialogex.h"
#include "supplementary.h"

IMPLEMENT_DYNAMIC(CRadioUPnPMapDialog, CDialogEx)

CRadioUPnPMapDialog::CRadioUPnPMapDialog(int *opt, unsigned short *http_iport, 
			unsigned short *http_eport, unsigned short *rtsp_iport, unsigned short *rtsp_eport)
	: CDialogEx(CRadioUPnPMapDialog::IDD, NULL)
{
	p_opt = opt;
	p_http_iport = http_iport;
	p_http_eport = http_eport;
	p_rtsp_iport = rtsp_iport;
	p_rtsp_eport = rtsp_eport;
	m_radio = 0;
	m_http_iport = *http_iport;
	m_rtsp_iport = *rtsp_iport;
	m_rtsp_eport = *rtsp_iport;
	m_http_eport = *http_iport;
}

CRadioUPnPMapDialog::~CRadioUPnPMapDialog()
{
}

void CRadioUPnPMapDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_UPNP_MAP_COVER, m_radio);
	DDX_Text(pDX, IDC_EDIT_HTTP_EPORT, m_http_eport);
	DDV_MinMaxUInt(pDX, m_http_eport, 0, 65536);
	DDX_Text(pDX, IDC_EDIT_HTTP_IPORT, m_http_iport);
	DDV_MinMaxUInt(pDX, m_http_iport, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_RTSP_EPORT, m_rtsp_eport);
	DDV_MinMaxUInt(pDX, m_rtsp_eport, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_RTSP_IPORT, m_rtsp_iport);
	DDV_MinMaxUInt(pDX, m_rtsp_iport, 0, 65535);
}


BEGIN_MESSAGE_MAP(CRadioUPnPMapDialog, CDialogEx)
	ON_BN_CLICKED(IDC_RADIO_UPNP_MAP_COVER, &CRadioUPnPMapDialog::OnBnClickedRadioUpnpMapCover)
	ON_BN_CLICKED(IDC_RADIO_UPNP_MAP_CHANGE, &CRadioUPnPMapDialog::OnBnClickedRadioUpnpMapChange)
	ON_BN_CLICKED(IDOK_RADIO_UPNP_MAP, &CRadioUPnPMapDialog::OnBnClickedRadioUpnpMap)
	ON_BN_CLICKED(IDCANCEL_RADIO_UPNP_MAP, &CRadioUPnPMapDialog::OnBnClickedRadioUpnpMapCancel)
END_MESSAGE_MAP()



void CRadioUPnPMapDialog::OnBnClickedRadioUpnpMapCover()
{
	*p_opt = m_radio = COVER;
	setEditAreaEnabled(false);
	m_http_iport = *p_http_iport; m_http_eport = m_http_iport;
	m_rtsp_iport = *p_rtsp_iport; m_rtsp_eport = m_rtsp_iport;
	((CEdit *)GetDlgItem(IDC_EDIT_HTTP_IPORT))->SetWindowTextA(ulong2CString(m_http_iport));
	((CEdit *)GetDlgItem(IDC_EDIT_HTTP_EPORT))->SetWindowTextA(ulong2CString(m_http_eport));
	((CEdit *)GetDlgItem(IDC_EDIT_RTSP_IPORT))->SetWindowTextA(ulong2CString(m_rtsp_iport));
	((CEdit *)GetDlgItem(IDC_EDIT_RTSP_EPORT))->SetWindowTextA(ulong2CString(m_rtsp_eport));
}


void CRadioUPnPMapDialog::OnBnClickedRadioUpnpMapChange()
{
	*p_opt = m_radio = CHANGE;
	setEditAreaEnabled(true);
	m_http_iport = *p_http_eport; m_http_eport = m_http_iport;
	m_rtsp_iport = *p_rtsp_eport; m_rtsp_eport = m_rtsp_iport;
	((CEdit *)GetDlgItem(IDC_EDIT_HTTP_IPORT))->SetWindowTextA(ulong2CString(m_http_iport));
	((CEdit *)GetDlgItem(IDC_EDIT_HTTP_EPORT))->SetWindowTextA(ulong2CString(m_http_eport));
	((CEdit *)GetDlgItem(IDC_EDIT_RTSP_IPORT))->SetWindowTextA(ulong2CString(m_rtsp_iport));
	((CEdit *)GetDlgItem(IDC_EDIT_RTSP_EPORT))->SetWindowTextA(ulong2CString(m_rtsp_eport));
}

void CRadioUPnPMapDialog::setEditAreaEnabled(bool enable)
{
	bool readOnly = !enable;
	((CEdit *)GetDlgItem(IDC_EDIT_HTTP_IPORT))->SetReadOnly(readOnly);
	((CEdit *)GetDlgItem(IDC_EDIT_HTTP_EPORT))->SetReadOnly(readOnly);
	((CEdit *)GetDlgItem(IDC_EDIT_RTSP_IPORT))->SetReadOnly(readOnly);
	((CEdit *)GetDlgItem(IDC_EDIT_RTSP_EPORT))->SetReadOnly(readOnly);

	if (!readOnly && *p_http_iport == 0) {
		((CEdit *)GetDlgItem(IDC_EDIT_HTTP_IPORT))->SetReadOnly(true);
		((CEdit *)GetDlgItem(IDC_EDIT_HTTP_EPORT))->SetReadOnly(true);
	}
	if (!readOnly && *p_rtsp_iport == 0) {
		((CEdit *)GetDlgItem(IDC_EDIT_RTSP_IPORT))->SetReadOnly(true);
		((CEdit *)GetDlgItem(IDC_EDIT_RTSP_EPORT))->SetReadOnly(true);
	}
}

void CRadioUPnPMapDialog::OnBnClickedRadioUpnpMap()
{
	UpdateData();
	*p_opt = m_radio;
	if (m_radio == CHANGE) {
		*p_http_iport = m_http_iport;
		*p_http_eport = m_http_eport;
		*p_rtsp_iport = m_rtsp_iport;
		*p_rtsp_eport = m_rtsp_eport;
	}
	CDialogEx::OnOK();
}


void CRadioUPnPMapDialog::OnBnClickedRadioUpnpMapCancel()
{
	CDialogEx::OnCancel();
}
