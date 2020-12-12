// NetworkClient.cpp: implementation of the CNetworkClient class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "net_messages.h"
#include "x_ray.h"
#include "xr_smallfont.h"
#include "NetworkClient.h"

#ifndef NO_XR_NETWORK
ENGINE_API CNetworkClient* pNetwork;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNetworkClient::CNetworkClient() : CNetwork()
{
	//////////////////////////////
	SelectProtocol	(dpTCP);	//  Invalid place for this code
	ApplyProtocol	();			//  but...it's ok for now
	//////////////////////////////
	Device.seqFrame.Add(this);
	bConnected		= false;
}

CNetworkClient::~CNetworkClient()
{
	Disconnect();
	Device.seqFrame.Remove(this);
}

BOOL FAR PASCAL _EnumSessionsCallback( LPCDPSESSIONDESC2 pdpsd,
                                       DWORD* pdwTimeout,
                                       DWORD dwFlags,
                                       VOID* pvContext )
{
    if( dwFlags & DPESC_TIMEDOUT ) return FALSE; // The enumeration has timed out, so stop the enumeration.

    // Found a good session, save it
	DPSessionInfo**	P	= (DPSessionInfo**)pvContext;
	DPSessionInfo*	S	= new DPSessionInfo(*P);
	*P					= S;

    S->guidSession = pdpsd->guidInstance;
    sprintf( S->szSession, "%s (%d/16)", pdpsd->lpszSessionNameA, pdpsd->dwCurrentPlayers );
    return TRUE;
}

DPSessionInfo* CNetworkClient::RefreshSessions(DPSessionInfo* old)
{
	_DELETE				(old);
	DPSessionInfo*		pHead=NULL;

	DPSESSIONDESC2		dpsd;
	ZeroMemory			( &dpsd, sizeof(dpsd) );
	dpsd.dwSize         = sizeof(dpsd);
	dpsd.guidApplication= g_AppGUID;
	if (FAILED(pDPlay->EnumSessions(&dpsd,0,_EnumSessionsCallback, &pHead, DPENUMSESSIONS_ASYNC|DPENUMSESSIONS_ALL)))
	{
		_DELETE(pHead);
		return NULL;
	}
	return pHead;
}


BOOL FAR PASCAL _EnumPlayersCallback(
  DPID dpId,
  DWORD dwPlayerType,
  LPCDPNAME lpName,
  DWORD dwFlags,
  LPVOID lpContext
)
{
	// skip funky or unneed players
	if (dwPlayerType != DPPLAYERTYPE_PLAYER)		return TRUE;
	if ((dwFlags & DPENUMPLAYERS_REMOTE)==0)		return TRUE;
	if ((dwFlags & DPENUMPLAYERS_SERVERPLAYER)!=0)	return TRUE;
	if ((dwFlags & DPENUMPLAYERS_SPECTATOR)!=0)		return TRUE;

	CNetworkClient* pClient = (CNetworkClient*)lpContext;

	SPlayer		P;
	strcpy		(P.name,lpName->lpszShortNameA);
	P.dwPing	= 15;		// defaults - 15ms
	P.id		= dpId;
	pClient->players.push_back(P);
	std::sort(pClient->players.begin(),pClient->players.end());
	pClient->OnPlayerCreate(P.id);

	return TRUE;
}

BOOL CNetworkClient::Join(char *name, GUID session)
{
	DPSESSIONDESC2	dpsd;
	DPNAME			dpname;

	Log("* NET: Join");
	// Setup the DPSESSIONDESC2, and get the session guid from
	// the selected listbox item
	ZeroMemory				( &dpsd, sizeof(dpsd) );
	dpsd.dwSize				= sizeof(dpsd);
	dpsd.dwFlags			= DPSESSION_OPTIMIZELATENCY | DPSESSION_DIRECTPLAYPROTOCOL | DPSESSION_NODATAMESSAGES;
	dpsd.guidInstance		= session;
	dpsd.guidApplication	= g_AppGUID;

	// Join the session
	R_CHK					(pDPlay->Open( &dpsd, DPOPEN_JOIN ));

	// Store the player's DPID in LocalPlayerID.
	ZeroMemory				( &dpname, sizeof(DPNAME) );
	dpname.dwSize			= sizeof(DPNAME);
	dpname.lpszShortNameA	= name;

	R_CHK					(pDPlay->CreatePlayer( &dpLocalPlayerID, &dpname, NULL, NULL, 0, 0 ));

	// waiting for game time
	Log("* NET: Syncronizing...");
	bMessageComplited	= true;
	fTime				= 0;
	bConnected			= TRUE;
	tDiff				= timeGetTime();
	return true;
}

void CNetworkClient::AfterConnect()
{
	// Enum remote players
	R_CHK(pDPlay->EnumPlayers( NULL, _EnumPlayersCallback, this, DPENUMPLAYERS_REMOTE ));
}

void CNetworkClient::Disconnect()
{
	// Cleanup DirectPlay
	if (bConnected) {
		R_ASSERT	(dpLocalPlayerID);
		R_CHK		(pDPlay->DestroyPlayer(dpLocalPlayerID));
		R_CHK		(pDPlay->Close());
		bConnected	= FALSE;
	}
	_RELEASE( pDPlay				);
	_FREE	( pConnection			);

	CreateDPlay	( );
}

DWORD CNetworkClient::GetMsg(DWORD *timestamp)
{
	DPID    idTo;
	DWORD   dwSize;
	HRESULT hrr;

	if (!bConnected) return MSG_NONE;

	if (bMessageComplited) {
once_more:
		// See what's out there
		dpIDFrom	= 0;
		idTo		= 0;

		dwSize	= 1024;
		hrr = pDPlay->Receive( &dpIDFrom, &idTo, DPRECEIVE_ALL, MessageData, &dwSize );

		if( hrr == DPERR_NOMESSAGES )	return MSG_NONE;
		else							R_CHK	(hrr);

		// Handle the net_messages. If its from DPID_SYSMSG, its a system message,
		// otherwise its an application message.
		if( dpIDFrom == DPID_SYSMSG ) {
			HandleSystemMessages( (DPMSG_GENERIC*)MessageData );
			goto once_more;
		} else {
//			PreProcessMessages	( (GAMEMSG*)MessageData, dpIDFrom);
		}
		if (bMessageComplited) {
			GAMEMSG *M = (GAMEMSG *)MessageData;
			dwRSize = dwSize-sizeof(GAMEMSG);
//			net_LogMessage("GET",M->dwType);
			bMessageComplited = false;
		} else {
			// it was system or unknown message
			// cycle to get next message if avail
			goto once_more;
		}
	}
	GAMEMSG *M = (GAMEMSG *)MessageData;
	if (timestamp) *timestamp = M->dwTimeStamp;
	return M->dwType;
}

void * CNetworkClient::GetMsgData(DWORD *size)
{
	VERIFY(!bMessageComplited);
	if (size) *size = dwRSize;
	return MessageData+sizeof(GAMEMSG);
}

void CNetworkClient::EndMsg()
{
	bMessageComplited = true;
}

void CNetworkClient::Show()
{
#ifdef DEBUG
	pApp->pFont->Size(.02f);
	pApp->pFont->Color(0xffffffff);
	pApp->pFont->Out(-.1f,-.6f,"NET_Timestamp: %d",GetTimestamp());
	for (int i=0; i<players.size(); i++)
		pApp->pFont->Out(0,-.5f,"%20s %d",players[i].name,players[i].dwPing);
#endif
}

void CNetworkClient::OnPlayerCreate(DPID id) {
	pApp->OnNetPlayerCreate(id);
}
void CNetworkClient::OnPlayerDestroy(DPID id) {
	pApp->OnNetPlayerDestroy(id);
}
void CNetworkClient::OnFrame()
{
	if (bConnected)
		CNetwork::OnMove(Device.fTimeDelta);
}

DWORD CNetworkClient::SendMsg(	// returns timestamp
	DWORD dwType,
	void *data, DWORD size,
	BOOL bGuaranty,
	DWORD *pID)
{
	return SendMsgTo(DPID_ALLPLAYERS,dwType,	data, size, bGuaranty, pID);
}

DWORD CNetworkClient::SendMsgTo(	// returns timestamp
	DPID  idTO,
	DWORD dwType,
	void *data, DWORD size,
	BOOL bGuaranty,
	DWORD *pID)
{
	VERIFY(size<1000	);

    GAMEMSG *msg	= (GAMEMSG *)MessageData;
    msg->dwType		= dwType;
	msg->dwTimeStamp= GetTimestamp();
	if (data && size)	CopyMemory	(MessageData+sizeof(GAMEMSG),data,size);
	else				{ data = 0; size=0; }

	// Send it to player(s)
	HRESULT hrr = pDPlay->Send(
		dpLocalPlayerID,
		idTO,
		bGuaranty?DPSEND_GUARANTEED:0,
		MessageData,
		sizeof(GAMEMSG)+size
	);
	if ((hrr!=0)&&(hrr!=DPERR_PENDING)) {
		InterpretError(hrr,__FILE__,__LINE__);
	}
//	net_LogMessage("SEND",dwType);
	return msg->dwTimeStamp;
}

void CNetworkClient::CancelMessage(DWORD ID) {
	pDPlay->CancelMessage(ID,0);
}
#endif