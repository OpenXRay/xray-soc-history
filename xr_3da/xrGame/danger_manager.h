////////////////////////////////////////////////////////////////////////////
//	Module 		: danger_manager.h
//	Created 	: 11.02.2005
//  Modified 	: 11.02.2005
//	Author		: Dmitriy Iassenev
//	Description : Danger manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "danger_object.h"

namespace MemorySpace {
	struct CVisibleObject;
	struct CSoundObject;
	struct CHitObject;
};

class CDangerManager  {
public:
	typedef xr_vector<CDangerObject>		OBJECTS;
	typedef OBJECTS							DANGERS;

public:
	typedef MemorySpace::CVisibleObject		CVisibleObject;
	typedef MemorySpace::CSoundObject		CSoundObject;
	typedef MemorySpace::CHitObject			CHitObject;

private:
	DANGERS				m_objects;
	const CDangerObject	*m_selected;
	u32					m_time_line;

private:
	CCustomMonster		*m_object;

public:
	IC					CDangerManager		(CCustomMonster *object);
	virtual				~CDangerManager		();
	virtual void		Load				(LPCSTR section);
	virtual void		reinit				();
	virtual void		reload				(LPCSTR section);
	virtual void		update				();
	virtual bool		useful				(const CDangerObject &object) const;
	virtual bool		is_useful			(const CDangerObject &object) const;
	virtual	float		evaluate			(const CDangerObject &object) const;
	virtual	float		do_evaluate			(const CDangerObject &object) const;
			void		remove_links		(const CObject *object);
	IC		void		reset				();

public:
			void		add					(const CVisibleObject &object);
			void		add					(const CSoundObject &object);
			void		add					(const CHitObject &object);
			void		add					(const CDangerObject &object);

public:
	IC		const CDangerObject	*selected	() const;
	IC		const OBJECTS		&objects	() const;
	IC		u32			time_line			() const;
	IC		void		time_line			(u32 value);
};

#include "danger_manager_inline.h"