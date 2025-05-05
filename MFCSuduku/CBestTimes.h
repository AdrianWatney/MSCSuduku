#pragma once
#include "afxdialogex.h"
#include "ChildView.h"

// CBestTimes dialog

class CBestTimes : public CDialogEx
{
	DECLARE_DYNAMIC(CBestTimes)

public:
	CBestTimes(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CBestTimes();
	virtual BOOL OnInitDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CListBox m_besttimeslist;
	BESTTIMERECORD	m_besttimes[10];  // The last 10 best times
};
