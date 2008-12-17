/**
 *  OptAppearancePg.h
 *
 *  Copyright (C) 2008  David Andrs <pda@jasnapaka.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if !defined(_OPTAPPEARANCEPG_H_INCLUDED_)
#define _OPTAPPEARANCEPG_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ctrls/CeEdit.h"
#include "ctrls/Caption.h"
#include "ctrls/ColorButton.h"

struct CFontItem;

/////////////////////////////////////////////////////////////////////////////
// COptAppearancePg dialog

class COptAppearancePg : public CPropertyPage {
	DECLARE_DYNCREATE(COptAppearancePg)

// Construction
public:
	COptAppearancePg();
	~COptAppearancePg();

// Dialog Data
	//{{AFX_DATA(COptAppearancePg)
	enum { IDD = IDD_OPT_APPEARANCE };
	CCaption m_lblFont;
	CCaption m_lblColors;
	CCaption m_lblClearType;
	CColorButton m_ctlItemColor;
	CComboBox m_ctlItems;
	CComboBox m_ctlFontSize;
	CComboBox m_ctlFontName;
	CComboBox m_ctlFontItem;
	BOOL m_bClearType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COptAppearancePg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange *pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFontItem *FontItems;

	// Generated message map functions
	//{{AFX_MSG(COptAppearancePg)
	virtual BOOL OnInitDialog();
	afx_msg void OnItemColor();
	afx_msg void OnSelendokItem();
	afx_msg void OnSelendokFontItem();
	afx_msg void OnSelendokFontName();
	afx_msg void OnEditupdateFontSize();
	afx_msg void OnSelendokFontSize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_OPTAPPEARANCEPG_H_INCLUDED_)
