#ifndef _RENDER_H_
#define _RENDER_H_

const float					fLightSmoothFactor = 4.f;

#include "frustum.h"

// refs
class ENGINE_API	CStream;
class ENGINE_API	CSector;
class ENGINE_API	CPortal;
class ENGINE_API	CVisual;
class ENGINE_API	CObject;
class ENGINE_API	xrLIGHT;
class ENGINE_API	CLightPPA;

// definition
class	ENGINE_API	CRender_interface	:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
public:
	// Data
	Fmatrix*						val_pTransform;
	int								val_iLightLevel;
	BOOL							val_bNearer;
	CFrustum						ViewBase;
	CFrustum*						View;
public:
	// Loading / Unloading
	virtual	void					level_Load				()			= 0;
	virtual void					level_Unload			()			= 0;

	// Information
	virtual int						getVisualsCount			()			= 0;
	virtual CPortal*				getPortal				(int id)	= 0;
	virtual CSector*				getSector				(int id)	= 0;
	virtual CSector*				getSectorActive			()			= 0;
	virtual CVisual*				getVisual				(int id)	= 0;
	virtual DWORD					getFVF					(int id)	= 0;
	virtual IDirect3DVertexBuffer8*	getVB					(int id)	= 0;
	virtual CSector*				detectSector			(Fvector& P)	= 0;
	
	// Main 
	IC		void					set_Frustum				(CFrustum*	O	)			{ VERIFY(O);	View = O;			}
	IC		void					set_Transform			(Fmatrix*	M	)			{ VERIFY(M);	val_pTransform = M;	}
	IC		void					set_LightLevel			(int		L	)			{ val_iLightLevel	= L;			}
	IC		void					set_Nearer				(BOOL 		V	)			{ val_bNearer		= V;			}
	virtual void					flush					()					= 0;	
	virtual void					set_Object				(CObject*	O	)	= 0;
	virtual void					add_Visual				(CVisual*	V	)	= 0;	// add visual leaf	(no culling performed at all)
	virtual void					add_Geometry			(CVisual*	V	)	= 0;	// add visual(s)	(all culling performed)
	virtual void					add_Lights				(vector<WORD> &V)	= 0;
	virtual void					add_Glows				(vector<WORD> &V)	= 0;
	virtual void					add_Patch				(Shader* S, Fvector& P1, float s, float a, BOOL bNearer)	= 0;

	// Lighting
	virtual void					L_add					(CLightPPA* L	)	= 0;
	virtual void					L_select				(Fvector &pos, float fRadius, vector<xrLIGHT>&	dest)	= 0;
	
	// Models
	virtual CVisual*				model_CreatePS			(LPCSTR name, PS::SEmitter* E)	= 0;
	virtual CVisual*				model_Create			(LPCSTR name)					= 0;
	virtual CVisual*				model_Create			(CStream* data)					= 0;
	virtual CVisual*				model_Duplicate			(CVisual* V)					= 0;
	virtual void					model_Delete			(CVisual* &V)					= 0;
	
	// Occlusion culling
	virtual BOOL					occ_visible				(Fbox&	B)						= 0;
	virtual BOOL					occ_visible				(sPoly& P)						= 0;
	
	// Main
	virtual void					Calculate				()	= 0;
	virtual void					Render					()	= 0;
	virtual void					Screenshot				()	= 0;

	// Render mode
	virtual void					rmNear					()	= 0;
	virtual void					rmFar					()	= 0;
	virtual void					rmNormal				()	= 0;

	// Constructor/destructor
	CRender_interface();
	virtual ~CRender_interface();
};

extern ENGINE_API CRender_interface*	Render;
	
#endif