#pragma once
#include "afxdialogex.h"


// CNumber dialog

class CNumber : public CDialogEx
{
	DECLARE_DYNAMIC(CNumber)

public:
	CNumber(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CNumber();
	int		m_numberofholes;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NUMBEROFHOLES };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CEdit numhole;
	afx_msg void OnBnClickedOk();
};
