#pragma once

class ENGINE_API CAI_Frustum
{
public:
	Fvector	from;
	Fvector	dir;
	float	fov_cos;	// degree of conuse (like 180)
	float	sight_sq;	// maximum distance (like 100m)

	IC void set_fov	(float degrees)
	{
		fov_cos = cosf(deg2rad(degrees/2));
	}
	IC void set_sight	(float sight)
	{
		sight_sq = sight*sight;
	}
	IC BOOL  visible_point(Fvector &C)
	{
		Fvector f2o_dir;
		float	f2o_dist;
		
		f2o_dir.sub(C,from);
		f2o_dist = f2o_dir.square_magnitude();
		
		if (f2o_dist > sight_sq) return FALSE;
		
		f2o_dir.div(sqrtf(f2o_dist));
		if (f2o_dir.dotproduct(dir)<fov_cos) return FALSE;

		return TRUE;
	}
	IC BOOL	visible_quad(Fvector &p1, Fvector &p2, Fvector &p3, Fvector &p4)
	{
		if (visible_point(p1)) return TRUE;
		if (visible_point(p2)) return TRUE;
		if (visible_point(p3)) return TRUE;
		if (visible_point(p4)) return TRUE;

		Fvector C;
		C.x = (p1.x + p2.x + p3.x + p4.x)*0.25f;
		C.y = (p1.y + p2.y + p3.y + p4.y)*0.25f;
		C.z = (p1.z + p2.z + p3.z + p4.z)*0.25f;
		if (visible_point(C)) return TRUE;

		return FALSE;
	}
};

