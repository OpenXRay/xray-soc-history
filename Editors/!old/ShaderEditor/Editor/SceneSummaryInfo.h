#ifndef SceneSummaryInfoH
#define SceneSummaryInfoH
#pragma once

#include "ETextureParams.h"

class SSceneSummary{
public:
	enum ESummaryTextureType{
    	sttFirst = 0,
    	sttBase  = 0,
        sttDetail,
        sttDO,
        sttGlow,
        sttLOD,
        sttLast
    }; // �� �������� ����� ������
private:
	struct STextureInfo{
    	shared_str 			file_name;
        shared_str			detail_name;
        STextureParams  	info;
    	ESummaryTextureType type;
        float				effective_area;
    public:
    	STextureInfo(const shared_str& fn, ESummaryTextureType t)
        {
        	file_name		= fn;
        	ZeroMemory		(&info,sizeof(info));
            type			= t;
        	effective_area 	= 0;
        }
        void		Prepare	();
        void		FillProp(PropItemVec& items, LPCSTR pref, u32& mem_use);   
        void		Export	(IWriter* F, u32& mem_use);
		bool operator < (const STextureInfo& other)	const{return xr_strcmp(file_name,other.file_name)<0;};
		bool operator < (const shared_str& fn)		const{return xr_strcmp(file_name,fn)<0;};
		bool operator ==(const shared_str& fn)		const{return file_name.equal(fn);};
    };
    DEFINE_SET	(STextureInfo,TISet,TISetIt);
    TISet		textures;
    struct SPairInfo{
    	xr_string			first;
    	xr_string			second;
    };
    DEFINE_VECTOR(SPairInfo,PIVec,PIVecIt);
	struct SObjectInfo{      
    	shared_str 			object_name;
        PIVec				info;
        u32					ref_count;
    public:
    	SObjectInfo(const shared_str& name)
        {
        	object_name		= name;
        	ref_count		= 0;
        }
        void		Prepare	();
        void		FillProp(PropItemVec& items, LPCSTR pref);   
        void		Export	(IWriter* F);
		bool operator < (const SObjectInfo& other)	const{return xr_strcmp(object_name,other.object_name)<0;};
		bool operator < (const shared_str& fn)		const{return xr_strcmp(object_name,fn)<0;};
		bool operator ==(const shared_str& fn)		const{return object_name.equal(fn);};
    };
    DEFINE_SET	(SObjectInfo,OISet,OISetIt);
    OISet		objects;
public:    
	RStringSet	lod_objects;
	RStringSet	mu_objects;
	RStringSet	waves;
	RStringSet	pe_static;
    int			face_cnt;
    int			vert_cnt;
    int			hom_face_cnt;
    int			hom_vert_cnt;
    int 		object_mu_ref_cnt;
    int 		object_lod_ref_cnt;
    int 		light_point_cnt;
    int 		light_spot_cnt;
    int 		light_dynamic_cnt;
    int 		light_static_cnt;
    int 		light_breakable_cnt;
    int 		light_procedural_cnt;
    int			glow_cnt;
    int			sector_cnt;
    int			portal_cnt;
    int 		sound_source_cnt;
    int 		pe_static_cnt;
    Fbox		bbox;
private:
	void 		OnFileClick			(PropValue* sender, bool& bModif, bool& bSafe);
    void		Prepare				();
public:
    			SSceneSummary		(){ Clear(); }
    void 		AppendTexture		(shared_str name, ESummaryTextureType type, float area)
    {
        TISetIt it 			= std::find(textures.begin(),textures.end(),name);
        if (it==textures.end()){
            std::pair<TISetIt,bool> res	= textures.insert		(STextureInfo(name,type));
            it 				= res.first;
        }
        STextureInfo* info	= (STextureInfo*)(&(*it));
        info->effective_area+=area;
    }
    void 		AppendObject		(shared_str name)
    {
        OISetIt it 			= std::find(objects.begin(),objects.end(),name);
        if (it==objects.end()){
            std::pair<OISetIt,bool> res	= objects.insert		(SObjectInfo(name));
            it 				= res.first;
        }
        SObjectInfo* info	= (SObjectInfo*)(&(*it));
        info->ref_count++;
    }
    void		ExportSummaryInfo	(LPCSTR fn);
    void		FillProp			(PropItemVec& items);
    void		Clear				(){
        bbox.invalidate		();
        textures.clear		();
        lod_objects.clear	();
        mu_objects.clear	();
        waves.clear			();
        pe_static;
        face_cnt			= 0;
        vert_cnt			= 0;
        hom_face_cnt		= 0;
        hom_vert_cnt		= 0;
        object_mu_ref_cnt	= 0;
        object_lod_ref_cnt	= 0;
        light_point_cnt		= 0;
        light_spot_cnt		= 0;
        light_dynamic_cnt	= 0;
        light_static_cnt	= 0;
        light_breakable_cnt	= 0;
        light_procedural_cnt= 0;
        glow_cnt			= 0;
        sector_cnt			= 0;
        portal_cnt			= 0;
        sound_source_cnt	= 0;
        pe_static_cnt		= 0;
    }
};
#endif
