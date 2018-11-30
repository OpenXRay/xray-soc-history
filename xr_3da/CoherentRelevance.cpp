// CoherentRelevance.cpp: implementation of the CCoherentRelevance class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CoherentRelevance.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

template <class T>
void CCoherentRelevance<T>::Update()
{
	if (objects.empty()) return;
	for (int i=0; i<refresh_count; i++) {
		Fvector*	P = Query(current_pos);
		float		D = Device.vCameraPosition.distance_to_sqr(*P);

		if	(D<distance)	relevant.insert	(current_pos);
		else				relevant.erase	(current_pos);

		if (++current_pos >= objects.size()) current_pos=0;
	}
}

template <class T>
void CCoherentRelevance::Opush	(T& v)
{
	objects.push_back(v);
	Fvector*	P = Query(current_pos);
	float		D = Device.vCameraPosition.distance_to_sqr(*P);

	if	(D<distance)	relevant.insert	(current_pos);
}

template <class T>
void CCoherentRelevance::Oremove (T& v)
{
	OBJ_IT	it = find(objects.begin(),objects.end(),v);
	VERIFY(it!=objects.end());
	objects.erase(it);
	int		id	= it-objects.begin();
	int		size= objects.size();
	current_pos = id;

	for (; id<size; id++)	relevant.erase(id);

	Update	();
}
