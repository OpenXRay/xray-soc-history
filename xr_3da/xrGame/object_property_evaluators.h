////////////////////////////////////////////////////////////////////////////
//	Module 		: object_property_evaluators.h
//	Created 	: 12.03.2004
//  Modified 	: 26.03.2004
//	Author		: Dmitriy Iassenev
//	Description : Object property evaluators
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "property_evaluator_const.h"

class CAI_Stalker;
class CWeapon;
class CMissile;

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorBase
//////////////////////////////////////////////////////////////////////////

template <typename _item_type>
class CObjectPropertyEvaluatorBase : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;
	_item_type		*m_item;

public:
	IC					CObjectPropertyEvaluatorBase		(_item_type *item, CAI_Stalker *owner);
};

typedef CObjectPropertyEvaluatorBase<CGameObject>	CSObjectPropertyEvaluatorBase;
typedef CPropertyEvaluatorConst<CAI_Stalker>		CObjectPropertyEvaluatorConst;

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorState
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorState : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_state;
	bool					m_equality;

public:
							CObjectPropertyEvaluatorState	(CWeapon *item, CAI_Stalker *owner, u32 state, bool equality = true);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorMember
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorMember : public CSObjectPropertyEvaluatorBase {
protected:
	typedef CSObjectPropertyEvaluatorBase inherited;
	bool					*m_member;
	bool					m_equality;

public:
							CObjectPropertyEvaluatorMember	(CGameObject *item, CAI_Stalker *owner, bool *member, bool equality = true);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorAmmo
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorAmmo : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
							CObjectPropertyEvaluatorAmmo	(CWeapon *item, CAI_Stalker *owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorEmpty
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorEmpty : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
							CObjectPropertyEvaluatorEmpty	(CWeapon *item, CAI_Stalker *owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorReady
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorReady : public CObjectPropertyEvaluatorBase<CWeapon> {
protected:
	typedef CObjectPropertyEvaluatorBase<CWeapon> inherited;
	u32						m_ammo_type;

public:
							CObjectPropertyEvaluatorReady	(CWeapon *item, CAI_Stalker *owner, u32 ammo_type);
	virtual _value_type		evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorNoItems
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorNoItems : public CPropertyEvaluator<CAI_Stalker> {
protected:
	typedef CPropertyEvaluator<CAI_Stalker> inherited;

public:
						CObjectPropertyEvaluatorNoItems	(CAI_Stalker *owner);
	virtual _value_type	evaluate						();
};

//////////////////////////////////////////////////////////////////////////
// CObjectPropertyEvaluatorMissile
//////////////////////////////////////////////////////////////////////////

class CObjectPropertyEvaluatorMissile : public CObjectPropertyEvaluatorBase<CMissile> {
protected:
	typedef CObjectPropertyEvaluatorBase<CMissile> inherited;
	u32						m_state;
	bool					m_equality;

public:
							CObjectPropertyEvaluatorMissile	(CMissile *item, CAI_Stalker *owner, u32 state, bool equality = true);
	virtual _value_type		evaluate						();
};

#include "object_property_evaluators_inline.h"