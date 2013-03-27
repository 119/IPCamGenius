#pragma once

class CRadioUPnPMapDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CRadioUPnPMapDialog)

public:
	CRadioUPnPMapDialog(int *opt, unsigned short *http_iport, 
			unsigned short *http_eport, unsigned short *rtsp_iport, unsigned short *rtsp_eport);
	virtual ~CRadioUPnPMapDialog();

	enum { IDD = IDD_DIALOG_RADIO_UPNP_MAP };
	enum { COVER = 0, CHANGE = 1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	int m_radio;
	afx_msg void OnBnClickedRadioUpnpMapCover();
	afx_msg void OnBnClickedRadioUpnpMapChange();
	void setEditAreaEnabled(bool enable);
private:
	int *p_opt;
	unsigned short *p_http_iport;
	unsigned short *p_http_eport;
	unsigned short *p_rtsp_iport;
	unsigned short *p_rtsp_eport;
public:
	UINT m_http_eport;
	UINT m_http_iport;
	UINT m_rtsp_eport;
	UINT m_rtsp_iport;
	afx_msg void OnBnClickedRadioUpnpMap();
	afx_msg void OnBnClickedRadioUpnpMapCancel();
};
