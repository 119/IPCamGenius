#include "stdafx.h"
#include "IPCamGenius.h"
#include "Authorization.h"
#include "afxdialogex.h"
#include "supplementary.h"

IMPLEMENT_DYNAMIC(CAuthorization, CDialogEx)

CAuthorization::CAuthorization()
	: CDialogEx(CAuthorization::IDD, NULL)
{
	m_username.SetLimitText(3);
	m_password.SetLimitText(3);
}

CAuthorization::~CAuthorization()
{
}

void CAuthorization::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_GET_USER_PWD_USER, m_username);
	DDX_Control(pDX, IDC_EDIT_GET_USER_PWD_PWD, m_password);
}


BEGIN_MESSAGE_MAP(CAuthorization, CDialogEx)
	ON_BN_CLICKED(IDOK_GET_USER_PWD, &CAuthorization::OnBnClickedGetUserPwd)
	ON_BN_CLICKED(IDCANCEL_GET_USER_PWD, &CAuthorization::OnBnClickedGetUserPwdCancel)
	ON_EN_CHANGE(IDC_EDIT_GET_USER_PWD_USER, &CAuthorization::OnEnChangeEditGetUserPwdUser)
	ON_EN_CHANGE(IDC_EDIT_GET_USER_PWD_PWD, &CAuthorization::OnEnChangeEditGetUserPwdPwd)
END_MESSAGE_MAP()


void CAuthorization::OnBnClickedGetUserPwd()
{
	UpdateData();
	m_username.GetWindowText(username);
	m_password.GetWindowText(password);
	CDialogEx::OnOK();
}

void CAuthorization::OnBnClickedGetUserPwdCancel()
{
	CDialogEx::OnCancel();
}

void CAuthorization::getAuth(CString &user, CString &pwd)
{
	user = username;
	pwd = password;
}


void CAuthorization::OnEnChangeEditGetUserPwdUser()
{
	CString str;
	UpdateData();

	m_username.GetWindowTextA(str);
	if (str.GetLength() > 13) {
		str = CStringTruncate(str, 13);
		m_username.SetWindowTextA(str);
		m_username.SetFocus();
		m_username.SetSel(str.GetLength(), str.GetLength());
	}
}


void CAuthorization::OnEnChangeEditGetUserPwdPwd()
{
	CString str;
	UpdateData();

	m_password.GetWindowTextA(str);
	if (str.GetLength() > 13) {
		str = CStringTruncate(str, 13);
		m_password.SetWindowTextA(str);
		m_password.SetFocus();
		m_password.SetSel(str.GetLength(), str.GetLength());
	}
}
