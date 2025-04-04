
// ChildView.h : interface of the CChildView class
//


#pragma once
// Suduku is a game of 9 boxes in a square and 9 squares of boxes.Each box has a number between 1 - 9 with no duplicates, and the numbers along the side of 3 boxes have no duplicates.
struct Ball
{
	RECT rect;
	int dX;
	int dY;
};
// use a state machine for drawing and winning screen animation.
enum SUDOKO_STATE {
	DRAWBOARD = 1,
	REVEALHOLES,
	ANIMATEWIN,
	GAMEFINNISHED
};

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:
	unsigned char m_suduko_board[81];
	unsigned char m_removedVals[81];
	unsigned char m_box[3][3][9];		// index into m_suduko_board for boxes
	unsigned char m_row[9][9];		// index into m_suduko_board for rows
	unsigned char m_col[9][9];		// index into m_suduko_board for columns
	int			  m_selected_box;
	BOOL		  m_bhelp;
	unsigned char m_selected_char;	// if not empty box selected then this in the number selected
	int			  m_num_holes;
	CDC			  m_hdcMemDC;		// used for the win screen animation
	HFONT		  m_hFont;
	RECT		  m_newBall;
	Ball		  m_ba;
	int			  m_cnt;			// timer count for animation
	SUDOKO_STATE  m_state;
	int			  m_wrongguess;		// number of times the gammer guesed wrong
	CString		  m_winstr;			// winning string.

// Operations
public:
	BOOL InitBoard();
	BOOL IsNumberGood(int x, int y, char n);
	unsigned char* fillPuzzle(unsigned char* startingBoard);
	unsigned char* Shuffle(unsigned char* array);
	BOOL rowSafe(unsigned char* puzzleArray, int y, char n);
	BOOL colSafe(unsigned char* puzzleArray, int x, char n);
	BOOL boxSafe(unsigned char* puzzleArray, int x, int y, char n);
	int  nextEmptyCell(unsigned char* startingBoard);
	BOOL safeToPlace(unsigned char* puzzleArray, int emptyCell, char num);
	char* pokeHoles(unsigned char* startingBoard, int holes);
	void SetupWinScreen(CPaintDC* dc);
	void MoveWinScreen(CPaintDC* dc);
	void ShowStats(CPaintDC* dc);
	void DrawBoard(CPaintDC* dc);
	void RevealHoles(CPaintDC* dc);
	void AnimateWin(CPaintDC* dc);
	int  CountHolesLeft();

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHelpShowhelp();
	afx_msg void OnSudokuNumberofholes();
};

