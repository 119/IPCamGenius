#pragma once

class CAuthorization : public CDialogEx
{
	DECLARE_DYNAMIC(CAuthorization)
public:
	CAuthorization();
	virtual ~CAuthorization();
	enum { IDD = IDD_DIALOG_GET_USER_PWD };
	void getAuth(CString &user, CString &pwd);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()

	CString username;
	CString password;
public:
	afx_msg void OnBnClickedGetUserPwd();
	CEdit m_username;
	CEdit m_password;
	afx_msg void OnBnClickedGetUserPwdCancel();
	afx_msg void OnEnChangeEditGetUserPwdUser();
	afx_msg void OnEnChangeEditGetUserPwdPwd();
};
