#pragma once

const u32							DSM_size		= 1024;
const float							DSM_distance	= 50.f;
const float							DSM_d_range		= 100.f;

class CLight_Render_Direct
{
public:
	CRT*							rt_smap;	// 32bit, (depth)	(eye-space)
	IDirect3DSurface9*				rt_ZB;

	SGeometry*						g_debug;
	Shader*							s_debug;
public:
	Fmatrix							L_view;
	Fmatrix							L_project;
	Fmatrix							L_combine;
public:
	void							Create			();
	void							Destroy			();
	void							compute_xf_1	();
	void							compute_xf_2	();
	void							render			();

	CLight_Render_Direct			();
	~CLight_Render_Direct			();
};
