//---------------------------------------------------------------------------
#ifndef Custom2DProjectorH
#define Custom2DProjectorH

class CCustom2DProjector{
protected:
    ref_shader		shader_blended;
    ref_shader		shader_overlap;
    string128		name;
    u32 			w;
    u32 			h;
    U32Vec			data;
    DEFINE_VECTOR	(FVF::V,TVertVec,TVertIt);
    TVertVec		mesh;
    ref_geom		geom;
public:
                    CCustom2DProjector	();
    IC bool			Valid				(){return (w>0)&&(h>0)&&(!!data.size());}
    IC void			Clear				(){name[0]=0; w=0; h=0; geom=0; data.clear(); mesh.clear(); DestroyShader();}
    void			CreateRMFromObjects	(const Fbox& box, ObjectList& lst);
    void			Render				();
    void			CreateShader		();
    void			DestroyShader		();
    bool			LoadImage			(LPCSTR nm);
    IC LPCSTR 		GetName				(){ return name; }
    IC bool 		GetColor			(u32& color, int U, int V){
        if (Valid()&&(U<(int)w)&&(V<(int)h)){
            color 	= data[V*w+U];
            return true;
        }
        return false;
    }
    IC float 		GetUFromX			(float x, const Fbox& box){
        R_ASSERT(Valid());
        return 		(x-box.min.x)/(box.max.x-box.min.x);
    }
    IC int			GetPixelUFromX		(float x, const Fbox& box){
        int U		= iFloor(GetUFromX(x,box)*(w-1)+0.5f); if (U<0) U=0;
        return U;
    }
    IC float 		GetVFromZ			(float z, const Fbox& box){
        R_ASSERT(Valid());
        return 		1.f-(z-box.min.z)/(box.max.z-box.min.z);
    }
    IC int			GetPixelVFromZ		(float z, const Fbox& box){
        int V 		= iFloor(GetVFromZ(z,box)*(h-1)+0.5f); if (V<0) V=0;
        return V;
    }
};
//---------------------------------------------------------------------------
#endif
