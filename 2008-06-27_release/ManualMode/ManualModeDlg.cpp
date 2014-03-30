// ManualModeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ManualMode.h"
#include "ManualModeDlg.h"
#include "poker_defs.h"
#include "debug.h"
#include "registry.h"
#include "EditDlg.h"
#include "GameInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Startup path
char	startup_path[MAX_PATH];

// Colors
// (more color codes here: http://htmlhelp.com/cgi-bin/color.cgi)
#define CARDSIZEX		36
#define	CARDSIZEY		50
#define	COLOR_RED		RGB(255,0,0)
#define	COLOR_BLACK		RGB(0,0,0)
#define	COLOR_BLUE		RGB(0,0,255)
#define	COLOR_GREEN		RGB(0,255,0)
#define	COLOR_WHITE		RGB(255,255,255)
#define COLOR_GRAY		RGB(182,182,182)
#define COLOR_YELLOW	RGB(255,255,0)

// Table layouts
int		cc[5][2] = { {-(CARDSIZEX*2 + 3*2 + CARDSIZEX/2), -(CARDSIZEY/2)},	// absolutes
					 {-(CARDSIZEX*1 + 3*1 + CARDSIZEX/2), -(CARDSIZEY/2)}, 
					 {-(CARDSIZEX*0 + 3*0 + CARDSIZEX/2), -(CARDSIZEY/2)}, 
					 {+(CARDSIZEX*0 + 3*1 + CARDSIZEX/2), -(CARDSIZEY/2)}, 
					 {+(CARDSIZEX*1 + 3*2 + CARDSIZEX/2), -(CARDSIZEY/2)} 
				   }; 

// Player locations as a percentage of width/height
// [chairnum][x/y]
double	pc[10][2] = { {.68,.11}, {.83,.21}, {.93,.47}, {.83,.73}, {.68,.83}, 
					  {.32,.83}, {.17,.73}, {.07,.47}, {.17,.21}, {.32,.11} };
	
// Player bet locations relative to player locations above
// numbers are in pixel units
// [chairnum][x/y]
int pcbet[10][2] = { {-40,+53}, {-40,+37}, {-40,+0}, {-40,-20}, {-40,-40}, 
					 {+40,-40}, {+40,-20}, {+40,+0}, {+40,+37}, {+40,+53} };

// Dealer button locations relative to player locations above
// numbers are in pixel units
// [chairnum][x/y]
int dbutn[10][2] = { {-60,+70}, {-60,+54}, {-60,+17}, {-60,-37}, {-60,-57}, 
					 {+60,-57}, {+60,-37}, {+60,+17}, {+60,+54}, {+60,+70} };

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog {
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CManualModeDlg dialog

CManualModeDlg::CManualModeDlg(CWnd* pParent /*=NULL*/) : CDialog(CManualModeDlg::IDD, pParent) {
#ifdef SEH_ENABLE
	// Set exception handler
	SetUnhandledExceptionFilter(MyUnHandledExceptionFilter);
#endif

#ifdef SEH_ENABLE
	try {
#endif
		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

		black_pen.CreatePen(PS_SOLID, 1, COLOR_BLACK);
		green_pen.CreatePen(PS_SOLID, 1, COLOR_GREEN);
		red_pen.CreatePen(PS_SOLID, 1, COLOR_RED);
		blue_pen.CreatePen(PS_SOLID, 1, COLOR_BLUE);
		white_pen.CreatePen(PS_SOLID, 1, COLOR_WHITE);
		white_dot_pen.CreatePen(PS_DOT, 1, COLOR_WHITE);
		null_pen.CreatePen(PS_NULL, 0, COLOR_BLACK);

		white_brush.CreateSolidBrush(COLOR_WHITE);
		gray_brush.CreateSolidBrush(COLOR_GRAY);
		red_brush.CreateSolidBrush(COLOR_RED);
		yellow_brush.CreateSolidBrush(COLOR_YELLOW);

		lf.lfHeight = -12;
		lf.lfWeight = FW_NORMAL;
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfItalic = 0;
		lf.lfUnderline = 0;
		lf.lfStrikeOut = 0;
		lf.lfCharSet = 0;
		lf.lfOutPrecision = 0;
		lf.lfClipPrecision = 0;
		lf.lfQuality = PROOF_QUALITY;
		lf.lfPitchAndFamily = 0;
		strcpy(lf.lfFaceName, "Verdana");
		cFont.CreateFontIndirect(&lf);

		all_cards.LoadBitmap(IDB_CARDS);

		// Save startup directory
		::GetCurrentDirectory(sizeof(startup_path) - 1, startup_path);


#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::CManualModeDlg\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
}

BOOL CManualModeDlg::DestroyWindow() {
	Registry		reg;
	WINDOWPLACEMENT wp;

	// Save window position
	reg.read_reg();
	GetWindowPlacement(&wp);
	reg.manual_x = wp.rcNormalPosition.left;
	reg.manual_y = wp.rcNormalPosition.top;
	reg.unobstructivePopup = dlgOptions.m_unobstructivePopup;
	reg.write_reg();

	all_cards.DeleteObject();
	cFont.DeleteObject();

	return CDialog::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CManualModeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_2_CLUBS, &CManualModeDlg::On2Clubs)
	ON_COMMAND(ID_2_DIAMONDS, &CManualModeDlg::On2Diamonds)
	ON_COMMAND(ID_2_HEARTS, &CManualModeDlg::On2Hearts)
	ON_COMMAND(ID_2_SPADES, &CManualModeDlg::On2Spades)
	ON_COMMAND(ID_3_CLUBS, &CManualModeDlg::On3Clubs)
	ON_COMMAND(ID_3_DIAMONDS, &CManualModeDlg::On3Diamonds)
	ON_COMMAND(ID_3_HEARTS, &CManualModeDlg::On3Hearts)
	ON_COMMAND(ID_3_SPADES, &CManualModeDlg::On3Spades)
	ON_COMMAND(ID_4_CLUBS, &CManualModeDlg::On4Clubs)
	ON_COMMAND(ID_4_DIAMONDS, &CManualModeDlg::On4Diamonds)
	ON_COMMAND(ID_4_HEARTS, &CManualModeDlg::On4Hearts)
	ON_COMMAND(ID_4_SPADES, &CManualModeDlg::On4Spades)
	ON_COMMAND(ID_5_CLUBS, &CManualModeDlg::On5Clubs)
	ON_COMMAND(ID_5_DIAMONDS, &CManualModeDlg::On5Diamonds)
	ON_COMMAND(ID_5_HEARTS, &CManualModeDlg::On5Hearts)
	ON_COMMAND(ID_5_SPADES, &CManualModeDlg::On5Spades)
	ON_COMMAND(ID_6_CLUBS, &CManualModeDlg::On6Clubs)
	ON_COMMAND(ID_6_DIAMONDS, &CManualModeDlg::On6Diamonds)
	ON_COMMAND(ID_6_HEARTS, &CManualModeDlg::On6Hearts)
	ON_COMMAND(ID_6_SPADES, &CManualModeDlg::On6Spades)
	ON_COMMAND(ID_7_CLUBS, &CManualModeDlg::On7Clubs)
	ON_COMMAND(ID_7_DIAMONDS, &CManualModeDlg::On7Diamonds)
	ON_COMMAND(ID_7_HEARTS, &CManualModeDlg::On7Hearts)
	ON_COMMAND(ID_7_SPADES, &CManualModeDlg::On7Spades)
	ON_COMMAND(ID_8_CLUBS, &CManualModeDlg::On8Clubs)
	ON_COMMAND(ID_8_DIAMONDS, &CManualModeDlg::On8Diamonds)
	ON_COMMAND(ID_8_HEARTS, &CManualModeDlg::On8Hearts)
	ON_COMMAND(ID_8_SPADES, &CManualModeDlg::On8Spades)
	ON_COMMAND(ID_9_CLUBS, &CManualModeDlg::On9Clubs)
	ON_COMMAND(ID_9_DIAMONDS, &CManualModeDlg::On9Diamonds)
	ON_COMMAND(ID_9_HEARTS, &CManualModeDlg::On9Hearts)
	ON_COMMAND(ID_9_SPADES, &CManualModeDlg::On9Spades)
	ON_COMMAND(ID_T_CLUBS, &CManualModeDlg::OnTClubs)
	ON_COMMAND(ID_T_DIAMONDS, &CManualModeDlg::OnTDiamonds)
	ON_COMMAND(ID_T_HEARTS, &CManualModeDlg::OnTHearts)
	ON_COMMAND(ID_T_SPADES, &CManualModeDlg::OnTSpades)
	ON_COMMAND(ID_J_CLUBS, &CManualModeDlg::OnJClubs)
	ON_COMMAND(ID_J_DIAMONDS, &CManualModeDlg::OnJDiamonds)
	ON_COMMAND(ID_J_HEARTS, &CManualModeDlg::OnJHearts)
	ON_COMMAND(ID_J_SPADES, &CManualModeDlg::OnJSpades)
	ON_COMMAND(ID_Q_CLUBS, &CManualModeDlg::OnQClubs)
	ON_COMMAND(ID_Q_DIAMONDS, &CManualModeDlg::OnQDiamonds)
	ON_COMMAND(ID_Q_HEARTS, &CManualModeDlg::OnQHearts)
	ON_COMMAND(ID_Q_SPADES, &CManualModeDlg::OnQSpades)
	ON_COMMAND(ID_K_CLUBS, &CManualModeDlg::OnKClubs)
	ON_COMMAND(ID_K_DIAMONDS, &CManualModeDlg::OnKDiamonds)
	ON_COMMAND(ID_K_HEARTS, &CManualModeDlg::OnKHearts)
	ON_COMMAND(ID_K_SPADES, &CManualModeDlg::OnKSpades)
	ON_COMMAND(ID_A_CLUBS, &CManualModeDlg::OnAClubs)
	ON_COMMAND(ID_A_DIAMONDS, &CManualModeDlg::OnADiamonds)
	ON_COMMAND(ID_A_HEARTS, &CManualModeDlg::OnAHearts)
	ON_COMMAND(ID_A_SPADES, &CManualModeDlg::OnASpades)
	ON_COMMAND(ID_CARDBACKS, &CManualModeDlg::OnCardBacks)
	ON_COMMAND(ID_NOCARDS, &CManualModeDlg::OnNoCards)
	ON_COMMAND(ID_NOCARD, &CManualModeDlg::OnNoCard)
	ON_COMMAND(ID_SEATPLAYER, &CManualModeDlg::OnSeatPlayer)
	ON_COMMAND(ID_SITIN, &CManualModeDlg::OnSitInPlayer)
	ON_COMMAND(ID_DEALERHERE, &CManualModeDlg::OnDealerHere)
	ON_COMMAND(ID_FOLD, &CManualModeDlg::OnFold)
	ON_COMMAND(ID_CALL, &CManualModeDlg::OnCall)
	ON_COMMAND(ID_RAISE, &CManualModeDlg::OnRaise)
	ON_COMMAND(ID_ALLIN, &CManualModeDlg::OnAllin)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_RESET, &CManualModeDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_PMINUS, &CManualModeDlg::OnBnClickedPminus)
	ON_BN_CLICKED(IDC_PPLUS, &CManualModeDlg::OnBnClickedPplus)
	ON_BN_CLICKED(IDC_MACRO, &CManualModeDlg::OnBnClickedMacro)
	ON_BN_CLICKED(IDC_DMINUS, &CManualModeDlg::OnBnClickedDminus)
	ON_BN_CLICKED(IDC_DPLUS, &CManualModeDlg::OnBnClickedDplus)
END_MESSAGE_MAP()

// CManualModeDlg message handlers
void CManualModeDlg::clear_scrape_areas(void) {
#ifdef SEH_ENABLE
	try {
#endif
		int i;

		for (i=0; i<5; i++) { 
			card[CC0+i] = CARD_NOCARD;
		}
		for (i=0; i<=9; i++) { 
			card[P0C0+i*2] = card[P0C1+i*2] = CARD_NOCARD; 
			seated[i] = active[i] = dealer[i] = false;
			playerbet[i] = "0";
			playerbalance[i] = "1000";
			buttonstate[i] = false;
			pot[i] = 0;
		}
		dealer[0] = true;

		playername[0] = "Player0";
		playername[1] = "Player1";
		playername[2] = "Player2";
		playername[3] = "Player3";
		playername[4] = "Player4";
		playername[5] = "Player5";
		playername[6] = "Player6";
		playername[7] = "Player7";
		playername[8] = "Player8";
		playername[9] = "Player9";

		//istournament = false;

		//handnumber = "1";
		//sblind = "5";
		//bblind = "10";
		//bbet = "20";
		//ante = "0";
		//limit = LIMIT_FL;

		ncommoncards_last = 0;

		CardMask_RESET(used_cards);

#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::clear_scrape_areas :\n"); 
		throw;
	}
#endif
}

BOOL CManualModeDlg::OnInitDialog() {
	Registry	reg;
	int			max_x, max_y;

	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strOptions;
		CString strAboutMenu;

		strOptions.LoadString(IDS_OPTIONS);
		strAboutMenu.LoadString(IDS_ABOUTBOX);

		if (!strOptions.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_OPTIONS, strOptions);
		}
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	SetWindowPos(NULL, 0, 0, MM_WIDTH, MM_HEIGHT, SWP_NOMOVE);

	// Restore window location and size
	reg.read_reg();
	max_x = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
	max_y = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON);
	SetWindowPos(NULL, min(reg.manual_x, max_x), min(reg.manual_y, max_y), MM_WIDTH, MM_HEIGHT, SWP_NOCOPYBITS);

	dlgOptions.m_unobstructivePopup = reg.unobstructivePopup;

	// Get last used macro
	macro_text = reg.macro;

	clear_scrape_areas();
	istournament = false;
	handnumber = "1";
	sblind = "5";
	bblind = "10";
	bbet = "20";
	ante = "0";
	limit = LIMIT_FL;
	network = "Stars";

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CManualModeDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID  == IDM_OPTIONS){
		dlgOptions.DoModal();
	}
	else {
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CManualModeDlg::OnPaint() {
	CPaintDC dc(this); // device context for painting

	if (IsIconic()) {

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();

		RECT		cr;
		int			i;

		// Get size of current client window
		GetClientRect(&cr);

		// Set background color (light gray)
		CBrush* pOldBrush = dc.SelectObject(&gray_brush);
		dc.PatBlt(cr.left, cr.top, cr.right-cr.left, cr.bottom-cr.top, PATCOPY);
		dc.SelectObject(pOldBrush);

		// Draw button state indicators
		draw_button_indicators();

		// Draw center info box
		draw_center_info_box();

		int shift = 0;
		if (card[CC0+0] == CARD_NOCARD || card[CC0+1] == CARD_NOCARD || card[CC0+2] == CARD_NOCARD)
			shift = 10;
		else if (card[CC0+3] == CARD_NOCARD && card[CC0+4] != CARD_NOCARD)
			shift = 10;

		// Draw common cards
		for (i=0; i<=4; i++) {
			draw_card(card[CC0+i], cr.right/2 + cc[i][0] , cr.bottom/2 + cc[i][1], shift);
		}

		// Draw collection of player info
		for (i=0; i<=9; i++) {
			// Draw active circle
			if (seated[i]) { draw_seated_active_circle(i); }
				
			// Draw player cards
			draw_card(card[P0C0+i*2], cr.right * pc[i][0] - CARDSIZEX - 2, cr.bottom * pc[i][1] - CARDSIZEY/2 - 5);
			draw_card(card[P0C1+i*2], cr.right * pc[i][0] + 1, cr.bottom * pc[i][1] - CARDSIZEY/2 - 5);
			
			// Draw dealer button
			if (dealer[i]) { draw_dealer_button(i); }

			// Draw name and balance boxes
			draw_name_box(i);
			draw_balance_box(i);

			// Draw player bet
			draw_player_bet(i);

			// Draw title text
			CString t;
			t.Format("OpenHoldem Poker - %s%s - blinds %s/%s - ante %s",
				(limit == LIMIT_NL ? "No Limit" : limit == LIMIT_PL ? "Pot Limit" : limit == LIMIT_FL ? "Fixed Limit" : "?L"), 
				(istournament ? " tourney" : ""), 
				sblind, bblind, ante);
			SetWindowText(t);

		}

	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CManualModeDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

void CManualModeDlg::draw_button_indicators(void) {
#ifdef SEH_ENABLE
	try {
#endif
		bool		fold_drawn, call_drawn, check_drawn, raise_drawn, allin_drawn;
		RECT		cr;

		// Get size of current client window
		GetClientRect(&cr);

		fold_drawn = call_drawn = check_drawn = raise_drawn = allin_drawn = false;
		if (buttonstate[0]) {
			draw_specific_button_indicator(0, 'F', cr.right-84, cr.bottom-16, cr.right-70, cr.bottom-2);
			fold_drawn = true;
		}
		if (buttonstate[1]) {
			draw_specific_button_indicator(1, 'C', cr.right-67, cr.bottom-16, cr.right-53, cr.bottom-2);
			call_drawn = true;
		}
		if (buttonstate[2]) {
			draw_specific_button_indicator(2, 'K', cr.right-50, cr.bottom-16, cr.right-36, cr.bottom-2);
			check_drawn = true;
		}
		if (buttonstate[3]) {
			draw_specific_button_indicator(3, 'R', cr.right-33, cr.bottom-16, cr.right-19, cr.bottom-2);
			raise_drawn = true;
		}
		if (buttonstate[4]) {
			draw_specific_button_indicator(4, 'A', cr.right-16, cr.bottom-16, cr.right-2, cr.bottom-2);
			allin_drawn = true;
		}

		if (!fold_drawn) {
			draw_specific_button_indicator(-1, 'F', cr.right-84, cr.bottom-16, cr.right-70, cr.bottom-2);
		}
		if (!call_drawn) {
			draw_specific_button_indicator(-1, 'C', cr.right-67, cr.bottom-16, cr.right-53, cr.bottom-2);
		}
		if (!check_drawn) {
			draw_specific_button_indicator(-1, 'K', cr.right-50, cr.bottom-16, cr.right-36, cr.bottom-2);
		}
		if (!raise_drawn) {
			draw_specific_button_indicator(-1, 'R', cr.right-33, cr.bottom-16, cr.right-19, cr.bottom-2);
		}
		if (!allin_drawn) {
			draw_specific_button_indicator(-1, 'A', cr.right-16, cr.bottom-16, cr.right-2, cr.bottom-2);
		}
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_specific_button_indicators\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::draw_specific_button_indicator(int button_num, char ch, int left, int top, int right, int bottom) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		RECT		rect;
		CFont		*oldfont;
		CString		t;
		CDC			*pDC = GetDC();

		// Set font basics
		oldfont = pDC->SelectObject(&cFont);

		// Background color
		pDC->SetBkColor(COLOR_GRAY);

		if (button_num == -1) {
			pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
			pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
			pDC->SetTextColor(COLOR_WHITE);
		}
		else {
			if (buttonstate[button_num]) {
				if (ch=='F') {
					pTempPen = (CPen*)pDC->SelectObject(&red_pen);
					pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
					pDC->SetTextColor(COLOR_RED);
				}
				else if (ch=='C') {
					pTempPen = (CPen*)pDC->SelectObject(&blue_pen);
					pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
					pDC->SetTextColor(COLOR_BLUE);
				}
				else if (ch=='K') {
					pTempPen = (CPen*)pDC->SelectObject(&blue_pen);
					pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
					pDC->SetTextColor(COLOR_BLUE);
				}
				else if (ch=='R') {
					pTempPen = (CPen*)pDC->SelectObject(&green_pen);
					pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
					pDC->SetTextColor(COLOR_GREEN);
				}
				else if (ch=='A') {
					pTempPen = (CPen*)pDC->SelectObject(&black_pen);
					pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
					pDC->SetTextColor(COLOR_BLACK);
				}
			}
			else {
				pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
				pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
				pDC->SetTextColor(COLOR_WHITE);
			}
		}

		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(left, top, right, bottom);

		// Set rectangle
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;

		t.Format("%c", ch);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(t.GetString(), t.GetLength(), &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		pDC->SelectObject(oldfont);
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_specific_button_indicator\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::draw_center_info_box(void) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		RECT		rect;
		CString		t, s;
		CFont		*oldfont;
		RECT		cr;
		int			left, top, right, bottom;
		CDC			*pDC = GetDC();
		
		// Get size of current client window
		GetClientRect(&cr);

		// Figure placement of box
		left = cr.right/2-60;
		top = 4;
		right = cr.right/2+60;
		bottom = 78;

		pTempPen = (CPen*)pDC->SelectObject(&black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(left, top, right, bottom);

		// Set font basics
		oldfont = pDC->SelectObject(&cFont);
		pDC->SetTextColor(COLOR_BLACK);

		// Set rectangle
		rect.left = left;
		rect.top = top;
		rect.right = right;
		rect.bottom = bottom;

		t = "";
		// handnumber
		s.Format(" Hand #: %s\n", handnumber);
		t.Append(s);

		// blinds/type
		s.Format(" %s%s %s/%s/%s\n", 
				(limit == LIMIT_NL ? "NL" : limit == LIMIT_PL ? "PL" : limit == LIMIT_FL ? "FL" : "?L"), 
				(istournament ? "T" : ""), 
				sblind, bblind, bbet);
		t.Append(s);

		// ante
		s.Format(" Ante: %s\n", ante);
		t.Append(s);

		// Pot
		if (pot[0] != (int) pot[0]) {
			s.Format(" Pot: %.2f\n", pot[0]);
		}
		else {
			s.Format(" Pot: %.0f\n", pot[0]);
		}
		t.Append(s);

		s.Format(" Net: %s\n", network);
		t.Append(s);

		// Draw it
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(t.GetString(), t.GetLength(), &rect, NULL);

		// Restore original pen, brush and font
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		pDC->SelectObject(oldfont);
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_center_info_box\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::draw_seated_active_circle(int chair) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		RECT		cr;
		int			left, top, right, bottom;
		CDC			*pDC = GetDC();
		
		// Get size of current client window
		GetClientRect(&cr);

		// Background color
		pDC->SetBkColor(COLOR_GRAY);

		// Figure placement of circle
		left = cr.right * pc[chair][0] - CARDSIZEX - 4;
		top = cr.bottom * pc[chair][1] - CARDSIZEX - 3;
		right = cr.right * pc[chair][0] + CARDSIZEX + 3;
		bottom = cr.bottom * pc[chair][1] + CARDSIZEX + 3;

		pTempPen = (CPen*)pDC->SelectObject(&black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen

		if (active[chair]) {
			pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
		}
		else {
			pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
		}
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->Ellipse(left, top, right, bottom);

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_seated_active_circle\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::draw_dealer_button(int chair) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		RECT		cr;
		int			left, top, right, bottom;
		CDC			*pDC = GetDC();
		
		// Get size of current client window
		GetClientRect(&cr);

		// Background color
		pDC->SetBkColor(COLOR_GRAY);

		// Figure placement of circle
		left = cr.right * pc[chair][0] + dbutn[chair][0] - 8;
		top = cr.bottom * pc[chair][1] + dbutn[chair][1] - 8;
		right = cr.right * pc[chair][0] + dbutn[chair][0] + 8;
		bottom = cr.bottom * pc[chair][1] + dbutn[chair][1] + 8;

		pTempPen = (CPen*)pDC->SelectObject(&black_pen);
		oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
		pTempBrush = (CBrush*)pDC->SelectObject(&red_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		pDC->Ellipse(left, top, right, bottom);

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_dealer_button\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::draw_card(unsigned int card, int left, int top, int shift) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		CDC			*pDC = GetDC();
		CDC			compatDC;
		CBitmap		one_card;
		int			suitoffset, rankoffset;

		// Setup DCs
		one_card.CreateCompatibleBitmap(pDC, CARDSIZEX, CARDSIZEY);
		compatDC.CreateCompatibleDC(pDC);
		compatDC.SelectObject(all_cards);
		pDC->SelectObject(one_card);

		// CARD BACK
		if (card == CARD_BACK) {
			pDC->BitBlt(left, top, CARDSIZEX, CARDSIZEY, &compatDC, 0, CARDSIZEY*4, SRCCOPY);
		}

		// NO CARD
		else if (card == CARD_NOCARD) {
			pDC->SetBkColor(COLOR_GRAY);

			pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
			oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
			pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
			oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

			pDC->SetBkMode(OPAQUE);
			pDC->RoundRect(left, top, left+CARDSIZEX, top+CARDSIZEY, 5, 5);

			pDC->SelectObject(oldpen);
			pDC->SelectObject(oldbrush);	
		}

		// NORMAL CARD
		else {
			rankoffset = (12-StdDeck_RANK(card)) * CARDSIZEX;
			suitoffset = StdDeck_SUIT(card) == Suit_SPADES ? CARDSIZEY*0 :
						 StdDeck_SUIT(card) == Suit_HEARTS ? CARDSIZEY*1 :
						 StdDeck_SUIT(card) == Suit_DIAMONDS ? CARDSIZEY*2 :
						 StdDeck_SUIT(card) == Suit_CLUBS ? CARDSIZEY*3 : 0;
			pDC->BitBlt(left, top + shift, CARDSIZEX, CARDSIZEY, &compatDC, rankoffset, suitoffset, SRCCOPY);
		}

		one_card.DeleteObject();
		compatDC.DeleteDC();
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_card\n"); 
		throw;
	}
#endif
}


void CManualModeDlg::draw_name_box(int chair) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		RECT		textrect, drawrect;
		CFont		*oldfont;
		RECT		cr;
		int			left, top, right, bottom;
		CDC			*pDC = GetDC();

		// Get size of current client window
		GetClientRect(&cr);

		// Background color
		pDC->SetBkColor(COLOR_GRAY);

		// Set font basics
		oldfont = pDC->SelectObject(&cFont);
		pDC->SetTextColor(COLOR_BLACK);

		// Figure placement of box
		left = cr.right * pc[chair][0] - 36;
		top = cr.bottom * pc[chair][1] + 15;
		right = cr.right * pc[chair][0] + 35;
		bottom = cr.bottom * pc[chair][1] + 30;

		if (seated[chair] || active[chair]) {
			pTempPen = (CPen*)pDC->SelectObject(&black_pen);
			oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
			pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
			oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

			// Calc rectangle size for text
			textrect.left = 0;
			textrect.top = 0;
			textrect.right = 0;
			textrect.bottom = 0;
			pDC->DrawText(playername[chair].GetString(), playername[chair].GetLength(), &textrect, DT_CALCRECT);

			// Figure out placement of rectangle
			drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
			drawrect.top = top;
			drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
			drawrect.bottom = bottom;

			// Draw it		
			pDC->SetBkMode(OPAQUE);
			pDC->Rectangle(drawrect.left, drawrect.top, drawrect.right, drawrect.bottom+2);
			pDC->SetBkMode(TRANSPARENT);
			pDC->DrawText(playername[chair].GetString(), playername[chair].GetLength(), &drawrect, 
				DT_CENTER | DT_SINGLELINE | DT_VCENTER);		
		}
		else {
			pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
			oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
			pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
			oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

			pDC->SetBkMode(OPAQUE);
			pDC->Rectangle(left, top, right, bottom+2);
		}

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		pDC->SelectObject(oldfont);
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_name_box\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::draw_balance_box(int chair) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		RECT		textrect, drawrect;
		CFont		*oldfont;
		CString		t;
		RECT		cr;
		int			left, top, right, bottom;
		CDC			*pDC = GetDC();

		// Get size of current client window
		GetClientRect(&cr);

		// Background color
		pDC->SetBkColor(COLOR_GRAY);

		// Set font basics
		oldfont = pDC->SelectObject(&cFont);
		pDC->SetTextColor(COLOR_BLACK);

		if (seated[chair] || active[chair]) {
			pTempPen = (CPen*)pDC->SelectObject(&black_pen);
			oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
			pTempBrush = (CBrush*)pDC->SelectObject(&white_brush);
			oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
			t = playerbalance[chair];
		}
		else {
			pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
			oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
			pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
			oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush
			t = "";
		}

		// Figure placement of box
		left = cr.right * pc[chair][0] - 26;
		top = cr.bottom * pc[chair][1] + 31;
		right = cr.right * pc[chair][0] + 25;
		bottom = cr.bottom * pc[chair][1] + 46;

		// Calc rectangle size for text
		textrect.left = 0;
		textrect.top = 0;
		textrect.right = 0;
		textrect.bottom = 0;
		pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
	
		// Figure out placement of rectangle
		drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
		drawrect.top = top;
		drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
		drawrect.bottom = bottom;

		// Draw it		
		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(drawrect.left, drawrect.top, drawrect.right, drawrect.bottom);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(t.GetString(), t.GetLength(), &drawrect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		// Restore original pen and brush
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		pDC->SelectObject(oldfont);
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_balance_box\n"); 
		throw;
	}
#endif
}


void CManualModeDlg::draw_player_bet(int chair) {
#ifdef SEH_ENABLE
	try {
#endif
		CPen		*pTempPen, oldpen;
		CBrush		*pTempBrush, oldbrush;	
		RECT		textrect, drawrect;
		CFont		*oldfont;
		CString		t;
		RECT		cr;
		int			xcenter, ycenter, xadj, yadj;
		CDC			*pDC = GetDC();
		
		// Get size of current client window
		GetClientRect(&cr);

		// Background color
		pDC->SetBkColor(COLOR_GRAY);

		// Figure placement
		xcenter = cr.right * pc[chair][0];
		ycenter = cr.bottom * pc[chair][1];
		xadj = pcbet[chair][0];
		yadj = pcbet[chair][1];
						
		// Set font basics
		oldfont = pDC->SelectObject(&cFont);
		pDC->SetTextColor(COLOR_WHITE);

		// Draw box
		if (active[chair]) {
			pTempPen = (CPen*)pDC->SelectObject(&white_pen);
			oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
			t = playerbet[chair];
		}
		else {
			pTempPen = (CPen*)pDC->SelectObject(&white_dot_pen);
			oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
			t = "";
		}
		pTempBrush = (CBrush*)pDC->SelectObject(&gray_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

		// Calc rectangle size for text
		textrect.left = 0;
		textrect.top = 0;
		textrect.right = 0;
		textrect.bottom = 0;
		pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
		if (textrect.right<40) textrect.right = 40;
		if (textrect.bottom<16) textrect.bottom = 16;

		// Figure out placement of rectangle
		if (xadj<0) {
			drawrect.left = xcenter + xadj - textrect.right;
			drawrect.top = ycenter + yadj - textrect.bottom/2;
			drawrect.right = xcenter + xadj;
			drawrect.bottom = ycenter + yadj + textrect.bottom/2;
		}
		else if (xadj>0) {
			drawrect.left = xcenter + xadj;
			drawrect.top = ycenter + yadj - textrect.bottom/2;
			drawrect.right = xcenter + xadj + textrect.right;
			drawrect.bottom = ycenter + yadj + textrect.bottom/2;
		}
		else {  // xadj==0
			drawrect.left = xcenter + xadj - textrect.right/2;
			drawrect.top = ycenter + yadj - textrect.bottom/2;
			drawrect.right = xcenter + xadj + textrect.right/2;
			drawrect.bottom = ycenter + yadj + textrect.bottom/2;
		}

		//Draw it
		pDC->SetBkMode(OPAQUE);
		pDC->Rectangle(drawrect.left-2, drawrect.top, drawrect.right+2, drawrect.bottom);
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(t.GetString(), t.GetLength(), &drawrect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
		pDC->SelectObject(oldfont);
		ReleaseDC(pDC);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::draw_player_bet\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::OnLButtonUp(UINT nFlags, CPoint point) {
#ifdef SEH_ENABLE
	try {
#endif
		// Find out where we clicked
		get_click_loc(point);

		// Clicked on a button indicator
		if  ( click_loc>=FB && click_loc<=AB ) {
			buttonstate[click_loc-FB] = !buttonstate[click_loc-FB];
			InvalidateRect(NULL, true);
		}

		// Clicked on the center info box
		else if ( click_loc==CIB ) {
		}

		// Clicked on a player's name
		else if ( click_loc>=PN0 && click_loc<=PN9 ) {
		}

		// Clicked on a player's balance
		else if ( click_loc>=PB0 && click_loc<=PB9 ) {
		}

		// Clicked on a player's bet
		else if ( click_loc>=BET0 && click_loc<=BET9 ) {
			// ctrl-shift click = allin
			if (nFlags & MK_SHIFT & MK_CONTROL) {
				do_allin();
				InvalidateRect(NULL, true);
			}

			// shift click = raise
			else if (nFlags & MK_SHIFT) {
				do_raise();
				InvalidateRect(NULL, true);
			}

			// plain click = call
			else if (atof(playerbet[click_chair]) != get_current_bet()) {
				do_call();
				InvalidateRect(NULL, true);
			}
		}

		// Clicked on a common card
		else if ( click_loc>=CC0 && click_loc<=CC4 ) {
		}

		// Clicked on a player card
		else if ( click_loc>=P0C0 && click_loc<=P9C1 ) {
		}

		CDialog::OnLButtonUp(nFlags, point);
#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::OnLButtonUp :\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::OnLButtonDblClk(UINT nFlags, CPoint point) {
#ifdef SEH_ENABLE
	try {
#endif
		// Find out where we clicked
		get_click_loc(point);

		// Clicked on a button indicator
		if  ( click_loc>=FB && click_loc<=AB ) {
		}

		// Clicked on the center info box
		else if ( click_loc==CIB ) {
		}

		// Clicked on a player's name
		else if ( click_loc>=PN0 && click_loc<=PN9 ) {
		}

		// Clicked on a player's balance
		else if ( click_loc>=PB0 && click_loc<=PB9 ) {
		}

		// Clicked on a player's bet
		else if ( click_loc>=BET0 && click_loc<=BET9 ) {
		}

		// Clicked on a common card
		else if ( click_loc>=CC0 && click_loc<=CC4 ) {
		}

		// Clicked on a player card
		else if ( click_loc>=P0C0 && click_loc<=P9C1 ) {
			// Sit in, sit down, set card backs
			seated[click_chair] = true;
			active[click_chair] = true;
			OnCardBacks();
		}

		CDialog::OnLButtonDblClk(nFlags, point);

#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::OnLButtonDblClk :\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::OnContextMenu(CWnd* pWnd, CPoint point) {
#ifdef SEH_ENABLE
	try {
#endif
		CMenu			contextMenu, *tracker, *subrank; 
		MENUITEMINFO	mii;
		int				i;
		CPoint			point_adj;
		Registry		reg;

		// Find out where we clicked
		point_adj = point;
		ScreenToClient(&point_adj);
		get_click_loc(point_adj);

		//////////////////////////////////////////////////////////////
		// If we clicked on the macro button, pop up the edit dialog
		if  ( click_loc==MACRO ) {
			edit.m_titletext.Format("Edit macro");
			edit.m_result = macro_text;
			if (edit.DoModal() == IDOK) {
				macro_text = edit.m_result;
				reg.read_reg();
				reg.macro = macro_text;
				reg.write_reg();
			}
		}

		//////////////////////////////////////////////////////////////
		// If we clicked on a button indicator, toggle it
		else if ( click_loc>=FB && click_loc<=AB ) {
			buttonstate[click_loc-FB] = !buttonstate[click_loc-FB];
			InvalidateRect(NULL, true);
		}

		//////////////////////////////////////////////////////////////
		// If we clicked on the center info box, popup dialog
		else if ( click_loc==CIB ) {
			gameinfo.m_sblind = sblind;
			gameinfo.m_bblind = bblind;
			gameinfo.m_bigbet = bbet;
			gameinfo.m_ante = ante;
			gameinfo.m_gametype = limit;
			gameinfo.m_network = network;
			gameinfo.m_tournament = istournament;

			if (gameinfo.DoModal() == IDOK) {
				sblind = gameinfo.m_sblind;
				bblind = gameinfo.m_bblind;
				bbet = gameinfo.m_bigbet;
				ante = gameinfo.m_ante;
				limit = gameinfo.m_gametype;
				network = gameinfo.m_network;
				istournament = gameinfo.m_tournament;
				InvalidateRect(NULL, true);
			}
		}

		//////////////////////////////////////////////////////////////
		// If we clicked on a player's name, popup dialog
		else if ( click_loc>=PN0 && click_loc<=PN9 ) {
			edit.m_titletext.Format("New name for seat %d", click_chair);
			edit.m_result = playername[click_chair];
			if (edit.DoModal() == IDOK) {
				playername[click_chair] = edit.m_result;
				InvalidateRect(NULL, true);
			}
		}

		//////////////////////////////////////////////////////////////
		// If we clicked on a player's balance, popup dialog
		else if ( click_loc>=PB0 && click_loc<=PB9 ) {
			edit.m_titletext.Format("New balance for seat %d", click_chair);
			edit.m_result = playerbalance[click_chair];
			if (edit.DoModal() == IDOK) {
				playerbalance[click_chair] = edit.m_result;
				InvalidateRect(NULL, true);
			}
		}

		//////////////////////////////////////////////////////////////
		// If we clicked on a player's bet, popup dialog
		else if ( click_loc>=BET0 && click_loc<=BET9 ) {
			edit.m_titletext.Format("New bet for seat %d", click_chair);
			edit.m_result = playerbet[click_chair];
			if (edit.DoModal() == IDOK) {
				double new_balance = atof(playerbalance[click_chair].GetString()) + 
									 atof(playerbet[click_chair].GetString()) - 
									 atof(edit.m_result.GetString());

				if (new_balance != new_balance) {
					playerbalance[click_chair].Format("%.2f", new_balance);
				}
				else {
					playerbalance[click_chair].Format("%.0f", new_balance);
				}

				playerbet[click_chair] = edit.m_result;
				InvalidateRect(NULL, true);
			}
		}

		//////////////////////////////////////////////////////////////
		// If we clicked on a common card popup the card menu
		else if ( click_loc>=CC0 && click_loc<=CC4 ) {
			
			// Load the popup menu
			contextMenu.LoadMenu(IDR_MENU_CARD_SELECTION);

			// Get a pointer to the first item of the menu
			tracker = contextMenu.GetSubMenu(0);

			// Set state of each menu item based on used_cards
			memset(&mii, 0, sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STATE;
			for (i=0; i<=12; i++) {
				subrank = tracker->GetSubMenu(i);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(0, &mii, true);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(1, &mii, true);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(2, &mii, true);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(3, &mii, true);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) &&
							 CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) &&
							 CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) &&
							 CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ? MFS_DISABLED : MFS_ENABLED;
				tracker->SetMenuItemInfo(i, &mii, true);
			}

			// Set state of no cards item
			mii.fState = card[click_loc]!=CARD_NOCARD ? MFS_ENABLED : MFS_DISABLED;
			tracker->SetMenuItemInfo(14, &mii, true);

			// Display the context menu
			tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
				point.x + dlgOptions.m_unobstructivePopup * (MM_WIDTH - point_adj.x), 
				point.y, this);
		}

		//////////////////////////////////////////////////////////////
		// If we clicked on a player card popup the card/seat menu
		else if ( click_loc>=P0C0 && click_loc<=P9C1 ) {
			
			// Load the popup menu
			contextMenu.LoadMenu(IDR_MENU_CARD_AND_SEAT_SELECTION);

			// Get a pointer to the first item of the menu
			tracker = contextMenu.GetSubMenu(0);

			// Set state and text of seat item
			memset(&mii, 0, sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STRING;
			mii.dwTypeData = seated[click_chair] ? "&Stand up" : "&Sit down";
			tracker->SetMenuItemInfo(0, &mii, true);

			// Set state and text of sit item
			memset(&mii, 0, sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STRING | MIIM_STATE;
			mii.fState = seated[click_chair] ? MFS_ENABLED : MFS_DISABLED;
			mii.dwTypeData = active[click_chair] ? "S&it out" : "S&it in";
			tracker->SetMenuItemInfo(1, &mii, true);

			// Set state of dealer item
			memset(&mii, 0, sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STATE;
			mii.fState = dealer[click_chair] ? MFS_DISABLED : MFS_ENABLED;
			tracker->SetMenuItemInfo(2, &mii, true);

			// Set state of 2-A menu items based on used_cards
			memset(&mii, 0, sizeof(MENUITEMINFO));
			mii.cbSize = sizeof(MENUITEMINFO);
			mii.fMask = MIIM_STATE;
			for (i=0; i<=12; i++) {
				subrank = tracker->GetSubMenu(i+4);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(0, &mii, true);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(1, &mii, true);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(2, &mii, true);

				mii.fState = CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ? MFS_DISABLED : MFS_ENABLED;
				subrank->SetMenuItemInfo(3, &mii, true);

				mii.fState = (CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_CLUBS)) &&
							  CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_DIAMONDS)) &&
							  CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_HEARTS)) &&
							  CardMask_CARD_IS_SET(used_cards, StdDeck_MAKE_CARD(i, Suit_SPADES)) ) ||
							  !active[click_chair] ? MFS_DISABLED : MFS_ENABLED;
				tracker->SetMenuItemInfo(i+4, &mii, true);
			}

			// Set state of cardbacks item
			mii.fState = active[click_chair] ? MFS_ENABLED : MFS_DISABLED;
			tracker->SetMenuItemInfo(18, &mii, true);

			// Set state of no cards item
			mii.fState = active[click_chair] &&
						 (card[P0C0+click_chair*2]!=CARD_NOCARD || card[P0C1+click_chair*2]!=CARD_NOCARD)
						 ? MFS_ENABLED : MFS_DISABLED;
			tracker->SetMenuItemInfo(19, &mii, true);

			// Display the context menu
			tracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
				point.x + dlgOptions.m_unobstructivePopup * (MM_WIDTH - point_adj.x), 
				point.y, this);
		}



#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::OnContextMenu :\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::get_click_loc(CPoint p) {
#ifdef SEH_ENABLE
	try {
#endif
		RECT		cr;
		int			i;
		int			xcenter, ycenter, xadj, yadj;
		int			left, top, right, bottom;
		RECT		textrect, drawrect;
		CDC			*pDC = GetDC();
		CString		t;

		GetClientRect(&cr);

		// see if we clicked on the macro button
		if (p.x >= 233 && p.x <= 233+51 &&
			p.y >= 307 && p.y <= 307+23) {
			click_loc = MACRO;
			click_chair = -1;
			return;
		}

		// see if we clicked on a common card
		for (i=0; i<=4; i++) {
			if (p.x >= cr.right/2 + cc[i][0] && 
				p.x <= cr.right/2 + cc[i][0] + CARDSIZEX &&
				p.y >= cr.bottom/2 + cc[i][1] &&
				p.y <= cr.bottom/2 + cc[i][1] + CARDSIZEY) {
				click_loc = CC0+i;
				click_chair = -1;
				return;
			}
		}

		// See if we clicked on a player's name
		for (i=0; i<=9; i++) {
			// Figure placement of box
			left = cr.right * pc[i][0] - 36;
			top = cr.bottom * pc[i][1] + 15;
			right = cr.right * pc[i][0] + 35;
			bottom = cr.bottom * pc[i][1] + 30;

			// Calc rectangle size for text
			textrect.left = 0;
			textrect.top = 0;
			textrect.right = 0;
			textrect.bottom = 0;
			pDC->DrawText(playername[i].GetString(), playername[i].GetLength(), &textrect, DT_CALCRECT);

			// Figure out placement of rectangle
			drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
			drawrect.top = top;
			drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
			drawrect.bottom = bottom;

			if (p.x >= drawrect.left && p.x <= drawrect.right &&
				p.y >= drawrect.top && p.y <= drawrect.bottom &&
				seated[i]) {
				click_loc = PN0+i;
				click_chair = i;
				return;
			}
		}

		// See if we clicked on a player's balance
		for (i=0; i<=9; i++) {
			t = "";
			if (seated[i] || active[i]) {
				t = playerbalance[i];
			}

			// Figure placement of box
			left = cr.right * pc[i][0] - 26;
			top = cr.bottom * pc[i][1] + 31;
			right = cr.right * pc[i][0] + 25;
			bottom = cr.bottom * pc[i][1] + 46;

			// Calc rectangle size for text
			textrect.left = 0;
			textrect.top = 0;
			textrect.right = 0;
			textrect.bottom = 0;
			pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
		
			// Figure out placement of rectangle
			drawrect.left = left < (left+(right-left)/2)-textrect.right/2-3 ? left : (left+(right-left)/2)-textrect.right/2-3;
			drawrect.top = top;
			drawrect.right = right > (left+(right-left)/2)+textrect.right/2+3 ? right : (left+(right-left)/2)+textrect.right/2+3;
			drawrect.bottom = bottom;
			
			if (p.x >= drawrect.left && p.x <= drawrect.right &&
				p.y >= drawrect.top && p.y <= drawrect.bottom &&
				seated[i]) {
				click_loc = PB0+i;
				click_chair = i;
				return;
			}
		}

		// see if we clicked on a player's bet
		for (i=0; i<=9; i++) {
			t = "";
			if (active[i]) {
				t = playerbet[i];
			}

			xcenter = cr.right * pc[i][0];
			ycenter = cr.bottom * pc[i][1];
			xadj = pcbet[i][0];
			yadj = pcbet[i][1];
			
			// Calc rectangle size for text
			textrect.left = 0;
			textrect.top = 0;
			textrect.right = 0;
			textrect.bottom = 0;
			pDC->DrawText(t.GetString(), t.GetLength(), &textrect, DT_CALCRECT);
			if (textrect.right<40) textrect.right = 40;
			if (textrect.bottom<16) textrect.bottom = 16;

			// Figure out placement of rectangle
			if (xadj<0) {
				drawrect.left = xcenter + xadj - textrect.right;
				drawrect.top = ycenter + yadj - textrect.bottom/2;
				drawrect.right = xcenter + xadj;
				drawrect.bottom = ycenter + yadj + textrect.bottom/2;
			}
			else if (xadj>0) {
				drawrect.left = xcenter + xadj;
				drawrect.top = ycenter + yadj - textrect.bottom/2;
				drawrect.right = xcenter + xadj + textrect.right;
				drawrect.bottom = ycenter + yadj + textrect.bottom/2;
			}
			else {  // xadj==0
				drawrect.left = xcenter + xadj - textrect.right/2;
				drawrect.top = ycenter + yadj - textrect.bottom/2;
				drawrect.right = xcenter + xadj + textrect.right/2;
				drawrect.bottom = ycenter + yadj + textrect.bottom/2;
			}

			if (p.x >= drawrect.left-2 && p.x <= drawrect.right+2 &&
				p.y >= drawrect.top && p.y <= drawrect.bottom &&
				active[i]) {
				click_loc = BET0+i;
				click_chair = i;
				return;
			}

		}

		// see if we clicked on a player's first card
		for (i=0; i<=9; i++) {
			if (p.x >= cr.right * pc[i][0] - CARDSIZEX - 2 && 
				p.x <= cr.right * pc[i][0] - 2 &&
				p.y >= cr.bottom * pc[i][1] - CARDSIZEY/2 - 5 &&
				p.y <= cr.bottom * pc[i][1] + CARDSIZEY/2 - 6) {
				click_loc = P0C0+i*2;
				click_chair = i;
				return;
			}
		}
	
		// see if we clicked on a player's second card
		for (i=0; i<=9; i++) {
			if (p.x >= cr.right * pc[i][0] + 1 && 
				p.x <= cr.right * pc[i][0] + CARDSIZEX + 1 &&
				p.y >= cr.bottom * pc[i][1] - CARDSIZEY/2 - 5 &&
				p.y <= cr.bottom * pc[i][1] + CARDSIZEY/2 - 6) {
				click_loc = P0C1+i*2;
				click_chair = i;
				return;
			}
		}

		// see if we clicked on the center info box
		if (p.x >= cr.right/2-60 && p.x <= cr.right/2+60 &&
			p.y >= 4 &&	p.y <= 78) {
			click_loc = CIB;
			click_chair = -1;
			return;
		}

		// see if we clicked on a button
		if (p.x >= cr.right-84 && p.x <= cr.right-70 &&
			p.y >= cr.bottom-16 && p.y <= cr.bottom-2) {
			click_loc = FB;
			click_chair = -1;
			return;
		}
		if (p.x >= cr.right-67 && p.x <= cr.right-53 &&
			p.y >= cr.bottom-16 && p.y <= cr.bottom-2) {
			click_loc = CB;
			click_chair = -1;
			return;
		}
		if (p.x >= cr.right-50 && p.x <= cr.right-36 &&
			p.y >= cr.bottom-16 && p.y <= cr.bottom-2) {
			click_loc = KB;
			click_chair = -1;
			return;
		}
		if (p.x >= cr.right-33 && p.x <= cr.right-19 &&
			p.y >= cr.bottom-16 && p.y <= cr.bottom-2) {
			click_loc = RB;
			click_chair = -1;
			return;
		}
		if (p.x >= cr.right-16 && p.x <= cr.right-2 &&
			p.y >= cr.bottom-16 && p.y <= cr.bottom-2) {
			click_loc = AB;
			click_chair = -1;
			return;
		}

		click_loc = -1;
		click_chair = -1;
		return;

#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::get_click_loc :\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::On2Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_CLUBS)); }
void CManualModeDlg::On2Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_DIAMONDS)); }
void CManualModeDlg::On2Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_HEARTS)); }
void CManualModeDlg::On2Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_2, Suit_SPADES)); }
void CManualModeDlg::On3Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_CLUBS)); }
void CManualModeDlg::On3Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_DIAMONDS)); }
void CManualModeDlg::On3Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_HEARTS)); }
void CManualModeDlg::On3Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_3, Suit_SPADES)); }
void CManualModeDlg::On4Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_CLUBS)); }
void CManualModeDlg::On4Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_DIAMONDS)); }
void CManualModeDlg::On4Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_HEARTS)); }
void CManualModeDlg::On4Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_4, Suit_SPADES)); }
void CManualModeDlg::On5Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_CLUBS)); }
void CManualModeDlg::On5Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_DIAMONDS)); }
void CManualModeDlg::On5Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_HEARTS)); }
void CManualModeDlg::On5Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_5, Suit_SPADES)); }
void CManualModeDlg::On6Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_CLUBS)); }
void CManualModeDlg::On6Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_DIAMONDS)); }
void CManualModeDlg::On6Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_HEARTS)); }
void CManualModeDlg::On6Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_6, Suit_SPADES)); }
void CManualModeDlg::On7Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_CLUBS)); }
void CManualModeDlg::On7Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_DIAMONDS)); }
void CManualModeDlg::On7Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_HEARTS)); }
void CManualModeDlg::On7Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_7, Suit_SPADES)); }
void CManualModeDlg::On8Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_CLUBS)); }
void CManualModeDlg::On8Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_DIAMONDS)); }
void CManualModeDlg::On8Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_HEARTS)); }
void CManualModeDlg::On8Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_8, Suit_SPADES)); }
void CManualModeDlg::On9Clubs()		{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_CLUBS)); }
void CManualModeDlg::On9Diamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_DIAMONDS)); }
void CManualModeDlg::On9Hearts()	{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_HEARTS)); }
void CManualModeDlg::On9Spades()	{ set_card(StdDeck_MAKE_CARD(Rank_9, Suit_SPADES)); }
void CManualModeDlg::OnTClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_CLUBS)); }
void CManualModeDlg::OnTDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_DIAMONDS)); }
void CManualModeDlg::OnTHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_HEARTS)); }
void CManualModeDlg::OnTSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_TEN, Suit_SPADES)); }
void CManualModeDlg::OnJClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_CLUBS)); }
void CManualModeDlg::OnJDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_DIAMONDS)); }
void CManualModeDlg::OnJHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_HEARTS)); }
void CManualModeDlg::OnJSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_JACK, Suit_SPADES)); }
void CManualModeDlg::OnQClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_CLUBS)); }
void CManualModeDlg::OnQDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_DIAMONDS)); }
void CManualModeDlg::OnQHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_HEARTS)); }
void CManualModeDlg::OnQSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_QUEEN, Suit_SPADES)); }
void CManualModeDlg::OnKClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_CLUBS)); }
void CManualModeDlg::OnKDiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_DIAMONDS)); }
void CManualModeDlg::OnKHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_HEARTS)); }
void CManualModeDlg::OnKSpades()	{ set_card(StdDeck_MAKE_CARD(Rank_KING, Suit_SPADES)); }
void CManualModeDlg::OnAClubs()		{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_CLUBS)); }
void CManualModeDlg::OnADiamonds()	{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_DIAMONDS)); }
void CManualModeDlg::OnAHearts()	{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_HEARTS)); }
void CManualModeDlg::OnASpades()	{ set_card(StdDeck_MAKE_CARD(Rank_ACE, Suit_SPADES)); }

void CManualModeDlg::OnCardBacks() {
	if (card[P0C0+click_chair*2]!=CARD_NOCARD && card[P0C0+click_chair*2]!=CARD_BACK) {
		CardMask_UNSET(used_cards, card[P0C0+click_chair*2]);
	}
	if (card[P0C1+click_chair*2]!=CARD_NOCARD && card[P0C1+click_chair*2]!=CARD_BACK) {
		CardMask_UNSET(used_cards, card[P0C1+click_chair*2]);
	}
	card[P0C0+click_chair*2] = CARD_BACK; 
	card[P0C1+click_chair*2] = CARD_BACK; 
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnNoCards() {
	if (card[P0C0+click_chair*2]!=CARD_NOCARD && card[P0C0+click_chair*2]!=CARD_BACK) {
		CardMask_UNSET(used_cards, card[P0C0+click_chair*2]);
	}
	if (card[P0C1+click_chair*2]!=CARD_NOCARD && card[P0C1+click_chair*2]!=CARD_BACK) {
		CardMask_UNSET(used_cards, card[P0C1+click_chair*2]);
	}
	card[P0C0+click_chair*2] = CARD_NOCARD; 
	card[P0C1+click_chair*2] = CARD_NOCARD; 
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnNoCard() {
	if (card[click_loc]!=CARD_NOCARD) {
		CardMask_UNSET(used_cards, card[click_loc]);
	}
	card[click_loc] = CARD_NOCARD; 
	InvalidateRect(NULL, true);
}

void CManualModeDlg::set_card(unsigned int c) {
	if (card[click_loc]!=CARD_NOCARD && card[click_loc]!=CARD_BACK) {
		CardMask_UNSET(used_cards, card[click_loc]);
	}
	card[click_loc] = c; 
	CardMask_SET(used_cards, c);

	if (click_loc>=CC0 && click_loc<=CC4) {
		do_scrape_bets_into_pot();
	}

	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnSeatPlayer() { 
	seated[click_chair] = !seated[click_chair];
	if (!seated[click_chair]) active[click_chair] = false;
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnSitInPlayer() { 
	active[click_chair] = !active[click_chair];
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnDealerHere() { 
	int i;
	for (i=0; i<=9; i++) {
		dealer[i] = false;
	}
	dealer[click_chair] = true;
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnFold() { 
	if (card[P0C0+click_chair*2]!=CARD_NOCARD && card[P0C0+click_chair*2]!=CARD_BACK) {
		CardMask_UNSET(used_cards, card[P0C0+click_chair*2]);
	}
	if (card[P0C1+click_chair*2]!=CARD_NOCARD && card[P0C1+click_chair*2]!=CARD_BACK) {
		CardMask_UNSET(used_cards, card[P0C1+click_chair*2]);
	}
	card[P0C0+click_chair*2] = CARD_NOCARD; 
	card[P0C1+click_chair*2] = CARD_NOCARD; 
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnCall() { 
	if (atof(playerbet[click_chair]) != get_current_bet()) {
		do_call();
		InvalidateRect(NULL, true);
	}
}

void CManualModeDlg::OnRaise() { 
	do_raise();
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnAllin() { 
	do_allin();
	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnBnClickedReset() {
	clear_scrape_areas();
	istournament = false;
	handnumber.Format("%i",atoi(handnumber) + 1);
	sblind = "5";
	bblind = "10";
	bbet = "20";
	ante = "0";
	limit = LIMIT_FL;

	InvalidateRect(NULL, true);
}

void CManualModeDlg::OnBnClickedPminus() {
	for (int i=9; i>=0; i--) {
		if (seated[i] == true) {
			seated[i] = false;
			active[i] = false;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::OnBnClickedPplus() {
	for (int i=0; i<=9; i++) {
		if (seated[i] == false) {
			seated[i] = true;
			active[i] = true;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::OnBnClickedMacro() {
#ifdef SEH_ENABLE
	try {
#endif

	int				i, j, chair=0, pl_card=0, com_card=0, dealer_pos;
	unsigned int	c;
	bool			found_sblind;
	int				cards_seen = 0;

	for (i=0; i<macro_text.GetLength(); i++) {
		if (macro_text.Mid(i,1) == "R") {
			clear_scrape_areas();
		}

		else if (macro_text.Mid(i,1) == "U") {
		}

		else if (macro_text.Mid(i,1) == "u") {
		}

		else if (macro_text.Mid(i,1) == "P") {
			for (j=chair; j<=chair+9; j++) {
				if (seated[j%10] == false) {
					seated[j%10] = true;
					active[j%10] = true;
					card[P0C0+((j%10)*2)] = CARD_BACK; 
					card[P0C1+((j%10)*2)] = CARD_BACK; 
					chair = j%10;
					pl_card=0;
					j = chair+10;
				}
			}
		}

		else if (macro_text.Mid(i,1) == "p") {
			for (j=chair+9; j>=chair; j--) {
				if (seated[j%10] == true) {
					seated[j%10] = false;
					active[j%10] = false;
					if (card[P0C0+((j%10)*2)]!=CARD_NOCARD && card[P0C0+((j%10)*2)]!=CARD_BACK) {
						CardMask_UNSET(used_cards, card[P0C0+((j%10)*2)]);
					}
					if (card[P0C1+((j%10)*2)]!=CARD_NOCARD && card[P0C1+((j%10)*2)]!=CARD_BACK) {
						CardMask_UNSET(used_cards, card[P0C1+((j%10)*2)]);
					}
					card[P0C0+((j%10)*2)] = CARD_NOCARD; 
					card[P0C1+((j%10)*2)] = CARD_NOCARD; 
					chair = j%10;
					pl_card=0;
					j = -1;
				}
			}
		}

		else if (macro_text.Mid(i,1) == "b") {
			for (j=0; j<=9; j++) {
				if (dealer[j]) {
					dealer_pos = j;
					j = 10;
				}
			}

			for (j=dealer_pos+1; j<=dealer_pos+10; j++) {
				if (seated[j%10] == true && active[j%10] == true) {

					if (atof(sblind) != (int) atof(sblind)) {
						playerbet[j%10].Format("%.2f", atof(sblind));
					}
					else {
						playerbet[j%10].Format("%.0f", atof(sblind));
					}
	
					double new_balance = atof(playerbalance[j%10].GetString()) - atof(sblind);
					if (new_balance != (int) new_balance) {
						playerbalance[j%10].Format("%.2f", new_balance);
					}
					else {
						playerbalance[j%10].Format("%.0f", new_balance);
					}

					j = dealer_pos+11;
				}
			}
		}

		else if (macro_text.Mid(i,1) == "B") {
			for (j=0; j<=9; j++) {
				if (dealer[j]) {
					dealer_pos = j;
					j = 10;
				}
			}

			found_sblind = false;
			for (j=dealer_pos+1; j<=dealer_pos+10; j++) {
				if (seated[j%10] == true && active[j%10] == true) {
					if (found_sblind==true) {
						if (atof(bblind) != (int) atof(bblind)) {
							playerbet[j%10].Format("%.2f", atof(bblind));
						}
						else {
							playerbet[j%10].Format("%.0f", atof(bblind));
						}
						
						double new_balance = atof(playerbalance[j%10].GetString()) - atof(bblind);
						if (new_balance != (int) new_balance) {
							playerbalance[j%10].Format("%.2f", new_balance);
						}
						else {
							playerbalance[j%10].Format("%.0f", new_balance);
						}

						j = dealer_pos+11;
					}
					else {
						found_sblind = true;
					}
				}
			}

		}

		else if ((macro_text.Mid(i,1) >= "2" && macro_text.Mid(i,1) <= "9") ||
				 macro_text.Mid(i,1).MakeLower() == "t" ||
				 macro_text.Mid(i,1).MakeLower() == "j" ||
				 macro_text.Mid(i,1).MakeLower() == "q" ||
				 macro_text.Mid(i,1).MakeLower() == "k" ||
				 macro_text.Mid(i,1).MakeLower() == "a") {

			c = CARD_NOCARD;
			if (macro_text.Mid(i+1,1).MakeLower() == "c") {
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_CLUBS);
				i++;
			}
			else if (macro_text.Mid(i+1,1).MakeLower() == "d") {
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_DIAMONDS);
				i++;
			}
			else if (macro_text.Mid(i+1,1).MakeLower() == "h") {
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_HEARTS);
				i++;
			}
			else if (macro_text.Mid(i+1,1).MakeLower() == "s") {
				c = StdDeck_MAKE_CARD(get_rank(macro_text.Mid(i,1).MakeLower().GetString()[0]), Suit_SPADES);
				i++;
			}

			if (c != CARD_NOCARD) {
				// First two cards seen get set to player
				if (cards_seen<2)
				{
					if (card[(P0C0+(pl_card%2))+((chair%10)*2)]!=CARD_NOCARD && card[(P0C0+(pl_card%2))+((chair%10)*2)]!=CARD_BACK) {
						CardMask_UNSET(used_cards, card[(P0C0+(pl_card%2))+((chair%10)*2)]);
					}
					card[(P0C0+(pl_card%2))+((chair%10)*2)] = c; 
					CardMask_SET(used_cards, c);
					pl_card++;
					cards_seen++;
				}
				// Next 5 cards get set to common
				else
				{
					if (card[CC0+(com_card%5)]!=CARD_NOCARD && card[CC0+(com_card%5)]!=CARD_BACK) {
						CardMask_UNSET(used_cards, card[CC0+(com_card%5)]);
					}
					card[CC0+(com_card%5)] = c; 
					CardMask_SET(used_cards, c);
					com_card++;
					cards_seen++;
				}
			}
		}

		else if (macro_text.Mid(i,1).MakeLower() == "n") {
			for (j=0; j<=9; j++) {
				dealer[j] = false;
			}
			dealer[chair] = true;
		}



	}
	InvalidateRect(NULL, true);

#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::OnBnClickedMacro :\n"); 
		throw;
	}
#endif
}

int CManualModeDlg::get_rank(char c) {
#ifdef SEH_ENABLE
	try {
#endif

		if (c=='2') return Rank_2;
		if (c=='3') return Rank_3;
		if (c=='4') return Rank_4;
		if (c=='5') return Rank_5;
		if (c=='6') return Rank_6;
		if (c=='7') return Rank_7;
		if (c=='8') return Rank_8;
		if (c=='9') return Rank_9;
		if (c=='t') return Rank_TEN;
		if (c=='j') return Rank_JACK;
		if (c=='q') return Rank_QUEEN;
		if (c=='k') return Rank_KING;
		if (c=='a') return Rank_ACE;

		return Rank_2;

#ifdef SEH_ENABLE
	}
	catch (...)	 { 
		logfatal("CManualModeDlg::get_rank :\n"); 
		throw;
	}
#endif
}

void CManualModeDlg::OnBnClickedDminus() {
	for (int i=0; i<=9; i++) {
		if (dealer[i] == true) {
			dealer[i] = false;
			if (--i < 0) { i = 9; }
			dealer[i] = true;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::OnBnClickedDplus() {
CString s;
	for (int i=0; i<=9; i++) {
		if (dealer[i] == true) {
			dealer[i] = false;
			if (++i > 9) { i = 0; }
			dealer[i] = true;
			InvalidateRect(NULL, true);
			return;
		}
	}
}

void CManualModeDlg::do_call(void) {

	double		diff;

	diff = get_current_bet() - atof(playerbet[click_chair]);
	if (diff > atof(playerbalance[click_chair])) {
		diff = atof(playerbalance[click_chair]);
	}

	if (diff+atof(playerbet[click_chair]) != (int) (diff+atof(playerbet[click_chair]))) {
		playerbet[click_chair].Format("%.2f", diff+atof(playerbet[click_chair]));
	}
	else {
		playerbet[click_chair].Format("%.0f", diff+atof(playerbet[click_chair]));
	}

	double new_balance = atof(playerbalance[click_chair].GetString()) - diff;
	if (new_balance != (int) new_balance) {
		playerbalance[click_chair].Format("%.2f", new_balance);
	}
	else {
		playerbalance[click_chair].Format("%.0f", new_balance);
	}
}

void CManualModeDlg::do_raise(void) {

	double diff, raise_amt;

	raise_amt = get_br()>=3 && limit==LIMIT_FL ? atof(bblind)*2 :atof(bblind);

	diff = get_current_bet() + raise_amt - atof(playerbet[click_chair]);
	if (diff > atof(playerbalance[click_chair])) {
		diff = atof(playerbalance[click_chair]);
	}

	if (diff+atof(playerbet[click_chair]) != (int) diff+atof(playerbet[click_chair])) {
		playerbet[click_chair].Format("%.2f", diff+atof(playerbet[click_chair]));
	}
	else {
		playerbet[click_chair].Format("%.0f", diff+atof(playerbet[click_chair]));
	}

	double new_balance = atof(playerbalance[click_chair].GetString()) - diff;
	if (new_balance != (int) new_balance) {
		playerbalance[click_chair].Format("%.2f", new_balance);
	}
	else {
		playerbalance[click_chair].Format("%.0f", new_balance);
	}
}

void CManualModeDlg::do_allin(void) {
	playerbet[click_chair] = playerbalance[click_chair];
	playerbalance[click_chair] = "0";
}

void CManualModeDlg::do_scrape_bets_into_pot(void) {
	int		i;
	int		ncommoncards = 0;

	for (i=0; i<=4; i++) {
		if (card[CC0+i]!=CARD_NOCARD && card[CC0+i]!=CARD_BACK) {
			ncommoncards += 1;
		}
	}

	if (ncommoncards_last == 0 && ncommoncards>0) {
		for (i=0; i<=9; i++) {
			pot[0] += atof(playerbet[i]);
			playerbet[i] = "0";
		}
	}
	else if (ncommoncards_last == 3 && ncommoncards>3) {
		for (i=0; i<=9; i++) {
			pot[0] += atof(playerbet[i]);
			playerbet[i] = "0";
		}
	}
	else if (ncommoncards_last == 4 && ncommoncards>4) {
		for (i=0; i<=9; i++) {
			pot[0] += atof(playerbet[i]);
			playerbet[i] = "0";
		}
	}

	ncommoncards_last = ncommoncards;
}


double CManualModeDlg::get_current_bet(void) {
	double	curbet=0;
	int		i;

	for (i=0; i<=9; i++) {
		if (atof(playerbet[i]) > curbet) {
			curbet = atof(playerbet[i]);
		}
	}

	if (get_br()==1) {
		if (curbet==0)  curbet = atof(sblind);
		else if (curbet==atof(sblind))  curbet = atof(bblind);
	}

	return curbet;
}

int CManualModeDlg::get_br(void) {
	int		i, br;
	int		ncommoncards = 0;

	for (i=0; i<=4; i++) {
		if (card[CC0+i]!=CARD_NOCARD && card[CC0+i]!=CARD_BACK) {
			ncommoncards += 1;
		}
	}

	if (ncommoncards<=2) br=1;
	else if (ncommoncards==3) br=2;
	else if (ncommoncards==4) br=4;
	else if (ncommoncards==5) br=5;

	return br;
}