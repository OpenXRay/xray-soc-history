////////////////////////////////////////////////////////////////////////////
//	Module 		: abstract_location_selector.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Abstract location selector
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class CAbstractLocationSelector {
protected:
	bool						m_failed;
	_VertexEvaluator			*m_evaluator;
	_vertex_id_type				m_selected_vertex_id;
	const _Graph				*m_graph;
	u32							m_last_query_time;
	u32							m_query_interval;
	
	IC		void				perform_search				(const _vertex_id_type game_vertex_id);
public:
	IC							CAbstractLocationSelector	();
	IC	virtual					~CAbstractLocationSelector	();
	IC	virtual void			Init						(const _Graph *graph = 0);

	IC			_vertex_id_type get_selected_vertex_id		() const;

	IC			void			set_query_interval			(const u32 query_interval);
	IC			void			set_evaluator				(_VertexEvaluator *evaluator);

	IC			bool			failed						() const;
	IC			bool			actual						(const _vertex_id_type start_vertex_id);
	IC			bool			used						() const;

	IC			void			select_location				(const _vertex_id_type start_vertex_id, _vertex_id_type &dest_vertex_id);
};

#include "abstract_location_selector_inline.h"

template <
	typename _Graph,
	typename _VertexEvaluator,
	typename _vertex_id_type
>
class 
	CBaseLocationSelector :
	public CAbstractLocationSelector <
		_Graph,
		_VertexEvaluator,
		_vertex_id_type
	>
{
};
