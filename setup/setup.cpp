/**
 *  setup.cpp
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

#include "StdAfx.h"
#include "setup.h"
#include <ce_setup.h>
#include "../share/defs.h"
#include "../share/reg.h"
#include <notify.h>
#include "../share/notif.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VALUE_NOT_SET					-1

//HINSTANCE g_hAppInstance;

static int TodayPluginEnabled = VALUE_NOT_SET;
static BOOL PreviouslyInstalled = FALSE;

//
// Today plugin helpers
//
void EnableTodayPlugin(BOOL enable = TRUE) {
	{	CRegistry regToday(HKEY_LOCAL_MACHINE, REG_TODAYROOT);
		regToday.Write(szEnabled, enable ? 1 : 0);
		// destructor needs to be called
	}
	SendMessage(HWND_BROADCAST, WM_WININICHANGE, 0xF2, 0);
}

//
// Install/Uninstall
//

codeINSTALL_INIT Install_Init(
    HWND        hwndParent,
    BOOL        fFirstCall,
    BOOL        fPreviouslyInstalled,
    LPCTSTR     pszInstallDir)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	PreviouslyInstalled = fPreviouslyInstalled;

	if (fPreviouslyInstalled) {
		// today plugin installation status
		CRegistry regToday(HKEY_LOCAL_MACHINE, REG_KEY_TODAY);
		TodayPluginEnabled = regToday.Read(szEnabled, VALUE_NOT_SET);

		// deactivate today plugin
		EnableTodayPlugin(FALSE);
	}
	else {
		// create the registry branch (to avoid unwanted removal of sub-branches on uninstall)
		DWORD dwDisposition;
		HKEY hApp;
		if (::RegCreateKeyEx(HKEY_CURRENT_USER, REG_KEY_COMPANY, 0, 0, 0, 0, NULL, &hApp, &dwDisposition) == ERROR_SUCCESS) {
			RegCloseKey(hApp);
		}
	}

    return codeINSTALL_INIT_CONTINUE;
}

codeINSTALL_EXIT Install_Exit(
    HWND    hwndParent,
    LPCTSTR pszInstallDir,
    WORD    cFailedDirs,
    WORD    cFailedFiles,
    WORD    cFailedRegKeys,
    WORD    cFailedRegVals,
    WORD    cFailedShortcuts)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	codeINSTALL_EXIT ret = codeINSTALL_EXIT_DONE;
	CString strCacheLocation;
//	CString strEnclosuresLocation;

	if (PreviouslyInstalled) {
/*		HKEY hApp;
		if (::RegOpenKeyEx(HKEY_CURRENT_USER, REG_KEY_APP, 0, 0, &hApp) == ERROR_SUCCESS) {
			// read the value from the registry
			strCacheLocation = RegReadString(hApp, szCacheLocation, _T(""));
			RegCloseKey(hApp);
		}
*/
	}
	else {
		CRegistry reg(HKEY_CURRENT_USER, REG_KEY_APP);
		reg.Write(szInstallDir, pszInstallDir);

		CRegistry regConf(HKEY_CURRENT_USER, REG_KEY_CONFIGURATION);
		strCacheLocation = regConf.Read(szCacheLocation, _T(""));

		if (strCacheLocation.IsEmpty()) {
			// create cache folder (try to reuse old value from the registry)
			strCacheLocation = CString(pszInstallDir) + _T("\\cache");
			CreateDirectory(strCacheLocation, NULL);
			regConf.Write(szCacheLocation, strCacheLocation);
		}

		// create cache structure
		CreateDirectory(strCacheLocation + _T("\\enclosures"), NULL);
		CreateDirectory(strCacheLocation + _T("\\favicons"), NULL);
		CreateDirectory(strCacheLocation + _T("\\feeds"), NULL);
		CreateDirectory(strCacheLocation + _T("\\html"), NULL);
	}

	//
	// Activate today plugin
	//
	if (TodayPluginEnabled == VALUE_NOT_SET) {
		CString sInstall;
		sInstall.LoadString(IDS_INSTALL_TITLE);
		CString sMsg;
		sMsg.LoadString(IDS_ACTIVATE_TODAY_PLUGIN);

		if (MessageBox(NULL, sMsg, sInstall, MB_YESNO | MB_SETFOREGROUND) == IDYES) {
			EnableTodayPlugin();
		}
	}
	else {
		// set the today plugin to the previous state
		EnableTodayPlugin(TodayPluginEnabled == 0 ? FALSE : TRUE);
	}

    return ret;
}

codeUNINSTALL_INIT Uninstall_Init(
    HWND        hwndParent,
    LPCTSTR     pszInstallDir)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString sTitle;
	sTitle.LoadString(IDS_INSTALL_TITLE);
	CString sText;
	sText.LoadString(IDS_KEEP_SETTINGS);
	if (MessageBox(NULL, sText, sTitle, MB_YESNO | MB_SETFOREGROUND) == IDNO) {
		// remove all registry keys (config, etc.)
		CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_APP);
	}

	// remove cradling event
	SetupCradlingEvent(FALSE);

	// Unregister today plugin
	EnableTodayPlugin(FALSE);
	// remove today plugin from the registry
	CRegistry::DeleteKey(HKEY_LOCAL_MACHINE, REG_KEY_TODAY);

	return codeUNINSTALL_INIT_CONTINUE;
}

codeUNINSTALL_EXIT Uninstall_Exit(HWND hwndParent) {
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return codeUNINSTALL_EXIT_DONE;
}


//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CSetupApp

BEGIN_MESSAGE_MAP(CSetupApp, CWinApp)
	//{{AFX_MSG_MAP(CSetupApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupApp construction

CSetupApp::CSetupApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSetupApp object

CSetupApp theApp;

