// NetworkClient.h: interface for the CNetworkClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NETWORKCLIENT_H__583020DC_7193_461A_B17F_5CE8415AB08B__INCLUDED_)
#define AFX_NETWORKCLIENT_H__583020DC_7193_461A_B17F_5CE8415AB08B__INCLUDED_
#ifndef NO_XR_NETWORK
#pragma once

#include "Network.h"

struct ENGINE_API	DPSessionInfo
{
	GUID			guidSession;
	TCHAR			szSession		[256];
	DPSessionInfo	*next;

	DPSessionInfo	(DPSessionInfo* tail)
	{ next=tail; }
	~DPSessionInfo	()
	{ if (next) delete next; }
};

class ENGINE_API CNetworkClient :
	public CNetwork,
	public pureFrame
{
	friend class ENGINE_API CApplication;
	friend BOOL FAR PASCAL _EnumSessionsCallback	(LPCDPSESSIONDESC2,DWORD,DWORD,VOID*);
	friend BOOL FAR PASCAL _EnumPlayersCallback		(DPID dpId, DWORD dwPlayerType, LPCDPNAME lpName, DWORD dwFlags, LPVOID lpContext);
private:
	BOOL				bConnected;
	BOOL				bMessageComplited;	// setted to false after GetMessage
	DPID				dpIDFrom;
public:
	DPSessionInfo*		RefreshSessions	(DPSessionInfo* old);
	BOOL				Join			(char *name, GUID session);
	void				AfterConnect	(void);
	void				Disconnect		(void);

	// message control
	DWORD				SendMsg			(	// send -  returns timestamp
		DWORD dwType, void *data=0, DWORD size=0,
		BOOL bGuaranty=false, DWORD *pID=0);		// pID - internal message handle
	DWORD				SendMsgTo		(		// send -  returns timestamp
		DPID idTO, DWORD dwType, void *data=0, DWORD size=0,
		BOOL bGuaranty=false, DWORD *pID=0);
	void				CancelMessage	( DWORD ID );

	// receive
	DWORD				GetMsg			(DWORD *timestamp=0);
	void*				GetMsgData		(DWORD *size=0);
	DPID				GetMsgFrom		(void) { return dpIDFrom; }
	void				EndMsg			(void);


	// some inherited functions
	virtual void		OnPlayerCreate			(DPID id);
	virtual void		OnPlayerDestroy			(DPID id);
	virtual void		OnFrame					();
	void				Show					();

	CNetworkClient();
	virtual ~CNetworkClient();

};

extern ENGINE_API CNetworkClient* pNetwork;
#endif
#endif // !defined(AFX_NETWORKCLIENT_H__583020DC_7193_461A_B17F_5CE8415AB08B__INCLUDED_)
