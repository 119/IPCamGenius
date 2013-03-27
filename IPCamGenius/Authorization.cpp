#include "stdafx.h"
#include "IPCamGenius.h"
#include "Authorization.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CAuthorization, CDialogEx)

CAuthorization::CAuthorization()
	: CDialogEx(CAuthorization::IDD, NULL)
{

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
