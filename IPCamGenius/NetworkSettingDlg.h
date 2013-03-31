#pragma once

class CNetworkSettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNetworkSettingDlg)

public:
	CNetworkSettingDlg(CWnd* pParent = NULL);
	virtual ~CNetworkSettingDlg();

	enum { IDD = IDD_DIALOG_SET_NETWORK };

	void setPortHTTP(unsigned short port);
	void setPortRTSP(unsigned short port);
	void setIP(CString ip);
	void setMask(CString mask);
	void setGateway(CString gateway);
	void setRTSPEnable(bool enable);

	CString getIP() const;
	CString getMask() const;
	CString getGateway() const;
	unsigned short getPortHTTP() const;
	unsigned short getPortRTSP() const;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
public:
	bool m_isRTSPEnabled;
	UINT m_port_http;
	UINT m_port_rtsp;
	DWORD m_gateway;
	DWORD m_ip;
	DWORD m_mask;
private:
	DWORD IPConvert(CString ip);
	CString IPConvert2(DWORD x) const;
public:
	afx_msg void OnBnClickedNetworkSetting();
	afx_msg void OnBnClickedNetworkSetting_OK();
};
