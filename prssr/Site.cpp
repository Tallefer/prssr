// SiteItem.cpp: implementation of the CSiteItem class.
//
//////////////////////////////////////////////////////////////////////

#ifdef PRSSR_APP
	#include "stdafx.h"
	#include "prssr.h"
#endif

#if defined PRSSR_TODAY
	#include "../prssrtoday/stdafx.h"
#endif

#include "Site.h"

#include "../share/reg.h"
#include "../share/defs.h"
#include "../share/helpers.h"

#include "xml/FeedFile.h"
#include "xml/OpmlFile.h"

#ifdef PRSSR_APP
	#include "Config.h"
#elif defined PRSSR_TODAY
	#include "../prssrtoday/Config.h"
#endif

#ifdef MYDEBUG
#undef THIS_FILE
static TCHAR THIS_FILE[] = _T(__FILE__);
#include "debug/crtdbg.h"
#define new MYDEBUG_NEW
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
/*
#define OPML_LOAD_OK							0
#define OPML_LOAD_ERROR							-1
#define OPML_PARSING_ERROR						-2
#define OPML_INTERNAL_ERROR                     -3
*/

// registry 
static LPCTSTR szName = _T("Name");
static LPCTSTR szIdx = _T("Idx");

static LPCTSTR szFileName = _T("File Name");
static LPCTSTR szXmlUrl = _T("XML URL");
static LPCTSTR szUseGlobalCacheOptions = _T("Use Global Cache Options");
static LPCTSTR szCacheItemImages = _T("Cache Item Images");
static LPCTSTR szCacheHtml = _T("Cache Html");
static LPCTSTR szCacheLimit = _T("Cache Limit");
static LPCTSTR szUpdateInterval = _T("Update Interval");
static LPCTSTR szCacheEnclosures = _T("Cache Enclosures");
static LPCTSTR szEnclosureLimit = _T("Enclosure Limit");
static LPCTSTR szETag = _T("ETag");
static LPCTSTR szLastModified = _T("LastModified");
static LPCTSTR szUserName = _T("Username");
static LPCTSTR szPassword = _T("Password");
static LPCTSTR szSort = _T("Sort");
static LPCTSTR szSortReversed = _T("SortReversed");

static LPCTSTR szUnreadCount = _T("Unread Count");
static LPCTSTR szFlaggedCount = _T("Flagged Count");
static LPCTSTR szCheckFavIcon = _T("Check FavIcon");

static LPCTSTR szRewriteRules = _T("Rewrite Rules");
static LPCTSTR szMatch = _T("Match");
static LPCTSTR szReplace = _T("Replace");

//#ifdef PRSSR_APP

CSiteList SiteList;

//#endif

/*
#if defined PRSSR_TODAY || PRSSR_TODAYSTUB

int DrawSiteTitle(CDC *dc, CSiteItem *si, CRect *rc, UINT uFormat) {
	int nNewCount = si->GetNewCount() + si->GetUnreadCount();
	CString strTitle = si->Name;
	ReplaceHTMLEntities(strTitle);

	CString buffer;
	if (nNewCount > 0)
		buffer.Format(_T("%s (%d)"), strTitle, nNewCount);
	else
		buffer.Format(_T("%s"), strTitle);

	int nWidth = rc->Width();

	CRect rcTemp = *rc;
	dc->DrawText(buffer, &rcTemp, uFormat | DT_CALCRECT);
	if (rcTemp.Width() > nWidth) {
		// Text doesn't fit in rect. We have to truncate it and add ellipsis to the end.
		for (int i = strTitle.GetLength(); i >= 0; i--) {
			CString strName = strTitle.Left(i);

			if (nNewCount > 0)
				buffer.Format(_T("%s... (%d)"), strName, nNewCount);
			else
				buffer.Format(_T("%s..."), strName);

			rcTemp = *rc;
			dc->DrawText(buffer, &rcTemp, uFormat | DT_CALCRECT);
			if (rcTemp.Width() < nWidth) {
				// Gotcha!
				break;
			}
		}
		return dc->DrawText(buffer, rc, uFormat);
	}
	return dc->DrawText(buffer, rc, uFormat);
}

#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFeedInfo::CFeedInfo() {
	TodayShow = TRUE;
#if defined PRSSR_APP
	UseGlobalCacheOptions = TRUE;
	CacheItemImages = FALSE;
	CacheHtml = FALSE;
	CacheLimit = CACHE_LIMIT_DEFAULT;
	UpdateInterval = UPDATE_INTERVAL_GLOBAL;
	CacheEnclosures = FALSE;
	EnclosureLimit = 0;
#endif
}

CFeedInfo &CFeedInfo::operator=(const CFeedInfo &o) {
	if (this != &o) {
		FileName = o.FileName;
		XmlUrl = o.XmlUrl;
		TodayShow = o.TodayShow;

#if defined PRSSR_APP
		UseGlobalCacheOptions = o.UseGlobalCacheOptions;
		CacheItemImages = o.CacheItemImages;
		CacheHtml = o.CacheHtml;
		CacheLimit = o.CacheLimit;
		UpdateInterval = o.UpdateInterval;
		CacheEnclosures = o.CacheEnclosures;
		EnclosureLimit = o.EnclosureLimit;
		ETag = o.ETag;
		LastModified = o.LastModified;
		UserName = o.UserName;
		Password = o.Password;

		RewriteRules.SetSize(o.RewriteRules.GetSize());
		for (int i = 0; i < o.RewriteRules.GetSize(); i++) {
			CRewriteRule *rr = o.RewriteRules[i];
			CRewriteRule *newRR = new CRewriteRule;
			*newRR = *rr;
			RewriteRules.SetAtGrow(i, newRR);
		}
#endif
	}

	return *this;
}

CFeedInfo::~CFeedInfo() {
#if defined PRSSR_APP
	for (int i = 0; i < RewriteRules.GetSize(); i++)
		delete RewriteRules[i];
#endif
}

#ifdef PRSSR_APP

CString CFeedInfo::GenerateFileNameFromTitle(const CString &title) {
	LOG0(5, "CFeedInfo::GenerateFileNameFromTitle()");

	// create filename (leave only alphanumeric chars from title)
	CString fileName;
	for (int i = 0; i < title.GetLength(); i++) {
		TCHAR ch = title.GetAt(i);
		if ((ch >= 'A' && ch <= 'Z') ||
			(ch >= 'a' && ch <= 'z') ||
			(ch >= '0' && ch <= '9'))
			fileName += title.GetAt(i);
	}

	if (fileName.IsEmpty())
		fileName = _T("Feed");

	return fileName;
}

void CFeedInfo::EnsureUniqueFileName(CString &strFileName) {
	LOG1(5, "CFeedInfo::EnsureUniqueFileName(%S)", strFileName);

	CString fileName = strFileName;

	// remove possible .xml file extension
//	if (fileName.Right(4).CompareNoCase(_T(".xml")) == 0)
//		fileName = fileName.Left(fileName.GetLength() - 4);

	CString strTestFN = fileName;
	// if the file exists, add numerical index
	CString strTestFullPath;
	strTestFullPath = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, strTestFN);
	int idx = 1;
	while (FileExists(strTestFullPath)) {
		strTestFN.Format(_T("%s%d"), fileName, idx++);
		strTestFullPath = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, strTestFN);
	}

	// create the file to prevent storing feeds into files with the same file name
	HANDLE hFile = CreateFile(strTestFullPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);

	strFileName = strTestFN;
}

#endif

// CSiteItem //////////////////////////////////////////////////////////

CSiteItem::CSiteItem(CSiteItem *parent, eType type) {
	LOG1(5, "CSiteItem::CSiteItem(%d)", type);

	Parent = parent;
	Type = type;
	Info = NULL;
	Feed = NULL;
//	UpdatedFeed = NULL;
	Sort.Item = CSortInfo::Date;
	Sort.Type = CSortInfo::Descending;

	if (Type == Site)
		Status = Empty;
	else
		Status = Ok;

	ImageIdx = -1;
	CheckFavIcon = TRUE;

	Modified = FALSE;

	UnreadItems = 0;
	FlaggedItems = 0;
	CheckFavIcon = TRUE;

//	InitializeCriticalSection(&CSUpdateFeed);
	InitializeCriticalSection(&CSLoadFeed);

#ifdef PRSSR_TODAY
	memset(&LastUpdate, 0, sizeof(FILETIME));
#endif
}


CSiteItem::CSiteItem(CSiteItem *parent, CSiteItem *siteItem) {
	LOG1(5, "CSiteItem::CSiteItem(%p)", siteItem);

	Parent = parent;
	Type = siteItem->Type;
	Name = siteItem->Name;
	ImageIdx = siteItem->ImageIdx;
	CheckFavIcon = siteItem->CheckFavIcon;
	Modified = siteItem->Modified;
	Sort = siteItem->Sort;
	Info = NULL;

	if (siteItem->Type == Site) {
		Status = Empty;
		Info = new CFeedInfo();
		*Info = *(siteItem->Info);
		Feed = NULL;

		UnreadItems = siteItem->UnreadItems;
		FlaggedItems = siteItem->FlaggedItems;
	}
	else if (siteItem->Type == VFolder) {
		Status = Empty;
		Feed = NULL;
		UnreadItems = 0;
		FlaggedItems = 0;
	}
	else {
		Status = Ok;
	}

//	InitializeCriticalSection(&CSUpdateFeed);
	InitializeCriticalSection(&CSLoadFeed);

#ifdef PRSSR_TODAY
	LastUpdate = siteItem->LastUpdate;
#endif
}

CSiteItem::~CSiteItem() {
	LOG1(1, "CSiteItem::~CSiteItem(%S)", Name);

//	DeleteCriticalSection(&CSUpdateFeed);
	DeleteCriticalSection(&CSLoadFeed);
}

CSiteItem *CSiteItem::Duplicate(CSiteItem *parent) {
	LOG0(5, "CSiteItem::Duplicate()");

	CSiteItem *dup = new CSiteItem(parent, this);

	if (Type == Group) {
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);

			dup->SubItems.AddTail(si->Duplicate(dup));
		}
	}

//	InitializeCriticalSection(&CSUpdateFeed);
	InitializeCriticalSection(&CSLoadFeed);

	return dup;
}


void CSiteItem::Destroy() {
	LOG1(5, "CSiteItem::Destroy(%S)", Name);

	switch (Type) {
		case Site:
			delete Info; Info = NULL;
			if (Feed != NULL) Feed->Destroy();
			delete Feed; Feed = NULL;
			break;

		case VFolder:
			delete Info; Info = NULL;
			// we do not destroy feed, since vfolder is an array of links to already existing items that are deallocated elsewhere
			delete Feed; Feed = NULL;
			break;
			
		case Group:
			while (!SubItems.IsEmpty()) {
				CSiteItem *item = SubItems.RemoveHead();
				item->Destroy();
				delete item;
			}
			break;
	}
}

/*
void CSiteItem::SetUpdatedFeed(CFeed *feed) {
	LOG0(5, "CSiteItem::SetUpdatedFeed()");

	if (Type == Site) {
		EnterCriticalSection(&CSUpdateFeed);
		delete UpdatedFeed;
		UpdatedFeed = feed;
		LeaveCriticalSection(&CSUpdateFeed);
	}
}

void CSiteItem::UseUpdatedFeed() {
	LOG0(5, "CSiteItem::UseUpdatedFeed()");

	if (UpdatedFeed != NULL && Type == Site) {
		EnterCriticalSection(&CSUpdateFeed);
		delete Feed;
		Feed = UpdatedFeed;
		UpdatedFeed = NULL;
		LeaveCriticalSection(&CSUpdateFeed);
	}
}
*/

void CSiteItem::EnsureSiteLoaded() {
	LOG0(5, "CSiteItem::EnsureSiteLoaded()");

	if (Type == Site) {
		// if not loaded ->load
		EnterCriticalSection(&CSLoadFeed);
		if (Status == Empty) {
			CFeed *feed = new CFeed();

			CString pathName = GetCacheFile(FILE_TYPE_FEED, Config.CacheLocation, Info->FileName);
			if (feed->Load(pathName, this)) {
				Status = Ok;

#ifdef PRSSR_APP
				feed->UpdateHiddenFlags();
//				feed->SetKeywordFlags(SiteList.GetKeywords());
#endif
				
				if (Feed != NULL) Feed->Destroy();
				delete Feed;
				Feed = feed;

				UpdateCachedCounts();
			}
			else {
				Status = Error;
#if defined PRSSR_APP
				// discard ETag and LastModified value (to allow update)
				Info->ETag.Empty();
				Info->LastModified.Empty();
#endif
				UpdateCachedCounts();

				if (feed != NULL) feed->Destroy();
				delete feed;
			}
		}
		LeaveCriticalSection(&CSLoadFeed);
	}
}

/*
int CSiteItem::GetItemCount() const {
	LOG0(5, "CSiteItem::GetItemCount()");

	if (Type == Site) {
		if (Status == Ok && Feed != NULL)
			return Feed->GetItemCount();
		else
			return TotalItems;
	}
	else {
		int totalCount = 0;
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			totalCount += si->GetItemCount();
		}

		return totalCount;
	}
}

int CSiteItem::GetNewCount() const {
	LOG0(5, "CSiteItem::GetNewCount()");

	if (Type == Site) {
		if (Status == Ok && Feed != NULL)
			return Feed->GetNewCount();
		else
			return NewItems;
	}
	else {
		int newCount = 0;
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			newCount += si->GetNewCount();
		}

		return newCount;
	}
}
*/

int CSiteItem::GetUnreadCount() const {
	LOG0(5, "CSiteItem::GetUnreadCount()");

	if (Type == Site) {
		if (Status == Ok && Feed != NULL)
			return Feed->GetUnreadCount() + Feed->GetNewCount();
		else
			return UnreadItems;
	}
	else if (Type == Group) {
		int unreadCount = 0;
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			unreadCount += si->GetUnreadCount();
		}

		return unreadCount;
	}
	else {
		return 0;
	}
}

int CSiteItem::GetFlaggedCount() const {
	LOG0(5, "CSiteItem::GetFlaggedCount()");

	if (Type == Site) {
		if (Status == Ok && Feed != NULL)
			return Feed->GetFlaggedCount();
		else
			return FlaggedItems;
	}
	else if (Type == Group) {
		int flaggedCount = 0;
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			flaggedCount += si->GetFlaggedCount();
		}

		return flaggedCount;
	}
	else {
		return 0;
	}
}

/*void CSiteItem::ReadItemCountsFromCache() {
	LOG0(5, "CSiteItem::ReadItemCountsFromCache()");

	if (Type == Site) {
		HKEY hSLCache = RegOpenSiteListCacheKey();
		if (hSLCache != NULL) {
			HKEY hItem;
			DWORD dwDisposition;
			if (RegCreateKeyEx(hSLCache, Info->FileName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hItem, &dwDisposition) == ERROR_SUCCESS) {
				NewItems = RegReadDword(hItem, szNewItems, 0);
				UnreadItems = RegReadDword(hItem, szUnreadItems, 0);
				TotalItems = RegReadDword(hItem, szTotalItems, 0);

				RegCloseKey(hItem);
			}	

			RegCloseKey(hSLCache);
		}
	}
	else {
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			si->ReadItemCountsFromCache();
		}
	}
}
*/

/*
void CSiteItem::WriteItemCountsToCache() {
	LOG0(5, "CSiteItem::WriteItemCountsToCache()");

	if (Type == Site) {
		HKEY hSLCache = RegOpenSiteListCacheKey();
		if (hSLCache != NULL) {
			HKEY hItem;
			DWORD dwDisposition;
			if (RegCreateKeyEx(hSLCache, Info->FileName, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &hItem, &dwDisposition) == ERROR_SUCCESS) {
				RegWriteDword(hItem, szNewItems, NewItems);
				RegWriteDword(hItem, szUnreadItems, UnreadItems);
				RegWriteDword(hItem, szTotalItems, TotalItems);

				RegCloseKey(hItem);
			}	

			RegCloseKey(hSLCache);
		}
	}
	else {
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			si->WriteItemCountsToCache();
		}
	}
}
*/

void CSiteItem::UpdateCachedCounts() {
	LOG0(5, "CSiteItem::UpdateCachedCounts()");

	if (Type == Site) {
		if (Feed != NULL)
			UnreadItems = Feed->GetNewCount() + Feed->GetUnreadCount();
		else
			UnreadItems = 0;
	}
	else if (Type == Group) {
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			si->UpdateCachedCounts();
		}
	}
}

void CSiteItem::GetSites(CList<CSiteItem *, CSiteItem *> &sites) {
	if (Type == Site)
		sites.AddTail(this);
	else if (Type == Group) {
		POSITION pos = SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = SubItems.GetNext(pos);
			sites.AddTail(si);
		}
	}
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSiteList::CSiteList()
//	: RWLock(_T("DaProfik::pRSSreader::RWLocks::SiteList"), 2000)
{
//	Dirty = TRUE;
	Root = NULL;

	Unread = NULL;
	Flagged = NULL;
}

CSiteList::~CSiteList() {
//	delete Unread; //Unread = NULL;
//	delete Flagged; //Flagged = NULL;
}

void CSiteList::Detach() {
	Root = NULL;
	Data.RemoveAll();

	Unread = NULL;
	Flagged = NULL;
}

void CSiteList::Destroy() {
	LOG0(5, "CSiteList::Destroy()");

	if (Root != NULL) {
		Root->Destroy();
		delete Root; Root = NULL;
	}

	if (Unread != NULL) Unread->Destroy();
	if (Flagged != NULL) Flagged->Destroy();

	Data.RemoveAll();
}

void CSiteList::SetKeywords(CStringArray &kws) {
	Keywords.RemoveAll();

	for (int i = 0; i < kws.GetSize(); i++)
		Keywords.Add(kws.GetAt(i));
}

void CSiteList::CreateFrom(CSiteItem *root) {
	if (root == NULL)
		return;

	if (root->Type == CSiteItem::Site) {
		Data.Add(root);		
	}
	else if (root->Type == CSiteItem::Group) {
		POSITION pos = root->SubItems.GetHeadPosition();
		while (pos != NULL) {
			CSiteItem *si = root->SubItems.GetNext(pos);
			CreateFrom(si);
		}
	}
}

int CSiteList::GetIndexOf(CSiteItem *item) {
	if (item == Flagged)
		return SITE_FLAGGED;
	else if (item == Unread)
		return SITE_UNREAD;
	else {
		for (int i = 0; i < Data.GetSize(); i++)
			if (Data[i] == item)
				return i;
		return SITE_INVALID;
	}
}

int CSiteList::GetCount() {
	return Data.GetSize();
}

CSiteItem *CSiteList::GetAt(int i) {
	if (i >= 0 && i < Data.GetSize())
		return Data.GetAt(i);
	else if (i == SITE_UNREAD)
		return Unread;
	else if (i == SITE_FLAGGED)
		return Flagged;
	else
		return NULL;
}

void CSiteList::SetRoot(CSiteItem *root) {
	Root = root;
	if (Unread != NULL) Unread->Parent = root;
	if (Flagged != NULL) Flagged->Parent = root;
}

// Save ///////

#if defined PRSSR_APP

BOOL SaveSiteItem(CSiteItem *item, int idx) {
	LOG2(5, "SaveSiteItem(%p, %d)", item, idx);

	CString sRegPath;
	sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx + 1);

	CRegistry::DeleteKey(HKEY_CURRENT_USER, sRegPath);
	CRegistry reg(HKEY_CURRENT_USER, sRegPath);

	reg.Write(szName, item->Name);
	if (item->Type == CSiteItem::Site && item->Info != NULL) {
		reg.Write(szFileName, item->Info->FileName);
		reg.Write(szXmlUrl, item->Info->XmlUrl);
		reg.Write(szTodayShow, item->Info->TodayShow);
		reg.Write(szUseGlobalCacheOptions, item->Info->UseGlobalCacheOptions);
		reg.Write(szCacheItemImages, item->Info->CacheItemImages);
		reg.Write(szCacheHtml, item->Info->CacheHtml);
		reg.Write(szCacheLimit, item->Info->CacheLimit);
		reg.Write(szUpdateInterval, item->Info->UpdateInterval);
		reg.Write(szCacheEnclosures, item->Info->CacheEnclosures);
		reg.Write(szEnclosureLimit, item->Info->EnclosureLimit);
		reg.Write(szETag, item->Info->ETag);
		reg.Write(szLastModified, item->Info->LastModified);
		reg.Write(szUserName, item->Info->UserName);
		reg.Write(szPassword, item->Info->Password);

		// save rewrite rules
		CRegistry regRewriteRules(reg, szRewriteRules);
		for (int i = 0; i < item->Info->RewriteRules.GetSize(); i++) {
			CRewriteRule *rr = item->Info->RewriteRules[i];

			CString sNum;
			sNum.Format(_T("%02d"), i);
			CRegistry regRule(regRewriteRules, sNum);
			regRule.Write(szMatch, rr->Match);
			regRule.Write(szReplace, rr->Replace);
		}
	}

	reg.Write(szCheckFavIcon, item->CheckFavIcon);

	switch (item->Sort.Item) {
		case CSortInfo::Date: reg.Write(szSort, _T("date")); break;
		case CSortInfo::Read: reg.Write(szSort, _T("read")); break;
	}
	
	if (item->Sort.Type == CSortInfo::Ascending)
		reg.Write(szSortReversed, FALSE);
	else
		reg.Write(szSortReversed, TRUE);

	return TRUE;
}

BOOL SaveSiteItemUnreadCount(CSiteItem *item, int idx) {
	LOG2(5, "SaveSiteItemUnreadCount(%p, %d)", item, idx);

	if (item == NULL)
		return TRUE;

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx + 1);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		reg.Write(szUnreadCount, item->GetUnreadCount());
	}

	return TRUE;
}

BOOL SaveSiteItemFlaggedCount(CSiteItem *item, int idx) {
	LOG2(5, "SaveSiteItemFlaggedCount(%p, %d)", item, idx);

	if (item == NULL)
		return TRUE;

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx + 1);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		reg.Write(szFlaggedCount, item->GetFlaggedCount());
	}

	return TRUE;
}

BOOL SaveSiteGroup(CSiteItem *item, CRegistry &reg) {
	LOG1(5, "SaveSiteGroup(%p)", item);

	// save sub items
	int k = 1;

	POSITION pos = item->SubItems.GetHeadPosition();
	while (pos != NULL) {
		CSiteItem *si = item->SubItems.GetNext(pos);

		CString sNum;
		sNum.Format(_T("%d"), k);
		CRegistry regItem(reg, sNum);
		SaveSiteGroup(si, regItem);

		k++;
	}

	// save item itself
	if (item->Type == CSiteItem::Site) {
		reg.Write(szIdx, SiteList.GetIndexOf(item));
	}
	else if (item->Type == CSiteItem::Group) {
		reg.Write(szName, item->Name);
	}

	return TRUE;
}

BOOL SaveSiteList() {
	LOG0(3, "SaveSiteList()");

	// sites
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_SUBSCRIPTIONS);
	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_SUBSCRIPTIONS);
	for (int i = 0; i < SiteList.GetCount(); i++) {
		CSiteItem *item = SiteList.GetAt(i);
		SaveSiteItem(item, i);
		SaveSiteItemUnreadCount(item, i);
		SaveSiteItemFlaggedCount(item, i);
	}

	// save hierarchy
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_GROUPS);
	CRegistry regGroups(HKEY_CURRENT_USER, REG_KEY_GROUPS);
	SaveSiteGroup(SiteList.GetRoot(), regGroups);

	return TRUE;
}

void SaveSiteListKeywords() {
	LOG0(3, "SaveSiteListKeywords()");

	// Keywords
	CRegistry::DeleteKey(HKEY_CURRENT_USER, REG_KEY_KEYWORDS);
	CRegistry regKeywords(HKEY_CURRENT_USER, REG_KEY_KEYWORDS);

	CStringArray &keywords = SiteList.GetKeywords();
	for (int i = 0; i < keywords.GetSize(); i++) {
		CString sNum;
		sNum.Format(_T("%d"), i + 1);

		CString kw = keywords.GetAt(i);
		regKeywords.Write(sNum, kw);
	}
}
#endif

// Load //////////

BOOL LoadSiteGroup(CSiteItem *item, CRegistry &reg, CArray<CSiteItem*, CSiteItem*> &sites) {
	LOG1(5, "LoadSiteGroup(%p, ,)", item);

	// load sub items
	DWORD cSubKeys;
	reg.QuerySubKeyNumber(&cSubKeys);
	for (DWORD k = 1; k <= cSubKeys; k++) {
		CString sNum;
		sNum.Format(_T("%d"), k);
		CRegistry regItem(reg, sNum);

		CString name = regItem.Read(szName, _T(""));
		int idx = regItem.Read(szIdx, -1);

		CSiteItem *si = NULL;
		if (idx == -1) {
			si = new CSiteItem(item, CSiteItem::Group);
			si->Name = name;
			item->AddItem(si);

			LoadSiteGroup(si, regItem, sites);
		}
		else {
			sites[idx]->Parent = item;
			item->AddItem(sites[idx]);
		}
	}

	return TRUE;
}


BOOL LoadSiteItem(CSiteItem *item, int idx) {
	LOG2(5, "LoadSiteItem(%p, %d)", item, idx);

	CString sRegPath;
	sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx);

	CRegistry reg(HKEY_CURRENT_USER, sRegPath);

	item->Name = reg.Read(szName, _T(""));
	if (item->Type == CSiteItem::Site) {
		CFeedInfo *info = new CFeedInfo();

		info->FileName = reg.Read(szFileName, _T(""));
		info->XmlUrl = reg.Read(szXmlUrl, _T(""));
		info->TodayShow = reg.Read(szTodayShow, TRUE);
#ifdef PRSSR_APP
		info->UseGlobalCacheOptions = reg.Read(szUseGlobalCacheOptions, TRUE);
		info->CacheItemImages = reg.Read(szCacheItemImages, FALSE);
		info->CacheHtml = reg.Read(szCacheHtml, FALSE);
		info->CacheLimit = reg.Read(szCacheLimit, CACHE_LIMIT_DEFAULT);
		info->UpdateInterval = reg.Read(szUpdateInterval, UPDATE_INTERVAL_GLOBAL);
		info->CacheEnclosures = reg.Read(szCacheEnclosures, FALSE);
		info->EnclosureLimit = reg.Read(szEnclosureLimit, 0);
		info->ETag = reg.Read(szETag, _T(""));
		info->LastModified = reg.Read(szLastModified, _T(""));
		info->UserName = reg.Read(szUserName, _T(""));
		info->Password = reg.Read(szPassword, _T(""));

		CString sortItem = reg.Read(szSort, _T(""));
		if (sortItem.CompareNoCase(_T("read")) == 0)
			item->Sort.Item = CSortInfo::Read;
		else
			item->Sort.Item = CSortInfo::Date;

		BOOL sortReversed = reg.Read(szSortReversed, FALSE);		
		if (sortReversed)
			item->Sort.Type = CSortInfo::Descending;
		else
			item->Sort.Type = CSortInfo::Ascending;

		// rewrite rules
		CRegistry regRewriteRules(reg, szRewriteRules);
		DWORD cSubKeys = 0;
		regRewriteRules.QuerySubKeyNumber(&cSubKeys);
		info->RewriteRules.SetSize(cSubKeys);

		for (DWORD i = 0; i < cSubKeys; i++) {
			CString sNum;
			sNum.Format(_T("%02d"), i);
			
			CRegistry regRule(regRewriteRules, sNum);
			CRewriteRule *rule = new CRewriteRule();

			rule->Match = regRule.Read(szMatch, _T(""));
			rule->Replace = regRule.Read(szReplace, _T(""));

			info->RewriteRules.SetAtGrow(i, rule);
		}

		//
		item->CheckFavIcon = reg.Read(szCheckFavIcon, TRUE);
#endif
		item->Info = info;
		item->Feed = NULL;
		item->Status = CSiteItem::Empty;
	}

	return TRUE;
}

BOOL LoadSiteItemUnreadCount(CSiteItem *item, int idx) {
	LOG2(5, "LoadSiteItemUnreadCount(%p, %d)", item, idx);

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		item->UnreadItems = reg.Read(szUnreadCount, 0);
	}

	return TRUE;
}

BOOL LoadSiteItemFlaggedCount(CSiteItem *item, int idx) {
	LOG2(5, "LoadSiteItemFlaggedCount(%p, %d)", item, idx);

	if (item->Type == CSiteItem::Site) {
		CString sRegPath;
		sRegPath.Format(_T("%s\\%d"), REG_KEY_SUBSCRIPTIONS, idx);

		CRegistry reg(HKEY_CURRENT_USER, sRegPath);
		item->FlaggedItems = reg.Read(szFlaggedCount, 0);
	}

	return TRUE;
}

int LoadSiteList(CSiteList &siteList) {
	LOG0(3, "LoadSiteList()");

	CSiteItem *rootItem = new CSiteItem(NULL, CSiteItem::Group);

	// read sites
	DWORD cSites;

	CRegistry reg(HKEY_CURRENT_USER, REG_KEY_SUBSCRIPTIONS);
	reg.QuerySubKeyNumber(&cSites);

	CArray<CSiteItem*, CSiteItem*> sites;
	if (cSites > 0) {
		sites.SetSize(cSites);
		for (DWORD i = 1; i <= cSites; i++) {
			CSiteItem *item = new CSiteItem(NULL, CSiteItem::Site);
			LoadSiteItem(item, i);
			LoadSiteItemUnreadCount(item, i);
			LoadSiteItemFlaggedCount(item, i);
			sites.SetAt(i - 1, item);
		}

		// read hierarchy
		CRegistry regGroup(HKEY_CURRENT_USER, REG_KEY_GROUPS);
		LoadSiteGroup(rootItem, regGroup, sites);
	}

	// read keywords
	DWORD cKeywords;
	CRegistry regKeywords(HKEY_CURRENT_USER, REG_KEY_KEYWORDS);
	regKeywords.QueryValueNumber(&cKeywords);

	CStringArray keywords;
	for (DWORD k = 1; k <= cKeywords; k++) {
		CString sNum;
		sNum.Format(_T("%d"), k);

		CString kword = regKeywords.Read(sNum, _T(""));
		if (!kword.IsEmpty())
			keywords.Add(kword);
	}

	//
	siteList.CreateFrom(rootItem);
	siteList.SetRoot(rootItem);
	siteList.SetKeywords(keywords);

#ifdef PRSSR_APP
	CString strLabel;
	delete siteList.Unread;
	siteList.Unread = new CSiteItem(rootItem, CSiteItem::VFolder);
	siteList.Unread->FlagMask = MESSAGE_UNREAD | MESSAGE_NEW;
	siteList.Unread->Name.LoadString(IDS_UNREAD);
	siteList.Unread->ImageIdx = 1;

	delete siteList.Flagged;
	siteList.Flagged = new CSiteItem(rootItem, CSiteItem::VFolder);
	siteList.Flagged->FlagMask = MESSAGE_FLAG;
	siteList.Flagged->Name.LoadString(IDS_FLAGGED);
	siteList.Flagged->ImageIdx = 1;
#endif

	return 0;
}
