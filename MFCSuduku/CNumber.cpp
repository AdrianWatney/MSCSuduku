// CNumber.cpp : implementation file
//

#include "pch.h"
#include "MFCSuduku.h"
#include "afxdialogex.h"
#include "CNumber.h"


// CNumber dialog

IMPLEMENT_DYNAMIC(CNumber, CDialogEx)

CNumber::CNumber(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NUMBEROFHOLES, pParent)
{
	
}

CNumber::~CNumber()
{
}

void CNumber::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, numhole);
	DDX_Text(pDX, IDC_EDIT1, m_numberofholes);
}


BEGIN_MESSAGE_MAP(CNumber, CDialogEx)
	ON_BN_CLICKED(IDOK, &CNumber::OnBnClickedOk)
END_MESSAGE_MAP()


// CNumber message handlers
BOOL CNumber::OnInitDialog()
{
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CNumber::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CDialogEx::OnOK();
}
