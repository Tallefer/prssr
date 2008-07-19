// CeTreeCtrl.cpp : implementation file
//

#include "..\stdafx.h"
#include "..\prssr.h"
#include "CeTreeCtrl.h"
#include "..\Appearance.h"

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "..\debug\crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCeTreeCtrl

CCeTreeCtrl::CCeTreeCtrl(UINT nID/* = IDS_NO_ITEMS_TO_DISPLAY*/) {
	m_nID = nID;
//	CreateFont();
}

CCeTreeCtrl::~CCeTreeCtrl() {
//	m_oSmallFont.DeleteObject();
}


BEGIN_MESSAGE_MAP(CCeTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CCeTreeCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCeTreeCtrl message handlers

//void CCeTreeCtrl::CreateFont() {
/*	HFONT hFont = (HFONT) GetStockObject(SYSTEM_FONT);
	CFont *pFont;
	pFont = CFont::FromHandle(hFont);
	LOGFONT lf = { 0 };
	pFont->GetLogFont(&lf);
	lf.lfHeight = (lf.lfHeight * 8) / 10;
	m_oSmallFont.CreateFontIndirect(&lf);
*/
//}

void CCeTreeCtrl::OnPaint() {
    Default();
    if (GetRootItem() == NULL) {
        COLORREF clrText = ::GetSysColor(COLOR_WINDOWTEXT);
        COLORREF clrTextBk = ::GetSysColor(COLOR_WINDOW);

        CDC *pDC = GetDC();
        int nSavedDC = pDC->SaveDC();

        CRect rc;
        GetWindowRect(&rc);
        ScreenToClient(&rc);

        rc.top += SCALEX(8);

        CString strText;
		strText.LoadString(m_nID);

        pDC->SetTextColor(clrText);
        pDC->SetBkColor(clrTextBk);
        pDC->FillRect(rc, &CBrush(clrTextBk));
        pDC->SelectObject(&Appearance.BaseFont);
        pDC->DrawText(strText, rc, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX | DT_NOCLIP);

        pDC->RestoreDC(nSavedDC);
        ReleaseDC(pDC);
    }
	// Do not call CTreeCtrl::OnPaint() for painting messages
}

void CCeTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
	// The code below fixes wrong MFC behaviour on Pocket PC 2002 devices
	// (running circles appears twice while you tap and hold
	Default();
}
