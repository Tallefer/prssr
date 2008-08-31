// FeedSync.h: interface for the CFeedSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FEEDSYNC_H__7EB2DE24_9B84_49C1_983F_A7779957FF02__INCLUDED_)
#define AFX_FEEDSYNC_H__7EB2DE24_9B84_49C1_983F_A7779957FF02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDownloader;
class CSiteItem;
class CFeed;
class CFeedItem;

class CFeedSync {
public:
	CFeedSync(CDownloader *downloader);
	virtual ~CFeedSync();

	virtual BOOL NeedAuth() = 0;
	virtual BOOL Authenticate() = 0; 
	virtual BOOL SyncFeed(CSiteItem *si, CFeed *feed, BOOL updateOnly) = 0;
	virtual BOOL MergeFeed(CSiteItem *si, CFeed *feed, CArray<CFeedItem *, CFeedItem *> &newItems, CArray<CFeedItem *, CFeedItem *> &itemsToClean) = 0;
	virtual BOOL DownloadFeed(CString &url, const CString &fileName) = 0;

	CString GetErrorMsg() { return ErrorMsg; }

protected:
	CDownloader *Downloader;

	/// error
	CString ErrorMsg;
};

#endif // !defined(AFX_FEEDSYNC_H__7EB2DE24_9B84_49C1_983F_A7779957FF02__INCLUDED_)