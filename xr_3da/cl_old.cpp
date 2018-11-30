/*
private:
	int	 __forceinline		Collide			( CCFModel *object, CCFModel *target, bool bAll = false );
public:	
	// Определяет есть ли столкновение в basic_pos объекта
	BOOL					Collision		( CCFModel *object, bool bAll = false, bool bGetTris = false, Fvector* dir = NULL, float range = 0.0f );
	BOOL					GetFieldTris	( Fvector &start, Fvector &dir,	float radius, float range );
	int						GetNearestObject( CCFModel* object, Fvector &point, float range );

*/

int CObjectSpace::GetNearestObject( CCFModel* object, Fvector &point, float range )
{
	nearest_list.clear	( );
	Irect				rect;
	GetRect				( point, rect, range );

	object->Enable		( false );
	CCFModel*		target;
	int 				ix, iz, q;
	for (ix=rect.x1;ix<=rect.x2;ix++)
		for (iz=rect.y1;iz<=rect.y2;iz++)
			for (q=0;q<slot_list(ix, iz).objects.count;q++){
				target	= slot_list(ix, iz).objects[q];
				if (!target->owner)			continue;
				if (!target->enabled) 		continue;
				nearest_list.insert( target );
			}

//	nearest_list.DelDup	( );
	object->EnableRollback( );
	return nearest_list.size();
}

void  CObjectSpace::AlignToGrid( Fvector &v )
{
	v.x = (Trans(v.x)+0.5f)*slot_size;
	v.z = (Trans(v.z)+0.5f)*slot_size;
}

void __forceinline _setMatIdent33(float dest[3][3]){
	CopyMemory			( dest, &ident33, sizeof(float)*9);
}

BOOL CObjectSpace::Collision( CCFModel* object, bool bAll, bool bGetTris, Fvector* dir, float range )
{
	_CHECK				( object );
	VERIFY				( object->owner );

	collide_list.Clear	( );
	tris_list.Clear		( );

	Fvector				point;

	if ( dir ){
		point.mul		( *dir, range );
		point.add		( object->owner->Position() );
	}else{
		point.set		( object->owner->Position() );
	}

	object->Enable		( false );
//	int res = GetNearest(point, object->in_radius);
	int res = GetNearest(point, object->objRadius);
	object->EnableRollback( );
	_CHECK				( res );

	Fmatrix33	R1, R2;
	Fvector		T1, T2;
	T1.set		( 0,0,0 );
	T2.set		( 0,0,0 );
	CCFModel*		target;


	object->owner->mTransform.get_rapid(R1);
	object->owner->mTransform.get_translate(T1);
	if ( dir ) T1.set( point );

	for ( set<CCFModel*>::iterator nl_idx=nearest_list.begin(); nl_idx!=nearest_list.end(); nl_idx++ ){
		target			= *nl_idx;

		if (!object->owner->OnNear(target)) continue;

		target->owner->mTransform.get_rapid(R2);
		target->owner->mTransform.get_translate(T2);

		XRC.Collide	(  R1,	T1,	1.0f, &object->model,
			           R2,	T2,	1.0f, &target->model);

		if ( XRC.GetModelContactCount() ){
			if (!object->owner->OnCollide(target)) continue;
			collide_list.Add( target );
			if (bGetTris)
				for (int i=0; i<XRC.GetModelContactCount(); i++)
					AddToTrisList( target, &target->model.tris[XRC.ModelContact[i].id2] );
		}
	}

	return collide_list.count;
}

int CObjectSpace::Collide( CCFModel *object, CCFModel *target, bool bAll )
{
	Fmatrix33	R1, R2;
	Fvector		T1, T2;

	object->owner->mTransform.get_rapid(R1);
	object->owner->mTransform.get_translate(T1);
	target->owner->mTransform.get_rapid(R2);
	target->owner->mTransform.get_translate(T2);

	XRC.Collide(R1,	T1,	1.0f, &object->model,
				R2,	T2,	1.0f, &target->model);

	return XRC.GetModelContactCount();
}

BOOL CObjectSpace::GetFieldTris( Fvector &start, Fvector &dir,	float radius, float range )
{
	tris_list.Clear		( );
	if ( !GetField		( start, dir, radius, range ) ) _FAIL;

	for ( set<CCFModel*>::iterator I=nearest_list.begin(); I!=nearest_list.end(); I++)
		AddToTrisList	( *I, NULL );

	return ( tris_list.count );
}

/*
BOOL  CObjectSpace::RayPick( Fvector &start, Fvector &dir, float* range, bool bFirstContact )
{
	dwRPPC++;
	collide_list.Clear	( );
	tris_list.Clear		( );
	tris_no.Clear		( );

	float	max_range	= (range && (*range) > 0)?*range:MAX_TEST_RANGE;

	if ( !GetField( start, dir,	0, max_range ) ) _FAIL;

	tri					ray[2];
	RAPID_model			model;

	Fvector				p1, p2, p3, end;

	end.direct			(start,dir,max_range);

	p1.set( start );	p2.set( end );		p3.set( end );
						p2.y += 0.01f; 		p3.y -= 0.01f;
	ray[0].P1.set(p1);	ray[0].P2.set(p2);	ray[0].P3.set(p3);
	p2.y -= 0.01f; 		p3.y += 0.01f; 		p2.x += 0.01f; 		p3.x -= 0.01f;
	p1.get( ray[1].p1 );p2.get( ray[1].p2 );p3.get( ray[1].p3 );

// create RAPID model
	model.BeginModel	( );
	model.AddAllTris	( 1, &ray[1] );
	model.EndModel		( );

	CCFModel*			target;
	CCFModel*			result = NULL;
	int					id = 0;
	float				R1[3][3];
	float				R2[3][3];
	Fvector				T1, T2;

	T1.set				( 0,0,0 );
	_setMatIdent33		( R1 );

// определение столкновения
	for ( int nl_idx=0; nl_idx<nearest_list.count; nl_idx++ ){
		target			= nearest_list[nl_idx];

		if (target->owner && !target->owner->OnRayPicked( )){
			nearest_list.DelIndex(nl_idx);
			continue;
		}

// AlexMX. для прорисовки в рендере всей модели
//		AddToTrisList( target, NULL );

		_SET_RAPID_MATRIX( target, T2, R2 );

		RAPID_Collide( R1,	T1.m,	1.0f, &model,
					   R2,	T2.m,	1.0f, &target->model,
					   RAPID_ALL_CONTACTS );
		RAPID_RayPick(R2, T2.m, 1.0f, &target->model,
					  start.m, dir.m);

		if ( RAPID_num_contacts ){

			Fvector		N, P;
			float		A, D, t;

// AlexMX. для прорисовки в рендере всех пораженных лучом трианглов
//			for ( int j = 0; j < RAPID_num_contacts; j++)
//				AddToTrisList( target, &target->model.tris[RAPID_contact[j].id2] );

			for ( int i = 0; i < RAPID_num_contacts; i++){
				int idx = RAPID_contact[i].id2;
				N.set	( target->model.tris[idx].n );
				if (target->owner) N.transform_dir( target->owner->mTransform );

				A = N.dotproduct( dir );
				if ( A < 0.0f )
				{
					P.set	( target->model.tris[idx].p1 );
					if (target->owner) P.transform_tiny( target->owner->mTransform );
					D = - N.dotproduct( P );
					t = -(N.dotproduct( start ) + D) / A;
					if (!range || bFirstContact){
						result = target;
						id = RAPID_contact->id2;
						break;
					}
					if ( (t < *range) && (t>=0)){
						*range	= t;
						id		= idx;
						result	= target;
					}
				}
			}
		}
	}

	if ( result ){
		collide_list.Add( result );
		AddToTrisList( result, &result->model.tris[id] );
		tris_no.Add(id);
	}

	return collide_list.count;
}
*/
/*
построение матрицы

Fvector				vWorldUp, vView;

// set rotate matrix
	vWorldUp.set		( 0, 1, 0 );
	vView.set			( dir );

	Fvector				vUp, vRight;

	FLOAT fDotProduct	= vWorldUp.dotproduct( vView );
	vUp.mul				( vView, -fDotProduct );
	vUp.add				( vWorldUp );
	vUp.normalize		( );
	vRight.crossproduct	( vUp, vView );

	R1[0][0] = vRight.x;  R1[0][1] = vUp.x;  R1[0][2] = vView.x;
	R1[1][0] = vRight.y;  R1[1][1] = vUp.y;  R1[1][2] = vView.y;
	R1[2][0] = vRight.z;  R1[2][1] = vUp.z;  R1[2][2] = vView.z;
*/

/*
отталкивание от каждого треугольника
				_SET_RAPID_MATRIX( object, T1, R1 );
				for (int nl_idx=0; nl_idx<nearest_list.count; nl_idx++){
					t_list.Clear( );
					target	= nearest_list[nl_idx];
					_SET_RAPID_MATRIX( target, T2, R2 );
					RAPID_Collide(	R1,	T1.m,	1.0f, &object->model,
									R2,	T2.m,	1.0f, &target->model, RAPID_ALL_CONTACTS );
					if ( !RAPID_num_contacts ) continue;

					for ( int i=0; i<RAPID_num_contacts; i++ )
						t_list.Add( &target->model.tris[RAPID_contact[i].id2] );
					t_list.DelDup( );

					for ( int tl_idx=0; tl_idx<t_list.count; tl_idx++ ){
						N.set	( t_list[tl_idx]->n );
						if (target->owner) N.transform_dir(target->owner->mTransform);
						Fvector back;
						back.mul(N,0.1f*Device.fTimeDelta);
						motion->add (back);
					}
				}

расчет расстояния до треугольника
					Fvector	back;
					float	D, dist_tgt, dist_obj, dist;
					D		= - N.dotproduct(P);
//					dist	= -(N.dotproduct(object->objCenter) + D) / dot;
					dist_tgt= N.dotproduct(object->objCenter) + D;

					if (dist_tgt<=0) continue;

					dist = dist_obj - dist_tgt;

					back.set( N );
					back.mul( dist );
					motion->add( back );
*/

// слайд вектор
//					Fvector slide;
//					slide.mul		( N, -dot );
//					motion->add		( slide );
//					float range		= motion->magnitude();
//					if (range<MIN_COLLIDE_RANGE){ bStop = true; motion->set(0,0,0); break;}

//	Msg("- start %f %f %f\n", start.x, start.y, start.z);
//	Msg("- start %f %f %f\n", start.x, start.y, start.z);
//	Msg("- start %f %f %f\n", start.x, start.y, start.z);
//	Msg("- start %f %f %f\n", start.x, start.y, start.z);
