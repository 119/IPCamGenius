#pragma once

#include "resource.h"
#include "WIFIAuthDlg.h"
#include "WIFIListDlg.h"
#include "supplementary.h"
#include "thread_functions.h"
#include "NetworkController.h"

class CIPCamGeniusDlg : public CDialogEx
{
public:
	CIPCamGeniusDlg(CWnd* pParent = NULL);
	enum { IDD = IDD_IPCAMGENIUS_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
protected:
	HICON m_hIcon;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void f_list_camera_info_clear();
	void f_list_camera_info_set(CString info[], int sz = 8);

	void f_list_cameras_clear();
	void f_list_cameras_update();

	void reset_network();
	void reset_ui();
public:
	bool isWifiButtonClicked;
	NetworkController network_controller;
	CListCtrl m_list_camera_info;
	CListCtrl m_list_cameras;
	CWIFIListDlg m_dialog_wifi_list;
	CWIFIAuthDlg m_dialog_wifi_auth;

	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnLvnItemchangedListCameras(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonAccessRouter();
	afx_msg void OnBnClickedButtonAccessCamera();
	afx_msg void OnBnClickedButtonOnekeyConfig();
	afx_msg void OnBnClickedButtonRefreshNetwork();
	afx_msg void OnBnClickedButtonWifiConfig();
};
