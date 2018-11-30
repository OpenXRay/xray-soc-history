// CoherentRelevance.h: interface for the CCoherentRelevance class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COHERENTRELEVANCE_H__B046E60D_93B8_4577_A427_3F2FB1449F08__INCLUDED_)
#define AFX_COHERENTRELEVANCE_H__B046E60D_93B8_4577_A427_3F2FB1449F08__INCLUDED_
#pragma once

template <class T>
class ENGINE_API CCoherentRelevance  
{
protected:
	float		distance;
	int			refresh_count;
	int			current_pos;
	vector<T>	objects;
	set<int>	relevant;
public:
	typedef set<int>::iterator	REL_IT;
	typedef vector<T>::iterator OBJ_IT;

	// Settings and initialization
	void				Init		(float D, int R) 
	{
		distance		= D*D;
		refresh_count	= R;
		current_pos		= 0;
	}

	// Main task
	void					Update	();
	virtual const Fvector&	Query	(T& Object) = 0;

	// Container interface - relevant
	IC REL_IT			Rbegin	()	{ return relevant.begin();	}
	IC REL_IT			Rend	()	{ return relevant.end();	}
	IC int				Rsize	()	{ return relevant.size();	}

	// Container interface - objects
	IC OBJ_IT			Obegin	()	{ return objects.begin();	}
	IC OBJ_IT			Oend	()	{ return objects.end();		}
	IC int				Osize	()	{ return objects.size();	}
	IC T&				Oget	(int id) { return objects[id];	}
	void				Opush	(T& v);
	void				Oremove (T& v);
	void				Oclear	();

};

template <class T>
void CCoherentRelevance<T>::Update()
{
	if (objects.empty()) return;
	int start_pos = current_pos;
	for (int i=0; i<refresh_count; i++) {
		float		D = Device.vCameraPosition.distance_to_sqr(Query(objects[current_pos]));

		if	(D<distance)	relevant.insert	(current_pos);
		else				relevant.erase	(current_pos);

		if (++current_pos >= objects.size()) current_pos=0;
		if (current_pos == start_pos) break;
	}
}

template <class T>
void CCoherentRelevance<T>::Opush	(T& v)
{
	objects.push_back(v);
	float		D = Device.vCameraPosition.distance_to_sqr(Query(objects[current_pos]));

	if	(D<distance)	relevant.insert	(current_pos);
}

template <class T>
void CCoherentRelevance<T>::Oremove (T& v)
{
	OBJ_IT	it = find(objects.begin(),objects.end(),v);
	VERIFY(it!=objects.end());
	objects.erase(it);
	int		id	= it-objects.begin();
	int		size= relevant.size();
	current_pos = (id>=objects.size())?0:id;

	for (; id<size; id++)	relevant.erase(id);

	Update	();
}

template <class T>
void CCoherentRelevance<T>::Oclear ()
{
	objects.clear();
	relevant.clear();
	current_pos = 0;
}

#endif // !defined(AFX_COHERENTRELEVANCE_H__B046E60D_93B8_4577_A427_3F2FB1449F08__INCLUDED_)
