#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"

void CRender::Render	()
{
	//******* Main calc
	Device.Statistic.RenderCALC.Begin		();
	{
		marker									++;
		phase									= PHASE_NORMAL;

		// Frustum & HOM rendering
		ViewBase.CreateFromMatrix				(Device.mFullTransform,FRUSTUM_P_LRTB|FRUSTUM_P_FAR);
		View									= 0;
		HOM.Enable								();
		HOM.Render								(ViewBase);

		// Calculate sector(s) and their objects
		set_Object								(0);
		if (0!=pLastSector) pLastSector->Render	(ViewBase);
		g_pGameLevel->pHUD->Render_Last				();
	}
	Device.Statistic.RenderCALC.End				();

	//******* Main render
	{
		// level
		Target.phase_scene						();
		render_hud								();
		render_scenegraph						();
		Details.Render							(Device.vCameraPosition);
		// mapSorted.traverseRL					(sorted_L1);
		mapSorted.clear							();		// unsupported
	}

	//******* Decompression on some HW :)
	if (RImplementation.b_nv3x)					Target.phase_decompress();

	//******* Directional light
	if (ps_r2_ls_flags.test(R2FLAG_SUN) )
	{
		for (u32 dls_phase=0; dls_phase<2; dls_phase++)
		{
			//******* Direct lighting+shadow		::: Calculate
			Device.Statistic.RenderCALC.Begin		();
			{
				marker									++;
				phase									= PHASE_SMAP_D;

				HOM.Disable								();
				LR.compute_xfd_1						(dls_phase);
				render_smap_direct						(LR.L_combine);
				LR.compute_xfd_2						(dls_phase);
			}
			Device.Statistic.RenderCALC.End			();

			//******* Direct lighting+shadow		::: Render
			{
				Target.phase_smap_direct				();

				RCache.set_xform_world					(Fidentity);
				RCache.set_xform_view					(LR.L_view);
				RCache.set_xform_project				(LR.L_project);
				render_scenegraph						();
			}

			//******* Direct lighting+shadow		::: Accumulate
			{
				Target.phase_accumulator				();
				Target.shadow_direct					(dls_phase);
			}
		}

		// Multiply by lighting contribution
		Target.phase_accumulator				();
		Target.accum_direct						();
	}

	// $$$
	// Target.phase_accumulator				();
	// Target.accum_direct					();

	// Point/spot lighting (unshadowed)
	if (1)
	{
		Target.phase_accumulator		();
		HOM.Disable						();
		xr_vector<light*>&	Lvec			= Lights.v_selected_unshadowed;
		for	(u32 pid=0; pid<Lvec.size(); pid++)
		{
			light*	L	= Lvec[pid];
			if (IRender_Light::POINT==L->flags.type)	Target.accum_point_unshadow	(L);
			else										Target.accum_spot_unshadow	(L);
		}
	}

	// Point/spot lighting (shadowed)
	if (1)
	{
		HOM.Disable								();
		xr_vector<light*>&	Lvec	= Lights.v_selected_shadowed;
		for	(u32 pid=0; pid<Lvec.size(); pid++)
		{
			light*	L	= Lvec[pid];
			if (IRender_Light::POINT==L->flags.type)	
			{
				R_ASSERT2	(!RImplementation.b_nv3x, "Shadowed point lights aren't implemented for nv3X HW");

				// Render shadowmap
				for (u32 pls_phase=0; pls_phase<6; pls_phase++)
				{
					marker									++;
					phase									= PHASE_SMAP_P;

					// calculate
					LR.compute_xfp_1						(pls_phase, L);
					render_smap_sector						(L->sector, LR.L_combine, L->position);
					LR.compute_xfp_2						(pls_phase, L);

					// rendering
					if (mapNormal.size())
					{
						Target.phase_smap_point				(pls_phase);
						RCache.set_xform_world				(Fidentity);			// ???
						RCache.set_xform_view				(LR.L_view);
						RCache.set_xform_project			(LR.L_project);
						render_scenegraph					();
					}
				}

				// Render light
				Target.phase_accumulator		();
				Target.accum_point_shadow		(L);
			}
			else
			{
				marker									++;
				phase									= PHASE_SMAP_S;

				// calculate
				LR.compute_xfs_1						(0, L);
				render_smap_sector						(L->sector, LR.L_combine, L->position);
				LR.compute_xfs_2						(0, L);

				// rendering
				if (mapNormal.size() || mapMatrix.size())
				{
					Target.phase_smap_spot				();
					RCache.set_xform_world				(Fidentity);			// ???
					RCache.set_xform_view				(LR.L_view);
					RCache.set_xform_project			(LR.L_project);
					render_scenegraph					();
				}

				// Render light
				Target.phase_accumulator		();
				Target.accum_spot_shadow		(L);
			}
		}
	}

	// Postprocess
	Target.phase_bloom						();
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	g_pGameLevel->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();
}
