////////////////////////////////////////////////////////////////////////////
//	Module 		: script_motivation_action_manager_wrapper.cpp
//	Created 	: 28.03.2004
//  Modified 	: 28.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script manager action manager wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_motivation_action_manager_wrapper.h"
#include "script_game_object.h"

CScriptMotivationActionManagerWrapper::~CScriptMotivationActionManagerWrapper	()
{
}

void CScriptMotivationActionManagerWrapper::reinit			(_object_type *object, bool clear_all)
{
	call_member<void>				("reinit",object,clear_all);
}

void CScriptMotivationActionManagerWrapper::reinit_static	(CScriptMotivationActionManager *manager, CScriptGameObject *object, bool clear_all)
{
	manager->CScriptMotivationActionManager::reinit		(object,clear_all);
}

void CScriptMotivationActionManagerWrapper::Load			(LPCSTR section)
{
	call_member<void>				("load",section);
}

void CScriptMotivationActionManagerWrapper::Load_static		(CScriptMotivationActionManager *manager, LPCSTR section)
{
	manager->CScriptMotivationActionManager::Load		(section);
}

void CScriptMotivationActionManagerWrapper::reload			(LPCSTR section)
{
	call_member<void>				("reload",section);
}

void CScriptMotivationActionManagerWrapper::reload_static	(CScriptMotivationActionManager *manager, LPCSTR section)
{
	manager->CScriptMotivationActionManager::reload	(section);
}

void CScriptMotivationActionManagerWrapper::update			()
{
	call_member<void>		("update");
}

void CScriptMotivationActionManagerWrapper::update_static	(CScriptMotivationActionManager *manager)
{
	manager->CScriptMotivationActionManager::update	();
}