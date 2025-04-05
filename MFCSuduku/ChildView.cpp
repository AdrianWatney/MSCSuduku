
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "MFCSuduku.h"
#include "ChildView.h"
#include "CNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView
#define BOTTOM_MARGIN	96
#define SFONTSIZE		48
#define BORDERSIZE		10

//	The constructor initializes various member variables, including the selected box, help flag, selected character, number of holes, timer count, game state, and wrong guess count.
CChildView::CChildView()
{
	m_selected_box = -1;	// no selected box.
	m_bhelp = FALSE;		// flag to display help
	m_selected_char = -1;	// no selected number
	m_num_holes = 50;		// 50 is a reasonable game.
	m_cnt = 0;				// count for timer
	m_state = DRAWBOARD;	// state machine for game
	m_wrongguess = 0;
	// initilise arrays 
	memset(m_suduko_board, 0, sizeof(m_suduko_board));
	memset(m_removedVals, 0, sizeof(m_removedVals));
	memset(m_box, 0, sizeof(m_box));
	memset(m_row, 0, sizeof(m_row));
	memset(m_col, 0, sizeof(m_col));
	memset(&m_ba, 0, sizeof(m_ba));
	memset(&m_newBall, 0, sizeof(m_newBall));
	m_hFont = NULL;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_HELP_SHOWHELP, &CChildView::OnHelpShowhelp)
	ON_COMMAND(ID_SUDOKU_NUMBEROFHOLES, &CChildView::OnSudokuNumberofholes)
END_MESSAGE_MAP()



// CChildView message handlers
//	This function is called before the window is created. It sets the window styles and registers the window class.
//	It also initializes the Sudoku board by calling InitBoard.
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);
	// create suduko board
	m_num_holes = AfxGetApp()->GetProfileInt(_T("Settings"), _T("NumberOfHoles"), 50);
	InitBoard();

	return TRUE;
}
// 	This function handles the painting of the window. It uses a state machine to determine what to draw based on the current game state.
//	It calls DrawBoard, RevealHoles, AnimateWin, and ShowStats as needed.
void CChildView::OnPaint() 
{
	int		cnt;

	CPaintDC dc(this); // device context for painting
	cnt = CountHolesLeft();
	switch (m_state)
	{
	case	DRAWBOARD:
		if (cnt < 9) {
			m_state = REVEALHOLES;
			SetTimer('NR', 400, 0);	// start animation	
			m_cnt = 100;			// animation timer count
			m_selected_box = -1;	// no selected box.
			m_selected_char = -1;	// no selected number
			m_bhelp = FALSE;		// flag to display help
		}
		DrawBoard(&dc);
		ShowStats(&dc);
		break;
	case	REVEALHOLES:
		RevealHoles(&dc);
		break;
	case	ANIMATEWIN:
		AnimateWin(&dc);
		if (m_cnt <= 0) {
			m_state = GAMEFINNISHED;
		}
		break;
	case	GAMEFINNISHED:
		DrawBoard(&dc);
		ShowStats(&dc);
		break;
	}
}
//	This function reveals the holes in the Sudoku board by filling in the removed values one by one.
//	If all holes are revealed, it transitions to the ANIMATEWIN state and sets up the win screen.
void CChildView::RevealHoles(CPaintDC* dc)
{
	int		cnt;
	int		i, x, y, w, h, sz, n;
	CRect	r, rr;

	m_selected_box = -1;	// no selected box.
	m_selected_char = -1;	// no selected number
	m_bhelp = FALSE;		// flag to display help
	cnt = CountHolesLeft();
	GetClientRect(&r);
	x = BORDERSIZE; y = BORDERSIZE;

	if (cnt > 0) {
		w = (r.right - r.left);
		h = (r.bottom - r.top);
		// calculate best drawing size
		if (w > h) sz = h;
		else sz = w;
		sz -= BOTTOM_MARGIN;
		// uncover holes
		for (i = 0; i < 81; i++) {
			if (m_suduko_board[i] == 0) {
				// found uncovered hole
				n = m_removedVals[i];		// get value of removed hole
				m_suduko_board[i] = n;		// fill in hole
				m_removedVals[i] = 0;		// remove hole from vals
				DrawBoard(dc);
				// Show stats
				ShowStats(dc);
				return;
			}
		}
	}
	else {
		m_state = ANIMATEWIN;
		SetupWinScreen(dc);		// Setup win screen
	}
}
//	This function handles the animation when the player wins the game.
//	It calls MoveWinScreen to move the win screen and ShowStats to display the game statistics.
void CChildView::AnimateWin(CPaintDC* dc)
{
	CRect	r;
	// drawing the win display
	GetClientRect(&r);
	MoveWinScreen(dc);
	ShowStats(dc);
}
// This function draws the Sudoku board on the screen.
// It uses various GDI objects like pens, brushes, and fonts to draw the grid, numbers, and highlights.
void CChildView::DrawBoard(CPaintDC* dc)
{
	int		n, h, w, sz, x, y, xx, yy, xn, yn;
	double	ww, wh;
	
	CRect	r;
	RECT	rr, rn;
	CPen	penBlack, penBlack2;
	CBrush	brushLLGray, brushLGray, brushLBlue;
	CString	s;
	HFONT	hFont, hOldFont;
	int		cnt = 0, fs;
	GetClientRect(&r);

	//FillRect(*dc, r, WHITE_BRUSH);	// clear screen

	w = (r.right - r.left);
	h = (r.bottom - r.top);
	// calculate best drawing size
	if (w > h) sz = h;
	else sz = w;
	sz -= BOTTOM_MARGIN;


	penBlack.CreatePen(PS_SOLID | PS_COSMETIC, 1, RGB(0, 0, 0));
	penBlack2.CreatePen(PS_SOLID | PS_COSMETIC, 3, RGB(0, 0, 0));
	brushLBlue.CreateSolidBrush(RGB(220, 255, 255));
	brushLGray.CreateSolidBrush(RGB(220, 220, 220));
	brushLLGray.CreateSolidBrush(RGB(212, 212, 212));
	//Logical units are device dependent pixels, so this will create a handle to a logical font that is 48 pixels in height.
	//The width, when set to 0, will cause the font mapper to choose the closest matching value.
	//The font face name will be Impact.
	hFont = CreateFont(SFONTSIZE, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	hOldFont = (HFONT)dc->SelectObject(hFont);

	CPen* pOldPen;
	pOldPen = dc->SelectObject(&penBlack);
	x = 10; y = 10;
	ww = sz / 9.0;

	// draw the text 
	for (yy = 0; yy < 9; yy++) {
		for (xx = 0; xx < 9; xx++) {
			wh = ww - 2;
			rr.left = x + int(xx * ww);
			rr.top = y + int(yy * ww);
			rr.right = x + int((xx * ww) + ww);
			rr.bottom = y + int((yy * ww) + ww);
			if (m_selected_char > 0 && m_suduko_board[xx + (yy * 9)] == m_selected_char) {
				dc->FillRect(&rr, &brushLBlue);
				dc->SetBkColor(RGB(220, 255, 255));
			}
			dc->SelectObject(hFont);
			s.Format(_T("%d"), m_suduko_board[xx + (yy * 9)]);
			fs = dc->DrawText(s, &rr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			dc->SetBkColor(RGB(255, 255, 255));
			if (m_suduko_board[xx + (yy * 9)] == 0) {
				// value not visable
				cnt++;
				rr.left += 2; rr.right -= 2; rr.top += 2; r.bottom -= 2;
				if (m_selected_box == xx + (yy * 9)) {
					// this square is selected
					dc->FillRect(&rr, &brushLBlue);
				}
				else
					dc->FillRect(&rr, WHITE_BRUSH); //dc->FillRect(&rr, &brushLBlue);
				dc->SelectObject(hOldFont);
				// if show help then find numbers that can go in this square
				// value not visable find possable missing value.
				// highlight any seleccted number
				// display in smaller rectangle
				if (m_bhelp) {
					for (n = 1; n < 10; n++) {
						if (safeToPlace(m_suduko_board, xx + (yy * 9), n)) {
							// draw in smaller square
							xn = (n - 1) % 3; yn = (n - 1) / 3;
							rn.left = rr.left + int(xn * (wh / 3));
							rn.top = rr.top + int(yn * (wh / 3));
							rn.right = rn.left + int(wh / 3);
							rn.bottom = rn.top + int(wh / 3);
							s.Format(_T("%d"), n);
							if (n == m_selected_char) {
								dc->FillRect(&rn, &brushLBlue);
								dc->SelectObject(&penBlack);
								dc->MoveTo(rn.left, rn.bottom);
								dc->LineTo(rn.right, rn.bottom);
								dc->LineTo(rn.right, rn.top);
								dc->LineTo(rn.left, rn.top);
								dc->LineTo(rn.left, rn.bottom);
								//dc->DrawEdge(&rn, EDGE_BUMP, BF_ADJUST);
								//dc->SetTextColor(RGB(182, 255, 0));
								dc->SetBkColor(RGB(220, 255, 255));
							}
							rn.left++; rn.right--; rn.top++; rn.bottom--;
							dc->DrawText(s, &rn, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							dc->SetTextColor(RGB(0, 0, 0));
							dc->SetBkColor(RGB(255, 255, 255));
						}
					}
				}

			}
		}
	}
	// Board thick lines reinforce
	dc->SelectObject(&penBlack2);
	dc->MoveTo(x, y);
	dc->LineTo(x + sz, y);
	dc->LineTo(x + sz, y + sz);
	dc->LineTo(x, y + sz);
	dc->LineTo(x, y);
	// verticals
	dc->SelectObject(&penBlack);
	for (xx = 1; xx < 9; xx++) {
		dc->MoveTo(x + int(xx * ww), y);
		dc->LineTo(x + int(xx * ww), y + sz);
	}
	// horizontals
	for (yy = 1; yy < 9; yy++) {
		dc->MoveTo(x, y + int(yy * ww));
		dc->LineTo(x + sz, y + int(yy * ww));
	}
	// box lines
	dc->SelectObject(&penBlack2);
	for (xx = 0; xx < 9; xx += 3) {
		dc->MoveTo(x + int(xx * ww), y);
		dc->LineTo(x + int(xx * ww), y + sz);
	}
	for (yy = 0; yy < 9; yy += 3) {
		dc->MoveTo(x, y + int(yy * ww));
		dc->LineTo(x + sz, y + int(yy * ww));
	}

	// Restore the previous pen.
	dc->SelectObject(pOldPen);

	dc->SelectObject(hOldFont);
	DeleteObject(hFont);
}
// This function counts the number of empty cells (holes) left on the Sudoku board.
int CChildView::CountHolesLeft()
{
	int		i;
	int		cnt = 0;
	// get count of holes left
	for (i = 0; i < 81; i++) {
		if (m_suduko_board[i] == 0) cnt++;
	}
	return cnt;
}
// This function displays the game statistics, such as the number of wrong guesses and the number of blanks left.
void CChildView::ShowStats(CPaintDC* dc)
{
	int		x, y, w, h, sz, cnt;
	RECT	rn;
	CRect	r;
	CString	s;
	GetClientRect(&r);
	w = (r.right - r.left);
	h = (r.bottom - r.top);
	// calculate best drawing size
	if (w > h) sz = h;
	else sz = w;
	sz -= BOTTOM_MARGIN;

	// get count of holes left
	cnt = CountHolesLeft();

	x = (BOTTOM_MARGIN / 2);
	y = sz + (BOTTOM_MARGIN / 2);
	rn.left = x;
	rn.top = y - 5;
	rn.right = rn.left + (w / 3);
	rn.bottom = rn.top + 24;
	s.Format(_T("Wrong guess: %d, blanks left:%d"), m_wrongguess, cnt);
	//dc->MoveTo(rn.left, rn.bottom);
	dc->DrawText(s, &rn, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}
// This function handles the timer events. It is used for animations, such as moving the win screen.
void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	CString	s;
	RECT	r;
	if (nIDEvent == 'NR')
	{	// image move
		if (m_cnt <= 0) {
			KillTimer(nIDEvent);
			InvalidateRect(NULL, TRUE);
			return;
		}

		GetClientRect(&r);
		InvalidateRect(&r, FALSE);
		m_cnt--;
	}
}
// MoveWinScreen is called every 1/10 second untill won display is finnished
// This function moves the win screen text around the window, bouncing off the edges.
void CChildView::MoveWinScreen(CPaintDC* dc) {
	int		x, y;
	CRect	r, rr;
	HFONT	hOldFont;
	CString	s;
	int		cnt = 0;
	GetClientRect(&r);
	x = BORDERSIZE; y = BORDERSIZE;
	// get count of holes left
	cnt = CountHolesLeft();

	// all holes are uncovered.
	m_hFont = CreateFont(SFONTSIZE * 2, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Times New Roman"));
	hOldFont = (HFONT)dc->SelectObject(m_hFont);
	dc->SelectObject(m_hFont);
	s = m_winstr;

	// Bit block transfer into our compatible memory DC.
	// undo the last win with the saved background.
	BitBlt(*dc, m_ba.rect.left, m_ba.rect.top,
		m_ba.rect.right - m_ba.rect.left, m_ba.rect.bottom - m_ba.rect.top,
		m_hdcMemDC,
		m_ba.rect.left, m_ba.rect.top, SRCCOPY);
	// update position
	OffsetRect(&m_ba.rect, m_ba.dX, m_ba.dY);
	if (m_ba.rect.right > r.right || m_ba.rect.left < 0)
	{
		m_ba.dX = -m_ba.dX;
	}
	if (m_ba.rect.bottom > r.bottom || m_ba.rect.top < 0)
	{
		m_ba.dY = -m_ba.dY;
	}
	// Set the background mode to transparent for the 
	// text-output operation.
	dc->SetBkMode(TRANSPARENT);	// draw text without changing background.
	dc->DrawText(s, &m_ba.rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc->SelectObject(hOldFont);
	dc->SetBkMode(OPAQUE);
}

void CChildView::SetupWinScreen(CPaintDC* dc) {
	int		x, y, w, h, fs, fh;
	CRect	r;

	HBITMAP hbmScreen = NULL;

	// Retrieve the handle to a display device context for the client 
   // area of the window. 


	// Create a compatible DC, which is used in a BitBlt from the window DC.
	m_hdcMemDC.DeleteDC();

	m_hdcMemDC.CreateCompatibleDC(dc);

	// Get the client area for size calculation.
	RECT rcClient;
	GetClientRect(&rcClient);

	// Create a compatible bitmap from the Window DC.
	hbmScreen = CreateCompatibleBitmap(dc->m_hDC, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

	// Select the compatible bitmap into the compatible memory DC.
	SelectObject(m_hdcMemDC, hbmScreen);

	// Bit block transfer screen into our compatible memory DC.
	BitBlt(m_hdcMemDC, 0, 0,
		rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
		*dc,
		0, 0, SRCCOPY);


	GetClientRect(&r);

	m_winstr = "You Won";
	fs = (SFONTSIZE * 2) * m_winstr.GetLength();		// number of charectors in win string
	fh = (SFONTSIZE * 2);
	w = r.right - r.left;
	h = r.bottom - r.top;
	x = rand() % (w / 2 - fs);
	y = rand() % (h / 2 - fh);
	//RECT newBall;
	m_newBall.left = x;
	m_newBall.top = y;
	m_newBall.right = m_newBall.left + fs;
	m_newBall.bottom = m_newBall.top + fh;
	m_ba.rect = m_newBall;
	m_ba.dX = SFONTSIZE;
	m_ba.dY = SFONTSIZE;

	m_cnt = 100;

	SetTimer('NR', 100, 0);		// image move	
}

void CChildView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int		n;
	if (nChar >= '1' && nChar <= '9' && m_selected_box >= 0) {
		// is this a value for our suduko board
		if (m_suduko_board[m_selected_box] == 0) {
			n = nChar - '0';
			if (m_removedVals[m_selected_box] == n) {
				m_suduko_board[m_selected_box] = n;
				m_removedVals[m_selected_box] = 0;
				m_selected_char = n;
				InvalidateRect(NULL, TRUE);
			}
			else {
				m_wrongguess++;
				InvalidateRect(NULL, TRUE);
			}
		}

	}
}

void CChildView::OnLButtonDown(UINT /* nFlags */, CPoint point)
{
	// Select the square
	int		h, w, sz, x, y, xx, yy;
	double	ww;
	CRect	r;
	RECT	rr;

	GetClientRect(&r);
	w = (r.right - r.left);
	h = (r.bottom - r.top);
	// calculate best drawing size
	if (w > h) sz = h;
	else sz = w;
	sz -= BOTTOM_MARGIN;

	x = BORDERSIZE; y = BORDERSIZE;
	ww = sz / 9.0;
	for (yy = 0; yy < 9; yy++) {
		for (xx = 0; xx < 9; xx++) {
			rr.left = x + int(xx * ww);
			rr.top = y + int(yy * ww);
			rr.right = x + int((xx * ww) + ww);
			rr.bottom = y + int((yy * ww) + ww);
			if (PtInRect(&rr, point)) {
				// Selected square
				m_selected_box = xx + (yy * 9);
				m_selected_char = m_suduko_board[m_selected_box];
				InvalidateRect(NULL, TRUE);
				return;
			}
		}
	}
}

// CSudukuView diagnostics


// CSudukuView message handlers
unsigned char numArray[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
unsigned char* CChildView::Shuffle(unsigned char* array)
{
	int		i;
	unsigned char* newArray;
	newArray = (unsigned char*)malloc(sizeof(numArray));
	if (newArray)
	{
		memcpy(newArray, numArray, sizeof(numArray));
		unsigned char c;
		for (i = sizeof(numArray) - 1; i > 0; i--) {
			int j = rand() % (i + 1); // Math.floor(Math.random() * (i + 1));
			//[newArray[i], newArray[j]] = [newArray[j], newArray[i]];
			// swap byte from array[i] with array[j]
			c = newArray[i];
			newArray[i] = newArray[j];
			newArray[j] = c;
		}
	}
	return newArray;
}

BOOL CChildView::rowSafe(unsigned char* puzzleArray, int y, char n)
{
	// check if number already exists in the row
	for (int x = 0; x < 9; x++) {
		if (puzzleArray[m_row[x][y]] == n)
			return FALSE;
	}
	return TRUE;
}

BOOL CChildView::colSafe(unsigned char* puzzleArray, int x, char n)
{
	// check if number already exists in the row
	for (int y = 0; y < 9; y++) {
		if (puzzleArray[m_row[x][y]] == n)
			return FALSE;
	}
	return TRUE;
}

BOOL CChildView::boxSafe(unsigned char* puzzleArray, int x, int y, char n)
{
	int	i, xx, yy;
	for (i = 0; i < 9; i++) {
		xx = x / 3; yy = y / 3;
		if (puzzleArray[m_box[xx][yy][i]] == n)
			return FALSE;
	}
	return TRUE;
}

BOOL CChildView::IsNumberGood(int x, int y, char n)
{
	BOOL ret = FALSE;
	if (n > 0 && m_suduko_board[x + (y * 9)] == 0) {	// currently has no number
		if (rowSafe(m_suduko_board, y, n)) {
			if (colSafe(m_suduko_board, x, n)) {
				if (boxSafe(m_suduko_board, x, y, n)) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

BOOL CChildView::InitBoard()
{
	int		i, x, y;
	unsigned char* startingBoard;
	BOOL	ret = FALSE;
	time_t	t;
	memset((void*)&m_suduko_board, 0, sizeof(m_suduko_board));	// clear suduko board
	// setup pointers to m_suduko_board
	for (y = 0; y < 9; y++) {
		for (x = 0; x < 9; x++) {
			m_row[x][y] = (y * 9) + x;
		}
	}
	for (y = 0; y < 3; y++) {
		for (x = 0; x < 3; x++) {
			for (i = 0; i < 3; i++) {
				m_box[x][y][i] = (y * 9 * 3) + (x * 3) + i;
				m_box[x][y][i + 3] = (y * 9 * 3) + (x * 3) + i + 9;
				m_box[x][y][i + 6] = (y * 9 * 3) + (x * 3) + i + 18;
			}
		}
	}
	time(&t);
	srand((unsigned int)t);	// seed random number generator
	startingBoard = fillPuzzle(m_suduko_board);
	memcpy(m_suduko_board, startingBoard, sizeof(m_suduko_board));
	pokeHoles(m_suduko_board, m_num_holes);		// fill m_removedVals 0-81 holes 

	free(startingBoard);
	return ret;
}

int CChildView::nextEmptyCell(unsigned char* startingBoard)
{
	int		i;
	for (i = 0; i < 81; i++) {
		if (startingBoard[i] == 0) {
			return i;
		}
	}
	return -1;
}

BOOL CChildView::safeToPlace(unsigned char* puzzleArray, int emptyCell, char num)
{
	int x, y;
	y = emptyCell / 9;
	x = emptyCell - (y * 9);
	return rowSafe(puzzleArray, y, num) &&
		colSafe(puzzleArray, x, num) &&
		boxSafe(puzzleArray, x, y, num);
}

unsigned char* CChildView::fillPuzzle(unsigned char* startingBoard)
{
	int		i;
	char	n;
	int	emptyCell;
	unsigned char* newArray;
	unsigned char* nextBoard;
	unsigned char* newBoard;
	emptyCell = nextEmptyCell(startingBoard);
	if (emptyCell < 0)
		return startingBoard;	// No more empty cells
	// Shuffled [0 - 9 ] array fills board randomly each pass
	newArray = Shuffle(numArray);	// returns allocated newarray
	nextBoard = (unsigned char*)malloc(sizeof(m_suduko_board));
	if (nextBoard)
	{
		memcpy(nextBoard, startingBoard, sizeof(m_suduko_board));
		for (i = 0; i < 9; i++) {
			n = newArray[i];	// n = Randon (1..9)
			if (safeToPlace(nextBoard, emptyCell, n)) {
				nextBoard[emptyCell] = n; // If safe to place number, place it
				// Recursively call the fill function to place num in next empty cell
				if (newBoard = fillPuzzle(nextBoard)) {
					free(newArray);
					if (newBoard != nextBoard) {
						free(nextBoard);
					}
					return newBoard;
				}
				// If we were unable to place the future num, that num was wrong. Reset it and try next value
				nextBoard[emptyCell] = 0;
			}
		}
		free(nextBoard);
	}
	free(newArray);	// free allocated newarray
	return FALSE;
}

char* CChildView::pokeHoles(unsigned char* startingBoard, int holes)
{
	int				cnt;
	unsigned char	val;
	unsigned char	proposedBoard[81];  // Clone this changed board
	unsigned char* newboard;
	memcpy(m_removedVals, startingBoard, sizeof(m_removedVals));
	cnt = 0;
	while (cnt < holes) {
		val = rand() % 81; // Value between 0 - 81
		if (startingBoard[val] == 0)
			continue; // If cell already empty, restart loop
		m_removedVals[val] = startingBoard[val];  // Store the current value at the coordinates
		startingBoard[val] = 0; // "poke a hole" in the board at the coords
		memcpy(proposedBoard, startingBoard, sizeof(proposedBoard));
		// Attempt to solve the board after removing value. If it cannot be solved, restore the old value.
		// and remove that option from the list
		newboard = fillPuzzle(proposedBoard);
		if (!newboard) {
			startingBoard[val] = m_removedVals[val];
			m_removedVals[val] = 0;
		}
		else {
			if (newboard != proposedBoard)
				free(newboard);
			cnt++;
		}
	}
	return NULL;
}


void CChildView::OnHelpShowhelp()
{
	if (m_bhelp) m_bhelp = FALSE;
	else m_bhelp = TRUE;
	InvalidateRect(NULL, TRUE);
}


void CChildView::OnSudokuNumberofholes()
{
	CNumber		DialogNumber;
	DialogNumber.m_numberofholes = m_num_holes;
	if (DialogNumber.DoModal() == IDOK)
	{
		m_num_holes = DialogNumber.m_numberofholes;
		AfxGetApp()->WriteProfileInt(_T("Settings"), _T("NumberOfHoles"), m_num_holes);
		InitBoard();
		m_state = DRAWBOARD;
		m_wrongguess = 0;
		InvalidateRect(NULL, TRUE);
	}
}


