////////////////////////////////////////////////////////////////////////////
//	Module 		: script_binder_object_wrapper.cpp
//	Created 	: 29.03.2004
//  Modified 	: 29.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Script object binder wrapper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_binder_object_wrapper.h"
#include "script_game_object.h"
#include "xrServer_Objects_ALife.h"
#include "net_utils.h"

CScriptBinderObjectWrapper::CScriptBinderObjectWrapper	(CScriptGameObject *object) :
	CScriptBinderObject	(object)
{
}

CScriptBinderObjectWrapper::~CScriptBinderObjectWrapper ()
{
}

void CScriptBinderObjectWrapper::reinit					()
{
	try {
		luabind::call_member<void>		(this,"reinit");
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::reinit_static			(CScriptBinderObject *script_binder_object)
{
	script_binder_object->CScriptBinderObject::reinit	();
}

void CScriptBinderObjectWrapper::reload					(LPCSTR section)
{
	try {
		luabind::call_member<void>		(this,"reload",section);
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::reload_static			(CScriptBinderObject *script_binder_object, LPCSTR section)
{
	script_binder_object->CScriptBinderObject::reload	(section);
}

bool CScriptBinderObjectWrapper::net_Spawn				(SpawnType DC)
{
	try {
		return							(luabind::call_member<bool>(this,"net_spawn",DC));
	}
	catch(...) {
		return							(false);
	}
}

bool CScriptBinderObjectWrapper::net_Spawn_static		(CScriptBinderObject *script_binder_object, SpawnType DC)
{
	return							(script_binder_object->CScriptBinderObject::net_Spawn(DC));
}

void CScriptBinderObjectWrapper::net_Destroy			()
{
	try {
		luabind::call_member<void>		(this,"net_destroy");
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::net_Destroy_static		(CScriptBinderObject *script_binder_object)
{
	script_binder_object->CScriptBinderObject::net_Destroy();
}

void CScriptBinderObjectWrapper::net_Import				(NET_Packet *net_packet)
{
	try {
		luabind::call_member<void>		(this,"net_import",net_packet);
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::net_Import_static		(CScriptBinderObject *script_binder_object, NET_Packet *net_packet)
{
	script_binder_object->CScriptBinderObject::net_Import	(net_packet);
}

void CScriptBinderObjectWrapper::net_Export				(NET_Packet *net_packet)
{
	try {
		luabind::call_member<void>		(this,"net_export",net_packet);
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::net_Export_static		(CScriptBinderObject *script_binder_object, NET_Packet *net_packet)
{
	script_binder_object->CScriptBinderObject::net_Export	(net_packet);
}

void CScriptBinderObjectWrapper::shedule_Update			(u32 time_delta)
{
	try {
		luabind::call_member<void>		(this,"update",time_delta);
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::shedule_Update_static	(CScriptBinderObject *script_binder_object, u32 time_delta)
{
	script_binder_object->CScriptBinderObject::shedule_Update	(time_delta);
}

void CScriptBinderObjectWrapper::save					(NET_Packet *output_packet)
{
	try {
		luabind::call_member<void>		(this,"save",output_packet);
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::save_static			(CScriptBinderObject *script_binder_object, NET_Packet *output_packet)
{
	script_binder_object->CScriptBinderObject::save		(output_packet);
}

void CScriptBinderObjectWrapper::load					(IReader *input_packet)
{
	try {
		luabind::call_member<void>		(this,"load",*input_packet);
	}
	catch(...) {
	}
}

void CScriptBinderObjectWrapper::load_static			(CScriptBinderObject *script_binder_object, IReader *input_packet)
{
	script_binder_object->CScriptBinderObject::load		(input_packet);
}

bool CScriptBinderObjectWrapper::net_SaveRelevant		()
{
	try {
		return							(luabind::call_member<bool>(this,"net_save_relevant"));
	}
	catch(...) {
		return							(false);
	}
}

bool CScriptBinderObjectWrapper::net_SaveRelevant_static(CScriptBinderObject *script_binder_object)
{
	return							(script_binder_object->CScriptBinderObject::net_SaveRelevant());
}
