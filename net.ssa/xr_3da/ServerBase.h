// ServerBase.h: interface for the CServerBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERBASE_H__471120ED_1A50_4A90_94CC_0C0273C8032C__INCLUDED_)
#define AFX_SERVERBASE_H__471120ED_1A50_4A90_94CC_0C0273C8032C__INCLUDED_
#pragma once

class ENGINE_API CServerEntity
{

};

class ENGINE_API CServerBase  
{
	char	cLevelName;
public:
	CServerBase();
	virtual ~CServerBase();
};

#endif // !defined(AFX_SERVERBASE_H__471120ED_1A50_4A90_94CC_0C0273C8032C__INCLUDED_)
