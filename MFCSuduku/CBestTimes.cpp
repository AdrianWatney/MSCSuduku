// CBestTimes.cpp : implementation file
//

#include "pch.h"
#include "MFCSuduku.h"
#include "afxdialogex.h"
#include "CBestTimes.h"


// CBestTimes dialog

IMPLEMENT_DYNAMIC(CBestTimes, CDialogEx)

CBestTimes::CBestTimes(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CBestTimes::~CBestTimes()
{
}

void CBestTimes::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_besttimeslist);
}


BEGIN_MESSAGE_MAP(CBestTimes, CDialogEx)
END_MESSAGE_MAP()


// CBestTimes message handlers
BOOL CBestTimes::OnInitDialog()
{
	int			i;
	CString		s;

	if (!CDialogEx::OnInitDialog())
		return FALSE;
	for (i = 0; i < 10; i++) {
		if (m_besttimes[i].m_datetime > 0) {
			// one time to show
			CTime		t(m_besttimes[i].m_datetime);
			s.Format(_T("%d Holes: %d,Time: %d:%02d %s"), i+1, m_besttimes[i].m_numberHoles, m_besttimes[i].m_besttime / 60, m_besttimes[i].m_besttime % 60, t.Format(_T("%H:%M %A, %B %d, %Y")));
			m_besttimeslist.AddString(s);
		}
	}


	return TRUE;
}